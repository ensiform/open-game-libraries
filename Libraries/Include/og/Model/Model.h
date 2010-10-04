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

#ifndef __OG_MODEL_H__
#define __OG_MODEL_H__

#include <og/Common/Common.h>

//! Open Game Libraries
namespace og {
	//! @defgroup Model Model (Library)
	//! You need to include <og/Model/Model.h>
	//! @see	UserCallbacksTodo List:
	//! @todo	Add Skeletal Animation
	//! @{

	// ==============================================================================
	//! Vertex Weight
	//!
	//! Holds one vertex weight
	// ==============================================================================
	struct VertexWeight {
		int		boneId;			//!< Identifier for the bone
		Vec3	origin;			//!< The weighted origin
		Vec3	normal;			//!< The weighted normal
		float	influence;		//!< The influence of this weight
	};

	// ==============================================================================
	//! Vertex
	//!
	//! Holds a list of weights
	// ==============================================================================
	struct Vertex {
		// ==============================================================================
		//! Constructor. 
		// ==============================================================================
		Vertex() : numWeights(0), weights(NULL) {}

		// ==============================================================================
		//! Destructor
		// ==============================================================================
		~Vertex();

		uInt			numWeights;	//!< Number of weights
		VertexWeight	*weights;	//!< List of weights
	};

	// ==============================================================================
	//! Bone
	//!
	//! Holds bone information
	// ==============================================================================
	struct Bone {
		String		name;		//!< Name
		int			idParent;	//!< Parent Id
		int			flags;		//!< Bone flags set in the file
		Vec3		origin;		//!< Relative offset from parent
		Quat		quat;		//!< Unit Quaternion
	};


	// ==============================================================================
	//! Mesh
	//!
	//! A 3D object, one model contains at least one. A Mesh can be static or animated,
	//! but within one model there are only meshes of the same type.
	//!
	//! @todo	should be uInt
	// ==============================================================================
	class Mesh {
	public:
		// ==============================================================================
		//! Constructor. 
		// ==============================================================================
		Mesh() : flags(0), detailLevel(0), numVerts(0), numIndices(0), texCoords(NULL), indices(NULL) {}

		// ==============================================================================
		//! Virtual Destructor
		// ==============================================================================
		virtual ~Mesh();

		int		flags;			//!< Flags set in the file
		int		detailLevel;	//!< Use for Level Of Detail
		int		numVerts;		//!< Number of vertices
		int		numIndices;		//!< Number of indices
		String	name;			//!< Mesh name
		String	material;		//!< Material / Texture to use
		Vec2 *	texCoords;		//!< List of texture coordinates
		int *	indices;		//!< List of indices
	};

	// ==============================================================================
	//! Static Mesh
	//!
	//! Static meshes have the vertices and normals preloaded in an array
	// ==============================================================================
	class MeshStatic : public Mesh {
	public:
		// ==============================================================================
		//! Constructor. 
		// ==============================================================================
		MeshStatic() : vertices(NULL), normals(NULL) {}

		// ==============================================================================
		//! Destructor
		// ==============================================================================
		~MeshStatic();

		Vec3 *	vertices;	//!< List of vertices
		Vec3 *	normals;	//!< List of normals
	};

	// ==============================================================================
	//! Animated Mesh
	//!
	//! Animated meshes use Vertex objects to calculate origin an normals of the final vertex position
	//!
	//! @see	Vertex
	// ==============================================================================
	class MeshAnimated : public Mesh {
	public:
		// ==============================================================================
		//! Destructor
		// ==============================================================================
		~MeshAnimated();

		Vertex *vertices;	//!< List of vertices
	};

	// ==============================================================================
	//! Model
	//!
	//! Game Model data
	// ==============================================================================
	class Model {
	public:
	// Static Interface
	//! @addtogroup Model
	//! @{

		// ==============================================================================
		//! Loads from a .gmd file
		//!
		//! @param	filename	The (relative) file path
		//!
		//! @return	true if it succeeds, false if it fails
		// ==============================================================================
		static Model *	Load( const char *filename );

		// ==============================================================================
		//! Store the file in .gmd file format
		//!
		//! @param	filename	The (relative) file path
		//!
		//! @return	true if it succeeds, false if it fails
		// ==============================================================================
		bool	Save( Model *mdl, const char *filename );

		// ==============================================================================
		//! Import from ASE (static model)
		//!
		//! @param	filename	The (relative) file path
		//!
		//! @return	true if it succeeds, false if it fails
		// ==============================================================================
		static Model *	ImportASE( const char *filename );

		// ==============================================================================
		//! Import from MD3 (static model, or first frame only)
		//!
		//! @param	filename	The (relative) file path
		//!
		//! @return	true if it succeeds, false if it fails
		// ==============================================================================
		static Model *	ImportMD3( const char *filename );

		// ==============================================================================
		//! Import from MD5Mesh (needs an extra animation file to build the skeleton)
		//!
		//! @param	filename	The (relative) file path
		//!
		//! @return	true if it succeeds, false if it fails
		// ==============================================================================
		static Model *	ImportMD5( const char *filename, const char *filenameAnim );

		// ==============================================================================
		//! Import from SKM (needs an extra animation file to build the skeleton)
		//!
		//! @param	filename	The (relative) file path
		//!
		//! @return	true if it succeeds, false if it fails
		// ==============================================================================
		static Model *	ImportSKM( const char *filename, const char *filenameAnim );

		// ==============================================================================
		//! Import from SMD
		//!
		//! @param	filename	The (relative) file path
		//!
		//! @return	true if it succeeds, false if it fails
		// ==============================================================================
		static Model *	ImportSMD( const char *filename );
	//! @}

	// Object Interface
		// ==============================================================================
		//! Constructor
		//!
		//! @param	isAnimated	true if this will be an animated model
		// ==============================================================================
		Model( bool isAnimated ) : animated( isAnimated ) {}

		// ==============================================================================
		//! Virtual Destructor
		// ==============================================================================
		virtual ~Model();

	public:
		List<Bone>		bones;		//!< The base skeleton
		List<Mesh *>	meshes;		//!< All available meshes
		bool			animated;	//!< true if this model can be animated
	};

	// ==============================================================================
	//! Model Animation Frame
	//!
	//! Holds a list of changed bones
	// ==============================================================================
	struct ModelFrame {
		List<int>	boneIds;
		List<Bone>	changedBones;
	};

	// ==============================================================================
	//! ModelAnimation
	// ==============================================================================
	class ModelAnimation {
	public:
	};
}

#endif
