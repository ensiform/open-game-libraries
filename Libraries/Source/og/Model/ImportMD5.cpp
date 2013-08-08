// ==============================================================================
//! @file
//! @brief	MD5 (Doom 3) Model Import
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
const int MD5_VERSION_DOOM3 = 10;
const int MD5_VERSION_QUAKEWARS = 11;
const float MD5_MODEL_SCALE = 1.0f/24.0f;

static void ParseVector( Lexer &lexer, float *vec, int dim ) {
	lexer.ExpectToken("(");
	for( int i=0; i<dim; i++ )
		vec[i] = lexer.ReadFloat();
	lexer.ExpectToken(")");
}

struct MD5Vertex {
	Vec2 texCoord;
	int firstWeight;
	int lastWeight;
};

struct MD5Weight {
	int jointIndex;
	float bias;
	Vec3 origin;
};

/*
================
ImportMD5AnimBaseFrame
================
*/
bool ImportMD5AnimBaseFrame( const char *filename, Model *model ) {
	Lexer lexer(LEXER_NO_BOM_WARNING);
	if ( !lexer.LoadFile( filename ) ) {
		delete model;
		return false;
	}

	try {
		// MD5Version must be the first keyword
		lexer.ExpectToken( "MD5Version" );
		int fileVersion = lexer.ReadInt();
		if ( fileVersion != MD5_VERSION_DOOM3 && fileVersion != MD5_VERSION_QUAKEWARS )
			lexer.Error( Format("MD5Version is $*, should be $* or $*") << fileVersion << MD5_VERSION_DOOM3 << MD5_VERSION_QUAKEWARS );

		const Token *token;
		String key;

		const char *p;

		bool inHierarchyGroup = false;
		bool inBoundsGroup = false;
		bool inBaseFrameGroup = false;
		bool inFrameGroup = false;

		int numJoints = -1;
		int frameRate, numFrames, currentFrame;

		bool noAnimate = false;
		bool readVertexColor = false;
		Color vertexColor;

		ListEx<MD5Vertex> vertexList;
		ListEx<MD5Weight> weightList;
		Vec3 vTemp;
		int boneIndex = 0;

		while ( (token = lexer.ReadToken()) != NULL ) {
			p = token->GetString();
			if ( !p || !*p )
				continue;

			if ( inHierarchyGroup ) {
				if ( *p == '}' ) {
					inHierarchyGroup = false;
					continue;
				}
				Bone &bone = model->bones.Alloc();
				bone.name = p;
				bone.idParent = lexer.ReadInt();
				bone.flags = lexer.ReadInt();
				lexer.ReadInt(); // start index ?
			} else if ( inBaseFrameGroup ) {
				if ( *p == '}' ) {
					inBaseFrameGroup = false;
					break; //! @todo	should be continue when loading all frames.
				}
				lexer.UnreadToken();
				Bone &bone = model->bones[boneIndex++];
				ParseVector( lexer, &bone.origin.x, 3 );
				ParseVector( lexer, &bone.quat.x, 3 );

				bone.origin *= MD5_MODEL_SCALE;

				// Compute w
				bone.quat.w = 1.0f - (bone.quat.x * bone.quat.x) - (bone.quat.y * bone.quat.y) - (bone.quat.z * bone.quat.z);
				bone.quat.w = ( bone.quat.w <= 0.0f ) ? 0.0f : -Math::Sqrt( bone.quat.w );
			} else if ( inBoundsGroup ) {
				if ( *p == '}' ) {
					inBoundsGroup = false;
					continue;
				}
				lexer.UnreadToken();
				ParseVector( lexer, &vTemp.x, 3 );
				ParseVector( lexer, &vTemp.x, 3 );
			} else if ( inFrameGroup ) {
				if ( *p == '}' ) {
					inFrameGroup = false;
					continue;
				}
				//! @todo	why was here a fixme ?
			} else {
				if ( String::Icmp( p, "commandline" ) == 0 ) {
					// Skip value, we don't need it
					lexer.ReadToken();
				} else if ( String::Icmp( p, "frameRate" ) == 0 ) {
					frameRate = lexer.ReadInt();
				} else if ( String::Icmp( p, "numAnimatedComponents" ) == 0 ) {
					// Skip value, we don't need it
					lexer.ReadToken();
				} else if ( String::Icmp( p, "numJoints" ) == 0 ) {
					numJoints = lexer.ReadInt();
					if ( numJoints > 0 ) {
						model->bones.SetGranularity( numJoints );
						model->bones.CheckSize( numJoints );
					}
				} else if ( String::Icmp( p, "numFrames" ) == 0 ) {
					numFrames = lexer.ReadInt();
					if ( numFrames <= 0 )
						lexer.Error("Zero frames on model");
					//! @todo	why was here a fixme ?
				} else if ( String::Icmp( p, "hierarchy" ) == 0 ) {
					if ( numJoints == -1 )
						lexer.Error("numJoins not set!");
					lexer.ExpectToken( "{" );
					inHierarchyGroup = true;
				} else if ( String::Icmp( p, "bounds" ) == 0 ) {
					lexer.ExpectToken( "{" );
					inBoundsGroup = true;
				} else if ( String::Icmp( p, "baseframe" ) == 0 ) {
					lexer.ExpectToken( "{" );
					inBaseFrameGroup = true;
				} else if ( String::Icmp( p, "frame" ) == 0 ) {
					currentFrame = lexer.ReadInt();
					lexer.ExpectToken( "{" );
					inFrameGroup = true;
				} else {
					lexer.Error( Format("Unexpected '$*'") << p );
				}
			}
		}
		if ( inHierarchyGroup || inBoundsGroup || inFrameGroup || numJoints == -1 )
			throw LexerError( LexerError::END_OF_FILE );
		return true;
	}
	catch( LexerError &err ) {
		delete model;
		String errStr;
		err.ToString( errStr );
		User::Error( ERR_LEXER_FAILURE, errStr.c_str(), filename );
		return false;
	}
}

