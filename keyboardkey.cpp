#include "keyboardkey.h"

KeyboardKey::KeyboardKey(Qt::Key vkey)
{
	myKey = vkey;
	mySymbol = keyToSymbol(vkey);
}

KeyboardKey::KeyboardKey(const KeyboardKey & other)
{
	myKey = other.key();
	mySymbol = other.symbol();
}

QMap<Qt::Key, QString> KeyboardKey::getKeysMap()
{
	if (keysMap.values().isEmpty())
	{
		// filling up map
		keysMap.insert(Qt::Key_0, "0");
		keysMap.insert(Qt::Key_1, "1");
		keysMap.insert(Qt::Key_2, "2");
		keysMap.insert(Qt::Key_3, "3");
		keysMap.insert(Qt::Key_4, "4");
		keysMap.insert(Qt::Key_5, "5");
		keysMap.insert(Qt::Key_6, "6");
		keysMap.insert(Qt::Key_7, "7");
		keysMap.insert(Qt::Key_8, "8");
		keysMap.insert(Qt::Key_9, "9");
		keysMap.insert(Qt::Key_Minus, "-");
		keysMap.insert(Qt::Key_Plus, "+");
		keysMap.insert(Qt::Key_A, "A");
		keysMap.insert(Qt::Key_B, "B");
		keysMap.insert(Qt::Key_C, "C");
		keysMap.insert(Qt::Key_D, "D");
		keysMap.insert(Qt::Key_E, "E");
		keysMap.insert(Qt::Key_F, "F");
		keysMap.insert(Qt::Key_G, "G");
		keysMap.insert(Qt::Key_H, "H");
		keysMap.insert(Qt::Key_I, "I");
		keysMap.insert(Qt::Key_J, "J");
		keysMap.insert(Qt::Key_K, "K");
		keysMap.insert(Qt::Key_L, "L");
		keysMap.insert(Qt::Key_M, "M");
		keysMap.insert(Qt::Key_N, "N");
		keysMap.insert(Qt::Key_O, "O");
		keysMap.insert(Qt::Key_P, "P");
		keysMap.insert(Qt::Key_Q, "Q");
		keysMap.insert(Qt::Key_R, "R");
		keysMap.insert(Qt::Key_S, "S");
		keysMap.insert(Qt::Key_T, "T");
		keysMap.insert(Qt::Key_U, "U");
		keysMap.insert(Qt::Key_V, "V");
		keysMap.insert(Qt::Key_W, "W");
		keysMap.insert(Qt::Key_X, "X");
		keysMap.insert(Qt::Key_Y, "Y");
		keysMap.insert(Qt::Key_Z, "Z");
		keysMap.insert(Qt::Key_Semicolon, ":");
		keysMap.insert(Qt::Key_Space, "[space]");
		keysMap.insert(Qt::Key_Slash, "/");
		keysMap.insert(Qt::Key_Backslash, "\\");
		keysMap.insert(Qt::Key_QuoteDbl, "\"");
		keysMap.insert(Qt::Key_Less, "<");
		keysMap.insert(Qt::Key_Greater, ">");
	}
	return keysMap;
}

QString KeyboardKey::keyToSymbol(Qt::Key key)
{
	return getKeysMap().value(key, "");
}

QMap<Qt::Key, QString> KeyboardKey::keysMap;
