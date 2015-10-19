#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <FL/Fl_Box.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Output.H>
#include <FL/Fl_Scroll.H>
#include <FL/Fl_Window.H>
#include <map>

class ToxWorker;

class MainWindow : public Fl_Window {
	private:
		static constexpr unsigned int h = 200;
		static constexpr unsigned int w = 400;

		ToxWorker *toxWorker;

		Fl_Button buttonTun, buttonFriendAdd, buttonFriendDelete, buttonChangeName;
		Fl_Box connectionState, lname, name, laddress;
		Fl_Output address;
		Fl_Scroll friendArea;
		std::map<uint32_t, Fl_Button> friendList;

		void tunButtonToConnect();
		void tunButtonToConnecting();
		void tunButtonToAccept();
		void tunButtonToClose();

		static void onFriendAdd(Fl_Widget *button, void *mainWindowV);
		static void onFriendDelete(Fl_Widget *button, void *mainWindowV);
		static void onChangeName(Fl_Widget *button, void *mainWindowV);
		static void onFriendList(Fl_Widget *button, void *mainWindowV);
		static void onTunButtonConnect(Fl_Widget *button, void *mainWindowV);
		static void onTunButtonClose(Fl_Widget *button, void *mainWindowV);
		static void onTunButtonAccept(Fl_Widget *button, void *mainWindowV);

	public:
		MainWindow();
		void setToxWorker(ToxWorker *toxWorker);

		static void selfConnected(void *mainWindowV);
		static void selfDisconnected(void *mainWindowV);
		static void setName(void *pV);
		static void setAddress(void *pV);
		static void friendAdd(void *tV);
		static void friendOnline(void *pV);
		static void friendOffline(void *pV);
		static void friendNameChanged(void *tV);
		static void connectionRequest(void *pV);
		static void connectionAccepted(void *pV);
		static void connectionRejected(void *pV);
		static void connectionClosed(void *pV);
};

#endif //MAIN_WINDOWS_H
