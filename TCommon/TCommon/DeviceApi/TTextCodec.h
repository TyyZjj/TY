#ifndef _TTEXTCODEC_H_
#define _TTEXTCODEC_H_

#include <QTextCodec>

class TTextCodec : public QTextCodec
{
public:
	//************************************
	// 参数: 	const QByteArray & ba: 字符流
	// 参数: 	QByteArray & defaultCodec: 输入输出, 默认编码格式, 返回匹配的编码格式
	// 返回:	QT_NAMESPACE::QString
	// 功能:	返回文本正确的字符串
	//			(基于NotePad的基础编码, 对中/日/俄/阿拉伯语进行了测试, 多语种适配时, 还需要测试与改进)
	//************************************
	static QString GetCorrectUnicode(const QByteArray& ba);
	static QString GetCorrectUnicode(const QByteArray &ba, 
		QByteArray &defaultCodec);

private:
}; 




#endif
