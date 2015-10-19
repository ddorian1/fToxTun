#ifndef THREAD_H
#define THREAD_H

#include <pthread.h>

class Thread {
	private:
		pthread_t thread;

	public:
		Thread(void *(f)(void *), void *arg) {pthread_create(&thread, nullptr, f, arg);}
		join() {pthread_join(thread, nullptr);}
};

#endif
