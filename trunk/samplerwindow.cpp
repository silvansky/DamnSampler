#include "samplerwindow.h"
#include "ui_samplerwindow.h"
#include "newsampledialog.h"
#include <QMessageBox>
#include <fmod_errors.h>
#include <QKeyEvent>
#include <QCloseEvent>
#include <QShowEvent>
#include <QSettings>
#include <QFileDialog>
#include <QDomDocument>
#include <QFile>
#include <QFileInfo>
#include <QDebug>

#define MAX_CHANNELS 32

QString SamplerWindow::lastStateDir;

SamplerWindow::SamplerWindow(QWidget *parent) :
		QMainWindow(parent),
		ui(new Ui::SamplerWindow)
{
	QSettings s;
	lastStateDir = s.value("lastStateDir").toString();
	if (lastStateDir.isEmpty())
		lastStateDir = ".";
	ui->setupUi(this);
	FMOD_RESULT res = FMOD::System_Create(&system);
	if (res != FMOD_OK)
	{
		QMessageBox::critical(0, "FMOD Error!", QString("FMOD returned error: %1 (%2)").arg(FMOD_ErrorString(res)).arg(res));
		close();
	}
	res = system->init(MAX_CHANNELS, FMOD_INIT_NORMAL, 0);
	if (res != FMOD_OK)
	{
		QMessageBox::critical(0, "FMOD Error!", QString("FMOD returned error: %1 (%2)").arg(FMOD_ErrorString(res)).arg(res));
		close();
	}
	running = false;
	stateSaved = true;
	firstShow = true;
	ui->actionStop->setEnabled(false);
}

SamplerWindow::~SamplerWindow()
{
	QSettings s;
	s.setValue("lastStateDir", lastStateDir);
	delete ui;
}

void SamplerWindow::changeEvent(QEvent *e)
{
	QMainWindow::changeEvent(e);
	switch (e->type()) {
	case QEvent::LanguageChange:
		ui->retranslateUi(this);
		break;
	default:
		break;
	}
}

void SamplerWindow::keyPressEvent(QKeyEvent * ke)
{
	if (running)
	{
		if (ke->isAutoRepeat())
			return;
		Qt::Key key = (Qt::Key)ke->key();
		foreach (Sample * sample, samples)
		{
			if (sample->key().key() == key)
			{
				if (sample->loopType() != Sample::AutoLoop)
					sample->start();
				else
				{
					if (sample->isPlaying())
						sample->stop();
					else
						sample->start();
				}
			}
		}
	}
	else
		QMainWindow::keyPressEvent(ke);
}

void SamplerWindow::keyReleaseEvent(QKeyEvent * ke)
{
	if (running)
	{
		if (ke->isAutoRepeat())
			return;
		Qt::Key key = (Qt::Key)ke->key();
		foreach (Sample * sample, samples)
		{
			if ((sample->loopType() != Sample::AutoLoop) && (sample->key().key() == key))
			{
				sample->stop();
			}
		}
	}
	else
		QMainWindow::keyReleaseEvent(ke);
}

void SamplerWindow::closeEvent(QCloseEvent * ce)
{
	if (!stateSaved)
	{
		int code = QMessageBox::question(this, "State not saved", "Do you want to save current state?", QMessageBox::Yes, QMessageBox::No, QMessageBox::Cancel);
		switch (code)
		{
		case QMessageBox::Yes:
			on_actionSave_state_triggered();
			break;
		case QMessageBox::Cancel:
			ce->ignore();
			return;
			break;
		default:
			break;
		}
	}
	QSettings s;
	s.setValue("windowState", saveState());
	s.setValue("windowGeometry", saveGeometry());
	QMainWindow::closeEvent(ce);
}

void SamplerWindow::showEvent(QShowEvent * e)
{
	if (firstShow)
	{
		firstShow = false;
		QSettings s;
		restoreState(s.value("windowState").toByteArray());
		restoreGeometry(s.value("windowGeometry").toByteArray());
	}
	QMainWindow::showEvent(e);
}

void SamplerWindow::saveSamplerState(const QString& file)
{
	if (!file.isEmpty())
	{
		QDomDocument doc;
		QDomElement root = doc.createElement("SamplerState");
		doc.appendChild(root);
		foreach (Sample * sample, samples)
		{
			QDomElement sampleEl = doc.createElement("Sample");
			sampleEl.setAttribute("name", sample->name());
			sampleEl.setAttribute("file", sample->fileName());
			sampleEl.setAttribute("key", sample->key().key());
			sampleEl.setAttribute("loopType", sample->loopType());
			sampleEl.setAttribute("volume", sample->getVolume());
			sampleEl.setAttribute("pan", sample->getPan());
			root.appendChild(sampleEl);
		}
		QFile f(file);
		if (f.open(QFile::WriteOnly | QFile::Truncate))
		{
			f.write(doc.toString().toUtf8());
			QFileInfo info(file);
			stateName = info.baseName();
			stateFile = file;
			setWindowTitle(stateName + " - DamnSampler");
		}
		else
			QMessageBox::critical(this, "Error", QString("Can\'t open file %1 for writing!").arg(file), QMessageBox::Ok);
	}
}

