#ifndef TEXT_INPUT_H
#define TEXT_INPUT_H

#include <FL/Fl_Box.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Window.H>
#include <string>

class TextInput : public Fl_Window {
	private:
		static constexpr unsigned int h = 95;
		static constexpr unsigned int w = 300;

		Fl_Box hint;
		Fl_Button buttonOk;
		Fl_Input text;

		static void onOk(Fl_Widget *button, void *textInputV);

		TextInput(const char* hint);

	public:
		static std::string get(const char *hint);
};

#endif //TEXT_INPUT_H
