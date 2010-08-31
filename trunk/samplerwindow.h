#ifndef SAMPLERWINDOW_H
#define SAMPLERWINDOW_H

#include <QMainWindow>
#include "sample.h"
#include "sampleparams.h"
#include <fmod.hpp>

namespace Ui {
	class SamplerWindow;
}

class SamplerWindow : public QMainWindow
{
	Q_OBJECT

public:
	explicit SamplerWindow(QWidget *parent = 0);
	~SamplerWindow();

protected:
	void changeEvent(QEvent *e);
	void keyPressEvent(QKeyEvent *);
	void keyReleaseEvent(QKeyEvent *);
	void closeEvent(QCloseEvent *);
private:
	void saveSamplerState(const QString& file);
	void loadSamplerState(const QString& file);

private:
	Ui::SamplerWindow *ui;
	QList<Sample*> samples;
	FMOD::System * system;
	bool running;
	QString stateName;
	QString stateFile;
	bool stateSaved;
	static QString lastStateDir;

private slots:
	void on_actionEdit_sample_triggered();
 void on_actionRemove_sample_triggered();
 void on_actionClear_state_triggered();
	void on_actionRestore_state_triggered();
	void on_actionSave_state_triggered();
	void on_actionAdd_sample_triggered();
	void on_actionStop_triggered();
	void on_actionStart_triggered();
};

#endif // SAMPLERWINDOW_H
