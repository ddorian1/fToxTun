bin_PROGRAMS = fToxTun

fToxTun_SOURCES = \
	dialog.cpp \
	dialog.h \
	fToxTun.cpp \
	mainWindow.cpp \
	mainWindow.h \
	mutex.h \
	thread.h \
	toxWorker.cpp \
	toxWorker.h \
	textInput.cpp \
	textInput.h

fToxTun_CPPFLAGS = ${DMINGW} ${FLTK_CFLAGS} ${TOXCORE_CFLAGS} ${TOXTUN_CFLAGS} -std=c++11
fToxTun_LDADD = ${TOXCORE_LIBS} ${TOXTUN_LIBS} ${FLTK_LIBS}
