// ==============================================================================
//! @file
//! @brief	Model Loader
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

const int GMD_VERSION = 1;	//!< The model file version
const int GMA_VERSION = 1;	//!< The animation file version

FileSystemCore *modelFS = NULL;

/*
================
FileReadString
================
*/
int FileReadString( File *f, String &str ) {
	uShort len = f->ReadUshort();
	if ( len > OG_STR_FILE_MAX_BYTES )
		len = OG_STR_FILE_MAX_BYTES;
	DynBuffer<char> in(len+1);
	f->Read(in.data, len);
	in.data[len] = '\0';
	str = in.data;
	return len;
}
/*
================
FileWriteString
================
*/
int FileWriteString( File *f, const char *string, int len=-1 ) {
	if ( len == -1 )
		len = String::Length( string );

	if ( len > OG_STR_FILE_MAX_BYTES )
		len = OG_STR_FILE_MAX_BYTES;

	f->WriteUshort(static_cast<uShort>(len));
	f->Write(string, len);
	return len;
}

/*
==============================================================================

  Vertex

==============================================================================
*/
/*
================
Vertex::~Vertex
================
*/
Vertex::~Vertex() {
	if ( weights )
		delete[] weights;
}

/*
==============================================================================

  Mesh

==============================================================================
*/
/*
================
Mesh::~Mesh
================
*/
Mesh::~Mesh() {
	if ( texCoords )
		delete[] texCoords;
	if ( indices )
		delete[] indices;
}

/*
==============================================================================

  MeshStatic

==============================================================================
*/
/*
================
MeshStatic::~MeshStatic
================
*/
MeshStatic::~MeshStatic() {
	if ( vertices )
		delete[] vertices;
	if ( normals )
		delete[] normals;
}

/*
==============================================================================

  MeshAnimated

==============================================================================
*/
/*
================
MeshAnimated::~MeshAnimated
================
*/
MeshAnimated::~MeshAnimated() {
	if ( vertices )
		delete[] vertices;
}

/*
==============================================================================

  Model

==============================================================================
*/
/*
================
Model::SetFileSystem
================
*/
void Model::SetFileSystem( FileSystemCore *fileSystem ) {
	Shared::Init();
	modelFS = fileSystem;
}

/*
================
Model::~Model
================
*/
Model::~Model() {
	int num = meshes.Num();
	for( int i=0; i<num; i++ )
		delete meshes[i];
	meshes.Clear();
	bones.Clear();
}

