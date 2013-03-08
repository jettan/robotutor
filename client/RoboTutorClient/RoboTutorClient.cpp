#include "AsioThread.h"
#include "RoboTutorClient.h"

namespace robotutor {

RoboTutorClient::RoboTutorClient(QWidget *parent) :
	QMainWindow(parent), status_label_(""), connected_(false)
{
	ui_.setupUi(this);

	ui_.statusBar->addWidget(&status_label_);
	highlighter_ = new ScriptHighlighter(ui_.scriptEditor->document());

	CoInitializeEx(NULL, COINIT_MULTITHREADED);
}

RoboTutorClient::~RoboTutorClient() {
	delete highlighter_;
}

void RoboTutorClient::connectSlots(AsioThread & asio) {
	connect(this, SIGNAL(connectRobot(QString, int)), &asio, SLOT(connectRobot(QString, int)));
	connect(this, SIGNAL(sendScript(QString)), &asio, SLOT(sendScript(QString)));
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
		emit connectRobot(ui_.serverEdit->text(), ui_.portSpinBox->value());
		ui_.connectButton->setEnabled(false);
		setStatus("Connecting...");
	} catch (std::exception const &e) {
		QMessageBox::critical(this, tr("Cannot connect"), QString("Failed to connect to the server: ") + QString(e.what()));
	}
}

void RoboTutorClient::on_runButton_clicked() {
	emit sendScript(ui_.scriptEditor->toPlainText());
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

void RoboTutorClient::setConnect(bool status) {
	ui_.connectButton->setEnabled(!status);
	ui_.runButton->setEnabled(status);
}

void RoboTutorClient::criticalInformation(QString title, QString info) {
	QMessageBox::critical(this, title, info);
}

}
