// ==============================================================================
//! @file
//! @brief	SKM (QFusion) Model Import
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
const float SKM_MODEL_SCALE = 0.05f;

#define SKMHEADER				"SKM1"

const int SKM_MAX_MESHES		= 32;
const int SKM_MAX_FRAMES		= 65536;
const int SKM_MAX_TRIS			= 65536;
const int SKM_MAX_VERTS			= (SKM_MAX_TRIS * 3);
const int SKM_MAX_BONES			= 256;
const int SKM_MAX_SHADERS		= 256;
const int SKM_MAX_FILESIZE		= 16777216;
const int SKM_MAX_ATTACHMENTS	= SKM_MAX_BONES;
const int SKM_MAX_LODS			= 4;

const int SKM_MODELTYPE			= 2;	// (hierarchical skeletal pose)

const int SKM_MAX_NAME			= 64;

/*
==============================================================================

  SKELETAL MODELS (in memory)

==============================================================================
*/

struct mskmesh_t {
	char			shadername[SKM_MAX_NAME];
	char			name[SKM_MAX_NAME];

	uInt			numverts;
	uInt			numtris;
	uInt			numreferences;
};

bool ReadBonesFromSKP( const char *filename, Model *model ) {
	if ( modelFS == NULL )
		return false;

	File *file = modelFS->OpenRead( filename );
	if( !file ) {
		delete model;
		return false;
	}
	try {
		// Read header
		char header_id[4];
		file->Read( header_id, 4 );
		if ( String::Icmpn( header_id, SKMHEADER, String::Length(SKMHEADER) ) != 0 )
			throw FileReadWriteError( "File Is not a skm File" );

		uInt type = file->ReadUint();
		uInt internalFilesize = file->ReadUint();
		uInt numbones = file->ReadUint();
		uInt numframes = file->ReadUint();
		uInt ofs_bones = file->ReadUint();
		uInt ofs_frames = file->ReadUint();

		if ( numframes <= 0 )
			throw FileReadWriteError("File has no frames");
		if( type != SKM_MODELTYPE )
			throw FileReadWriteError( Format("File has wrong type number ($* should be $*)\n" ) << type << SKM_MODELTYPE );
		if( numbones > SKM_MAX_BONES )
			throw FileReadWriteError("File has too many bones");
		if ( numframes > SKM_MAX_FRAMES )
			throw FileReadWriteError("File has too many frames");

		file->Seek( ofs_bones, SEEK_SET );

		model->bones.SetGranularity( numbones );
		model->bones.CheckSize( numbones );
		char name[SKM_MAX_NAME];
		for( int i = 0; i < numbones; i++ ) {
			file->Read( name, SKM_MAX_NAME );
			Bone &bone = model->bones.Alloc();
			bone.name = name ;
			bone.idParent = file->ReadInt();
			bone.flags = file->ReadUint();
		}

		file->Seek( ofs_frames, SEEK_SET );

		// Read first frame
		file->Seek( SKM_MAX_NAME, SEEK_CUR); // frame name, not used here
		file->Seek( file->ReadUint(), SEEK_SET ); // goto bone offset

		for( int i=0; i<numbones; i++ ) {
			Bone &bone = model->bones[i];
			file->ReadFloatArray( &bone.quat.x, 4 );
			file->ReadFloatArray( &bone.origin.x, 3 );
			bone.origin *= SKM_MODEL_SCALE;
		}

		file->Close();
		return true;
	}
	catch( FileReadWriteError &err ) {
		file->Close();
		delete model;
		User::Error( ERR_FILE_CORRUPT, Format("SKP: $*" ) << err.ToString(), filename );
		return false;
	}
}

