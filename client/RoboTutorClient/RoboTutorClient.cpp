#include "StdAfx.h"

namespace robotutor {

RoboTutorClient::RoboTutorClient(AsioThread & io, QWidget *parent) :
	io_(io), QMainWindow(parent), status_label_(""), connected_(false), paused_(false)
{
	ui_.setupUi(this);

	ui_.statusBar->addWidget(&status_label_);
	highlighter_ = new ScriptHighlighter(ui_.scriptEditor->document());
}

RoboTutorClient::~RoboTutorClient() {
	delete highlighter_;
}

void RoboTutorClient::log(QString info) {
	ui_.logEdit->append(info);
}

void RoboTutorClient::on_openScript_triggered() {
	QString fileName = QFileDialog::getOpenFileName(this, tr("Open Script"), "", tr("Text Files (*.txt);;All files (*.*)"));
	if (fileName == "") // no file selected
		return;

	QFile script(fileName);
	if (script.open(QIODevice::ReadOnly | QIODevice::Text) == false)
	{
		//QMessageBox::warning(this, tr("Cannot open file"), "The file that was selected could not be opened.");
		log("The file that was selected could not be opened.");
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
		if (connected_) {
			io_.getIos().post(std::bind(&AsioThread::disconnect, &io_));
			setStatus("Disconnected...");
			setConnect(false);
		}
		else {
			io_.getIos().post(std::bind(&AsioThread::connectRobot, &io_, ui_.serverEdit->text(), ui_.portSpinBox->value()));
			ui_.connectButton->setEnabled(false);
			setStatus("Connecting...");
		}
	} catch (std::exception const &e) {
		//QMessageBox::critical(this, tr("Cannot connect"), QString("Failed to connect to the server: ") + QString(e.what()));
		log(QString("Failed to connect to the server: ") + QString(e.what()));
	}
}

void RoboTutorClient::on_runButton_clicked() {
	io_.getIos().post(std::bind(&AsioThread::sendScript, &io_, ui_.scriptEditor->toPlainText()));
}

void RoboTutorClient::saveScript(QString fileName) {
	QFile script(fileName);
	if (script.open(QIODevice::WriteOnly | QIODevice::Text) == false)
	{
		//QMessageBox::warning(this, tr("Cannot save file"), "The file that was selected could not be saved.");
		log("The file that was selected could not be saved.");
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

void RoboTutorClient::setConnect(bool status) {
	connected_ = status;
	ui_.connectButton->setEnabled(true);
	ui_.connectButton->setText(connected_ ? "Disconnect" : "Connect");
	ui_.runButton->setEnabled(status);
	ui_.serverEdit->setEnabled(!status);
	ui_.portSpinBox->setEnabled(!status);
	ui_.pauseButton->setEnabled(status);
	ui_.stopButton->setEnabled(status);
}

void RoboTutorClient::on_pauseButton_clicked() {
	paused_ = !paused_;
	ui_.pauseButton->setText(paused_ ? "Unpause" : "Pause");
	io_.getIos().post(std::bind(&AsioThread::pauseScript, &io_, paused_));
}

void RoboTutorClient::on_stopButton_clicked() {
	io_.getIos().post(std::bind(&AsioThread::stopScript, &io_));
}

}
