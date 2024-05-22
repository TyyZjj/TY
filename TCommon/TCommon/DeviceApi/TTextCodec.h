#ifndef _TTEXTCODEC_H_
#define _TTEXTCODEC_H_

#include <QTextCodec>

class TTextCodec : public QTextCodec
{
public:
	//************************************
	// ����: 	const QByteArray & ba: �ַ���
	// ����: 	QByteArray & defaultCodec: �������, Ĭ�ϱ����ʽ, ����ƥ��ı����ʽ
	// ����:	QT_NAMESPACE::QString
	// ����:	�����ı���ȷ���ַ���
	//			(����NotePad�Ļ�������, ����/��/��/������������˲���, ����������ʱ, ����Ҫ������Ľ�)
	//************************************
	static QString GetCorrectUnicode(const QByteArray &ba, 
		QByteArray &defaultCodec = QByteArray());

private:
}; 




#endif
