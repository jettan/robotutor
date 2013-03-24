#ifndef ROBOTUTORCLIENT_H
#define ROBOTUTORCLIENT_H

class AsioThread;

namespace robotutor {

class AsioThread;

class RoboTutorClient : public QMainWindow
{
	Q_OBJECT

public:
	RoboTutorClient(AsioThread & io, QWidget *parent = 0);
	~RoboTutorClient();

private:
	Ui::RoboTutorClientClass ui_;

	AsioThread & io_;

	bool connected_;
	bool paused_;

	QString script_path_;
	QLabel status_label_;
	QString turning_point_path_;

	ScriptHighlighter *highlighter_;

	void setScriptPath(QString path);
	void saveScript(QString fileName);

	void parseTpXml();

public slots:
	void setStatus(QString status);
	void setConnect(bool status);
	void log(QString info);

private slots:
	void on_openScript_triggered();
	void on_saveScript_triggered();
	void on_saveScriptAs_triggered();
	void on_presentationButton_clicked();
	void on_connectButton_clicked();
	void on_runButton_clicked();
	void on_pauseButton_clicked();
	void on_stopButton_clicked();
};

}

#endif // ROBOTUTORCLIENT_H
