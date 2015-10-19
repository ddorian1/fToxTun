#include "mainWindow.h"
#include "dialog.h"
#include "toxWorker.h"

#include <FL/Fl.H>

int main(int argc, char **argv) {
#ifdef MINGW
	pthread_win32_thread_attach_np();
#endif
	Fl::lock();

	MainWindow mainWindow;
	mainWindow.show(argc, argv);

	ToxWorker *toxWorker;
	try {
		toxWorker = new ToxWorker(&mainWindow);
	} catch (ToxWorkerException &e) {
		Dialog::notify("Can't start Tox or ToxTun");
		return EXIT_FAILURE;
	}
	mainWindow.setToxWorker(toxWorker);

	Fl::run();

	mainWindow.setToxWorker(nullptr);
	delete toxWorker;

#ifdef MINGW
	pthread_win32_thread_detach_np();
#endif

	return EXIT_SUCCESS;
}

