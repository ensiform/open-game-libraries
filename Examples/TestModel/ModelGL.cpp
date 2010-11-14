/*
===========================================================================
The Open Game Libraries.
Copyright (C) 2007-2010 Lusito Software

Author:  Santo Pfingsten (TTK-Bandit)
Purpose: OpenGL Model
-----------------------------------------

This software is provided 'as-is', without any express or implied
warranty.  In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not
   claim that you wrote the original software. If you use this software
   in a product, an acknowledgment in the product documentation would be
   appreciated but is not required.

2. Altered source versions must be plainly marked as such, and must not be
   misrepresented as being the original software.

3. This notice may not be removed or altered from any source distribution.
===========================================================================
*/


#include<gl/glew.h>
#include <og/Gloot/glmin.h>

#include <og/Console.h>
#include "ModelGL.h"

const char *validRenderTypes[] = { "Flat", "Textured", "TexturedTris", "Wireframe", "Textureframe", NULL };
og::CVar r_renderType( "r_renderType", "Textured", og::CVAR_GAME|og::CVAR_ENUM|og::CVAR_RENDERER, "How to render the model", validRenderTypes );
og::CVar r_drawJoints( "r_drawJoints", "1", og::CVAR_GAME|og::CVAR_INTEGER|og::CVAR_RENDERER, "Draw Joints", 0, 2 );
og::CVar r_drawSkeleton( "r_drawSkeleton", "1", og::CVAR_GAME|og::CVAR_BOOL|og::CVAR_RENDERER, "Draw Skeleton" );

#define BUFFER_OFFSET(i) ((char*)NULL + (i))


static int currentNumBones = 0;
static og::DynBuffer<og::Bone> currentSkeleton;

/*
==============================================================================

  ogMeshGL

==============================================================================
*/
/*
================
ogMeshGL::ogMeshGL
================
*/
ogMeshGL::ogMeshGL() {
	indexBuffer = 0;
	wireFrameIndexBuffer = 0;
	vertexBuffer = 0;
	dataSize = 0;
	ofsTexCoords = 0;
	ofsXYZ = 0;
	ofsNormals = 0;

	image		= NULL;
	model		= NULL;
	mesh		= NULL;
	vertices = NULL;
	normals = NULL;
}
/*
================
ogMeshGL::~ogMeshGL
================
*/
ogMeshGL::~ogMeshGL() {
	if ( image )
		og::Image::Free( image );
	delete[] wireFrameIndices;
	delete[] vertices;
	delete[] normals;
}

/*
================
ogMeshGL::Draw
================
*/
void ogMeshGL::Draw( void ) {
	if ( !vertexBuffer )
		return;

	int rtype = r_renderType.GetInt();
	if ( rtype > 0 )
		image->BindTexture();
	else
		glDisable(GL_TEXTURE_2D);
	glColor4f(1,1,1,1);

	glBindBufferARB(GL_ARRAY_BUFFER_ARB, vertexBuffer);
	glVertexPointer(3, GL_FLOAT, 0, BUFFER_OFFSET(ofsXYZ));
	glTexCoordPointer(2, GL_FLOAT, 0, BUFFER_OFFSET(ofsTexCoords));

	if ( rtype < 3 ) {
		glNormalPointer(GL_FLOAT, 0, BUFFER_OFFSET(ofsNormals));
		glBindBufferARB (GL_ELEMENT_ARRAY_BUFFER_ARB, indexBuffer);
		glDrawElements(GL_TRIANGLES, mesh->numIndices, GL_UNSIGNED_INT, 0);
	}

	// Draw Wireframe
	if ( rtype > 1  ) {
		glDisable(GL_LIGHTING);
		if ( rtype != 4 )
			glDisable(GL_TEXTURE_2D);
		glBindBufferARB (GL_ELEMENT_ARRAY_BUFFER_ARB, wireFrameIndexBuffer);
		glDrawElements(GL_LINES, mesh->numIndices*2, GL_UNSIGNED_INT, 0);
		if ( rtype != 4 )
			glEnable(GL_TEXTURE_2D);
		
		glEnable(GL_LIGHTING);
	}
	glBindBufferARB (GL_ARRAY_BUFFER_ARB, 0);
	if ( rtype == 0 )
		glEnable(GL_TEXTURE_2D);
}

