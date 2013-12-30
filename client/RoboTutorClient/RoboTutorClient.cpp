#include "StdAfx.h"

namespace robotutor {

RoboTutorClient::RoboTutorClient(AsioThread & io, QWidget *parent) :
	io_(io), QMainWindow(parent), status_label_(""), connected_(false), paused_(false)
{
	ui_.setupUi(this);

	ui_.statusBar->addWidget(&status_label_);
	highlighter_ = new ScriptHighlighter(ui_.scriptEditor->document());

	new QShortcut(QKeySequence("Ctrl+R"), this, SLOT(on_runButton_clicked()));
	new QShortcut(QKeySequence("Ctrl+T"), this, SLOT(on_stopButton_clicked()));

    ui_.serverEdit->addItem("192.168.0.103");
    ui_.serverEdit->addItem("127.0.0.1");

	timer = new QTimer(this);
    bool ba = connect(timer, SIGNAL(timeout()), this, SLOT(PPmonitor()));
    timer->start(1000);

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
			io_.getIos().post(std::bind(&AsioThread::connectRobot, &io_, ui_.serverEdit->currentText(), ui_.portSpinBox->value()));
			ui_.connectButton->setEnabled(false);
			setStatus("Connecting...");
		}
	} catch (std::exception const &e) {
		//QMessageBox::critical(this, tr("Cannot connect"), QString("Failed to connect to the server: ") + QString(e.what()));
		log(QString("Failed to connect to the server: ") + QString(e.what()));
	}



	//---------
	/*
	 QPalette* palette = new QPalette;
	 

	if(ui_.powerpointIndicator->text() == "Powerpoint Status: Linked")
	{ui_.powerpointIndicator->setText("Powerpoint Status: Link Broken!");
	palette->setColor(QPalette::ColorRole::WindowText, Qt::red);
	ui_.powerpointIndicator->setPalette(*palette);}

	else
	{ui_.powerpointIndicator->setText("Powerpoint Status: Linked");
	palette->setColor(QPalette::ColorRole::WindowText, Qt::darkGreen);
	ui_.powerpointIndicator->setPalette(*palette);

	
	}
	*/
}

void RoboTutorClient::powerpointDisconnect()
{
	QPalette* palette = new QPalette;
	ui_.powerpointIndicator->setText("Powerpoint Status: Link Broken!");
	palette->setColor(QPalette::ColorRole::WindowText, Qt::red);
	ui_.powerpointIndicator->setPalette(*palette);
}


void RoboTutorClient::PPmonitor()
{

//	log("timer triggered");
	io_.getIos().post(std::bind(&AsioThread::monitor, &io_));

}


void RoboTutorClient::on_runButton_clicked() {
	if (ui_.runButton->isEnabled()){
		QString script = ui_.scriptEditor->toPlainText();
	
		for(int i=0;i<ui_.lineSkipSpinBox->value();i++)
		{
			//iterator start = script.begin();
			int line_end = script.indexOf(QRegExp("[\r\n]"));
			script.remove(0,line_end+1);
		}
		io_.getIos().post(std::bind(&AsioThread::sendScript, &io_, script));

	}
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
	if (ui_.stopButton->isEnabled())
		io_.getIos().post(std::bind(&AsioThread::stopScript, &io_));
}


void RoboTutorClient::on_insertBehaviorButton_clicked(){

	QString behavior = ui_.behaviorComboBox->currentText();
	behavior.replace(QString(" - "), QString("/"));
	behavior.prepend(QString("{behavior|robotutor/"));
	behavior.append(QString("}"));
	ui_.scriptEditor->textCursor().insertText(behavior);

}

void RoboTutorClient::on_ResetSkipLinesButton_clicked()
{
    ui_.lineSkipSpinBox->setValue(0);
}

}
