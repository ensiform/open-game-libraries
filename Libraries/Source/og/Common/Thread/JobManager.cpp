/*
===========================================================================
The Open Game Libraries.
Copyright (C) 2007-2010 Lusito Software

Author:  Santo Pfingsten (TTK-Bandit)
Purpose: Job Manager
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

#include <og/Common/Thread/JobManager.h>

namespace og {
/*
==============================================================================

  WorkerThread

==============================================================================
*/

/*
================
WorkerThread::Run
================
*/
void WorkerThread::Run( void ) {
	wakeUpEvent.Lock();
	while( keepRunning ) {
		wakeUpEvent.Wait();
		if ( job ) {
			switch( job->Execute() ) {
				case JOB_DONE: break;
				case JOB_REPEAT:
					manager->AddJob( job );
					break;
				case JOB_DELETE:
					delete job;
					break;
			}
			job = NULL;
		}
		manager->WorkerIsDone( this );
	}
	wakeUpEvent.Unlock();
}


/*
==============================================================================

  JobManager

==============================================================================
*/

/*
================
JobManager::JobManager
================
*/
JobManager::~JobManager() {
	WaitForDone();
	listMutex.lock();
	int num = allThreads.Num();
	for( int i=0; i<num; i++ )
		allThreads[i]->Stop(true);
	allThreads.Clear();
	listMutex.unlock();
}

/*
================
JobManager::AddJob
================
*/
void JobManager::AddJob( Job *job ) {
	if ( waitForDone )
		job->Cancel();
	else {
		jobList.Produce( job );
		TriggerNextJob();
	}
}

/*
================
JobManager::SetNumWorkers
================
*/
void JobManager::SetNumWorkers( int num ) {
	listMutex.lock();
	if ( num != numThreadsWanted ) {
		numThreadsWanted = num;
		while( numThreadsWanted < allThreads.Num() ) {
			WorkerThread *worker = new WorkerThread(this);
			allThreads.Append( worker );
			AddFreeWorker( worker );
		}
	}
	listMutex.unlock();
}

/*
================
JobManager::WaitForDone
================
*/
void JobManager::WaitForDone( void ) {
	listMutex.lock();
	if ( allThreads.IsEmpty() )
		return;
	listMutex.unlock();

	// do not allow adding new jobs while waiting for done
	waitForDone = true;
	doneWaiter.Lock();

	// create a job and wait for it to be done
	jobList.Produce( new JobStopWaitDone(this) );
	TriggerNextJob();

	while( waitForDone )
		doneWaiter.Wait();
	doneWaiter.Unlock();
}

/*
================
JobManager::TriggerNextJob
================
*/
void JobManager::TriggerNextJob( void ) {
	WorkerThread *worker = availableThreads.Consume();
	if ( worker ) {
		Job *job = jobList.Consume();
		if( job != NULL )
			worker->RunJob( job );
		else
			availableThreads.Produce( worker );
	}
}

/*
================
JobManager::WorkerIsDone
================
*/
void JobManager::WorkerIsDone( WorkerThread *worker ) {
	listMutex.lock();
	if ( numThreadsWanted >= allThreads.Num() )
		AddFreeWorker( worker );
	else {
		worker->Stop( false );
		int index = allThreads.Find( worker );
		if ( index )
			allThreads.Remove( index );
	}
	listMutex.unlock();
}

/*
================
JobManager::AddFreeWorker
================
*/
void JobManager::AddFreeWorker( WorkerThread *worker ) {
	availableThreads.Produce( worker );
	TriggerNextJob();
}

}
