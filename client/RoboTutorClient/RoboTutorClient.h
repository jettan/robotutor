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

#include <boost/asio.hpp>

#include "protobuf.hpp"
#include "protocol/messages.pb.h"
#include "connection.hpp"
#include "client.hpp"

#include "PptController.h"
#include "ScriptHighlighter.h"

namespace robotutor {

typedef ascf::ProtocolBuffers<ClientMessage, RobotMessage> Protocol;

class RoboTutorClient : public QMainWindow
{
	Q_OBJECT

public:
	RoboTutorClient(QWidget *parent = 0);
	~RoboTutorClient();

private:
	Ui::RoboTutorClientClass ui_;
	PptController *ppt_controller_;

	boost::asio::io_service ios_;
	std::shared_ptr<ascf::Client<Protocol>> client_;
	bool connected_;

	QString script_path_;
	QLabel status_label_;

	ScriptHighlighter *highlighter_;

	QTimer *timer_;

	void handleServerMessage(std::shared_ptr<ascf::Client<Protocol>> connection, RobotMessage const & message);
	void handleConnect(std::shared_ptr<ascf::Client<Protocol>> connection, boost::system::error_code const & error);

	void setScriptPath(QString path);
	void setStatus(QString status);
	void saveScript(QString fileName);

private slots:
	void on_openScript_triggered();
	void on_saveScript_triggered();
	void on_saveScriptAs_triggered();
	void on_presentationButton_clicked();
	void on_connectButton_clicked();
	void on_runButton_clicked();

	void processOnce();
};

}

#endif // ROBOTUTORCLIENT_H
