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

#ifndef __OG_MODEL_EX_H__
#define __OG_MODEL_EX_H__

#include <og/Model/Model.h>

//! Open Game Libraries
namespace og {
	extern FileSystemCore *modelFS;

	OG_INLINE void InitIndices( Mesh *mesh, int numIndices ) {
		mesh->numIndices = numIndices;
		mesh->indices = new int[numIndices];
	}
	OG_INLINE void InitVertices( MeshStatic *mesh, int numVerts ) {
		mesh->numVerts = numVerts;
		mesh->texCoords	= new Vec2[numVerts];
		mesh->vertices = new Vec3[numVerts];
		mesh->normals = new Vec3[numVerts];
	}
	OG_INLINE void InitVertices( MeshAnimated *mesh, int numVerts ) {
		mesh->numVerts = numVerts;
		mesh->texCoords	= new Vec2[numVerts];
		mesh->vertices = new Vertex[numVerts];
	}
	OG_INLINE void InitVertex( Vertex *vert, int numWeights ) {
		vert->numWeights = numWeights;
		vert->weights = new VertexWeight[numWeights];
	}
}

#endif
