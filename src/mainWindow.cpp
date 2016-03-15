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
	friendsWidget(this, 5, 20, w/2-10, h-60)
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
	p->first->name.copy_label(p->second.c_str());
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

	uint32_t friendNumber = mw->friendsWidget.getSelectedFriend();

	if (Dialog::confirm("Delete selected friend?")) {
		mw->toxWorker->deleteFriend(friendNumber);
		mw->friendsWidget.deleteFriend(friendNumber);
	}
}

void MainWindow::onChangeName(Fl_Widget *button, void *mainWindowV) {
	MainWindow *mw = static_cast<MainWindow*>(mainWindowV);
	if (!mw->toxWorker) return;
	
	std::string name = TextInput::get("New Name");
	if (name.empty()) return;

	mw->toxWorker->changeName(name);
	mw->name.copy_label(name.c_str());
	mw->name.redraw();
}

void MainWindow::tunButtonToConnecting() {
	buttonTun.copy_label("Connecting...");
	buttonTun.labelcolor(fl_rgb_color(255, 215, 0));
	buttonTun.callback(onTunButtonClose, this);
}

void MainWindow::tunButtonToConnect() {
	buttonTun.copy_label("Connect");
	buttonTun.labelcolor(fl_rgb_color(0, 255, 0));
	buttonTun.callback(onTunButtonConnect, this);
}

void MainWindow::tunButtonToClose() {
	buttonTun.copy_label("Connected");
	buttonTun.labelcolor(fl_rgb_color(255, 0, 0));
	buttonTun.callback(onTunButtonClose, this);
}

void MainWindow::tunButtonToAccept() {
	buttonTun.copy_label("Accept connection");
	buttonTun.labelcolor(fl_rgb_color(255, 215, 0));
	buttonTun.callback(onTunButtonAccept, this);
}

void MainWindow::onTunButtonConnect(Fl_Widget *button, void *mainWindowV) {
	MainWindow *mw = static_cast<MainWindow*>(mainWindowV);
	if (!mw->toxWorker) return;

	uint32_t friendNumber = mw->friendsWidget.getSelectedFriend();

	if (mw->toxWorker->tunConnect(friendNumber)) {
		mw->tunButtonToConnecting();
	}
}

void MainWindow::onTunButtonClose(Fl_Widget *button, void *mainWindowV) {
	MainWindow *mw = static_cast<MainWindow*>(mainWindowV);
	if (!mw->toxWorker) return;

	uint32_t friendNumber = mw->friendsWidget.getSelectedFriend();

	mw->toxWorker->tunClose(friendNumber);
	mw->tunButtonToConnect();
}

void MainWindow::onTunButtonAccept(Fl_Widget *button, void *mainWindowV) {
	MainWindow *mw = static_cast<MainWindow*>(mainWindowV);
	if (!mw->toxWorker) return;

	uint32_t friendNumber = mw->friendsWidget.getSelectedFriend();
	
	if (mw->toxWorker->tunAccept(friendNumber)) {
		mw->tunButtonToClose();
	}
}

void MainWindow::friendAdd(void *tV) {
	MainWindow *mw;
	uint32_t friendNumber;
	std::string friendName;
	auto t = static_cast<std::tuple<MainWindow*, uint32_t, std::string>*>(tV);
	std::tie(mw, friendNumber, friendName) = *t;

	mw->friendsWidget.addFriend(friendNumber, friendName);
	delete t;
}

void MainWindow::friendOnline(void *pV) {
	auto p = static_cast<std::pair<MainWindow*, uint32_t>*>(pV);
	p->first->friendsWidget.friendOnline(p->second);
	delete p;
}

void MainWindow::friendOffline(void *pV) {
	auto p = static_cast<std::pair<MainWindow*, uint32_t>*>(pV);
	p->first->friendsWidget.friendOffline(p->second);
	delete p;
}

void MainWindow::friendNameChanged(void *tV) {
	MainWindow *mw;
	uint32_t friendNumber;
	std::string friendName;
	auto t = static_cast<std::tuple<MainWindow*, uint32_t, std::string>*>(tV);
	std::tie(mw, friendNumber, friendName) = *t;

	mw->friendsWidget.changeName(friendNumber, friendName);
	delete t;
}

void MainWindow::connectionRequest(void *pV) {
	auto p = static_cast<std::pair<MainWindow*, uint32_t>*>(pV);
	p->first->friendsWidget.setSelectedFriend(p->second);

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

void MainWindow::selectedFriendChanged(void *MainWindowV) {
	MainWindow *mw = static_cast<MainWindow*>(MainWindowV);

	uint32_t sf = mw->friendsWidget.getSelectedFriend();
	ToxTun::ConnectionState cs = mw->toxWorker->getConnectionState(sf);

	mw->buttonTun.activate();

	switch(cs) {
		case ToxTun::ConnectionState::Connected:
			mw->tunButtonToClose();
			break;
		case ToxTun::ConnectionState::Disconnected:
			mw->tunButtonToConnect();
			//TODO if (!friend is online) buttonTun.deactivate();
			break;
		case ToxTun::ConnectionState::FriendIsRinging:
			mw->tunButtonToAccept();
			break;
		case ToxTun::ConnectionState::RingingAtFriend:
			mw->tunButtonToConnecting();
			break;
	}
}
