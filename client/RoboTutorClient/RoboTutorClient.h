#ifndef ROBOTUTORCLIENT_H
#define ROBOTUTORCLIENT_H

#include <QtWidgets/QMainWindow>
#include "ui_RoboTutorClient.h"
#include <QMessageBox>
#include <QFileDialog>
#include <QTextStream>
#include <QLabel>

#include <iostream>

#include <boost/asio.hpp>

#include "protobuf.hpp"
#include "protocol/messages.pb.h"
#include "connection.hpp"
#include "client.hpp"

#include "PptController.h"

typedef ascf::ProtocolBuffers<robotutor::ClientMessage, robotutor::RobotMessage> Protocol;

class RoboTutorClient : public QMainWindow
{
	Q_OBJECT

public:
	RoboTutorClient(QWidget *parent = 0);
	~RoboTutorClient();

private:
	Ui::RoboTutorClientClass ui_;
	boost::asio::io_service ios_;
	PptController *ppt_controller_;
	std::shared_ptr<ascf::Client<Protocol>> client_;

	QString script_path_;
	QLabel status_label_;

	void handleServerMessage(std::shared_ptr<ascf::Client<Protocol>> connection, robotutor::RobotMessage const & message);
	void setScriptPath(QString path);
	void setStatus(QString status);
	void saveScript(QString fileName);

private slots:
	void on_openScript_triggered();
	void on_saveScript_triggered();
	void on_saveScriptAs_triggered();
	void on_presentationButton_clicked();
};

#endif // ROBOTUTORCLIENT_H
