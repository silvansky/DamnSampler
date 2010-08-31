#ifndef NEWSAMPLEDIALOG_H
#define NEWSAMPLEDIALOG_H

#include <QDialog>
#include <fmod.hpp>
#include <fmod_errors.h>
#include "keyboardkey.h"
#include "sample.h"

namespace Ui {
	class NewSampleDialog;
}

class NewSampleDialog : public QDialog
{
	Q_OBJECT

public:
	NewSampleDialog(Sample * sample, FMOD::System * fmodSystem, QWidget *parent = 0);
	~NewSampleDialog();

	QString getName() const;
	QString getFileName() const;
	int getKey() const;
	Sample::LoopType getLoopType() const;
	KeyboardKey keyboardKey() const;

protected:
	void changeEvent(QEvent *e);
	void preview(bool start);

private:
	Ui::NewSampleDialog *ui;
	// FMOD for preview
	FMOD::System * system;
	FMOD::Sound * sound;
	FMOD::Channel * channel;

	// last dir
	static QString lastDir;

private slots:
	void on_previewButton_toggled(bool checked);
	void on_cancelButton_clicked();
	void on_createButton_clicked();
	void on_browseButton_clicked();
};

#endif // NEWSAMPLEDIALOG_H
