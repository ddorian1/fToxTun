#ifndef TOX_WORKER_H
#define TOX_WORKER_H

#ifdef MINGW
#include "mutex.h"
#include "thread.h"
#else
#include <mutex>
#include <thread>
using Mutex=std::mutex;
using Thread=std::thread;
using LockGuard=std::lock_guard<std::mutex>;
#endif
#include <string>
#include <tox/tox.h>
#include <ToxTun.hpp>

class MainWindow;

class ToxWorkerException {};

class ToxWorker {
	private:
		bool quit, backgroundStarted, backgroundError;
		Tox *tox;
		ToxTun *toxTun;
		MainWindow *mainWindow;
		Mutex mutex;
		Thread thread;

		static void* backgroundJob(void *toxWorker);
		static std::string toxFriendName(Tox *tox, uint32_t friendNumber);

		static void selfConnectionStatusCb(Tox *tox, TOX_CONNECTION connectionStatus, void *mainWindowV);
		static void friendConnectionStatusCb(Tox *tox, uint32_t friendNumber, TOX_CONNECTION connectionStatus, void *mainWindowV);
		static void friendRequestCb(Tox *tox, const uint8_t *pubKey, const uint8_t *message, size_t length, void *mainWindowV);
		static void friendNameCb(Tox *tox, uint32_t friendNumber, const uint8_t *name, size_t length, void *mainWindowV);
		static void toxTunCb(ToxTun::Event event, uint32_t friendNumber, void *userData);

	public:
		ToxWorker(MainWindow *mainWindow);
		~ToxWorker();

		int64_t addFriend(std::string address);
		void deleteFriend(uint32_t friendNumber);
		void changeName(std::string name);
		bool tunConnect(uint32_t friendNumber);
		void tunClose(uint32_t friendNumber);
		bool tunAccept(uint32_t friendNumber);
		ToxTun::ConnectionState getConnectionState(uint32_t friendNumber);
};

#endif //TOX_WORKER_H
