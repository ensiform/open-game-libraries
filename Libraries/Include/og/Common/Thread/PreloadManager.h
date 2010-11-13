// ==============================================================================
//! @file
//! @brief	Preload Manager
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

#ifndef __OG_PRELOAD_MANAGER_H__
#define __OG_PRELOAD_MANAGER_H__

#include <og/Common/Thread/JobManager.h>

//! Open Game Libraries
namespace og {
//! @defgroup Common Common (Library)
//! @{

	// ==============================================================================
	//! PreloadTask
	// ==============================================================================
	class PreloadTask {
	public:
		// ==============================================================================
		//! Default constructor
		// ==============================================================================
		PreloadTask() : isLoaded(false) {}

		// ==============================================================================
		//! Virtual destructor
		// ==============================================================================
		virtual ~PreloadTask() {}

		// ==============================================================================
		//! Load/Generate data in a separate thread before it gets merged into the scene
		//!
		//! @return	true on success, false otherwise
		// ==============================================================================
		virtual bool	Preload( void ) = 0;

		// ==============================================================================
		//! Merge the data into the scene ( within the call to PreloadManager::Synchronize )
		//!
		//! @note	This is only called if Preload returned true
		// ==============================================================================
		virtual void	Synchronize( void ) = 0;

	private:
		friend class PreloadJob;
		friend class PreloadManager;

		bool	isLoaded;	//!< The return value of Preload()
	};

	// ==============================================================================
	//! Background data loading/generation, threaded
	// ==============================================================================
	class PreloadManager {
	public:
		// ==============================================================================
		//! Default constructor
		// ==============================================================================
		PreloadManager();

		// ==============================================================================
		//! Start the background loader with one worker thread
		//!
		//! @param	num		The maximum number of preload tasks to be processed before the Synchronize
		// ==============================================================================
		void	Start( int num=16 );

		// ==============================================================================
		//! Kill all remaining preload tasks, stop the worker threads and reset the progess
		// ==============================================================================
		void	Stop( void );

		// ==============================================================================
		//! Set the number of worker threads
		//!
		//! @param	num		The new number of worker threads
		// ==============================================================================
		void	SetNumWorkers( int num );

		// ==============================================================================
		//! Add a preload task
		//!
		//! @param	task	The preload task to add
		// ==============================================================================
		void	AddTask( PreloadTask *task );

		// ==============================================================================
		//! Synchronize the processed preload tasks
		//!
		//! @return	The current progress in percent
		// ==============================================================================
		float	Synchronize( void );

		// ==============================================================================
		//! Get the current progress in percent
		//!
		//! @return	The current progress in percent
		// ==============================================================================
		float	GetProgress( void ) const { return progress; }

		// ==============================================================================
		//! Find out if all preload tasks have been processed
		//!
		//! @return	true if done, false if not
		// ==============================================================================
		bool	IsDone( void ) const { return numProcessed == numTotal; }

		// ==============================================================================
		//! Reset the progress to 0%
		// ==============================================================================
		void	ResetProgress( void );

	private:
		friend class PreloadJob;

		JobManager	manager;			//!< The manager
		float		progress;			//!< The current progress
		int			maxPreload;			//!< The maximum number of preload tasks to be processed before the Synchronize
		Queue<PreloadTask *> preloaded;	//!< The preload tasks that have been processed
		Condition	condition;			//!< The condition
		int			numProcessed;		//!< Number of processed preload tasks
		int			numTotal;			//!< Total number of preload tasks
	};

//! @}
}

#endif
