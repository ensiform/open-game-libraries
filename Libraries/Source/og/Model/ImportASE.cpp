// ==============================================================================
//! @file
//! @brief	ASE (Unreal) Model Import
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
const float ASE_MODEL_SCALE = 1.0f/24.0f;

/*
================
SkipUnknown
================
*/
static bool SkipUnknown( Lexer &lexer ) {
	const Token *token;
	const char *p;
	int line = lexer.GetLine();
	while ( (token = lexer.ReadToken()) != NULL ) {
		p = token->GetString();
		if ( !p || !*p )
			continue;

		if ( line != lexer.GetLine() ) {
			lexer.UnreadToken();
			return true;
		}
		if ( String::Icmp( p, "{" ) == 0 ) {
			if ( !lexer.GotoNextLine() )
				return false;
			while( lexer.CheckToken("*") ) {
				if ( !SkipUnknown(lexer) )
				return false;
			}
			
			lexer.FindToken("}");
			return lexer.GotoNextLine();
		}
	}
	return false;
}

/*
==============================================================================

  aseVertex

==============================================================================
*/
struct aseVertex {
	Vec3 origin;
	Vec3 normal;
};

/*
==============================================================================

  aseTriangle

==============================================================================
*/
struct aseTriangle {
	int	v[3];
	int	t[3];
};

/*
==============================================================================

  aseMesh

==============================================================================
*/
class aseMesh {
public:
	aseMesh() {
		numVerts = 0;
		numTris = 0;
		vertices = NULL;
		triangles = NULL;
		texCoords = NULL;
		materialId = 0;
	}
	~aseMesh(){
		if ( vertices )
			delete[] vertices;
		if ( triangles )
			delete[] triangles;
		if ( texCoords )
			delete[] texCoords;
	}
	String name;

	uInt	numVerts;
	aseVertex *vertices;

	uInt	numTris;
	aseTriangle		*triangles;

	uInt	numTVerts;
	Vec2	*texCoords;

	int		materialId;
};


/*
================
ReadMaterial
================
*/
static void ReadMaterial( Lexer &lexer ) {
	lexer.ReadInt();
	lexer.ExpectToken("{");
	const char *p;
	const Token *token;
	while ( (token = lexer.ReadToken()) != NULL ) {
		p = token->GetString();
		if ( !p || !*p )
			continue;
		if ( String::Icmp( p, "}" ) == 0 ) 
			return;
		if ( String::Icmp( p, "*" ) != 0 ) 
			lexer.Error( TS("expected *, got '$*'") << p );

		//! @todo	Finish this

		if ( !SkipUnknown(lexer) )
			break;
	}
}

/*
================
ReadMaterialList
================
*/
static void ReadMaterialList( Lexer &lexer ) {
	lexer.ExpectToken("{");
	const char *p;
	const Token *token;
	int numMaterials;
	while ( (token = lexer.ReadToken()) != NULL ) {
		p = token->GetString();
		if ( !p || !*p )
			continue;
		if ( String::Icmp( p, "}" ) == 0 ) 
			return;
		if ( String::Icmp( p, "*" ) != 0 ) 
			lexer.Error( TS("expected *, got '$*'") << p );

		if ( lexer.CheckToken( "MATERIAL_COUNT" ) )
			numMaterials = lexer.ReadInt();
		else if ( lexer.CheckToken( "MATERIAL" ) )
			ReadMaterial( lexer );
		else if ( !SkipUnknown(lexer) )
			break;
	}
}

