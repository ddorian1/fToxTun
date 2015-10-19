#include "dialog.h"

#include <FL/Fl.H>

Dialog::Dialog(const char* text, bool cancel)
:
	Fl_Window(w, h, "fToxTun"),
	text(5, 5, w-10, 40, text),
	buttonOk(w/2-45, 55, 40, 30, "OK"),
	buttonCancel(w/2+5, 55, 40, 30, "No")
{
	buttonOk.callback(onOk, this);

	if (cancel) {
		buttonCancel.callback(onCancel, this);
		buttonOk.copy_label("Yes");
	} else {
		buttonCancel.hide();
		buttonOk.position(w/2-20, 55);
	}

	end();
	set_modal();
	show();
}

void Dialog::onOk(Fl_Widget *button, void *dialogV) {
	Dialog *d = static_cast<Dialog*>(dialogV);
	d->answer = true;
	d->hide();
}

void Dialog::onCancel(Fl_Widget *button, void *dialogV) {
	Dialog *d = static_cast<Dialog*>(dialogV);
	d->answer = false;
	d->hide();
}

void Dialog::notify(const char *text) {
	Dialog d(text);
	while (d.shown()) Fl::wait();
}

bool Dialog::confirm(const char *text) {
	Dialog d(text, true);
	while (d.shown()) Fl::wait();
	return d.answer;
}
