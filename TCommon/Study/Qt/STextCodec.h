#ifndef _STEXTCODEC_H_
#define _STEXTCODEC_H_


//QTextCodec的学习注释

/*
一、基本信息
1、可用的编/解码名称:	QList<QByteArray> availableCodecs()
2、可用的编/解码Mib:	QList<int> availableMibs()
3、设置本地编/解码:		void setCodecForLocale(QTextCodec *c)

二、获取QTextCodec
1、适合某编码名称:			QTextCodec* codecForName(const QByteArray &name)
2、适合本地:				QTextCodec* codecForLocale()
3、适合BOM(只适用带BOM头):	QTextCodec *codecForUtfText(const QByteArray &ba)
							QTextCodec *codecForUtfText(const QByteArray &ba, QTextCodec *defaultCodec);
	(1)、尝试使用BOM(字节顺序标记)检测提供的代码段ba的编码, 并返回将ba解码为unicode的QTextCodec实例.
	(2)、此函数可以检测以下编码:
		UTF-32 Little Endian
		UTF-32大端
		UTF-16 Little Endian
		UTF-16大端
		UTF-8
	(3)、如果无法检测到合适的编码, 返回defaultCodec. 没有defaultCodec, 返回Latin-1.
4、适合	HTML:			QTextCodec *codecForHtml(const QByteArray &ba)
						QTextCodec *codecForHtml(const QByteArray &ba, QTextCodec *defaultCodec)
5、适合Mib:				QTextCodec* codecForMib(int mib)

三、转码
1、Unicode->非Unicode	QByteArray fromUnicode(const QChar *in, int length, ConverterState *state = nullptr);
2、非Unicode->Unicode	QString toUnicode(const char *in, int length, ConverterState *state = nullptr)

四、网络传输
1、QTextDecoder* makeDecoder(ConversionFlags flags)
2、QTextEncoder* makeEncoder(ConversionFlags flags)

五、其他
1、bool canEncode(...):			是否可以被编码
2、QList<QByteArray> aliases(): 返回编码器的别名, 如果没有别名,返回空. 如:"ISO-8859-1"的别名"latin1"
*/


#endif



