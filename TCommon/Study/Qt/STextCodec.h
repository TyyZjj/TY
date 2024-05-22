#ifndef _STEXTCODEC_H_
#define _STEXTCODEC_H_


//QTextCodec��ѧϰע��

/*
һ��������Ϣ
1�����õı�/��������:	QList<QByteArray> availableCodecs()
2�����õı�/����Mib:	QList<int> availableMibs()
3�����ñ��ر�/����:		void setCodecForLocale(QTextCodec *c)

������ȡQTextCodec
1���ʺ�ĳ��������:			QTextCodec* codecForName(const QByteArray &name)
2���ʺϱ���:				QTextCodec* codecForLocale()
3���ʺ�BOM(ֻ���ô�BOMͷ):	QTextCodec *codecForUtfText(const QByteArray &ba)
							QTextCodec *codecForUtfText(const QByteArray &ba, QTextCodec *defaultCodec);
	(1)������ʹ��BOM(�ֽ�˳����)����ṩ�Ĵ����ba�ı���, �����ؽ�ba����Ϊunicode��QTextCodecʵ��.
	(2)���˺������Լ�����±���:
		UTF-32 Little Endian
		UTF-32���
		UTF-16 Little Endian
		UTF-16���
		UTF-8
	(3)������޷���⵽���ʵı���, ����defaultCodec. û��defaultCodec, ����Latin-1.
4���ʺ�	HTML:			QTextCodec *codecForHtml(const QByteArray &ba)
						QTextCodec *codecForHtml(const QByteArray &ba, QTextCodec *defaultCodec)
5���ʺ�Mib:				QTextCodec* codecForMib(int mib)

����ת��
1��Unicode->��Unicode	QByteArray fromUnicode(const QChar *in, int length, ConverterState *state = nullptr);
2����Unicode->Unicode	QString toUnicode(const char *in, int length, ConverterState *state = nullptr)

�ġ����紫��
1��QTextDecoder* makeDecoder(ConversionFlags flags)
2��QTextEncoder* makeEncoder(ConversionFlags flags)

�塢����
1��bool canEncode(...):			�Ƿ���Ա�����
2��QList<QByteArray> aliases(): ���ر������ı���, ���û�б���,���ؿ�. ��:"ISO-8859-1"�ı���"latin1"
*/


#endif



