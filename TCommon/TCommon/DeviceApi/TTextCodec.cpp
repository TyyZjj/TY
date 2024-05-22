#include "TTextCodec.h"

QString TTextCodec::GetCorrectUnicode(const QByteArray &ba, 
	QByteArray &defaultCodec)
{
	if (defaultCodec.isEmpty())
		defaultCodec = "UTF-8";
	
	QTextCodec *defaultcodec = QTextCodec::codecForName(defaultCodec);
	QTextCodec *codec = QTextCodec::codecForUtfText(ba, defaultcodec);
	QTextCodec::ConverterState state;
	QString text = codec->toUnicode(ba.constData(), ba.size(), &state);
	if (state.invalidChars > 0)
	{
		defaultCodec = "GBK";
		codec = QTextCodec::codecForName(defaultCodec);
		text = codec->toUnicode(ba);
	}
	defaultCodec = codec->name();
	QList<QByteArray> lst = codec->aliases();
	return text;
}