void SamplerWindow::loadSamplerState(const QString& file)
{
	if (!file.isEmpty())
	{
		QFile f(file);
		if (f.open(QFile::ReadOnly))
		{
			QByteArray fc = f.readAll();
			qDebug() << fc;
			QDomDocument doc;
			doc.setContent(QString::fromUtf8(fc));
			if (!doc.isNull())
			{
				qDebug() << doc.firstChild().nodeName();
				QDomElement root = doc.firstChildElement("SamplerState");
				if (!root.isNull())
				{
					on_actionClear_state_triggered();
					QList<Sample*> tempList;
					QDomElement sampleEl = root.firstChildElement("Sample");
					while (!sampleEl.isNull())
					{
						SampleInfo info;
						info.name = sampleEl.attribute("name");
						info.file = sampleEl.attribute("file");
						info.key = KeyboardKey((Qt::Key)sampleEl.attribute("key").toInt());
						info.loopType = (Sample::LoopType)sampleEl.attribute("loopType").toInt();
						info.volume = sampleEl.attribute("volume").toInt();
						info.panning = sampleEl.attribute("pan").toInt();
						tempList.append(new Sample(info, system, tempList.count()));
						sampleEl = sampleEl.nextSiblingElement("Sample");
					}
					samples = tempList;
					foreach (Sample* sample, samples)
					{
						SampleParams * sp = new SampleParams(sample);
						ui->samplesWidget->layout()->addWidget(sp);
					}
					stateSaved = true;
					QFileInfo info(file);
					stateName = info.baseName();
					stateFile = file;
					setWindowTitle(stateName + " - DamnSampler");
				}
				else
					QMessageBox::critical(this, "Error", QString("File %1 is not a valid state file!").arg(file), QMessageBox::Ok);
			}
			else
				QMessageBox::critical(this, "Error", QString("File %1 is not a valid state file!").arg(file), QMessageBox::Ok);
		}
		else
			QMessageBox::critical(this, "Error", QString("Can\'t open file %1!").arg(file), QMessageBox::Ok);
	}
}

void SamplerWindow::onSampleChanged()
{
	stateSaved = false;
}

void SamplerWindow::on_actionAdd_sample_triggered()
{
	bool wasRunning = running;
	if (running)
		on_actionStop_triggered();
	if (samples.count() < MAX_CHANNELS)
	{
		NewSampleDialog * dlg = new NewSampleDialog(0, system, this);
		if (dlg->exec() == QDialog::Accepted)
		{
			Sample * sample = new Sample(dlg->getName(), dlg->getFileName(), dlg->getLoopType(), dlg->keyboardKey(), system, samples.count());
			samples.append(sample);
			SampleParams * sp = new SampleParams(sample);
			ui->samplesWidget->layout()->addWidget(sp);
			stateSaved = false;
		}
		dlg->deleteLater();
	}
	else
		QMessageBox::critical(this, "No free channels", QString("Only %1 channels are allowed in this version.").arg(MAX_CHANNELS), QMessageBox::Ok);
	if (wasRunning)
		on_actionStart_triggered();
}

void SamplerWindow::on_actionStart_triggered()
{
	if (!running)
	{
		grabKeyboard();
		running = true;
		ui->actionStart->setEnabled(false);
		ui->actionStop->setEnabled(true);
	}
}

void SamplerWindow::on_actionStop_triggered()
{
	if (running)
	{
		releaseKeyboard();
		running = false;
		ui->actionStart->setEnabled(true);
		ui->actionStop->setEnabled(false);
		foreach(Sample * sample, samples)
			sample->stop();
	}
}

void SamplerWindow::on_actionSave_state_triggered()
{
	if (stateName.isEmpty() || stateFile.isEmpty())
	{
		QString file = QFileDialog::getSaveFileName(this, "Save state", lastStateDir, "Sampler state files (*.ssf)");
		QFileInfo finfo(file);
		lastStateDir = finfo.dir().absolutePath();
		saveSamplerState(file);
	}
	else
		saveSamplerState(stateFile);
}

void SamplerWindow::on_actionRestore_state_triggered()
{
	QString file = QFileDialog::getOpenFileName(this, "Restore state", lastStateDir, "Sampler state files (*.ssf)");
	QFileInfo finfo(file);
	lastStateDir = finfo.dir().absolutePath();
	loadSamplerState(file);
}

void SamplerWindow::on_actionClear_state_triggered()
{
	if ((!stateSaved) && (QMessageBox::question(this, "Clear state", "Do you want to save current state?", QMessageBox::Yes, QMessageBox::No) == QMessageBox::Yes))
		on_actionSave_state_triggered();
	foreach(Sample* sample, samples)
		sample->deleteLater();
	samples.clear();
	foreach(QObject * obj, ui->samplesWidget->children())
	{
		SampleParams * sp = qobject_cast<SampleParams *>(obj);
		if (sp)
		{
			sp->hide();
			sp->deleteLater();
		}
	}
	stateSaved = true;
	setWindowTitle("DamnSampler");
}

void SamplerWindow::on_actionRemove_sample_triggered()
{
	if (qApp->focusWidget())
	{
		SampleParams * p = qobject_cast<SampleParams *>(qApp->focusWidget()->parent());
		if (p)
		{
			p->hide();
			samples.removeOne(p->sample());
			p->sample()->deleteLater();
			p->deleteLater();
		}
	}
}

void SamplerWindow::on_actionEdit_sample_triggered()
{
	if (qApp->focusWidget())
	{
		SampleParams * p = qobject_cast<SampleParams *>(qApp->focusWidget()->parent());
		if (p)
		{
			NewSampleDialog * dlg = new NewSampleDialog(p->sample(), system, this);
			if (dlg->exec() == QDialog::Accepted)
			{
				p->sample()->setName(dlg->getName());
				p->sample()->setFileName(dlg->getFileName());
				p->sample()->setKey(dlg->keyboardKey());
				p->sample()->setLoopType(dlg->getLoopType());
				p->updateSampleInfo();
			}
			dlg->deleteLater();
		}
	}
}

void SamplerWindow::on_actionExit_triggered()
{
	close();
}