/*
================
ReadMesh
================
*/
static void ReadMesh( Lexer &lexer, aseMesh *inMesh ) {
	int idx;
	Vec3 temp;
	lexer.ExpectToken("{");
	const char *p;
	const Token *token;
	while ( (token = lexer.ReadToken()) != NULL ) {
		p = token->GetString();
		if ( !p || !*p )
			continue;
		if ( String::Icmp( p, "}" ) == 0 ) 
			return;
		if ( String::Icmp( p, "*" ) != 0 ) 
			lexer.Error( TS("expected *, got '$*'") << p );

		if ( lexer.CheckToken( "MESH_NUMVERTEX" ) ) {
			inMesh->numVerts = lexer.ReadInt();
			inMesh->vertices = new aseVertex[inMesh->numVerts];
		}
		else if ( lexer.CheckToken( "MESH_NUMFACES" ) ) {
			inMesh->numTris = lexer.ReadInt();
			inMesh->triangles = new aseTriangle[inMesh->numTris];
		}
		else if ( lexer.CheckToken( "MESH_VERTEX_LIST" ) ) {
			lexer.ExpectToken("{");
			for( int i=0; i<inMesh->numVerts; i++ ) {
				lexer.ExpectToken("*");
				lexer.ExpectToken("MESH_VERTEX");
				idx = lexer.ReadInt();
				inMesh->vertices[idx].origin.x = lexer.ReadFloat() * ASE_MODEL_SCALE;
				inMesh->vertices[idx].origin.y = lexer.ReadFloat() * ASE_MODEL_SCALE;
				inMesh->vertices[idx].origin.z = lexer.ReadFloat() * ASE_MODEL_SCALE;
			}
			lexer.ExpectToken("}");
		}
		else if ( lexer.CheckToken( "MESH_NORMALS" ) ) {
			lexer.ExpectToken("{");
			for( ;; ) {
				if ( lexer.CheckToken("}") )
					break;
				lexer.ExpectToken("*");
				// don't need the face normal
				if ( lexer.CheckToken("MESH_FACENORMAL") ) {
					lexer.GotoNextLine();
					continue;
				}
				lexer.ExpectToken("MESH_VERTEXNORMAL");
				idx = lexer.ReadInt();
				inMesh->vertices[idx].normal.x = lexer.ReadFloat();
				inMesh->vertices[idx].normal.y = lexer.ReadFloat();
				inMesh->vertices[idx].normal.z = lexer.ReadFloat();
			}
		}
		else if ( lexer.CheckToken( "MESH_FACE_LIST" ) ) {
			lexer.ExpectToken("{");
			for( int i=0; i<inMesh->numTris; i++ ) {
				lexer.ExpectToken("*");
				lexer.ExpectToken("MESH_FACE");
				idx = lexer.ReadInt();
				lexer.CheckToken(":"); // might or might not be there
				lexer.ExpectToken("A");
				lexer.ExpectToken(":");
				inMesh->triangles[idx].v[0] = lexer.ReadInt();
				lexer.ExpectToken("B");
				lexer.ExpectToken(":");
				inMesh->triangles[idx].v[1] = lexer.ReadInt();
				lexer.ExpectToken("C");
				lexer.ExpectToken(":");
				inMesh->triangles[idx].v[2] = lexer.ReadInt();
				lexer.GotoNextLine();
			}
			lexer.ExpectToken("}");
		}
		else if ( lexer.CheckToken( "MESH_NUMTVERTEX" ) ) {
			inMesh->numTVerts = lexer.ReadInt();
			inMesh->texCoords = new Vec2[inMesh->numTVerts];
		}
		else if ( lexer.CheckToken( "MESH_TVERTLIST" ) ) {
			lexer.ExpectToken("{");
			for( int i=0; i<inMesh->numTVerts; i++ ) {
				lexer.ExpectToken("*");
				lexer.ExpectToken("MESH_TVERT");
				idx = lexer.ReadInt();
				inMesh->texCoords[idx].x = lexer.ReadFloat();
				inMesh->texCoords[idx].y = 1.0f-lexer.ReadFloat();
				lexer.ReadFloat();// don't need 3rd component
			}
			lexer.ExpectToken("}");
		}
		else if ( lexer.CheckToken( "MESH_NUMTVFACES" ) )
			lexer.ExpectToken( TS() << inMesh->numTris );
		else if ( lexer.CheckToken( "MESH_TFACELIST" ) ) {
			lexer.ExpectToken("{");
			for( int i=0; i<inMesh->numTris; i++ ) {
				lexer.ExpectToken("*");
				lexer.ExpectToken("MESH_TFACE");
				idx = lexer.ReadInt();

				inMesh->triangles[idx].t[0] = lexer.ReadInt();
				inMesh->triangles[idx].t[1] = lexer.ReadInt();
				inMesh->triangles[idx].t[2] = lexer.ReadInt();
			}
			lexer.ExpectToken("}");
		}
		else if ( lexer.CheckToken( "MATERIAL_REF" ) )
			inMesh->materialId = lexer.ReadInt();
		else if ( !SkipUnknown(lexer) )
			break;
	}
}

