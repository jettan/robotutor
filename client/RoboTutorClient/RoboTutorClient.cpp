#include "StdAfx.h"

namespace robotutor {

RoboTutorClient::RoboTutorClient(AsioThread & io, QWidget *parent) :
	io_(io), QMainWindow(parent), status_label_(""), connected_(false), paused_(false)
{
	ui_.setupUi(this);

	ui_.statusBar->addWidget(&status_label_);
	highlighter_ = new ScriptHighlighter(ui_.scriptEditor->document());

	turning_point_path_ = QString(getenv("APPDATA")) + QString("\\Turning Technologies\\TurningPoint\\Current Session\\");
}

RoboTutorClient::~RoboTutorClient() {
	delete highlighter_;
}

void RoboTutorClient::parseTpXml() {
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

	file.close();
}

void RoboTutorClient::log(QString info) {
	ui_.logEdit->append(info);
}

void RoboTutorClient::on_presentationButton_clicked() {
	QString fileName = QFileDialog::getOpenFileName(this, tr("Choose Presentation"), "", tr("Powerpoint Files (*.ppt *.pptx);;All files (*.*)"));
	if (fileName != "") {
		ui_.presentationEdit->setText(fileName);
		io_.getIos().post(std::bind(&AsioThread::openPresentation, &io_, fileName));
	}
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
