#ifndef FRIENDS_WIDGET_H
#define FRIENDS_WIDGET_H

#include <FL/Fl_Box.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Scroll.H>
#include <map>

class MainWindow;

class FriendsWidget : public Fl_Scroll {
	private:
		MainWindow *mainWindow;
		std::map<uint32_t, Fl_Button> friendList;

		static void onFriendList(Fl_Widget *button, void *mainWindowV);

	public:
		FriendsWidget(MainWindow *mainWindow, size_t x, size_t y, size_t w, size_t h);

		void addFriend(uint32_t friendNumber, std::string name);
		void deleteFriend(uint32_t friendNumber);
		void changeName(uint32_t friendNumber, std::string name);
		void friendOnline(uint32_t friendNumber);
		void friendOffline(uint32_t friendNumber);
		uint32_t getSelectedFriend();
		void setSelectedFriend(uint32_t friendNumber);
};

#endif //FRIENDS_WIDGET_H
