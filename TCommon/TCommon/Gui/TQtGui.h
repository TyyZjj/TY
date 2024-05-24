#ifndef _TQTGUI_H_
#define _TQTGUI_H_

#include <QString>
#include <QTranslator>

class TQtGui
{
public:



private:

};


//�ı���ʽ
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


//���÷���
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

#pragma region �ַ����ڽ����е�ʵ�ʴ�С
#include <QFontMetrics>

//******Taoyuyu******2024/05/24**********
// ����: (��Ҫ��GUI�߳��е���)����һ���ַ���(�޻���)������ʾ��ȡ�font, �õ���ô�������ʵ�����ʱ, ����ʾ���ַ�
// ����: bool: �Ƿ����/�ض�
// ����: QFont font: ����������
// ����: QString str: �������ַ���
// ����: int iMaxWidth: ����ʾ�Ŀ��
// ����: int iMode: ���е�ģʽ
// ����: QString & strElidedText: ����, ���к���ַ���
//************************************
static bool GetElidedText(QFont font, QString str, int iMaxWidth, int iMode, QString& strElidedText)
{
	bool ret = false;
	QFontMetrics fontWidth(font);
	int iWidth = fontWidth.width(str);  //�����ַ������
	if (iWidth >= iMaxWidth)  //���ַ�����ȴ��������ʱ����ת��
	{
		strElidedText = fontWidth.elidedText(str, (Qt::TextElideMode)iMode, iMaxWidth);  //�Ҳ���ʾʡ�Ժ�
		ret = true;
	}
	return ret;   //�����Ƿ�ü�
}

//******Taoyuyu******2024/05/24**********
// ����: (��Ҫ��GUI�߳��е���)�����߶ȡ����塢������, ����Ӧ��ȡ�ʺϵ����ִ�С
// ����: bool: �Ƿ�ɹ�
// ����: const int iMaxHeight: ���������߶�
// ����: QFont & font: ����ֵ
// ����: const QString & family: ����������
// ����: const int weight: ������������
// ����: const QString str: �����ַ���, ����ȡ�߶���Ϣʱ, ����ʡ��
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
// ����: (��Ҫ��GUI�߳��е���)�����ַ�������ȡ����塢������, ����Ӧ��ȡ�ʺϵ����ִ�С
// ����: bool �Ƿ�ɹ�
// ����: const QString str: �����ַ���
// ����: const int iMaxWidth: �����������
// ����: QFont & font: ����ֵ
// ����: const QString & family: ����������
// ����: const int weight: ������������
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
// ����: (��Ҫ��GUI�߳��е���)�����ַ������������塢������, ����Ӧ��ȡ�ʺϵ����ִ�С
// ����: bool �Ƿ�ɹ�
// ����: const QRect maxRect: �������������
// ����: const QString str: �����ַ���
// ����: QFont & font: ����ֵ
// ����: const QString & family: ����������
// ����: const int weight: ������������
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
