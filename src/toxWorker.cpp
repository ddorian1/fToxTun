#include "mainWindow.h"
#include "toxWorker.h"

#include <FL/Fl.H>
#include <tox/tox.h>
#include <chrono>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits>
#include <sstream>
#include <tuple>
#include <vector>

ToxWorker::ToxWorker(MainWindow *mainWindow)
:
	quit(false),
	backgroundStarted(false),
	backgroundError(false),
	tox(nullptr),
	toxTun(nullptr),
	mainWindow(mainWindow),
	thread(&ToxWorker::backgroundJob, this)
{
	const std::chrono::milliseconds t(100);

	while(true) {
		{
			LockGuard l(mutex);
			if (backgroundStarted || backgroundError) break;
		}
#ifdef _WIN32
		Sleep(t.count());
#else
		std::this_thread::sleep_for(t);
#endif
	}

	LockGuard l(mutex);
	if (backgroundError) {
		thread.join();
		throw ToxWorkerException();
	}
}

ToxWorker::~ToxWorker() {
	{
		LockGuard l(mutex);
		quit = true;
	}
	thread.join();
}

void* ToxWorker::backgroundJob(void *twV) {
	ToxWorker *tw = static_cast<ToxWorker*>(twV);

	struct Tox_Options *options = tox_options_new(nullptr);
	tox_options_default(options);

	std::vector<uint8_t> saveData;
	std::ifstream iFile("fToxTun.tox", std::ios_base::in | std::ios_base::binary);
	if (iFile.good()) {
		iFile.seekg(0, iFile.end);
		saveData.resize(iFile.tellg());
		iFile.seekg(0, iFile.beg);

		iFile.read(reinterpret_cast<char*>(&saveData[0]), saveData.size());
	}
	iFile.close();

	if (!saveData.empty()) {
		options->savedata_type = TOX_SAVEDATA_TYPE_TOX_SAVE;
		options->savedata_data = &saveData[0];
		options->savedata_length = saveData.size();
	}

	{
		LockGuard l(tw->mutex);
		enum TOX_ERR_NEW err;
		tw->tox = tox_new(options, &err);
		tox_options_free(options);

		if (!tw->tox) {
			std::cerr << "Can't start tox: " << err << "\n";
			tw->backgroundError = true;
			return nullptr;
		}

		if (saveData.empty()) {
			uint8_t name[9] = "SomeName";
			tox_self_set_name(tw->tox, name, sizeof(name)-1, nullptr);
		}

		{
			std::string name(tox_self_get_name_size(tw->tox), '#');
			tox_self_get_name(tw->tox, reinterpret_cast<uint8_t*>(&name[0]));
			auto p = new std::pair<MainWindow*, std::string>;
			p->first = tw->mainWindow;
			p->second = name;
			Fl::awake(MainWindow::setName, static_cast<void*>(p));
		}

		tox_callback_self_connection_status(tw->tox, selfConnectionStatusCb, tw->mainWindow);
		tox_callback_friend_connection_status(tw->tox, friendConnectionStatusCb, tw->mainWindow);
		tox_callback_friend_request(tw->tox, friendRequestCb, tw->mainWindow);
		tox_callback_friend_name(tw->tox, friendNameCb, tw->mainWindow);

		std::vector<uint32_t> friendList(tox_self_get_friend_list_size(tw->tox));
		tox_self_get_friend_list(tw->tox, &friendList[0]);
		for (const auto &f : friendList) {
			auto *t = new std::tuple<MainWindow*, uint32_t, std::string>(
					tw->mainWindow,
					f,
					toxFriendName(tw->tox, f)
			);
			Fl::awake(MainWindow::friendAdd, t);
		}

		{
			uint8_t address[TOX_ADDRESS_SIZE];
			tox_self_get_address(tw->tox, address);
			std::stringstream s;
			for (const auto &c : address) s << std::setfill('0') << std::setw(2) << std::hex << static_cast<int>(c);
			auto p = new std::pair<MainWindow*, std::string>;
			p->first = tw->mainWindow;
			p->second = s.str();
			Fl::awake(MainWindow::setAddress, static_cast<void*>(p));
		}
		
		std::vector<uint8_t> p1 = {0x04, 0x11, 0x9E, 0x83, 0x5D, 0xF3, 0xE7, 0x8B, 0xAC, 0xF0, 0xF8, 0x42, 0x35, 0xB3, 0x00, 0x54, 0x6A, 0xF8, 0xB9, 0x36, 0xF0, 0x35, 0x18, 0x5E, 0x2A, 0x8E, 0x9E, 0x0A, 0x67, 0xC8, 0x92, 0x4F};

		if (!tox_bootstrap(tw->tox, "144.76.60.215", 33445, &p1[0], nullptr))
			std::cout << "Error bootstrap\n";
		if (!tox_add_tcp_relay(tw->tox, "144.76.60.215", 33445, &p1[0], nullptr))
			std::cout << "Error relay\n";

		tw->toxTun = ToxTun::newToxTunNoExp(tw->tox);
		if (!tw->toxTun) {
			std::cerr << "Can't open ToxTun";
			tox_kill(tw->tox);
			tw->tox = nullptr;
			tw->backgroundError = true;
			return nullptr;
		}
		tw->toxTun->setCallback(toxTunCb, tw->mainWindow);

		tw->backgroundStarted = true;
	}

	while(true) {
		std::chrono::milliseconds t;
		{
			LockGuard l(tw->mutex);
			if (tw->quit) break;
			tox_iterate(tw->tox);
			tw->toxTun->iterate();
			t = std::chrono::milliseconds(tox_iteration_interval(tw->tox));
		}
#ifdef _WIN32
		Sleep(t.count());
#else
		std::this_thread::sleep_for(t);
#endif
	}

	{
		LockGuard l(tw->mutex);
		saveData.resize(tox_get_savedata_size(tw->tox));
		tox_get_savedata(tw->tox, &saveData[0]);
		std::ofstream oFile("fToxTun.tox", std::ios_base::out | std::ios_base::binary);
		if (oFile.good()) {
			oFile.write(reinterpret_cast<char*>(&saveData[0]), saveData.size());
		}

		delete tw->toxTun;
		tw->toxTun = nullptr;

		tox_kill(tw->tox);
		tw->tox = nullptr;
	}

	return nullptr;
}