/*
================
Model::ImportSKM
================
*/
Model *Model::ImportSKM( const char *filename, const char *filenameAnim ) {
	if ( modelFS == NULL )
		return NULL;

	File *file = modelFS->OpenRead( filename );
	if( !file )
		return NULL;

	Model *model = new Model(true);

	// Bones are all stored in skp files!
	if ( !ReadBonesFromSKP( filenameAnim, model ) )
		return NULL;

	try {
		// Read header
		char header_id[4];
		file->Read( header_id, 4 );
		if ( String::Icmpn( header_id, SKMHEADER, String::Length(SKMHEADER) ) != 0 )
			throw FileReadWriteError( "Not a skm file" );
		uInt type = file->ReadUint();
		uInt filesize = file->ReadUint();
		uInt numbones = file->ReadUint();
		uInt nummeshes = file->ReadUint();
		uInt ofs_meshes = file->ReadUint();

		if( type != SKM_MODELTYPE )
			throw FileReadWriteError( Format("File has wrong type number ($* should be $*)" ) << type << SKM_MODELTYPE );
		if( filesize > SKM_MAX_FILESIZE )
			throw FileReadWriteError( Format("File has has wrong filesize ($* should be less than $*)" ) << filesize << SKM_MAX_FILESIZE );

		if( nummeshes <= 0 )
			throw FileReadWriteError("File has no meshes");
		else if( nummeshes > SKM_MAX_MESHES )
			throw FileReadWriteError("File has too many meshes");

		if( numbones <= 0 )
			throw FileReadWriteError("File has no bones");
		else if( numbones > SKM_MAX_BONES )
			throw FileReadWriteError("File has too many bones");

		file->Seek( ofs_meshes, SEEK_SET );

		if ( numbones != model->bones.Num() )
			throw FileReadWriteError( Format("NumBones do not match : (SKP: $*, SKM: $*)\n" ) << model->bones.Num() << numbones );

		mskmesh_t skm_mesh;
		uInt ofs_verts, ofs_texcoords, ofs_indices, ofs_references;

		long lastPos;
		for( int i=0; i<nummeshes; i++ ) {
			// Read Mesh Header
			file->Read( skm_mesh.shadername, SKM_MAX_NAME);
			file->Read( skm_mesh.name, SKM_MAX_NAME);
			skm_mesh.numverts = file->ReadUint();
			skm_mesh.numtris = file->ReadUint();
			skm_mesh.numreferences = file->ReadUint();
			ofs_verts = file->ReadUint();	
			ofs_texcoords = file->ReadUint();
			ofs_indices = file->ReadUint();
			ofs_references = file->ReadUint();

			lastPos = file->Tell();

			// Print out some warnings
			if( skm_mesh.numverts <= 0 )
				throw FileReadWriteError( Format("mesh $* has no vertexes" ) << i );
			else if( skm_mesh.numverts > SKM_MAX_VERTS )
				throw FileReadWriteError( Format("mesh $* has too many vertexes" ) << i );

			if( skm_mesh.numtris <= 0 )
				throw FileReadWriteError( Format("mesh $* has no indices" ) << i );
			else if( skm_mesh.numtris > SKM_MAX_TRIS )
				throw FileReadWriteError( Format("mesh $* has too many indices" ) << i );

			if( skm_mesh.numreferences <= 0 )
				throw FileReadWriteError( Format("mesh $* has no bone references" ) << i );
			else if( skm_mesh.numreferences > SKM_MAX_BONES )
				throw FileReadWriteError( Format("mesh $* has too many bone references" ) << i );

			// Read Vertex Information
			file->Seek( ofs_verts, SEEK_SET );

			MeshAnimated *mesh = new MeshAnimated;
			model->meshes.Append(mesh);
			mesh->name = skm_mesh.name;
			mesh->material = skm_mesh.shadername;
			InitVertices( mesh, skm_mesh.numverts );
			for( int j = 0; j < skm_mesh.numverts; j++ ) {
				Vertex &vInfo = mesh->vertices[j];
				InitVertex( &vInfo, file->ReadUint() );

				for( int l = 0; l < vInfo.numWeights; l++ ) {
					VertexWeight &weight = vInfo.weights[l];
					file->ReadFloatArray( &weight.origin.x, 3 );
					weight.influence = file->ReadFloat();
					file->ReadFloatArray( &weight.normal.x, 3 );
					weight.boneId = file->ReadInt();
					weight.origin *= SKM_MODEL_SCALE;
				}
			}

			file->Seek( ofs_texcoords, SEEK_SET );

			file->ReadFloatArray( &mesh->texCoords[0].x, 2 * skm_mesh.numverts );

			file->Seek( ofs_indices, SEEK_SET );

			int num = skm_mesh.numtris * 3;
			InitIndices( mesh, num );
			file->ReadIntArray( mesh->indices, num );

			file->Seek( lastPos, SEEK_SET );
		}

		file->Close();
		return model;
	}
	catch( FileReadWriteError &err ) {
		file->Close();
		delete model;
		User::Error( ERR_FILE_CORRUPT, Format("SKM: $*" ) << err.ToString(), filename );
		return NULL;
	}
}

}