/*
================
ogMeshGL::Init
================
*/
void ogMeshGL::Init( og::Model *mdl, og::Mesh *msh ) {
	model = mdl;
	mesh = msh;
	OG_ASSERT( vertexBuffer == 0 && indexBuffer == 0 && wireFrameIndexBuffer == 0 );
	image = og::Image::Find( mesh->material.c_str() );

	ofsTexCoords = 0;
	ofsXYZ = ofsTexCoords + sizeof(og::Vec2) * mesh->numVerts;
	ofsNormals = ofsXYZ + sizeof(og::Vec3) * mesh->numVerts;
	dataSize = ofsNormals + sizeof(og::Vec3) * mesh->numVerts;

	int j=0;
	wireFrameIndices = new uInt[mesh->numIndices*2];
	for( int i=0; i<mesh->numIndices; i+=3 ) {
		wireFrameIndices[j++] = mesh->indices[i];
		wireFrameIndices[j++] = mesh->indices[i+1];
		wireFrameIndices[j++] = mesh->indices[i+1];
		wireFrameIndices[j++] = mesh->indices[i+2];
		wireFrameIndices[j++] = mesh->indices[i+2];
		wireFrameIndices[j++] = mesh->indices[i];
	}

	// Buffer indices
	glGenBuffersARB( 1, &indexBuffer );
	glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, indexBuffer);
	glBufferDataARB(GL_ELEMENT_ARRAY_BUFFER_ARB, sizeof(int)*mesh->numIndices, mesh->indices, GL_STATIC_DRAW_ARB);

	glGenBuffersARB( 1, &wireFrameIndexBuffer );
	glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, wireFrameIndexBuffer);
	glBufferDataARB(GL_ELEMENT_ARRAY_BUFFER_ARB, sizeof(int)*mesh->numIndices*2, wireFrameIndices, GL_STATIC_DRAW_ARB);

	glGenBuffersARB( 1, &vertexBuffer );
	glBindBufferARB(GL_ARRAY_BUFFER_ARB, vertexBuffer);
	if ( model->animated ) {
		vertices = new og::Vec3[mesh->numVerts];
		normals = new og::Vec3[mesh->numVerts];
		glBufferDataARB(GL_ARRAY_BUFFER_ARB, dataSize, NULL, GL_DYNAMIC_DRAW_ARB);
		Update();
	} else {
		og::MeshStatic *meshEx = static_cast<og::MeshStatic *>(mesh);
		glBufferDataARB(GL_ARRAY_BUFFER_ARB, dataSize, NULL, GL_STATIC_DRAW_ARB);
		glBufferSubDataARB(GL_ARRAY_BUFFER_ARB, ofsXYZ, sizeof(og::Vec3) * mesh->numVerts, meshEx->vertices);
		glBufferSubDataARB(GL_ARRAY_BUFFER_ARB, ofsNormals, sizeof(og::Vec3) * mesh->numVerts, meshEx->normals);
	}
	glBufferSubDataARB(GL_ARRAY_BUFFER_ARB, ofsTexCoords, sizeof(og::Vec2) * mesh->numVerts, mesh->texCoords);
}

/*
================
ogMeshGL::Update
================
*/
void ogMeshGL::Update( void ) {
	if ( model->animated ) {
		og::MeshAnimated *meshEx = static_cast<og::MeshAnimated *>(mesh);
		OG_ASSERT( mesh->numVerts );

		if ( !vertexBuffer )
			return;

		for( int i=0; i<mesh->numVerts; i++ ) {
			vertices[i].Zero();
			og::Vertex &vInfo = meshEx->vertices[i];
			for ( int j=0; j<vInfo.numWeights; j++ ) {
				const og::VertexWeight &weight = vInfo.weights[j];

				if ( weight.boneId == -1 )
					vertices[i] += weight.origin;
				else {
					if ( weight.boneId >= model->bones.Num() ) {
						og::User::Warning("boneId out of Range!");
						return;
					}
					const og::Bone &bone = currentSkeleton.data[weight.boneId];
					vertices[i] += ((bone.quat * weight.origin) + bone.origin) * weight.influence;
				}
			}
		}

		glBindBufferARB(GL_ARRAY_BUFFER_ARB, vertexBuffer);
		glBufferSubDataARB(GL_ARRAY_BUFFER_ARB, ofsXYZ, sizeof(og::Vec3) * mesh->numVerts, vertices);
		glBufferSubDataARB(GL_ARRAY_BUFFER_ARB, ofsNormals, sizeof(og::Vec3) * mesh->numVerts, normals);
	}
}