std::string ToxWorker::toxFriendName(Tox *tox, uint32_t friendNumber) {
	const size_t len = tox_friend_get_name_size(tox, friendNumber, nullptr);
	std::string name(len, '#');
	if (!tox_friend_get_name(tox, friendNumber, reinterpret_cast<uint8_t*>(&name[0]), nullptr))
		return "Noname";

	return name;
}

void ToxWorker::selfConnectionStatusCb(Tox *tox, TOX_CONNECTION connectionStatus, void *mainWindowV) {
	MainWindow *mw = static_cast<MainWindow*>(mainWindowV);

	switch(connectionStatus) {
		case TOX_CONNECTION_NONE:
			Fl::awake(mw->selfDisconnected, mainWindowV);
			break;
		case TOX_CONNECTION_TCP:
		case TOX_CONNECTION_UDP:
			Fl::awake(mw->selfConnected, mainWindowV);
			break;
	}
}

void ToxWorker::friendConnectionStatusCb(Tox *tox, uint32_t friendNumber, TOX_CONNECTION connectionStatus, void *mainWindowV) {
	MainWindow *mw = static_cast<MainWindow*>(mainWindowV);

	auto *p = new std::pair<MainWindow*, uint32_t>(mw, friendNumber);

	switch(connectionStatus) {
		case TOX_CONNECTION_NONE:
			Fl::awake(mw->friendOffline, p);
			break;
		case TOX_CONNECTION_TCP:
		case TOX_CONNECTION_UDP:
			Fl::awake(mw->friendOnline, p);
			break;
	}
}

int64_t ToxWorker::addFriend(std::string address) {
	std::vector<uint8_t> a;
	while(address.size() >= 2) {
		std::stringstream s;
		s << address[0] << address[1];
		a.push_back(std::stoi(s.str(), 0, 16));
		address = address.substr(2);
	}

	if (a.size() != TOX_ADDRESS_SIZE) return -1;

	int64_t friendNumber = -1;
	{
		LockGuard l(mutex);
		friendNumber = tox_friend_add(tox, &a[0], reinterpret_cast<const uint8_t*>("Add me!"), 7, nullptr);
	}

	if (friendNumber == std::numeric_limits<uint32_t>::max()) friendNumber = -1;

	return friendNumber;
}

void ToxWorker::deleteFriend(uint32_t friendNumber) {
	LockGuard l(mutex);
	tox_friend_delete(tox, friendNumber, nullptr);
}

void ToxWorker::changeName(std::string name) {
	LockGuard l(mutex);
	tox_self_set_name(tox, reinterpret_cast<uint8_t*>(&name[0]), name.size(), nullptr);
}

void ToxWorker::tunConnect(uint32_t friendNumber) {
	LockGuard l(mutex);
	toxTun->sendConnectionRequest(friendNumber);
}

void ToxWorker::tunClose() {
	LockGuard l(mutex);
	toxTun->closeConnection();
}

void ToxWorker::tunAccept(uint32_t friendNumber) {
	LockGuard l(mutex);
	toxTun->acceptConnection(friendNumber);
}

void ToxWorker::friendNameCb(Tox *tox, uint32_t friendNumber, const uint8_t *name, size_t length, void *mainWindowV) {
	MainWindow *mw = static_cast<MainWindow*>(mainWindowV);

	auto *t = new std::tuple<MainWindow*, uint32_t, std::string>(mw, friendNumber, reinterpret_cast<const char*>(name));
	Fl::awake(mw->friendNameChanged, t);
}

void ToxWorker::friendRequestCb(Tox *tox, const uint8_t *pubKey, const uint8_t *message, size_t length, void *mainWindowV) {
	MainWindow *mw = static_cast<MainWindow*>(mainWindowV);

	int64_t friendNumber = -1;
	friendNumber = tox_friend_add_norequest(tox, pubKey, nullptr);

	if (friendNumber == std::numeric_limits<uint32_t>::max()) return;

	auto *t = new std::tuple<MainWindow*, uint32_t, std::string>(mw, friendNumber, toxFriendName(tox, friendNumber));
	Fl::awake(MainWindow::friendAdd, t);
}

void ToxWorker::toxTunCb(ToxTun::Event event, uint32_t friendNumber, void *mainWindowV) {
	MainWindow *mw = static_cast<MainWindow*>(mainWindowV);

	auto *p = new std::pair<MainWindow*, uint32_t>(mw, friendNumber);
	switch(event) {
		case ToxTun::Event::ConnectionRequested:
			Fl::awake(mw->connectionRequest, p);
			break;
		case ToxTun::Event::ConnectionAccepted:
			Fl::awake(mw->connectionAccepted, p);
			break;
		case ToxTun::Event::ConnectionRejected:
			Fl::awake(mw->connectionRejected, p);
			break;
		case ToxTun::Event::ConnectionClosed:
			Fl::awake(mw->connectionClosed, p);
			break;
	}
}
