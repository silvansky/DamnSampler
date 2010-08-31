#ifndef KEYBOARDKEY_H
#define KEYBOARDKEY_H

#include <Qt>
#include <QString>
#include <QMap>

class KeyboardKey
{
public:
	KeyboardKey(Qt::Key vkey);
	KeyboardKey(const KeyboardKey &);
	Qt::Key key() const
	{
		return myKey;
	}
	QString symbol() const
	{
		return mySymbol;
	}
private:
	Qt::Key myKey;
	QString mySymbol;
public:
	static QMap<Qt::Key, QString> getKeysMap();
	static QString keyToSymbol(Qt::Key key);
private:
	static QMap<Qt::Key, QString> keysMap;
};

#endif // KEYBOARDKEY_H
