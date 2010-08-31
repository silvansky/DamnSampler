#ifndef SAMPLE_H
#define SAMPLE_H

#include <QObject>
#include <QTimer>
#include <QMetaType>
#include <fmod.hpp>
#include "keyboardkey.h"

struct SampleInfo;

class Sample : public QObject
{
	Q_OBJECT
private:
	void init();
public:
	enum LoopType
	{
		NoLoop,
		SimpleLoop,
		AutoLoop
	};

	Sample(const QString & name, const QString & file, LoopType looping, KeyboardKey kbdKey, FMOD::System * fmodSystem, int numChannel);
	Sample(const SampleInfo & info, FMOD::System * fmodSystem, int numChannel);
	~Sample()
	{
		stop();
		if (sound)
			sound->release();
	}
	QString name() const
	{
		return sampleName;
	}
	QString fileName() const
	{
		return sampleFileName;
	}
	KeyboardKey key() const
	{
		return keyboardKey;
	}
	unsigned int length() const // in ms
	{
		unsigned int l;
		sound->getLength(&l, FMOD_TIMEUNIT_MS);
		return l;
	}
	LoopType loopType() const
	{
		return loop;
	}
	bool isPlaying() const
	{
		return playing;
	}
	int getVolume() const
	{
		return volume;
	}
	int getPan() const
	{
		return panning;
	}
	SampleInfo info() const;
protected:
	bool resOk(FMOD_RESULT res);
	void reload();

signals:
	void started();
	void stopped();
	void changed();
public slots:
	void start();
	void stop();
	void setMute(bool mute);
	// props
	void setVolume(int volume);
	void setPanning(int pan);
	void setLoopType(LoopType loop);
	void setKey(KeyboardKey newKey);
	void setName(const QString & newName);
	void setFileName(const QString & newFile);
protected slots:
	void onFadeOutTimer();
private:
	QString sampleName;
	QString sampleFileName;
	bool playing;
	LoopType loop;
	QTimer fadeOutTimer;
	KeyboardKey keyboardKey;
	int volume;
	int panning;
	int oldVolume;
	bool needReloading;
	// FMOD
	FMOD::System * system;
	FMOD::Sound * sound;
	FMOD::Channel * channel;
	int channelNumber;
};

struct SampleInfo
{
	SampleInfo()
		: key(Qt::Key_Space)
	{}
	QString name;
	QString file;
	Sample::LoopType loopType;
	KeyboardKey key;
	int volume;
	int panning;
};

Q_DECLARE_METATYPE(SampleInfo)
#endif // SAMPLE_H
