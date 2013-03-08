#include "AsioThread.h"
#include "RoboTutorClient.h"

namespace robotutor {

AsioThread::AsioThread() : 
	running_(true), client_(ascf::Client<Protocol>::create(ios_))
{
	client_->message_handler = std::bind(&AsioThread::handleServerMessage, this, std::placeholders::_1, std::placeholders::_2);
	client_->connect_handler = std::bind(&AsioThread::handleConnect, this, std::placeholders::_1, std::placeholders::_2);
}

void AsioThread::connectSlots(RoboTutorClient & gui) {
	connect(this, SIGNAL(setStatus(QString)), &gui, SLOT(setStatus(QString)));
	connect(this, SIGNAL(setConnect(bool)), &gui, SLOT(setConnect(bool)));
	connect(this, SIGNAL(criticalInformation(QString, QString)), &gui, SLOT(criticalInformation(QString, QString)));
}

void AsioThread::quit() {
	running_ = false;
	ios_.stop();
}

void AsioThread::handleServerMessage(std::shared_ptr<ascf::Client<Protocol>> connection, RobotMessage const & message) {
	if (message.has_slide()) {// && ppt_controller_ != NULL) {
		ppt_controller_.setSlide(message.slide().offset(), message.slide().relative());
	}
}

void AsioThread::handleConnect(std::shared_ptr<ascf::Client<Protocol>> connection, boost::system::error_code const & error) {
	if (error) {
		throw boost::system::system_error(error);
	} else {
		emit setConnect(true);
		emit setStatus("Connected to server.");
	}
}

void AsioThread::run() {
	emit setStatus("Running IO thread.");

	while (running_) {
		try {
			boost::asio::io_service::work work(ios_);
			ios_.run();
		} catch (std::exception &e) {
			ios_.reset();

			emit setConnect(false);
			emit setStatus("Failed to connect.");
			emit criticalInformation("Connection error", "IO error occured: " + QString(e.what()));
		}
	}
}

void AsioThread::connectRobot(QString host, int port) {
	client_->connect(boost::asio::ip::tcp::endpoint(boost::asio::ip::address_v4::from_string(host.toStdString()), port));
}

void AsioThread::sendScript(QString script) {
	try {
		ClientMessage message;
		message.mutable_run_script()->set_script(script.toStdString());
		client_->sendMessage(message);
		emit setStatus("Sent Run Script command to server.");
	} catch (std::exception const &e) {
		emit criticalInformation("Sending script failed", "Failed to send script command: " + QString(e.what()));
	}
}

}