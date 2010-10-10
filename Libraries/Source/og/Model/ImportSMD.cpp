// ==============================================================================
//! @file
//! @brief	SMD (Source Engine) Model Import
//! @author	Santo Pfingsten (TTK-Bandit)
//! @note	Copyright (C) 2007-2010 Lusito Software
// ==============================================================================
//
// The Open Game Libraries.
//
// This software is provided 'as-is', without any express or implied
// warranty. In no event will the authors be held liable for any damages
// arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented; you must not
//    claim that you wrote the original software. If you use this software
//    in a product, an acknowledgment in the product documentation would be
//    appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such, and must not be
//    misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source distribution.
//
// ==============================================================================

#include "ModelEx.h"

namespace og {
const int SMD_VERSION = 1;
const float SMD_MODEL_SCALE = 1.0f/24.0f;

static void ParseVector( Lexer &lexer, float *vec, int dim ) {
	for( int i=0; i<dim; i++ )
		vec[i] = lexer.ReadFloat();
}

struct smdWeight_t {
	int		jointId;
	float	influence;
};

struct smdVertex_t {
	int		jointId;
	Vec3	origin;
	Vec3	normal;
	Vec2	texCoords;
	ListEx<smdWeight_t> weights;
};

struct smdMesh_t {
	String name;
	String materialName;
	ListEx<smdVertex_t> vertices;
};

Model *LoadSMD( const char *filename, ListEx<smdMesh_t> &smdMeshes ) {
	Lexer lexer(LEXER_NO_BOM_WARNING);

	// SMD does not have any of these as far as I know
	lexer.SetSingleTokenChars( "" );
	lexer.SetCommentStrings( "", "", "" );

	if ( !lexer.LoadFile( filename ) )
		return NULL;

	Model *model = new Model(true);
	try {
		// Version must be the first keyword
		lexer.ExpectToken( "version" );
		int fileVersion = lexer.ReadInt();
		if ( fileVersion != SMD_VERSION )
			lexer.Error( Format("Version is $*, should be $*") << fileVersion << SMD_VERSION );

		const Token *token;
		const char *string;

		int nodeIndex, nodeTime;
		int numWeights;
		int i, j, index, line;

		Angles eulerAngles;

		StringList materialNames;
		String material;

		while ( (token = lexer.ReadToken()) != NULL ) {
			string = token->GetString();
			if ( String::Icmp(string, "nodes" ) == 0 ) {
				do {
					nodeIndex = lexer.ReadInt();
					if ( nodeIndex != model->bones.Num() )
						lexer.Error( Format("Bad Node Index, should be $*") << model->bones.Num() );

					Bone &bone = model->bones.Alloc();
					bone.name = lexer.ReadString();
					bone.idParent = lexer.ReadInt();
					bone.flags = 0;
					if ( lexer.CheckToken("end") )
						break;
				} while (1);
			} else if ( String::Icmp(string, "skeleton" ) == 0 ) {
				lexer.ExpectToken("time");
				nodeTime = lexer.ReadInt();

				do {
					nodeIndex = lexer.ReadInt();
					if ( nodeIndex < 0 || nodeIndex >= model->bones.Num() )
						lexer.Error( Format("Node Index out of range: $*") << nodeIndex );

					Bone &bone = model->bones[nodeIndex];
					ParseVector( lexer, &bone.origin.x, 3);
					bone.origin *= SMD_MODEL_SCALE;

					eulerAngles.roll = lexer.ReadFloat();
					eulerAngles.pitch = lexer.ReadFloat();
					eulerAngles.yaw = lexer.ReadFloat();

					eulerAngles *= Math::RAD_TO_DEG;
					bone.quat = eulerAngles.ToQuat();
					bone.quat.Conjugate();

					if ( lexer.CheckToken("end") )
						break;
					if ( lexer.CheckToken("time") ) {// currently only support one time frame
						lexer.FindToken("end");
						break;
					}
				} while (1);
			} else if ( String::Icmp(string, "triangles" ) == 0 ) {
				do {
					material = lexer.ReadString();
					index = materialNames.IFind( material.c_str() );
					if ( index == -1 ) {
						index = materialNames.Num();
						materialNames.Append( material );
						smdMesh_t &mesh = smdMeshes.Alloc();
						mesh.name = Format("mesh $*") << index;
						mesh.materialName = material;
						mesh.vertices.SetGranularity(128);
					}
					smdMesh_t &mesh = smdMeshes[index];

					for( i=0; i<3; i++ ) {
						smdVertex_t &vert = mesh.vertices.Alloc();
						vert.jointId = lexer.ReadInt();
						ParseVector( lexer, &vert.origin.x, 3);
						ParseVector( lexer, &vert.normal.x, 3);
						ParseVector( lexer, &vert.texCoords.x, 2);
						vert.texCoords.y *= -1;
						vert.origin *= SMD_MODEL_SCALE;

						if ( vert.normal.IsZero() ) {
							User::Warning( Format("Zero Normal found on vertex $*" ) << mesh.vertices.Num() );
							vert.normal.z = 1.0f;
						}
						// Check if the next token changes line
						line = lexer.GetLine();
						if ( (token = lexer.ReadToken()) == NULL )
							throw LexerError( LexerError::END_OF_FILE );
						lexer.UnreadToken();

						if ( line == lexer.GetLine() ) {
							// No line change, so this token is the numWeights param
							numWeights = lexer.ReadInt();

							vert.weights.SetGranularity(numWeights);
							vert.weights.CheckSize(numWeights);
							for( j=0; j<numWeights; j++ ) {
								smdWeight_t &weight = vert.weights.Alloc();
								weight.jointId = lexer.ReadInt();
								weight.influence = lexer.ReadFloat();
							}
						} else {
							// Line has changed, so no weights are present.. add a default weight
							vert.weights.SetGranularity(1);
							smdWeight_t &weight = vert.weights.Alloc();
							weight.jointId = vert.jointId;
							weight.influence = 1.0f;
						}
					}

					if ( lexer.CheckToken("end") )
						break;
				} while (1);
			} else {
				lexer.FindToken("end");
			}
		}
		return model;
	}
	catch( LexerError err ) {
		delete model;
		String errStr;
		err.ToString( errStr );
		User::Error( ERR_LEXER_FAILURE, errStr.c_str(), filename );
		return NULL;
	}
}

/*
================
Model::ImportSMD
================
*/
Model *Model::ImportSMD( const char *filename ) {
	ListEx<smdMesh_t> smdMeshes;
	Model *model = LoadSMD( filename, smdMeshes );
	if ( !model )
		return NULL;
	
	int i, j, k, m;

	// We need a full build skeleton to recalculate the real weight origins.
	int numBones = model->bones.Num();
	DynBuffer<Bone> buildSkeleton( numBones );
	for( i=0; i<numBones; i++ ) {
		Bone &bone = buildSkeleton.data[i];
		bone.origin = model->bones[i].origin;
		bone.quat = model->bones[i].quat;

		j = model->bones[i].idParent;
		if( j >= 0 ) {
			bone.quat = buildSkeleton.data[j].quat * bone.quat;
			bone.origin = (buildSkeleton.data[j].quat * bone.origin) + buildSkeleton.data[j].origin;
		}
	}

	ListEx<VertexWeight> inWeights;
	List<int> indices;
	indices.SetGranularity(128);
	float infTotal, factor;
	int numVertices, numWeights, addedVertices, numIndices;
	int numMeshes = smdMeshes.Num();
	for( i=0; i<numMeshes; i++ ) {
		MeshAnimated *mesh = new MeshAnimated;
		model->meshes.Append(mesh);
		smdMesh_t &smdMesh = smdMeshes[i];
		mesh->name = smdMesh.name;
		mesh->material = smdMesh.materialName;

		numVertices = smdMesh.vertices.Num();
		InitVertices( mesh, numVertices );
		addedVertices = 0;
		indices.Clear();

		for( j=0; j<numVertices; j++ ) {
			smdVertex_t &vert = smdMesh.vertices[j];

			inWeights.Clear();
			numWeights = vert.weights.Num();
			infTotal = 0.0f;
			for( k=0; k<numWeights; k++ ) {
				smdWeight_t &smdWeight = vert.weights[k];
				VertexWeight &weight = inWeights.Alloc();
				weight.influence = Math::Min(smdWeight.influence, 1.0f);
				weight.boneId = smdWeight.jointId;
				infTotal += weight.influence;

				// Convert Weights
				Quat quat = buildSkeleton.data[smdWeight.jointId].quat;
				quat.Conjugate();
				weight.origin = quat * (vert.origin - buildSkeleton.data[smdWeight.jointId].origin);
				weight.normal = quat * vert.normal;
				
				// Normalize
				weight.normal.Normalize();
			}
			// Make sure the weight influences sum up to 1.0f
			if ( infTotal != 1.0f ) {
				factor = 1.0f/infTotal;
				for( k=0; k<numWeights; k++ )
					inWeights[k].influence *= factor;
			}
			// Add the vertex
			indices.Append( addedVertices );
			mesh->texCoords[addedVertices] = vert.texCoords;

			Vertex &vInfo = mesh->vertices[addedVertices];
			InitVertex( &vInfo, numWeights );
			for( m=0; m<numWeights; m++ ) {
				VertexWeight &inWeight = inWeights[m];
				VertexWeight &curWeight = vInfo.weights[m];

				curWeight.boneId = inWeight.boneId;
				curWeight.influence = inWeight.influence;
				curWeight.origin = inWeight.origin;
				curWeight.normal = inWeight.normal;
			}
			addedVertices++;
		}
		// Since we used smdMesh.vertices.Num() to init the vertices, but did not add all vertices,
		// as some where double, we need to set the real amount of vertices here.
		mesh->numVerts = addedVertices;

		// Now add all indices in reverse order
		numIndices = indices.Num();
		InitIndices( mesh, numIndices );
		for( j=0; j<numIndices; j+=3 ) {
			mesh->indices[j] = indices[j+2];
			mesh->indices[j+1] = indices[j+1];
			mesh->indices[j+2] = indices[j];
		}
	}
	return model;
}

}
