#include "StdAfx.h"

namespace robotutor {

AsioThread::AsioThread(boost::asio::io_service & ios) :
	ios_(ios), 
	running_(true), client_(ascf::Client<Protocol>::create(ios_))
{
	client_->message_handler = std::bind(&AsioThread::handleServerMessage, this, std::placeholders::_1, std::placeholders::_2);
	client_->connect_handler = std::bind(&AsioThread::handleConnect, this, std::placeholders::_1, std::placeholders::_2);

	turning_point_path_ = QString(getenv("APPDATA")) + QString("\\Turning Technologies\\TurningPoint\\Current Session\\");
}

void AsioThread::connectSlots(RoboTutorClient & gui) {
	connect(this, SIGNAL(setStatus(QString)), &gui, SLOT(setStatus(QString)));
	connect(this, SIGNAL(setConnect(bool)), &gui, SLOT(setConnect(bool)));
	connect(this, SIGNAL(log(QString)), &gui, SLOT(log(QString)));
}

void AsioThread::quit() {
	running_ = false;
	ios_.stop();
}

void AsioThread::parseTpXml() {
	QString filename("TPSession.xml");
	int errorLine, errorColumn;
	QString errorMsg;
	QFile file(turning_point_path_ + filename);
	QDomDocument document;

	if (!document.setContent(&file, &errorMsg, &errorLine, &errorColumn)) {
		QString error("Syntax error line %1, column %2:\n%3");
		error = error.arg(errorLine).arg(errorColumn).arg(errorMsg);
		log(error);
		return;
	}

	QDomElement lastQuestionNode = document.firstChild().firstChild().lastChild().toElement();
	QDomElement responsesNode = lastQuestionNode.lastChild().toElement();
	QDomElement answersNode = responsesNode.previousSibling().toElement();
	
	int participants = document.firstChild().lastChild().lastChild().toElement().attribute("count").toInt();

	std::vector<std::pair<QString, int>> responses;
	for (QDomNode node = answersNode.firstChild(); !node.isNull(); node = node.nextSibling()) {
		responses.push_back(std::make_pair(node.firstChild().toElement().text(), 0));
	}

	for (QDomNode node = responsesNode.firstChild(); !node.isNull(); node = node.nextSibling()) {
		QDomElement element = node.toElement();
		QString test = element.text();
		responses[element.text().toInt() - 1].second++;
	}

	ClientMessage message;
	for (int i = 0; i < responses.size(); i++) {
		message.mutable_turningpoint()->add_answers(responses[i].first.toStdString());
		message.mutable_turningpoint()->add_votes(responses[i].second);
	}
	client_->sendMessage(message);

	file.close();
}

void AsioThread::handleServerMessage(std::shared_ptr<ascf::Client<Protocol>> connection, RobotMessage const & message) {
	if (message.has_slide()) {// && ppt_controller_ != NULL) {
		ppt_controller_.setSlide(message.slide().offset(), message.slide().relative());
	}
	
	if (message.has_show_image()) {
		ppt_controller_.createSlide("http://" + host_ + "/capture.jpg");
	}

	if (message.has_fetch_turningpoint()) {
		parseTpXml();
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

	CoInitializeEx(NULL, COINIT_MULTITHREADED);

	ppt_controller_.init();

	while (running_) {
		try {
			boost::asio::io_service::work work(ios_);
			ios_.run();
		} catch (std::exception &e) {
			ios_.reset();
			client_->close();

			emit setConnect(false);
			emit setStatus("Failed to connect.");
			emit log("[Critical] IO error occured: " + QString(e.what()));
		}
	}

	ppt_controller_.closePowerpoint();
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