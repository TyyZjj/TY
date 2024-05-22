#ifndef _TQTGUI_H_
#define _TQTGUI_H_

#include <QString>
#include <QTranslator>

class TQtGui
{
public:



private:

};


//改变样式
template<typename T>
static bool InstallStyle(T* window, QString styleFile)
{
	QFile file(styleFile);
	QString strStyleSheet;
	if (!file.open(QFile::ReadOnly | QFile::Text))
		strStyleSheet = "";
	else
		strStyleSheet = file.readAll();
	file.close();
	if (strStyleSheet.isEmpty())
		return false;
	window->setStyleSheet(strStyleSheet);
	return true;
}


//设置翻译
template<typename QAPP>
static void SetTranslator(const QString& fliename, QAPP qApp)
{
	static QTranslator* ptranslator = nullptr;
	if (ptranslator)
	{
		qApp->removeTranslator(ptranslator);
		delete ptranslator;
		ptranslator = nullptr;
	}
	ptranslator->load(fliename);
	qApp->installTranslator(ptranslator);
};



#endif // !_TQTGUI_H_
