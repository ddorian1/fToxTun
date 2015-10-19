#include "mainWindow.h"
#include "dialog.h"
#include "toxWorker.h"
#include "textInput.h"

#include <FL/Fl.H>
#include <string>
#include <tuple>

MainWindow::MainWindow()
:
	Fl_Window(w, h, "fToxTun"),
	toxWorker(nullptr),
	buttonTun(55, h-25, w/2-60, 20, "Connect"),
	buttonFriendAdd(5, h-25, 20, 20, "@+"),
	buttonFriendDelete(30, h-25, 20, 20, "@line"),
	buttonChangeName(w-55, 5, 50, 45, "Edit"),
	connectionState(w-25, h-25, 20, 20),
	lname(w/2+5, 5, w/2-60, 20, "Your Name:"),
	name(w/2+5, 30, w/2-60, 20),
	laddress(w/2+5, 70, w/2-10, 20, "Your Address:"),
	address(w/2+5, 95, w/2-10, 20),
	friendArea(5, 20, w/2-10, h-60, "Friends")
{
	buttonFriendAdd.callback(onFriendAdd, this);
	buttonFriendDelete.callback(onFriendDelete, this);
	buttonChangeName.callback(onChangeName, this);

	buttonFriendAdd.labelcolor(fl_rgb_color(0, 255, 0));
	buttonFriendDelete.labelcolor(fl_rgb_color(255, 0, 0));

	tunButtonToConnect();
	buttonTun.deactivate();

	connectionState.box(FL_OFLAT_BOX);
	selfDisconnected(this);

	friendArea.type(Fl_Scroll::VERTICAL_ALWAYS);

	end();
}

void MainWindow::setToxWorker(ToxWorker *tw) {
	toxWorker = tw;
}

void MainWindow::selfConnected(void *mainWindowV) {
	MainWindow *mw = static_cast<MainWindow*>(mainWindowV);
	mw->connectionState.color(fl_rgb_color(0, 255, 0));
	mw->connectionState.redraw();
}

void MainWindow::selfDisconnected(void *mainWindowV) {
	MainWindow *mw = static_cast<MainWindow*>(mainWindowV);
	mw->connectionState.color(fl_rgb_color(255, 0, 0));
	mw->connectionState.redraw();
}

void MainWindow::setName(void *pV) {
	auto p = static_cast<std::pair<MainWindow*, std::string>*>(pV);
	p->first->name.label(p->second.c_str());
	p->first->name.redraw();
	delete p;
}

void MainWindow::setAddress(void *pV) {
	auto p = static_cast<std::pair<MainWindow*, std::string>*>(pV);
	p->first->address.value(p->second.c_str());
	p->first->address.redraw();
	delete p;
}

void MainWindow::onFriendAdd(Fl_Widget *button, void *mainWindowV) {
	MainWindow *mw = static_cast<MainWindow*>(mainWindowV);
	if (!mw->toxWorker) return;

	std::string address = TextInput::get("Insert friend address to add");
	int64_t friendNumber = mw->toxWorker->addFriend(address);
	if (friendNumber < 0) return;

	auto *t = new std::tuple<MainWindow*, uint32_t, std::string>(mw, friendNumber, "New Friend");
	MainWindow::friendAdd(t);
}

void MainWindow::onFriendDelete(Fl_Widget *button, void *mainWindowV) {
	MainWindow *mw = static_cast<MainWindow*>(mainWindowV);
	if (!mw->toxWorker) return;

	uint32_t friendNumber;
	for (auto &f : mw->friendList) {
		if (f.second.value()) friendNumber = f.first;
	}

	if (Dialog::confirm("Delete selected friend?")) {
		mw->toxWorker->deleteFriend(friendNumber);
		mw->friendList.erase(friendNumber);
		mw->friendArea.redraw();
	}
}

void MainWindow::onChangeName(Fl_Widget *button, void *mainWindowV) {
	MainWindow *mw = static_cast<MainWindow*>(mainWindowV);
	if (!mw->toxWorker) return;
	
	std::string name = TextInput::get("New Name");
	mw->toxWorker->changeName(name);
	mw->name.copy_label(name.c_str());
	mw->name.redraw();
}

void MainWindow::tunButtonToConnecting() {
	for (auto &f : friendList) {
		f.second.deactivate();
	}

	buttonTun.copy_label("Connecting...");
	buttonTun.labelcolor(fl_rgb_color(255, 215, 0));
	buttonTun.callback(onTunButtonClose, this);
}

void MainWindow::tunButtonToConnect() {
	for (auto &f : friendList) {
		f.second.activate();
	}

	buttonTun.copy_label("Connect");
	buttonTun.labelcolor(fl_rgb_color(0, 255, 0));
	buttonTun.callback(onTunButtonConnect, this);
}

