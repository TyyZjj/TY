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
static void SetTranslator(const QString& fliename)
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

#pragma region 字符串在界面中的实际大小
#include <QFontMetrics>

//******Taoyuyu******2024/05/24**********
// 功能: (需要在GUI线程中调用)给定一个字符串(无换行)、能显示宽度、font, 得到这么宽的区域实际输出时, 能显示的字符
// 返回: bool: 是否裁切/截断
// 参数: QFont font: 给定的字体
// 参数: QString str: 给定的字符串
// 参数: int iMaxWidth: 能显示的宽度
// 参数: int iMode: 裁切的模式
// 参数: QString & strElidedText: 返回, 裁切后的字符串
//************************************
static bool GetElidedText(QFont font, QString str, int iMaxWidth, int iMode, QString& strElidedText)
{
	bool ret = false;
	QFontMetrics fontWidth(font);
	int iWidth = fontWidth.width(str);  //计算字符串宽度
	if (iWidth >= iMaxWidth)  //当字符串宽度大于最大宽度时进行转换
	{
		strElidedText = fontWidth.elidedText(str, (Qt::TextElideMode)iMode, iMaxWidth);  //右部显示省略号
		ret = true;
	}
	return ret;   //返回是否裁剪
}

//******Taoyuyu******2024/05/24**********
// 功能: (需要在GUI线程中调用)给定高度、字体、字体宽度, 自适应获取适合的文字大小
// 返回: bool: 是否成功
// 参数: const int iMaxHeight: 给定的最大高度
// 参数: QFont & font: 返回值
// 参数: const QString & family: 给定的字体
// 参数: const int weight: 给定的字体宽度
// 参数: const QString str: 给定字符串, 当获取高度信息时, 可以省略
//************************************
static bool GetHeightAdaptiveFont(const int iMaxHeight, QFont &font,
	const QString &family, const int weight = -1, const QString str = "")
{
	font = QFont(family);
	font.setWeight(weight);
	int iCount(0);
	do 
	{
		iCount++;
		int iPointSize = font.pointSize();
		if (iPointSize <= 0)
			return false;
		QFontMetrics metrics(font);
		int iTextHeight = metrics.height();
		if (!str.isEmpty())
			iTextHeight = metrics.boundingRect(str).height();
		int iPointSizeNext(iPointSize);
		if (iTextHeight <= iMaxHeight)
			iPointSizeNext = iPointSize * 2;
		else
			iPointSizeNext = iPointSize / 2;
		font.setPointSize(iPointSizeNext);
		QFontMetrics metricsNext(font);
		int iTextHeightNext = metricsNext.height();
		if (!str.isEmpty())
			iTextHeightNext = metricsNext.boundingRect(str).height();
		if (iTextHeight <= iMaxHeight)
		{
			if (iTextHeightNext > iMaxHeight)
			{
				int iPointSize3rd = (iPointSizeNext + iPointSize) / 2;
				if (iPointSize3rd == iPointSize ||
					iPointSize3rd == iPointSizeNext)
				{
					font.setPointSize(iPointSize);
					return (iPointSize > 0);
				}
				else
				{
					font.setPointSize(iPointSize3rd);
					continue;
				}
			}
			else
			{
				int iPointSize3rd = iPointSizeNext * 2;
				font.setPointSize(iPointSize3rd);
				continue;
			}
		}
		else
		{
			if (iTextHeightNext <= iMaxHeight)
			{
				int iPointSize3rd = (iPointSizeNext + iPointSize) / 2;
				if (iPointSize3rd == iPointSize ||
					iPointSize3rd == iPointSizeNext)
				{
					font.setPointSize(iPointSizeNext);
					return (iPointSizeNext > 0);
				}
				else
				{
					font.setPointSize(iPointSize3rd);
					continue;
				}
			}
			else
			{
				int iPointSize3rd = iPointSizeNext / 2;
				font.setPointSize(iPointSize3rd);
				continue;
			}
		}
	} while (iCount<20);
	return false;
}


