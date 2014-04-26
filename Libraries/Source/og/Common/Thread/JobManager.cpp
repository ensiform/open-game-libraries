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
		if ( !job )
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
			job = OG_NULL;
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
	SetNumWorkers(0, true);
	KillAll();
}

/*
================
JobManager::AddJob
================
*/
void JobManager::AddJob( Job *job ) {
	if ( waitForDone ) {
		if ( job->Cancel() == JOB_DELETE )
			delete job;
	} else {
		jobList.Produce( job );
		TriggerNextJob();
	}
}

/*
================
JobManager::SetNumWorkers
================
*/
void JobManager::SetNumWorkers( int num, bool blocking ) {
	listMutex.lock();
	if ( num != numThreadsWanted ) {
		if ( num > allThreads.Num() ) {
			numThreadsWanted = num;
			while( num > allThreads.Num() ) {
				WorkerThread *worker = new WorkerThread(this);
				worker->Start("Job Manager");
				allThreads.Append( worker );
				AddFreeWorker( worker );
			}
		} else if ( blocking ) {
			numThreadsWanted = -1;
			for( int i=allThreads.Num()-1; i >= num; i-- ) {
				listMutex.unlock();
				allThreads[i]->Stop( true );
				listMutex.lock();
				allThreads.Remove(i);
			}
			numThreadsWanted = num;
		}
		else
			numThreadsWanted = num;
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
JobManager::KillAll
================
*/
void JobManager::KillAll( void ) {
	Job *job;
	while( (job=jobList.Consume()) != OG_NULL ) {
		if ( job->Cancel() == JOB_DELETE )
			delete job;
	}
}

/*
================
JobManager::TriggerNextJob
================
*/
void JobManager::TriggerNextJob( bool wakeUp ) {
	WorkerThread *worker = availableThreads.Consume();
	if ( worker ) {
		Job *job = jobList.Consume();
		if( job == OG_NULL )
			availableThreads.Produce( worker );
		else {
			worker->SetJob( job );
			if ( wakeUp )
				worker->WakeUp();
		}
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
		AddFreeWorker( worker, false );
	else if ( numThreadsWanted != -1 ) {
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
void JobManager::AddFreeWorker( WorkerThread *worker, bool wakeUp ) {
	availableThreads.Produce( worker );
	TriggerNextJob( wakeUp );
}

}