/*
==============================================================================

  ogModelGL

==============================================================================
*/
/*
================
ogModelGL::Load
================
*/
bool ogModelGL::Load( const char *filename ) {
	og::SafeDelete( model );

	og::String ext = og::String::GetFileExtension(filename, og::String::ByteLength(filename));
	if ( ext.Icmp("gmd") == 0 )
		model = og::Model::Load(filename);
	else if ( ext.Icmp("ase") == 0 )
		model = og::Model::ImportASE(filename);
	else if ( ext.Icmp("md3") == 0 )
		model = og::Model::ImportMD3(filename);

	else {
		og::String fnAnim = filename;
		if ( ext.Icmp("md5mesh") == 0 ) {
			fnAnim.SetFileExtension("md5anim");
			model = og::Model::ImportMD5(filename, fnAnim.c_str());
		} else if ( ext.Icmp("skm") == 0 ) {
			fnAnim.SetFileExtension("skp");
			model = og::Model::ImportSKM(filename, fnAnim.c_str());
		}
	}
	if ( !model )
		return false;

	if ( model->animated )
		BuildSkeleton(false);
	int numMeshes = model->meshes.Num();
	for( int i=0; i<numMeshes; i++ )
		meshes.Alloc().Init( model, model->meshes[i] );
	return true;
}

/*
================
ogModelGL::Draw
================
*/
void ogModelGL::Draw( void ) {
	if ( model->animated )
		BuildSkeleton( true );
	glPushMatrix();
	glScalef( scale.x, scale.y, scale.z );
	glTranslatef( offset.x, offset.y, offset.z );
	glRotatef( angles.roll, 1, 0, 0 );
	glRotatef( angles.pitch, 0, 1, 0 );
	glRotatef( angles.yaw, 0, 0, 1 );

	int num = meshes.Num();
	for( int i=0; i<num; i++ )
		meshes[i].Draw();

	int drawJoints = r_drawJoints.GetInt();
	int numBones = model->bones.Num();
	if ( drawJoints == 2 || ( drawJoints == 1 && numBones > 1 ) ) {
		glDisable(GL_TEXTURE_2D);
		glDisable( GL_DEPTH_TEST );

		glPointSize (5.0f);
		glColor3f (1.0f, 0.0f, 0.0f);
		glBegin (GL_POINTS);
			for( int i=0; i<numBones; i++ )
				glVertex3fv( &currentSkeleton.data[i].origin.x );
		glEnd ();
		
		glEnable(GL_TEXTURE_2D);
		glEnable( GL_DEPTH_TEST );
	}
	if ( r_drawSkeleton.GetBool() ) {
		glDisable(GL_TEXTURE_2D);
		glDisable( GL_DEPTH_TEST );

		glLineWidth( 2.0f );
		glBegin( GL_LINES );
			for( int i=0; i<numBones; i++ ) {
				if ( model->bones[i].idParent >= 0 ) {
					glColor3f( 0.7f, 0.8f, 0.9f );
					glVertex3fv( &currentSkeleton.data[model->bones[i].idParent].origin.x );
					glColor3f( 0.0f, 0.0f, 1.0f );
					glVertex3fv( &currentSkeleton.data[i].origin.x );
				}
			}
		glEnd();

		glEnable(GL_TEXTURE_2D);
		glEnable( GL_DEPTH_TEST );
	}
	glPopMatrix();
}

/*
================
ogModelGL::ForceMaterial
================
*/
void ogModelGL::ForceMaterial( int index, const char *material ) {
	meshes[index].image = og::Image::Find( material );
}

/*
================
ogModelGL::BuildSkeleton
================
*/
void ogModelGL::BuildSkeleton( bool updateMeshes ) {
	currentNumBones = model->bones.Num();
	currentSkeleton.CheckSize( currentNumBones );
	int parentId;
	for( int i=0; i<currentNumBones; i++ ) {
		og::Bone &bone = currentSkeleton.data[i];
		bone.origin = model->bones[i].origin;
		bone.quat = model->bones[i].quat;

		parentId = model->bones[i].idParent;
		if( parentId >= 0 ) {
			bone.quat = currentSkeleton.data[parentId].quat * bone.quat;
			bone.origin = currentSkeleton.data[parentId].quat * bone.origin;
			bone.origin += currentSkeleton.data[parentId].origin;
		}
	}
	if ( updateMeshes ) {
		int num = meshes.Num();
		for ( int i = 0; i < num; i++ )
			meshes[i].Update();
	}
}