//******Taoyuyu******2024/05/24**********
// 功能: (需要在GUI线程中调用)给定字符串、宽度、字体、字体宽度, 自适应获取适合的文字大小
// 返回: bool 是否成功
// 参数: const QString str: 给定字符串
// 参数: const int iMaxWidth: 给定的最大宽度
// 参数: QFont & font: 返回值
// 参数: const QString & family: 给定的字体
// 参数: const int weight: 给定的字体宽度
//************************************
static bool GetWidthAdaptiveFont(const QString str, 
	const int iMaxWidth, QFont &font,
	const QString &family, const int weight = -1)
{
	if (str.isEmpty())
		return false;
	font = QFont(family);
	font.setWeight(weight);
	int iCount(0);
	do
	{
		iCount++;
		int iPointSize = font.pointSize();
		if (iPointSize <= 0)
			return false;
		QFontMetrics metrics(font);
		int iTextWidth = metrics.boundingRect(str).width();
		int iPointSizeNext(iPointSize);
		if (iTextWidth <= iMaxWidth)
			iPointSizeNext = iPointSize * 2;
		else
			iPointSizeNext = iPointSize / 2;
		font.setPointSize(iPointSizeNext);
		QFontMetrics metricsNext(font);
		int iTextWidthNext = metricsNext.boundingRect(str).width();
		if (iTextWidth <= iMaxWidth)
		{
			if (iTextWidthNext > iMaxWidth)
			{
				int iPointSize3rd = (iPointSizeNext + iPointSize) / 2;
				if (iPointSize3rd == iPointSize ||
					iPointSize3rd == iPointSizeNext)
				{
					font.setPointSize(iPointSize);
					return (iPointSize > 0);
				}
				else
				{
					font.setPointSize(iPointSize3rd);
					continue;
				}
			}
			else
			{
				int iPointSize3rd = iPointSizeNext * 2;
				font.setPointSize(iPointSize3rd);
				continue;
			}
		}
		else
		{
			if (iTextWidthNext <= iMaxWidth)
			{
				int iPointSize3rd = (iPointSizeNext + iPointSize) / 2;
				if (iPointSize3rd == iPointSize ||
					iPointSize3rd == iPointSizeNext)
				{
					font.setPointSize(iPointSizeNext);
					return (iPointSizeNext > 0);
				}
				else
				{
					font.setPointSize(iPointSize3rd);
					continue;
				}
			}
			else
			{
				int iPointSize3rd = iPointSizeNext / 2;
				font.setPointSize(iPointSize3rd);
				continue;
			}
		}
	} while (iCount < 20);
	return false;
}


//******Taoyuyu******2024/05/24**********
// 功能: (需要在GUI线程中调用)给定字符串、区域、字体、字体宽度, 自适应获取适合的文字大小
// 返回: bool 是否成功
// 参数: const QRect maxRect: 给定的最大区域
// 参数: const QString str: 给定字符串
// 参数: QFont & font: 返回值
// 参数: const QString & family: 给定的字体
// 参数: const int weight: 给定的字体宽度
//************************************
static bool GetAdaptiveFont(const QRect maxRect,
	const QString str, QFont &font,
	const QString &family, const int weight = -1)
{
	if (str.isEmpty())
		return false;
	font = QFont(family);
	font.setWeight(weight);
	int iCount(0);
	do 
	{
		iCount++;
		int iPointSize = font.pointSize();
		if (iPointSize <= 0)
			return false;
		QFontMetrics metrics(font);
		QRect rect = metrics.boundingRect(str);
		int iPointSizeNext(iPointSize);
		if (rect.width() <= maxRect.width() &&
			rect.height() <= maxRect.height())
			iPointSizeNext = iPointSize * 2;
		else
			iPointSizeNext = iPointSize / 2;
		font.setPointSize(iPointSizeNext);
		QFontMetrics metricsNext(font);
		QRect rectNext = metricsNext.boundingRect(str);
		if (rect.width() <= maxRect.width() &&
			rect.height() <= maxRect.height())
		{
			if (rectNext.width() > maxRect.width() ||
				rectNext.height() > maxRect.height())
			{
				int iPointSize3rd = (iPointSizeNext + iPointSize) / 2;
				if (iPointSize3rd == iPointSize ||
					iPointSize3rd == iPointSizeNext)
				{
					font.setPointSize(iPointSize);
					return (iPointSize > 0);
				}
				else
				{
					font.setPointSize(iPointSize3rd);
					continue;
				}
			}
			else
			{
				int iPointSize3rd = iPointSizeNext * 2;
				font.setPointSize(iPointSize3rd);
				continue;
			}
		}
		else
		{
			if (rectNext.width() <= maxRect.width() &&
				rectNext.height() <= maxRect.height())
			{
				int iPointSize3rd = (iPointSizeNext + iPointSize) / 2;
				if (iPointSize3rd == iPointSize ||
					iPointSize3rd == iPointSizeNext)
				{
					font.setPointSize(iPointSizeNext);
					return (iPointSizeNext > 0);
				}
				else
				{
					font.setPointSize(iPointSize3rd);
					continue;
				}
			}
			else
			{
				int iPointSize3rd = iPointSizeNext / 2;
				font.setPointSize(iPointSize3rd);
				continue;
			}
		}
	} while (iCount < 20);
	return false;
}

#pragma endregion


#endif // !_TQTGUI_H_
