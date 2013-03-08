#ifndef ROBOTUTORCLIENT_H
#define ROBOTUTORCLIENT_H

#include <QtWidgets/QMainWindow>
#include "ui_RoboTutorClient.h"
#include <QMessageBox>
#include <QFileDialog>
#include <QTextStream>
#include <QLabel>
#include <QTimer>

#include <iostream>

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

	QString script_path_;
	QLabel status_label_;

	ScriptHighlighter *highlighter_;

	void setScriptPath(QString path);
	void saveScript(QString fileName);

public slots:
	void setStatus(QString status);
	void setConnect(bool status);
	void criticalInformation(QString title, QString info);

private slots:
	void on_openScript_triggered();
	void on_saveScript_triggered();
	void on_saveScriptAs_triggered();
	void on_presentationButton_clicked();
	void on_connectButton_clicked();
	void on_runButton_clicked();

signals:
	void connectRobot(QString host, int port);
	void sendScript(QString script);
};

}

#endif // ROBOTUTORCLIENT_H
