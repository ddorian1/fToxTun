#include "textInput.h"

#include <FL/Fl.H>

TextInput::TextInput(const char* hint)
:
	Fl_Window(w, h, "fToxTun"),
	hint(5, 5, w-10, 20, hint),
	buttonOk(w/2-20, 55, 40, 30, "OK"),
	text(5, 30, w-10, 20)
{
	buttonOk.callback(onOk, this);

	end();
	set_modal();
	show();
}

void TextInput::onOk(Fl_Widget *button, void *textInputV) {
	TextInput *ti = static_cast<TextInput*>(textInputV);
	ti->hide();
}

std::string TextInput::get(const char *hint) {
	TextInput ti(hint);
	while (ti.shown()) Fl::wait();
	return ti.text.value();
}
