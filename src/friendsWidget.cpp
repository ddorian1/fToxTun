#include "friendsWidget.h"
#include "mainWindow.h"

#include <FL/Fl.H>
#include <string>

FriendsWidget::FriendsWidget(MainWindow *mainWindow, size_t x, size_t y, size_t w, size_t h)
:
	mainWindow(mainWindow),
	Fl_Scroll(x, y, w, h, "Friends")
{
	type(Fl_Scroll::VERTICAL_ALWAYS);
	end();
}

void FriendsWidget::onFriendList(Fl_Widget *button, void *mainWindowV) {
	MainWindow *mw = static_cast<MainWindow*>(mainWindowV);
	mw->selectedFriendChanged(mainWindowV);
}

void FriendsWidget::addFriend(uint32_t friendNumber, std::string name) {
	size_t posH = 20 + 25 * friendList.size();
	Fl_Group::current(this);
	friendList.emplace(std::piecewise_construct, 
			std::forward_as_tuple(friendNumber),
			std::forward_as_tuple(5, posH, w() - 20, 20)
	);
	friendList.at(friendNumber).copy_label(name.c_str());
	friendList.at(friendNumber).callback(onFriendList, mainWindow);
	friendList.at(friendNumber).type(102);
	friendList.at(friendNumber).labelcolor(fl_rgb_color(255, 0, 0));
	friendList.at(friendNumber).show();
	end();
}

void FriendsWidget::friendOnline(uint32_t friendNumber) {
	friendList.at(friendNumber).labelcolor(fl_rgb_color(0, 255, 0));
	friendList.at(friendNumber).redraw();
}

void FriendsWidget::friendOffline(uint32_t friendNumber) {
	friendList.at(friendNumber).labelcolor(fl_rgb_color(255, 0, 0));
	friendList.at(friendNumber).redraw();
}

void FriendsWidget::changeName(uint32_t friendNumber, std::string name) {
	friendList.at(friendNumber).copy_label(name.c_str());
	friendList.at(friendNumber).redraw();
}

void FriendsWidget::deleteFriend(uint32_t friendNumber) {
	friendList.erase(friendNumber);

	size_t posH = 20;
	for (auto &f : friendList) {
		f.second.position(5, posH);
		posH += 25;
	}

	redraw();
}

uint32_t FriendsWidget::getSelectedFriend() {
	for (auto &f : friendList) {
		if (f.second.value()) return f.first;
	}
	throw 1;
}

void FriendsWidget::setSelectedFriend(uint32_t friendNumber) {
	for (auto &f : friendList) {
		f.second.value(f.first == friendNumber);
	}
}
