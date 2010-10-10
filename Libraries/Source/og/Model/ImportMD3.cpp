// ==============================================================================
//! @file
//! @brief	MD3 (Quake 3) Model Import
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
const int MAX_QPATH = 64;

/*
==============================================================================

  MD3Header

==============================================================================
*/
struct MD3Header {
	char fileId[4];
	int version;
	char name[MAX_QPATH];
	int flags;
	int numFrames;
	int numTags;
	int numSurfaces;
	int numSkins;
	int surfaceSize;

	int offsetTags;
	int offsetSurfaces;
	int offsetEnd;
};

/*
==============================================================================

  MD3Surface

==============================================================================
*/
struct MD3Surface {
	char surfId[4];
	char name[MAX_QPATH];
	int flags;
	int numFrames;
	int numShaders;
	int numVerts;
	int numTriangles;
	int offsetTriangles;
	int offsetShaders;
	int offsetST;
	int offsetVerts;
	int offsetEnd;
};

/*
==============================================================================

  MD3Vertex

==============================================================================
*/
struct MD3Vertex {
	short x;
	short y;
	short z;
	short normal;
};

/*
==============================================================================

  MD3Tag

==============================================================================
*/
struct MD3Tag {
	char	name[MAX_QPATH];
	Vec3	origin;
	Mat3	axis;
};

/*
==============================================================================

  MD3Shader

==============================================================================
*/
struct MD3Shader {
	char	name[MAX_QPATH];
	int		index;
};

/*
==============================================================================

  MD3Triangle

==============================================================================
*/
struct MD3Triangle {
	int		indices[3];
};

const int MD3_FRAME_SIZE = 56;
const float MD3_MODEL_SCALE = 1.0f/2048.0f;

/*
================
Model::ImportMD3
================
*/
Model *Model::ImportMD3( const char *filename ) {
	if ( modelFS == NULL )
		return NULL;

	File *file = modelFS->OpenRead( filename );
	if( !file ) {
		User::Warning( TS("Can't open file: $*!" ) << filename );
		return NULL;
	}
	
	Model *model = new Model(false);
	try {
		MD3Header header;
		file->Read( &header, sizeof(MD3Header) );

		if ( strncmp( header.fileId, "IDP3", 4 ) != 0 || header.version != 15 )
			throw FileReadWriteError("Not a MD3 file");

		// Init
		if ( header.numTags > 0 ) {
			model->bones.SetGranularity( header.numTags );
			model->bones.CheckSize( header.numTags );
		}
		if ( header.numSurfaces < 0 )
			throw FileReadWriteError("Zero meshes");

		model->meshes.SetGranularity( header.numSurfaces );
		model->meshes.CheckSize( header.numSurfaces );
		
		// Skip frames, don't need them..
		file->Seek( MD3_FRAME_SIZE* header.numFrames, SEEK_CUR );

		int		i, j, k;
		Mat3	matrix;
		char	name[MAX_QPATH];

		// Read Tags (Joints)
		for( i=0; i<header.numTags; i++ ) {
			Bone &bone = model->bones.Alloc();
			file->Read( name, MAX_QPATH );
			bone.name = name;
			bone.idParent = -1;
			bone.flags = 0;
			file->ReadFloatArray( &bone.origin.x, 3 );
			file->ReadFloatArray( &matrix[0].x, 9 );
			bone.quat = matrix.ToQuat();
		}

		// Skip the rest of the tags
		if ( header.numFrames > 1 )
			file->Seek( sizeof(MD3Tag) * (header.numFrames-1) * header.numTags, SEEK_CUR );

		DynBuffer<MD3Triangle> md3_Triangles;
		DynBuffer<Vec2> md3_TexCoords;
		DynBuffer<MD3Vertex> md3_Vertices;

		MD3Surface surface;
		float lat, lng;

		long surfaceOffset = file->Tell();
		for ( i = 0; i < header.numSurfaces; i++ ) {
			// Read Surface Header
			file->Seek( surfaceOffset, SEEK_SET );
			file->Read( surface.surfId, 4 );
			file->Read( surface.name, MAX_QPATH );
			file->ReadIntArray( &surface.flags, 10 );

			// Read Shaders (Materials)
			file->Seek( surfaceOffset + surface.offsetShaders, SEEK_SET );
			if ( surface.numShaders >= 1 )
				file->Read( name, MAX_QPATH );

			// Read Triangles
			md3_Triangles.CheckSize(surface.numTriangles);
			file->Seek( surfaceOffset + surface.offsetTriangles, SEEK_SET );
			file->ReadIntArray( &md3_Triangles.data[0].indices[0], surface.numTriangles * 3 );

			// Read TexCoords
			md3_TexCoords.CheckSize(surface.numVerts);
			file->Seek( surfaceOffset + surface.offsetST, SEEK_SET );
			file->ReadFloatArray( &md3_TexCoords.data[0].x, 2 * surface.numVerts );

			// Read Vertices
			md3_Vertices.CheckSize(surface.numVerts);
			file->Seek( surfaceOffset + surface.offsetVerts, SEEK_SET );
			file->ReadShortArray( &md3_Vertices.data[0].x, 4 * surface.numVerts );

			// Store Mesh Info
			MeshStatic *mesh = new MeshStatic;
			model->meshes.Append(mesh);
			mesh->name = surface.name;
			if ( surface.numShaders >0 )
				mesh->material = name;

			InitVertices( mesh, surface.numVerts );
			InitIndices( mesh, surface.numTriangles * 3 );

			// Convert Vertices
			for( j=0; j<surface.numVerts; j++ ) {
				mesh->texCoords[j] = md3_TexCoords.data[j];

				mesh->vertices[j].Set( md3_Vertices.data[j].x, md3_Vertices.data[j].y, md3_Vertices.data[j].z );
				mesh->vertices[j] *= MD3_MODEL_SCALE;

				// decode the normal
				lat = static_cast<float>(( md3_Vertices.data[j].normal >> 8 ) & 0xff) * Math::TWO_PI/255.0f;
				lng = static_cast<float>(( md3_Vertices.data[j].normal & 0xff )) * Math::TWO_PI/255.0f;
				mesh->normals[j].Set( Math::Cos(lat) * Math::Sin(lng), Math::Sin(lat) * Math::Sin(lng), Math::Cos(lng) );
				mesh->normals[j].Normalize();
			}

			// Convert Indices
			for( j=0, k=0; j<surface.numTriangles; j++ ) {
				mesh->indices[k++] = md3_Triangles.data[j].indices[0];
				mesh->indices[k++] = md3_Triangles.data[j].indices[1];
				mesh->indices[k++] = md3_Triangles.data[j].indices[2];
			}

			surfaceOffset += surface.offsetEnd;
		}

		file->Close();
		return model;
	}
	catch( FileReadWriteError err ) {
		file->Close();
		delete model;
		User::Error( ERR_FILE_CORRUPT, TS("MD3: $*" ) << err.ToString(), filename );
		return NULL;
	}
}

}
