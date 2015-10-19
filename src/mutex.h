#ifndef MUTEX_H
#define MUTEX_H

#include <pthread.h>

class Mutex {
	private:
		pthread_mutex_t mutex;

	public:
		Mutex() {pthread_mutex_init(&mutex, nullptr);}
		lock() {pthread_mutex_lock(&mutex);}
		unlock() {pthread_mutex_unlock(&mutex);}
};

class LockGuard {
	private:
		Mutex &mutex;

	public:
		LockGuard(Mutex &mutex) : mutex(mutex) {mutex.lock();}
		~LockGuard() {mutex.unlock();}
};

#endif
