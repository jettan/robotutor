#ifndef ASIOTHREAD_H
#define ASIOTHREAD_H

#include <boost/thread.hpp>
#include <boost/asio.hpp>

#include <QObject>
#include <QString>

#include "protobuf.hpp"
#include "protocol/messages.pb.h"
#include "connection.hpp"

#include "client.hpp"
#include "PptController.h"

namespace robotutor {

class RoboTutorClient;

typedef ascf::ProtocolBuffers<ClientMessage, RobotMessage> Protocol;

class AsioThread : public QObject {
	Q_OBJECT

public:
	AsioThread(boost::asio::io_service & ios);

	void connectSlots(RoboTutorClient & gui);
	void run();
	void quit();

	void handleServerMessage(std::shared_ptr<ascf::Client<Protocol>> connection, RobotMessage const & message);
	void handleConnect(std::shared_ptr<ascf::Client<Protocol>> connection, boost::system::error_code const & error);

public slots:
	void connectRobot(QString host, int port);
	void disconnect();
	void sendScript(QString script);
	void openPresentation(QString file);
	void pauseScript(bool pause);
	void stopScript();

private:
	bool running_;
	PptController ppt_controller_;
	boost::asio::io_service & ios_;
	std::string host_;
	int port_;
	std::shared_ptr<ascf::Client<Protocol>> client_;

signals:
	void setStatus(QString status);
	void setConnect(bool status);
	void log(QString info);
};

}

#endif