/*
================
Model::Load
================
*/
Model *Model::Load( const char *filename ) {
	if ( modelFS == NULL )
		return NULL;

	File *file = modelFS->OpenRead( filename );
	if ( !file ) {
		User::Warning( TS( "Can't open file: '$*'" ) << filename );
		return NULL;
	}

	Model *model = NULL;
	try {
		// Read Header
		char fileId[4];
		file->Read( fileId, 4 );
		if ( String::Cmp( fileId, "GMD" ) != 0 ) 
			throw FileReadWriteError( "Not a gmd file" );

		int version = file->ReadInt();
		if ( version != GMD_VERSION )
			throw FileReadWriteError( TS( "Wrong Version($*), should be ($*)'" ) << version << GMD_VERSION );

		String modelName, author, appName;
		FileReadString( file, modelName );
		FileReadString( file, author );
		FileReadString( file, appName );

		uInt numBones = file->ReadUint();
		uInt numMeshes = file->ReadUint();
		uInt numChunks = file->ReadUint();

		model = new Model(true); //! @todo	isAnimated should be read from file!!

		uInt size, entries;
		String name;
		for( int i=0; i<numChunks; i++ ) {
			FileReadString( file, name );
			size  = file->ReadUint();
			entries = file->ReadUint();
			if ( name.Icmp("Bones") == 0 ) {
				if ( entries != numBones )
					throw FileReadWriteError( TS( "numBones($*) does not match the chunks numEntries($*)" ) << numBones << entries );

				// Read Bones
				for ( int j=0; j<entries; j++ ) {
					Bone &bone = model->bones.Alloc();
					FileReadString( file, bone.name );
					bone.idParent = file->ReadInt();
					file->ReadFloatArray( &bone.origin.x, 3 );
					file->ReadFloatArray( &bone.quat.x, 4 );
				}
			} else if ( name.Icmp("Meshes") == 0 ) {
				if ( entries != numMeshes )
					throw FileReadWriteError( TS( "numMeshes($*) does not match the chunks numEntries($*)" ) << numMeshes << entries );

				// Read Meshes
				for ( int j=0; j<entries; j++ ) {
					MeshAnimated *mesh = new MeshAnimated; //! @todo non-animated
					model->meshes.Append(mesh);

					FileReadString( file, mesh->name );
					FileReadString( file, mesh->material );

					mesh->flags = file->ReadInt();
					mesh->detailLevel = file->ReadInt();

					InitVertices( mesh, file->ReadUint() );
					InitIndices( mesh, file->ReadUint() );

					// Read Indices
					file->ReadIntArray( mesh->indices, mesh->numIndices );

					// Read TexCoords
					file->ReadFloatArray( &mesh->texCoords[0].x, 2 * mesh->numVerts );

					// Read VertexInfos
					for( int k=0; k<mesh->numVerts; k++ ) {
						Vertex &vInfo = mesh->vertices[k];
						InitVertex( &vInfo, file->ReadUint() );
						for( int l=0; l<vInfo.numWeights; l++ ) {
							VertexWeight &weight = vInfo.weights[l];
							weight.boneId = file->ReadInt();
							file->ReadFloatArray( &weight.origin.x, 3 );
							file->ReadFloatArray( &weight.normal.x, 3 );
							weight.influence = file->ReadFloat();
						}
					}
				}
			} else {
				file->Seek( size, SEEK_CUR );
			}
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

/*
================
StartChunk
================
*/
OG_INLINE long StartChunk( File *file, const char *name, uInt entries ) {
	FileWriteString( file, name );
	long sizePos = file->Tell();
	file->WriteUint( 0 );
	file->WriteUint( entries );
	return sizePos;
}

/*
================
FinishChunk
================
*/
OG_INLINE void FinishChunk( File *file, long sizePos ) {
	long curPos = file->Tell();
	file->Seek( sizePos, SEEK_SET );
	file->WriteUint( curPos-sizePos-8 );
	file->Seek( curPos, SEEK_SET );
}

/*
================
Model::Save
================
*/
bool Model::Save( Model *model, const char *filename ) {
	if ( modelFS == NULL )
		return false;

	File *file = modelFS->OpenWrite( filename, false );
	if ( !file )
		return false;

	try {
		// Write header
		file->Write( "GMD", 4 );
		file->WriteInt( GMD_VERSION );
		
		//! @todo	guess what
		String modelName = filename;
		modelName.StripFileExtension();
		modelName.StripPath();
		FileWriteString( file, modelName.c_str() );
		FileWriteString( file, "Unknown" );
		FileWriteString( file, "ogTools" );

		uInt numBones = model->bones.Num();
		uInt numMeshes = model->meshes.Num();
		file->WriteUint( numBones );
		file->WriteUint( numMeshes );
		file->WriteUint( 2 ); // number of chunks we write

		// Write Bones
		long sizePos = StartChunk( file, "Bones", numBones );
		for( int i=0; i<numBones; i++ ) {
			const Bone &bone = model->bones[i];
			FileWriteString( file, bone.name.c_str() );
			file->WriteInt( bone.idParent );
			file->WriteFloatArray( &bone.origin.x, 3 );
			file->WriteFloatArray( &bone.quat.x, 4 );
		}
		FinishChunk( file, sizePos );

		// Write Meshes
		sizePos = StartChunk( file, "Meshes", numMeshes );
		for ( int i=0; i<numMeshes; i++ ) {
			const MeshAnimated *mesh = static_cast<MeshAnimated *>(model->meshes[i]);
			// Write Mesh Info
			FileWriteString( file, mesh->name.c_str() );
			FileWriteString( file, mesh->material.c_str() );
			file->WriteInt( mesh->flags );
			file->WriteInt( mesh->detailLevel );
			file->WriteUint( mesh->numVerts );
			file->WriteUint( mesh->numIndices );

			// Write Indices
			file->WriteIntArray( mesh->indices, mesh->numIndices );

			// Write TexCoords
			file->WriteFloatArray( &mesh->texCoords[0].x, 2 * mesh->numVerts );

			// Write VertexInfos
			for( int j=0; j<mesh->numVerts; j++ ) {
				Vertex &vInfo = mesh->vertices[j];
				file->WriteUint( vInfo.numWeights );
				for ( int k=0; k<vInfo.numWeights; k++ ) {
					const VertexWeight &weight = vInfo.weights[k];
					file->WriteInt( weight.boneId );
					file->WriteFloatArray( &weight.origin.x, 3 );
					file->WriteFloatArray( &weight.normal.x, 3 );
					file->WriteFloat( weight.influence );
				}
			}
		}
		FinishChunk( file, sizePos );
		file->Close();
		return true;
	}
	catch( FileReadWriteError err ) {
		file->Close();
		User::Error( ERR_FILE_WRITEFAIL, TS("GMD: $*" ) << err.ToString(), filename );
		return NULL;
	}
}

}