/*
================
Model::ImportMD5
================
*/
Model *Model::ImportMD5( const char *filename, const char *filenameAnim ) {
	Lexer lexer(LEXER_NO_BOM_WARNING);
	if ( !lexer.LoadFile( filename ) )
		return NULL;

	Model *model = new Model(true);
	int numJoints = -1;
	try {

		// MD5Version must be the first keyword
		lexer.ExpectToken( "MD5Version" );
		int fileVersion = lexer.ReadInt();
		if ( fileVersion != MD5_VERSION_DOOM3 && fileVersion != MD5_VERSION_QUAKEWARS )
			lexer.Error( Format( "MD5Version is $*, should be $* or $*") << fileVersion << MD5_VERSION_DOOM3 << MD5_VERSION_QUAKEWARS );

		const Token *token;
		String key;

		const char *p;

		bool inJointGroup = false;
		bool inMeshGroup = false;
		bool readMeshVerts = false;
		bool readMeshTris = false;

		int numMeshes = -1;
		int numVerts, vertIndex;
		int triCounter, indexCounter;
		int numWeights;
		MeshAnimated *mesh = NULL;
		
		int i, j, num;
		Vec3 vTemp;

		bool noAnimate = false;
		bool readVertexColor = false;
		Color vertexColor;

		ListEx<MD5Vertex> vertexList;
		ListEx<MD5Weight> weightList;

		while ( (token = lexer.ReadToken()) != NULL ) {
			p = token->GetString();
			if ( !p || !*p )
				continue;

			if ( inJointGroup ) {
				if ( *p == '}' ) {
					inJointGroup = false;
					continue;
				}
				lexer.ReadInt(); // unused
				ParseVector( lexer, &vTemp.x, 3 );
				ParseVector( lexer, &vTemp.x, 3 );
			} else if ( inMeshGroup ) {
				lexer.UnreadToken();
				if ( readMeshVerts ) {
					if ( lexer.CheckToken( "numtris" ) ) {
						if ( numVerts != vertexList.Num() )
							lexer.Error( Format("numVerts don't match: $*") << vertexList.Num() );
						InitIndices( mesh, lexer.ReadInt() * 3 ); // numtris * 3
						triCounter = 0;
						indexCounter = 0;
						readMeshVerts = false;
						readMeshTris = true;
						continue;
					}
					lexer.ExpectToken( "vert" );
					
					vertIndex = lexer.ReadInt();
					if ( vertIndex != vertexList.Num() )
						lexer.Error( Format("Bad Vert Index, should be $*") << vertexList.Num() );

					MD5Vertex &vertex = vertexList.Alloc();
					ParseVector( lexer, &vertex.texCoord.x, 2 );
					vertex.firstWeight = lexer.ReadInt();
					vertex.lastWeight = lexer.ReadInt() + vertex.firstWeight - 1;
					
					if ( readVertexColor ) {
						// Currently we don't have use for vertex color..
						// I haven't seen md5meshes with other values than ( 1 1 1 1 ) for vertexColor anyway..
						ParseVector( lexer, &vertexColor.r, 4 );
					}
				} else if ( readMeshTris ) {
					if ( lexer.CheckToken( "numweights" ) ) {
						numWeights = lexer.ReadInt();
						weightList.CheckSize( numWeights );
						readMeshTris = false;
						continue;
					}
					lexer.ExpectToken( "tri" );

					if ( lexer.ReadInt() != triCounter )
						lexer.Error( Format("Bad Tri Index, should be $*") << triCounter );

					mesh->indices[indexCounter++] = lexer.ReadInt();
					mesh->indices[indexCounter++] = lexer.ReadInt();
					mesh->indices[indexCounter++] = lexer.ReadInt();
					triCounter++;
				} else {
					if ( lexer.CheckToken( "}" ) ) {
						if ( numWeights != weightList.Num() )
							lexer.Error( Format("numWeights doesn't match $*") << weightList.Num() );

						// Convert vertices:
						num = vertexList.Num();
						for( i=0; i<num; i++ ) {
							if ( i >= vertexList.Num() )
								lexer.Error( "Vertex index out of range" );

							const MD5Vertex &md5Vert = vertexList[i];

							mesh->texCoords[i] = md5Vert.texCoord;

							Vertex &vInfo = mesh->vertices[i];
							InitVertex( &vInfo, 1 + md5Vert.lastWeight - md5Vert.firstWeight );
							vInfo.numWeights = 0;

							for ( j=md5Vert.firstWeight; j<=md5Vert.lastWeight; j++ ) {
								if ( j >= weightList.Num() )
									lexer.Error( "Weight index out of range" );
								const MD5Weight &md5Weight = weightList[j];

								if ( md5Weight.jointIndex >= numJoints )
									lexer.Error( "Weight joint index out of range" );

								if ( md5Weight.bias == 0.0f )
									continue;

								VertexWeight &weight = vInfo.weights[vInfo.numWeights++];
								weight.boneId = md5Weight.jointIndex;
								weight.origin = md5Weight.origin;
								//weight.normal = md5Weight.normal; //! @todo	calculate normal
								weight.influence = md5Weight.bias;
							}
						}
						inMeshGroup = false;
						continue;
					}
					lexer.ExpectToken( "weight" );

					if ( lexer.ReadInt() != weightList.Num() )
						lexer.Error( Format("Bad VertexWeight Index, should be $*") << weightList.Num() );

					MD5Weight &weight = weightList.Alloc();
					weight.jointIndex = lexer.ReadInt();
					weight.bias = lexer.ReadFloat();
					ParseVector( lexer, &weight.origin.x, 3 );
					weight.origin *= MD5_MODEL_SCALE;
				}
			} else {
				if ( String::Icmp( p, "commandline" ) == 0 ) {
					// Skip value, we don't need it
					lexer.ReadToken();
				} else if ( String::Icmp( p, "numJoints" ) == 0 ) {
					numJoints = lexer.ReadInt();
					if ( numJoints > 0 ) {
						model->bones.SetGranularity( numJoints );
						model->bones.CheckSize( numJoints );
					}
				} else if ( String::Icmp( p, "numMeshes" ) == 0 ) {
					numMeshes = lexer.ReadInt();
					if ( numMeshes <= 0 )
						lexer.Error("Zero meshes");
					model->meshes.SetGranularity( numMeshes );
					model->meshes.CheckSize( numMeshes );
				} else if ( String::Icmp( p, "joints" ) == 0 ) {
					if ( numJoints == -1 )
						lexer.Error( "numJoins not set!" );

					lexer.ExpectToken( "{" );
					inJointGroup = true;
				} else if ( String::Icmp( p, "mesh" ) == 0 ) {
					if ( numMeshes == -1 )
						lexer.Error( "numMeshes not set!" );

					lexer.ExpectToken( "{" );

					mesh = new MeshAnimated;
					model->meshes.Append(mesh);

					if ( fileVersion != MD5_VERSION_QUAKEWARS )
						mesh->name = Format("mesh $*") << model->meshes.Num();
					else {
						lexer.ExpectToken( "name" );
						mesh->name = lexer.ReadString();
					}

					lexer.ExpectToken( "shader" );
					mesh->material = lexer.ReadString();

					noAnimate = false;
					readVertexColor = false;
					if ( fileVersion == MD5_VERSION_QUAKEWARS ) {
						lexer.ExpectToken( "flags" );
						lexer.ExpectToken( "{" );
						while( !lexer.CheckToken("}") ) {
							if ( lexer.CheckToken("noAnimate") )
								noAnimate = true;
							else if ( lexer.CheckToken("vertexColor") )
								readVertexColor = true;
							else
								lexer.Error( Format("Unknown flag: '$*'") << lexer.ReadString() );
						}
					}
					lexer.ExpectToken( "numverts" );

					numVerts = lexer.ReadInt();

					vertexList.Clear();
					vertexList.CheckSize( numVerts );
					weightList.Clear();
					InitVertices( mesh, numVerts );
					inMeshGroup = true;
					readMeshVerts = true;
				} else {
					lexer.Error( Format("Unexpected '$*'") << p );
				}
			}
		}
		if ( inJointGroup || inMeshGroup || numJoints == -1 || numMeshes == -1 )
			throw LexerError( LexerError::END_OF_FILE );
	}
	catch( LexerError &err ) {
		delete model;
		String errStr;
		err.ToString( errStr );
		User::Error( ERR_LEXER_FAILURE, errStr.c_str(), filename );
		return NULL;
	}

	if ( !ImportMD5AnimBaseFrame( filenameAnim, model ) )
		return NULL;

	if ( numJoints != model->bones.Num() ) {
		delete model;
		User::Error( ERR_FILE_CORRUPT, "Number of Joints do not match", filename );
		return NULL;
	}
	return model;
}

}
