#ifndef ROBOTUTORCLIENT_H
#define ROBOTUTORCLIENT_H

#include <iostream>

#include <boost/asio/io_service.hpp>

#include <QtWidgets/QMainWindow>
#include "ui_RoboTutorClient.h"
#include <QMessageBox>
#include <QFileDialog>
#include <QTextStream>
#include <QLabel>
#include <QTimer>
#include <QDomDocument>

#include "ScriptHighlighter.h"

namespace robotutor {

class AsioThread;

class RoboTutorClient : public QMainWindow
{
	Q_OBJECT

public:
	RoboTutorClient(QWidget *parent = 0);
	~RoboTutorClient();

	void connectSlots(AsioThread & asio);

private:
	Ui::RoboTutorClientClass ui_;

	bool connected_;
	bool paused_;

	QString script_path_;
	QLabel status_label_;
	QString turning_point_path_;

	ScriptHighlighter *highlighter_;

	void setScriptPath(QString path);
	void saveScript(QString fileName);

	void parseTpXml();

public slots:
	void setStatus(QString status);
	void setConnect(bool status);
	void log(QString info);

private slots:
	void on_openScript_triggered();
	void on_saveScript_triggered();
	void on_saveScriptAs_triggered();
	void on_presentationButton_clicked();
	void on_connectButton_clicked();
	void on_runButton_clicked();
	void on_pauseButton_clicked();
	void on_stopButton_clicked();

signals:
	void connectRobot(QString host, int port);
	void disconnect();
	void openPresentation(QString file);
	void sendScript(QString script);
	void pauseScript(bool);
	void stopScript();
};

}

#endif // ROBOTUTORCLIENT_H
