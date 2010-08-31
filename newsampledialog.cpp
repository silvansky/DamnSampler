#include "newsampledialog.h"
#include "ui_newsampledialog.h"

#include <QFileDialog>
#include <QSettings>

QString NewSampleDialog::lastDir = ".";

NewSampleDialog::NewSampleDialog(Sample * sample, FMOD::System * fmodSystem, QWidget *parent) :
		QDialog(parent),
		ui(new Ui::NewSampleDialog)
{
	QSettings s;
	lastDir = s.value("lastDir").toString();
	if (lastDir.isEmpty())
		lastDir = ".";
	system = fmodSystem;
	channel = 0;
	sound = 0;
	ui->setupUi(this);
	QMap<Qt::Key, QString> kbdKeys = KeyboardKey::getKeysMap();
	foreach(Qt::Key key, kbdKeys.keys())
	{
		ui->keyCombo->addItem("Key " + kbdKeys.value(key), int(key));
	}
	ui->keyCombo->setCurrentIndex(0);
	if (sample)
	{
		ui->nameEdit->setText(sample->name());
		ui->fileNameEdit->setText(sample->fileName());
		switch (sample->loopType())
		{
		case Sample::SimpleLoop:
			ui->simpleLoopRadio->setChecked(true);
			break;
		case Sample::AutoLoop:
			ui->autoLoopRadio->setChecked(true);
			break;
		default:
			ui->noLoopRadio->setChecked(true);
			break;
		}
		for (int i = 0; i < ui->keyCombo->count(); i++)
		{
			if ((Qt::Key)ui->keyCombo->itemData(i).toInt() == sample->key().key())
			{
				ui->keyCombo->setCurrentIndex(i);
				break;
			}
		}
		ui->createButton->setText("Save");
	}
}

NewSampleDialog::~NewSampleDialog()
{
	QSettings s;
	s.setValue("lastDir", lastDir);
	preview(false);
	if (sound)
	{
		sound->release();
//		delete sound;
	}
	if (channel)
	{
//		delete channel;
	}
	delete ui;
}

void NewSampleDialog::changeEvent(QEvent *e)
{
	QDialog::changeEvent(e);
	switch (e->type())
	{
	case QEvent::LanguageChange:
		ui->retranslateUi(this);
		break;
	default:
		break;
	}
}

void NewSampleDialog::preview(bool start)
{
	if (start)
	{
		FMOD_RESULT res = system->createSound(ui->fileNameEdit->text().toLatin1().data(), FMOD_SOFTWARE | FMOD_LOOP_NORMAL, 0, &sound);
		if (res == FMOD_OK)
		{
			system->playSound((channel ? FMOD_CHANNEL_REUSE : FMOD_CHANNEL_FREE), sound, true, &channel);
			channel->setPaused(false);
			ui->browseButton->setEnabled(false);
			ui->fileNameEdit->setEnabled(false);
		}
		else
			ui->previewButton->setChecked(false);
	}
	else
	{
		if (channel)
			channel->stop();
		ui->browseButton->setEnabled(true);
		ui->fileNameEdit->setEnabled(true);
	}
}

QString NewSampleDialog::getName() const
{
	return ui->nameEdit->text();
}

QString NewSampleDialog::getFileName() const
{
	return ui->fileNameEdit->text();
}

int NewSampleDialog::getKey() const
{
	return ui->keyCombo->currentIndex();
}

Sample::LoopType NewSampleDialog::getLoopType() const
{

	if (ui->simpleLoopRadio->isChecked())
		return Sample::SimpleLoop;
	if (ui->autoLoopRadio->isChecked())
		return Sample::AutoLoop;
	return Sample::NoLoop;
}

KeyboardKey NewSampleDialog::keyboardKey() const
{
	return KeyboardKey((Qt::Key)ui->keyCombo->itemData(ui->keyCombo->currentIndex()).toInt());
}

void NewSampleDialog::on_browseButton_clicked()
{
	QString file = QFileDialog::getOpenFileName(this, "Select file", lastDir, "Sound files (*.wav *.ogg *.mp3)");
	if (!file.isEmpty())
	{
		ui->fileNameEdit->setText(file);
		QFileInfo info(file);
		lastDir = info.dir().absolutePath();
	}
}

void NewSampleDialog::on_createButton_clicked()
{
	preview(false);
	accept();
}

void NewSampleDialog::on_cancelButton_clicked()
{
	preview(false);
	reject();
}

void NewSampleDialog::on_previewButton_toggled(bool checked)
{
	preview(checked);
}
