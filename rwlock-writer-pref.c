#include "rwlock.h"

void InitalizeReadWriteLock(struct read_write_lock * rw)
{
  //	Write the code for initializing your read-write lock.
	rw->readers = 0;
	rw->waiting_readers = 0;
	rw->writers = 0;		// 0 or 1
	rw->writers_waiting = 0;
	sem_init(&rw->lock, 0, 1);
	sem_init(&rw->writelock, 0, 0);   //
	sem_init(&rw->readlock, 0, 0);    // want to block even the first read thread
}

void ReaderLock(struct read_write_lock * rw)
{
  //	Write the code for aquiring read-write lock by the reader.
	sem_wait(&rw->lock);
	if(rw->writers_waiting + rw->writers != 0) {
		rw->waiting_readers++;
		sem_post(&rw->lock);
		sem_wait(&rw->readlock);
	}
	else {
		rw->readers++;
		sem_post(&rw->lock);
	}
}

void ReaderUnlock(struct read_write_lock * rw)
{
  //	Write the code for releasing read-write lock by the reader.
	sem_wait(&rw->lock);
	rw->readers--;
	if((rw->readers == 0) && (rw->writers_waiting > 0)) {
		sem_post(&rw->writelock);
		rw->writers++;
		rw->writers_waiting--;
	}
	sem_post(&rw->lock);
}

void WriterLock(struct read_write_lock * rw)
{
  //	Write the code for aquiring read-write lock by the writer.
	sem_wait(&rw->lock);
	if(rw->writers_waiting + rw->readers +  rw->writers != 0) {
		rw->writers_waiting++;
		sem_post(&rw->lock);
		sem_wait(&rw->writelock);
	}
	else{
		rw->writers++;
		sem_post(&rw->lock);
	}

}

void WriterUnlock(struct read_write_lock * rw)
{
  //	Write the code for releasing read-write lock by the writer.
	sem_wait(&rw->lock);
	rw->writers--;
	if(rw->writers_waiting >0) {
		sem_post(&rw->writelock);
		rw->writers++;
		rw->writers_waiting--;
	}
	else {
		while(rw->waiting_readers > 0) {
			sem_post(&rw->readlock);
			rw->readers++;
			rw->waiting_readers--;
		}
	}
	sem_post(&rw->lock);
}
