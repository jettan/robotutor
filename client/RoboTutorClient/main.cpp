#include "RoboTutorClient.h"
#include "AsioThread.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);

	robotutor::RoboTutorClient gui;
	robotutor::AsioThread io;

	io.connectSlots(gui);
	gui.connectSlots(io);

	boost::thread ios_thread(std::bind(&robotutor::AsioThread::run, &io));

	gui.show();
	int result = a.exec();

	io.quit();
	ios_thread.join();
	return result;
}
