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

#ifndef __OG_MODEL_GL_H__
#define __OG_MODEL_GL_H__

#include <og/Model/Model.h>
#include <og/Image/Image.h>

/*
==============================================================================

  ogMeshGL

==============================================================================
*/
class ogMeshGL {
public:
	ogMeshGL();
	~ogMeshGL();

	void		Init( og::Model *mdl, og::Mesh *msh );
	void		Update( void );
	void		Draw( void );

	// vbo
	uInt		indexBuffer;
	uInt		wireFrameIndexBuffer;
	uInt		vertexBuffer;
	int			dataSize;
	uInt		ofsTexCoords;
	uInt		ofsXYZ;
	uInt		ofsNormals;
	uInt		*wireFrameIndices;

	og::Vec3	*vertices;
	og::Vec3	*normals;

	og::Image	*image;
	og::Model	*model;
	og::Mesh	*mesh;
};

/*
==============================================================================

  ogModelGL

==============================================================================
*/
class ogModelGL {
public:
	ogModelGL() : scale(1.0f, 1.0f, 1.0f), model(NULL) {}
	~ogModelGL() { if ( model ) delete model;} //! @todo	should be freed in the model library

	bool		Load( const char *filename );
	void		Draw( void );
	void		ForceMaterial( int index, const char *material );
	void		BuildSkeleton( bool updateMeshes );

	// change model on the fly
	const og::Vec3&		GetScale( void ) { return scale; }
	void				SetScale( const og::Vec3 &scale ) { this->scale = scale; }
	const og::Vec3&		GetOffset( void ) { return offset; }
	void				SetOffset( const og::Vec3 &offset ) { this->offset = offset; }
	const og::Angles&	GetAngles( void ) { return angles; }
	void				SetAngles( const og::Angles &angles ) { this->angles = angles; }

private:
	og::Model	*model;
	og::ListEx<ogMeshGL> meshes;

	// Model Parameters for changing the mesh on the fly:
	og::Vec3	scale;
	og::Vec3	offset;
	og::Angles	angles;
};

#endif
