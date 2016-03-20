#ifndef DIALOG_H
#define DIALOG_H

#include <FL/Fl_Box.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Window.H>

class Dialog : public Fl_Window {
	private:
		static constexpr unsigned int h = 95;
		static constexpr unsigned int w = 300;

		bool answer;

		Fl_Box text;
		Fl_Button buttonOk, buttonCancel;

		static void onOk(Fl_Widget *button, void *dialogV);
		static void onCancel(Fl_Widget *button, void *dialogV);

		Dialog(const char* text, bool cancel=false);

	public:
		static void notify(const char *text);
		static void notifyAndDelete(void *textV);
		static bool confirm(const char *hint);
};

#endif //DIALOG_H
