/*
===========================================================================
The Open Game Libraries.
Copyright (C) 2007-2010 Lusito Software

Author:  Santo Pfingsten (TTK-Bandit)
Purpose: Preload Manager
-----------------------------------------

This software is provided 'as-is', without any express or implied
warranty. In no event will the authors be held liable for any damages
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

#include <og/Common/Thread/PreloadManager.h>

namespace og {

/*
==============================================================================

  PreloadJob

==============================================================================
*/
class PreloadJob : public Job {
public:
	PreloadJob( PreloadManager *mgr, PreloadTask *tsk )
		:manager(mgr), task(tsk) {
	}
	JobResult	Execute( void ) {
		manager->condition.Lock();
		if ( manager->preloaded.Num() >= manager->maxPreload )
			manager->condition.Wait();
		manager->condition.Unlock();

		task->isLoaded = task->Preload();

		manager->condition.Lock();
		manager->preloaded.Push( task );
		manager->condition.Unlock();
		return JOB_DELETE;
	}
	JobResult	Cancel( void ) {
		OG_ASSERT( false );
		return JOB_DELETE;
	}

private:
	PreloadManager *	manager;
	PreloadTask *		task;
};


/*
==============================================================================

  PreloadManager

==============================================================================
*/
/*
================
PreloadManager::PreloadManager
================
*/
PreloadManager::PreloadManager() {
	progress = 0.0f;
	maxPreload = 1;
	numProcessed = 0;
	numTotal = 0;
}

/*
================
PreloadManager::Start
================
*/
void PreloadManager::Start( int max ) {
	manager.SetNumWorkers(1);
	maxPreload = Max( max, 1 );
}

/*
================
PreloadManager::Stop
================
*/
void PreloadManager::Stop( void ) {
	manager.KillAll();
	manager.SetNumWorkers(0, true);
	ResetProgress();
}

/*
================
PreloadManager::SetNumWorkers
================
*/
void PreloadManager::SetNumWorkers( int num ) {
	manager.SetNumWorkers( num );
}

/*
================
PreloadManager::AddTask
================
*/
void PreloadManager::AddTask( PreloadTask *task ) {
	numTotal++;
	manager.AddJob( new PreloadJob(this, task) );
}

/*
================
PreloadManager::Synchronize
================
*/
float PreloadManager::Synchronize( void ) {
	condition.Lock();
	PreloadTask *task;
	int num = preloaded.Num();
	while( !preloaded.IsEmpty() ) {
		task = preloaded.Front();
		if ( task->isLoaded )
			task->Synchronize();
		delete task;
		preloaded.Pop();
		numProcessed++;
	}
	if ( numTotal == 0 )
		progress = 0.0f;
	else
		progress = static_cast<float>(numProcessed) / static_cast<float>(numTotal);
	condition.Unlock();
	condition.Signal();

	return progress;
}

/*
================
PreloadManager::ResetProgress
================
*/
void PreloadManager::ResetProgress( void ) {
	// fixme: lock
	progress = 0.0f;
	numTotal -= numProcessed;
	numProcessed = 0;
}

}
