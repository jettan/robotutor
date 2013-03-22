#include "AsioThread.h"
#include "RoboTutorClient.h"

namespace robotutor {

AsioThread::AsioThread(boost::asio::io_service & ios) :
	ios_(ios), 
	running_(true), client_(ascf::Client<Protocol>::create(ios_))
{
	client_->message_handler = std::bind(&AsioThread::handleServerMessage, this, std::placeholders::_1, std::placeholders::_2);
	client_->connect_handler = std::bind(&AsioThread::handleConnect, this, std::placeholders::_1, std::placeholders::_2);

	CoInitializeEx(NULL, COINIT_MULTITHREADED);

	ppt_controller_.init();
}

void AsioThread::connectSlots(RoboTutorClient & gui) {
	connect(this, SIGNAL(setStatus(QString)), &gui, SLOT(setStatus(QString)));
	connect(this, SIGNAL(setConnect(bool)), &gui, SLOT(setConnect(bool)));
	connect(this, SIGNAL(log(QString)), &gui, SLOT(log(QString)));
}

void AsioThread::quit() {
	ppt_controller_.closePowerpoint();
	running_ = false;
	ios_.stop();
}

void AsioThread::handleServerMessage(std::shared_ptr<ascf::Client<Protocol>> connection, RobotMessage const & message) {
	if (message.has_slide()) {// && ppt_controller_ != NULL) {
		emit setStatus("Message had slide!");
		ppt_controller_.setSlide(message.slide().offset(), message.slide().relative());
	}
	else if (message.has_show_image()) {
		ppt_controller_.createSlide("http://" + host_ + "/capture.jpg");
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
			emit log("[Critical] IO error occured: " + QString(e.what()));
		}
	}
}

void AsioThread::connectRobot(QString host, int port) {
	host_  = host.toStdString();;
	port_  = port;
	client_->connect(boost::asio::ip::tcp::endpoint(boost::asio::ip::address_v4::from_string(host.toStdString()), port));
}

void AsioThread::disconnect() {
	client_->close();
}

void AsioThread::openPresentation(QString file) {
	ppt_controller_.openPresentation(file.toStdString());
}

void AsioThread::sendScript(QString script) {
	try {
		ClientMessage message;
		message.mutable_run()->set_script(script.toStdString());
		client_->sendMessage(message);
		ppt_controller_.startSlideShow();
		emit setStatus("Sent Run Script command to server.");
	} catch (std::exception const &e) {
		emit log("[Critical] Failed to send script command: " + QString(e.what()));
	}
}

void AsioThread::pauseScript(bool pause) {
	ClientMessage message;
	if (pause)
		message.mutable_pause();
	else
		message.mutable_resume();
	client_->sendMessage(message);
}

void AsioThread::stopScript() {
	ClientMessage message;
	message.mutable_stop();
	client_->sendMessage(message);
}

}