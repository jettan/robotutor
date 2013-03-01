#include "RoboTutorClient.h"
#include "CodeEditor.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	RoboTutorClient w;
	w.show();
	return a.exec();
}