void MainWindow::tunButtonToClose() {
	for (auto &f : friendList) {
		f.second.deactivate();
	}
	
	buttonTun.copy_label("Connected");
	buttonTun.labelcolor(fl_rgb_color(255, 0, 0));
	buttonTun.callback(onTunButtonClose, this);
}

void MainWindow::tunButtonToAccept() {
	for (auto &f : friendList) {
		f.second.deactivate();
	}
	
	buttonTun.copy_label("Accept connection");
	buttonTun.labelcolor(fl_rgb_color(255, 215, 0));
	buttonTun.callback(onTunButtonAccept, this);
}

void MainWindow::onFriendList(Fl_Widget *button, void *mainWindowV) {
	MainWindow *mw = static_cast<MainWindow*>(mainWindowV);
	mw->buttonTun.activate();
}

void MainWindow::onTunButtonConnect(Fl_Widget *button, void *mainWindowV) {
	MainWindow *mw = static_cast<MainWindow*>(mainWindowV);
	if (!mw->toxWorker) return;

	uint32_t friendNumber;
	for (auto &f : mw->friendList) {
		if (f.second.value()) friendNumber = f.first;
	}

	mw->toxWorker->tunConnect(friendNumber);
	mw->tunButtonToConnecting();
}

void MainWindow::onTunButtonClose(Fl_Widget *button, void *mainWindowV) {
	MainWindow *mw = static_cast<MainWindow*>(mainWindowV);
	if (!mw->toxWorker) return;

	mw->toxWorker->tunClose();
	mw->tunButtonToConnect();
}

void MainWindow::onTunButtonAccept(Fl_Widget *button, void *mainWindowV) {
	MainWindow *mw = static_cast<MainWindow*>(mainWindowV);
	if (!mw->toxWorker) return;

	uint32_t friendNumber;
	for (auto &f : mw->friendList) {
		if (f.second.value()) friendNumber = f.first;
	}
	
	mw->toxWorker->tunAccept(friendNumber);
	mw->tunButtonToClose();
}

void MainWindow::friendAdd(void *tV) {
	MainWindow *mw;
	uint32_t friendNumber;
	std::string friendName;
	auto t = static_cast<std::tuple<MainWindow*, uint32_t, std::string>*>(tV);
	std::tie(mw, friendNumber, friendName) = *t;

	size_t posH = 20 + 25*mw->friendList.size();
	Fl_Group::current(&(mw->friendArea));
	mw->friendList.emplace(std::piecewise_construct, 
			std::forward_as_tuple(friendNumber),
			std::forward_as_tuple(5, posH, w/2-30, 20)
	);
	mw->friendList.at(friendNumber).copy_label(friendName.c_str());
	mw->friendList.at(friendNumber).callback(onFriendList, std::get<0>(*t));
	mw->friendList.at(friendNumber).type(102);
	mw->friendList.at(friendNumber).labelcolor(fl_rgb_color(255, 0, 0));
	mw->friendList.at(friendNumber).show();
	mw->redraw();
	mw->end();
	delete t;
}

void MainWindow::friendOnline(void *pV) {
	auto p = static_cast<std::pair<MainWindow*, uint32_t>*>(pV);
	p->first->friendList.at(p->second).labelcolor(fl_rgb_color(0, 255, 0));
	p->first->friendList.at(p->second).redraw();
	delete p;
}

void MainWindow::friendOffline(void *pV) {
	auto p = static_cast<std::pair<MainWindow*, uint32_t>*>(pV);
	p->first->friendList.at(p->second).labelcolor(fl_rgb_color(255, 0, 0));
	p->first->friendList.at(p->second).redraw();
	delete p;
}

void MainWindow::friendNameChanged(void *tV) {
	MainWindow *mw;
	uint32_t friendNumber;
	std::string friendName;
	auto t = static_cast<std::tuple<MainWindow*, uint32_t, std::string>*>(tV);
	std::tie(mw, friendNumber, friendName) = *t;

	mw->friendList.at(friendNumber).copy_label(friendName.c_str());
	mw->friendList.at(friendNumber).redraw();
	delete t;
}

void MainWindow::connectionRequest(void *pV) {
	auto p = static_cast<std::pair<MainWindow*, uint32_t>*>(pV);
	for (auto &f : p->first->friendList) {
		f.second.value(f.first == p->second);
	}

	p->first->buttonTun.activate();
	p->first->tunButtonToAccept();
	delete p;
}

void MainWindow::connectionAccepted(void *pV) {
	auto p = static_cast<std::pair<MainWindow*, uint32_t>*>(pV);
	p->first->tunButtonToClose();
	delete p;
}

void MainWindow::connectionRejected(void *pV) {
	auto p = static_cast<std::pair<MainWindow*, uint32_t>*>(pV);
	p->first->tunButtonToConnect();
	delete p;
}

void MainWindow::connectionClosed(void *pV) {
	auto p = static_cast<std::pair<MainWindow*, uint32_t>*>(pV);
	p->first->tunButtonToConnect();
	delete p;
}
