#-------------------------------------------------
#
# Project created by QtCreator 2010-08-30T13:18:30
#
#-------------------------------------------------

QT       += core gui xml

TARGET = DamnSampler
TEMPLATE = app

SOURCES += main.cpp\
	samplerwindow.cpp \
	sampleparams.cpp \
	newsampledialog.cpp \
	sample.cpp \
    keyboardkey.cpp

HEADERS  += samplerwindow.h \
	sampleparams.h \
	newsampledialog.h \
	sample.h \
    keyboardkey.h

FORMS    += samplerwindow.ui \
	sampleparams.ui \
	newsampledialog.ui

INCLUDEPATH += FMOD/inc

unix: {
	LIBS += -lfmodex
}

RESOURCES += \
    images.qrc

win32: {
	RC_FILE = damnsampler.rc
	LIBS += -LFMOD/lib -lfmodex_vc
}
