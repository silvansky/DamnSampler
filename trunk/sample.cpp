#include "sample.h"

#include <QMessageBox>
#include <fmod_errors.h>
#include <QDebug>

void Sample::init()
{
	fadeOutTimer.setInterval(30);
	fadeOutTimer.setSingleShot(false);
	fadeOutTimer.stop();
	connect(&fadeOutTimer, SIGNAL(timeout()), SLOT(onFadeOutTimer()));
	needReloading = false;
	playing = false;
	sound = 0;
	channel = 0;
	reload();
}

Sample::Sample(const QString &name, const QString &file, LoopType looping, KeyboardKey kbdKey, FMOD::System *fmodSystem, int numChannel)
	: keyboardKey(kbdKey)
{
	system = fmodSystem;
	sampleName = name;
	sampleFileName = file;
	loop = looping;
	volume = 100;
	panning = 0;
	channelNumber = numChannel;
	init();
}

Sample::Sample(const SampleInfo & info, FMOD::System * fmodSystem, int numChannel)
	: keyboardKey(info.key)
{
	system = fmodSystem;
	sampleName = info.name;
	sampleFileName = info.file;
	loop = info.loopType;
	volume = info.volume;
	panning = info.panning;
	channelNumber = numChannel;
	init();
}

SampleInfo Sample::info() const
{
	SampleInfo info;
	info.name = sampleName;
	info.file = sampleFileName;
	info.key = keyboardKey;
	info.loopType = loop;
	info.volume = volume;
	info.panning = panning;
	return info;
}

bool Sample::resOk(FMOD_RESULT res)
{
	if (res != FMOD_OK)
	{
		qDebug() << QString("FMOD returned error: %1 (%2)").arg(FMOD_ErrorString(res)).arg(res);
		return false;
	}
	else
		return true;

}

void Sample::reload()
{
	FMOD_RESULT result = system->createSound(sampleFileName.toLatin1().data(), FMOD_SOFTWARE | ((loop != NoLoop) ? FMOD_LOOP_NORMAL : FMOD_LOOP_OFF), 0, &sound);
	if (!resOk(result))
		sound = 0;
	needReloading = false;
}

void Sample::start()
{
	if (fadeOutTimer.isActive())
	{
		fadeOutTimer.stop();
		volume = oldVolume;
	}
	playing = resOk(system->playSound((FMOD_CHANNELINDEX)channelNumber, sound, true, &channel));
	if (playing)
	{
		channel->setVolume(volume / 100.0);
		channel->setPan(panning / 100.0);
		FMOD_RESULT result = channel->setPaused(false);
		resOk(result);
		emit started();
	}
}

void Sample::stop()
{
	if (playing)
	{
		oldVolume = volume;
		fadeOutTimer.start();
		playing = false;
	}
}

void Sample::setVolume(int newVolume)
{
	volume = newVolume;
	if (channel)
		channel->setVolume(volume / 100.0);
	emit changed();
}

void Sample::setPanning(int newPan)
{
	panning = newPan;
	if (channel)
		channel->setPan(panning / 100.0);
	emit changed();
}

void Sample::setLoopType(LoopType loopType)
{
	loop = loopType;
	if (sound)
		sound->setMode(FMOD_SOFTWARE | ((loop != NoLoop) ? FMOD_LOOP_NORMAL : FMOD_LOOP_OFF));
	emit changed();
}

void Sample::setKey(KeyboardKey newKey)
{
	keyboardKey = newKey;
	emit changed();
}

void Sample::setName(const QString & newName)
{
	sampleName = newName;
	emit changed();
}

void Sample::setFileName(const QString & newFile)
{
	sampleFileName = newFile;
	if (!playing)
		reload();
	else
		needReloading = true;
	emit changed();
}

void Sample::onFadeOutTimer()
{
	setVolume(volume / 1.2);
	if (!volume)
	{
		channel->stop();
		fadeOutTimer.stop();
		emit stopped();
		volume = oldVolume;
		if (needReloading)
			reload();
	}
}
