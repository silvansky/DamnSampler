#include "sampleparams.h"
#include "ui_sampleparams.h"

SampleParams::SampleParams(Sample * sample, QWidget *parent) :
		QWidget(parent),
		ui(new Ui::SampleParams)
{
	ui->setupUi(this);
	mySample = sample;
	connect(mySample, SIGNAL(started()), SLOT(onSampleStarted()));
	connect(mySample, SIGNAL(stopped()), SLOT(onSampleStopped()));
	updateSampleInfo();
	connect(ui->volumeSlider, SIGNAL(sliderMoved(int)), mySample, SLOT(setVolume(int)));
	connect(ui->volumeSlider, SIGNAL(valueChanged(int)), mySample, SLOT(setVolume(int)));
	connect(ui->panDial, SIGNAL(sliderMoved(int)), mySample, SLOT(setPanning(int)));
	connect(ui->panDial, SIGNAL(valueChanged(int)), mySample, SLOT(setPanning(int)));
	connect(ui->keyCombo, SIGNAL(currentIndexChanged(int)), SLOT(onKeyComboChanged(int)));
	connect(ui->noLoopRadio, SIGNAL(toggled(bool)), SLOT(onRadioChanged(bool)));
	connect(ui->simpleLoopRadio, SIGNAL(toggled(bool)), SLOT(onRadioChanged(bool)));
	connect(ui->autoLoopRadio, SIGNAL(toggled(bool)), SLOT(onRadioChanged(bool)));
}

SampleParams::~SampleParams()
{
	delete ui;
}

void SampleParams::updateSampleInfo()
{
	ui->namLabel->setText(mySample->name());
	ui->filenameLabel->setText(mySample->fileName());
	ui->durationLabel->setText(msToString(mySample->length()));
	switch (mySample->loopType())
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

	QMap<Qt::Key, QString> kbdKeys = KeyboardKey::getKeysMap();
	foreach(Qt::Key key, kbdKeys.keys())
		ui->keyCombo->addItem("Key " + kbdKeys.value(key), int(key));
	for (int i = 0; i < ui->keyCombo->count(); i++)
	{
		if ((Qt::Key)ui->keyCombo->itemData(i).toInt() == mySample->key().key())
		{
			ui->keyCombo->setCurrentIndex(i);
			break;
		}
	}
	ui->volumeSlider->setValue(mySample->getVolume());
	ui->panDial->setValue(mySample->getPan());
}

void SampleParams::changeEvent(QEvent *e)
{
	QWidget::changeEvent(e);
	switch (e->type()) {
	case QEvent::LanguageChange:
		ui->retranslateUi(this);
		break;
	default:
		break;
	}
}

QString SampleParams::msToString(unsigned int time)
{
	// milliseconds
	qint64 msecs = time % 1000;
	qint64 secs = (time / 1000) % 60;
	qint64 mins = (time / 1000) / 60;
	return QString("%1:%2:%3").arg(mins).arg(secs, 2, 10, QChar('0')).arg(msecs, 3, 10, QChar('0'));
}

void SampleParams::onSampleStarted()
{
	ui->namLabel->setStyleSheet("background-color: red;");
}

void SampleParams::onSampleStopped()
{
	ui->namLabel->setStyleSheet("");
}

void SampleParams::onKeyComboChanged(int index)
{
	mySample->setKey(KeyboardKey((Qt::Key)ui->keyCombo->itemData(index).toInt()));
}

void SampleParams::onRadioChanged(bool on)
{
	if (on)
	{
		Sample::LoopType lt = Sample::NoLoop;;
		if (sender() == ui->simpleLoopRadio)
			lt = Sample::SimpleLoop;
		if (sender() == ui->autoLoopRadio)
			lt = Sample::AutoLoop;
		mySample->setLoopType(lt);
	}
}