/*
================
ReadGeoMObject
================
*/
static void ReadGeoMObject( Lexer &lexer, ListEx<aseMesh> &myMeshList ) {
	lexer.ExpectToken("{");
	const char *p;
	const Token *token;
	String name;
	while ( (token = lexer.ReadToken()) != NULL ) {
		p = token->GetString();
		if ( !p || !*p )
			continue;
		if ( String::Icmp( p, "}" ) == 0 ) 
			return;
		if ( String::Icmp( p, "*" ) != 0 ) 
			lexer.Error( TS("expected *, got '$*'") << p );

		if ( lexer.CheckToken( "NODE_NAME" ) )
			name = lexer.ReadString();
		else if ( lexer.CheckToken( "MESH" ) ) {
			aseMesh *inMesh = &myMeshList.Alloc();
			inMesh->name = name;
			ReadMesh( lexer, inMesh );
		}
		else if ( !SkipUnknown(lexer) )
			break;
	}
}

/*
================
Model::ImportASE
================
*/
Model *Model::ImportASE( const char *filename ) {
	ListEx<aseMesh> myMeshList;
	Lexer lexer(LEXER_NO_BOM_WARNING);
	lexer.SetSingleTokenChars( "{}*:" );
	lexer.SetCommentStrings( "", "", "" );

	if ( !lexer.LoadFile( filename ) )
		return NULL;

	int numJoints = -1;
	Model *model = new Model(false);
	try {
		const Token *token;
		String key;

		const char *p;

		while ( (token = lexer.ReadToken()) != NULL ) {
			p = token->GetString();
			if ( !p || !*p )
				continue;
			if ( String::Icmp( p, "*" ) != 0 ) 
				lexer.Error( TS("expected *, got '$*'") << p );

			if ( lexer.CheckToken( "MATERIAL_LIST" ) ) 
				ReadMaterialList( lexer );
			else if ( lexer.CheckToken( "GEOMOBJECT" ) )
				ReadGeoMObject( lexer, myMeshList );
			else if ( !SkipUnknown(lexer) )
				break;
		}
//		if ( inJointGroup || inMeshGroup || numJoints == -1 || numMeshes == -1 )
//			throw LexerError( LexerError::END_OF_FILE );
	}
	catch( LexerError err ) {
		delete model;
		String errStr;
		err.ToString( errStr );
		User::Error( ERR_LEXER_FAILURE, errStr.c_str(), filename );
		return NULL;
	}

	// add root bone
	Bone &bone = model->bones.Alloc();
	bone.name = "root";
	bone.idParent = -1;
	bone.flags = 0;

	int numMeshes = myMeshList.Num();

	int index, index2, j, k, l, m;
	for( int i=0; i<numMeshes; i++ ) {
		aseMesh &inMesh = myMeshList[i];
		// Store Mesh Info
		MeshStatic *mesh = new MeshStatic;
		model->meshes.Append(mesh);
		mesh->name = inMesh.name;
//		mesh->material = ?;

		InitIndices( mesh, inMesh.numTris * 3 );

		//! @todo	need a nice way to get all needed vertices. the given vertices are not enough
#if 1
		InitVertices( mesh, inMesh.numTris * 3 );

		// Convert Indices
		for( j=0, k=0, m=0; j<inMesh.numTris; j++ ) {
			for( int i=0; i<3; i++ ) {
				index = inMesh.triangles[j].v[2-i];
				index2 = inMesh.triangles[j].t[2-i];
#if 1 //! @todo	this is too slow
				for( l=0; l<k; l++ ) {
					if ( mesh->vertices[l] == inMesh.vertices[index].origin && mesh->texCoords[l] == inMesh.texCoords[index2] )
						break;
				}
				if ( l < k )
					mesh->indices[m++] = l;
				else
#endif
				{
					mesh->vertices[k] = inMesh.vertices[index].origin;
					mesh->texCoords[k] = inMesh.texCoords[index2];
					mesh->indices[m++] = k++;
				}
			}
		}
#else
		// Flawed:
		InitVertices( mesh, inMesh.numVerts );

		// Convert Vertices
		for( int j=0; j<inMesh.numVerts; j++ )
			mesh->vertices[j] = inMesh.vertices[j].origin;
		// texcoords
		for (int j = 0; j < inMesh.numTris; j++) {
			for( int k=0; k<3; k++ ) {
				index = inMesh.triangles[j].v[k];
				mesh->texCoords[index] = inMesh.texCoords[inMesh.triangles[j].t[k]];
			}
		}

		// Convert Indices
		for( int j=0, k=0; j<inMesh.numTris; j++ ) {
			mesh->indices[k++] = inMesh.triangles[j].v[2];
			mesh->indices[k++] = inMesh.triangles[j].v[1];
			mesh->indices[k++] = inMesh.triangles[j].v[0];
		}
#endif
	}
	return model;
}

}
