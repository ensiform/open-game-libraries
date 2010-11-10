// ==============================================================================
//! @file
//! @brief	Job Manager
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

#ifndef __OG_JOB_MANAGER_H__
#define __OG_JOB_MANAGER_H__

#include <og/Common.h>
#include <og/Common/Thread/LockFreeQueue.h>

//! Open Game Libraries
namespace og {
//! @defgroup Common Common (Library)
//! @{
	class JobManager;

	// ==============================================================================
	//! Return value of a job
	// ==============================================================================
	enum JobResult {
		JOB_DONE,	//!< Nothing happens ( user manually cleans the job )
		JOB_REPEAT,	//!< Job gets re-added to the end of the job-queue
		JOB_DELETE	//!< The job object will be deleted
	};

	// ==============================================================================
	//! A job to execute
	// ==============================================================================
	class Job {
	public:
		// ==============================================================================
		//! Virtual destructor
		// ==============================================================================
		virtual ~Job() {}

		// ==============================================================================
		//! Executes the job
		//!
		//! @return	JobResult: What to do next
		//! 
		//! @see JobResult
		// ==============================================================================
		virtual JobResult	Execute( void ) = 0;

		// ==============================================================================
		//! Called when a job is canceled due to the job manager stopping.
		//!
		//! @return	JobResult: Default is JOB_DELETE, all others won't do anything
		// ==============================================================================
		virtual JobResult	Cancel( void ) { return JOB_DELETE; }
	};

	// ==============================================================================
	//! A worker thread
	// ==============================================================================
	class WorkerThread : public Thread {
	public:
		// ==============================================================================
		//! Constructor
		//!
		//! @param	_manager	The manager of this worker
		// ==============================================================================
		WorkerThread( JobManager *_manager ) : job(NULL), manager(_manager) {}

		// ==============================================================================
		//! Starts the job
		//!
		//! @param	_job	The job
		// ==============================================================================
		void RunJob( Job *_job ) { job = _job; WakeUp(); }

	private:
		Job *		job;		//!< The job
		JobManager *manager;	//!< The manager

	protected:
		// ==============================================================================
		//! The thread main function
		// ==============================================================================
		void Run( void );
	};

	// ==============================================================================
	//! A job manager
	// ==============================================================================
	class JobManager {
	public:
		// ==============================================================================
		//! Default constructor
		// ==============================================================================
		JobManager() : numThreadsWanted(0), waitForDone(false) {}

		// ==============================================================================
		//! Destructor
		// ==============================================================================
		~JobManager();

		// ==============================================================================
		//! Add a job to the queue
		//!
		//! @param	job	The job to add
		// ==============================================================================
		void	AddJob( Job *job );

		// ==============================================================================
		//! Change the number workers
		//!
		//! @param	num	Number of workers
		// ==============================================================================
		void	SetNumWorkers( int num );

		// ==============================================================================
		//! Wait for all jobs to be done
		// ==============================================================================
		void	WaitForDone( void );

		// ==============================================================================
		//! Kill all remaining jobs
		// ==============================================================================
		void	KillAll( void );

	private:
		friend class WorkerThread;

		// ==============================================================================
		//! Trigger the next job
		// ==============================================================================
		void	TriggerNextJob( void );

		// ==============================================================================
		//! A worker is done
		//!
		//! @param	worker	The worker
		// ==============================================================================
		void	WorkerIsDone( WorkerThread *worker );

		// ==============================================================================
		//! Add a free worker
		//!
		//! @param	worker	The worker. 
		// ==============================================================================
		void	AddFreeWorker( WorkerThread *worker );

		ogst::mutex					listMutex;			//!< Protects numThreadsWanted and allThreads
		int							numThreadsWanted;	//!< The number of wanted threads wanted
		List<WorkerThread *>		allThreads;			//!< A list of all worker threads
		bool						waitForDone;		//!< Waiting to be done ( see WaitForDone )
		Condition					doneWaiter;			//!< The done waiter condition
		LowLockQueue<WorkerThread>	availableThreads;	//!< The available (job-less) threads
		LowLockQueue<Job>			jobList;			//!< The list of jobs to do

		// ==============================================================================
		//! The job that ends the WaitForDone()
		// ==============================================================================
		class JobStopWaitDone : public Job {
		public:
			// ==============================================================================
			//! Constructor
			//!
			//! @param	mgr	The manager
			// ==============================================================================
			JobStopWaitDone( JobManager *mgr ) : manager(mgr) {}

			// ==============================================================================
			//! Stop waiting for done and signal the waiter
			//!
			//! @return	JOB_DELETE
			// ==============================================================================
			JobResult	Execute( void ) {
				manager->waitForDone = false;
				manager->doneWaiter.Signal();
				return JOB_DELETE;
			}

			// ==============================================================================
			//! Cancel should never be called on this one
			// ==============================================================================
			JobResult	Cancel( void ) {
				OG_ASSERT( false );
				return JOB_DELETE;
			}

		private:
			JobManager *manager;	//!< The manager
		};
	};
//! @}
}

#endif
