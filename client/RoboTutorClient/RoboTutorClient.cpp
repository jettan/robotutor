#include "RoboTutorClient.h"

namespace robotutor {

RoboTutorClient::RoboTutorClient(QWidget *parent) :
	QMainWindow(parent), ppt_controller_(new PptController), client_(ascf::Client<Protocol>::create(ios_)), status_label_(""),
	timer_(new QTimer(this)), connected_(false)
{
	ui_.setupUi(this);
	client_->message_handler = std::bind(&RoboTutorClient::handleServerMessage, this, std::placeholders::_1, std::placeholders::_2);
	client_->connect_handler = std::bind(&RoboTutorClient::handleConnect, this, std::placeholders::_1, std::placeholders::_2);

	ui_.statusBar->addWidget(&status_label_);
	highlighter_ = new ScriptHighlighter(ui_.scriptEditor->document());

	connect(timer_, SIGNAL(timeout()), this, SLOT(processOnce()));
	timer_->start();

	CoInitializeEx(NULL, COINIT_MULTITHREADED);
}

RoboTutorClient::~RoboTutorClient() {
	delete ppt_controller_;
	delete highlighter_;
	delete timer_;
}

void RoboTutorClient::processOnce() {
	try {
		ios_.poll_one();
	} catch (std::exception const &e) {
		QMessageBox::critical(this, tr("Cannot connect"), QString("Failed to connect to the server: ") + QString(e.what()));
		setStatus("Connection closed.");
		connected_ = false;
		ui_.connectButton->setEnabled(true);
		ui_.runButton->setEnabled(false);
		client_->close();
	}
}

void RoboTutorClient::on_presentationButton_clicked() {
	QString fileName = QFileDialog::getOpenFileName(this, tr("Choose Presentation"), "", tr("Powerpoint Files (*.ppt *.pptx);;All files (*.*)"));
	if (fileName != "")
		ui_.presentationEdit->setText(fileName);
}

void RoboTutorClient::on_openScript_triggered() {
	QString fileName = QFileDialog::getOpenFileName(this, tr("Open Script"), "", tr("Text Files (*.txt);;All files (*.*)"));
	if (fileName == "") // no file selected
		return;

	QFile script(fileName);
	if (script.open(QIODevice::ReadOnly | QIODevice::Text) == false)
	{
		QMessageBox::warning(this, tr("Cannot open file"), "The file that was selected could not be opened.");
		return;
	}

	ui_.scriptEditor->setPlainText(script.readAll());

	setScriptPath(fileName);

	setStatus("Opened filed \'" + fileName + "\'");
}

void RoboTutorClient::on_saveScript_triggered() {
	if (script_path_ != "")
		saveScript(script_path_);
	else
		on_saveScriptAs_triggered();
}

void RoboTutorClient::on_saveScriptAs_triggered() {
	QString fileName = QFileDialog::getSaveFileName(this, tr("Save Script"), script_path_, tr("Text Files (*.txt);;All files (*.*)"));
	if (fileName == "") // no file selected
		return;

	saveScript(fileName);
}

void RoboTutorClient::on_connectButton_clicked() {
	try {
		client_->connect(boost::asio::ip::tcp::endpoint(boost::asio::ip::address_v4::from_string(ui_.serverEdit->text().toStdString()), ui_.portSpinBox->value()));
		ui_.connectButton->setEnabled(false);
		setStatus("Connecting...");
	} catch (std::exception const &e) {
		QMessageBox::critical(this, tr("Cannot connect"), QString("Failed to connect to the server: ") + QString(e.what()));
	}
}

void RoboTutorClient::on_runButton_clicked() {
	try {
		ClientMessage message;
		message.mutable_run_script()->set_script(ui_.scriptEditor->toPlainText().toStdString());
		client_->sendMessage(message);
		setStatus("Sent Run Script command to server.");
	} catch (std::exception const &e) {
		QMessageBox::critical(this, tr("Cannot connect"), QString("Failed to send script command: ") + QString(e.what()));
	}
}

void RoboTutorClient::saveScript(QString fileName) {
	QFile script(fileName);
	if (script.open(QIODevice::WriteOnly | QIODevice::Text) == false)
	{
		QMessageBox::warning(this, tr("Cannot save file"), "The file that was selected could not be saved.");
		return;
	}

	QTextStream outStream(&script);
	outStream << ui_.scriptEditor->toPlainText();
	script.close();

	setScriptPath(fileName);
	setStatus("Saved file to \'" + script_path_ + "\'");
}

void RoboTutorClient::setScriptPath(QString path) {
	script_path_ = path;
	if (path == "")
		setWindowTitle("RoboTutorClient");
	else
		setWindowTitle("RoboTutorClient - " + path);
}

void RoboTutorClient::setStatus(QString status) {
	status_label_.setText(status);
}

void RoboTutorClient::handleServerMessage(std::shared_ptr<ascf::Client<Protocol>> connection, RobotMessage const & message) {
	if (message.has_slide() && ppt_controller_ != NULL) {
		ppt_controller_->setSlide(message.slide().offset(), message.slide().relative());
	}
}

void RoboTutorClient::handleConnect(std::shared_ptr<ascf::Client<Protocol>> connection, boost::system::error_code const & error) {
	if (error) {
		throw boost::system::system_error(error);
	} else {
		connected_ = true;
		ui_.runButton->setEnabled(true);
		setStatus("Connected to server.");
	}
}

}
