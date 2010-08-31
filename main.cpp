#include <QtGui/QApplication>
#include "samplerwindow.h"
#include <QCleanlooksStyle>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	a.setStyle(new QCleanlooksStyle);
	a.setApplicationName("DamnSmpler");
	a.setOrganizationName("silvansky");
	SamplerWindow w;
	w.show();
	return a.exec();
}
