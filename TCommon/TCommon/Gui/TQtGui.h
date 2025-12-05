#ifndef _TQTGUI_H_
#define _TQTGUI_H_
#include <QFile>
#include <QDebug>
#include <QString>
#include <QPainter>
#include <QTranslator>

/*
* 1.改变样式
* 2.设置翻译
* 
* 字符串在界面中的实际大小
* 1.(需要在GUI线程中调用)给定一个字符串(无换行)、能显示宽度、font, 得到这么宽的区域实际输出时, 能显示的字符
* 2.(需要在GUI线程中调用)给定高度、字体、字体宽度, 自适应获取适合的文字大小
* 3.(需要在GUI线程中调用)给定字符串、宽度、字体、字体宽度, 自适应获取适合的文字大小
* 4.(需要在GUI线程中调用)给定字符串、区域、字体、字体宽度, 自适应获取适合的文字大小
* 5.(需要在GUI线程中调用)在图片的指定矩形区域内居中绘制文字
* 6.(需要在GUI线程中调用)在图片的指定矩形区域左上角插入 QStringList，每行一个字符串
* 改变样式
*/

class TQtGui
{
public:
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
	static void SetTranslator(const QString& filename)
	{
		static QTranslator* ptranslator = nullptr;
		if (ptranslator)
		{
			bool succ = qApp->removeTranslator(ptranslator);
			if (!succ)
				qDebug() << QString("removeTranslator failed! translator filename:%1").arg(filename);
			delete ptranslator;
			ptranslator = nullptr;
		}
		ptranslator->load(filename);
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
	static bool GetHeightAdaptiveFont(const int iMaxHeight, QFont& font,
		const QString& family, const int weight = -1, const QString str = "")
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
		} while (iCount < 20);
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
		const int iMaxWidth, QFont& font,
		const QString& family, const int weight = -1)
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
		const QString str, QFont& font,
		const QString& family, const int weight = -1)
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

	/**
	* @brief 在图片的指定矩形区域内居中绘制文字
	* @param image 目标图片
	* @param rect 文字绘制的矩形区域
	* @param text 要绘制的文字
	* @param font 文字字体（可选，默认系统字体）
	* @param color 文字颜色（可选，默认黑色）
	*/
	static void drawCenteredTextOnImage(QImage& image, const QRect& rect, const QString& text,
		const QFont& font = QFont(), const QColor& color = ::Qt::black)
	{
		QPainter painter(&image);

		// 设置字体和颜色
		painter.setFont(font);
		painter.setPen(color);

		// 计算文字的实际占用大小
		QFontMetrics metrics(font);
		int height = metrics.height();          // 总高度（像素）
		int ascent = metrics.ascent();          // 基线以上高度
		int descent = metrics.descent();        // 基线以下高度
		int capHeight = metrics.capHeight();    // 大写字母高度
		QRect textRect = metrics.boundingRect(text);

		// 计算居中坐标（相对于rect的中心）
		int x = rect.x() + (rect.width() - textRect.width()) / 2;
		int y = rect.y() + (rect.height() + metrics.ascent() - metrics.descent()) / 2;

		// 绘制文字
		painter.drawText(x, y, text);
	}

	/**
	* @brief 在图片的指定矩形区域左上角插入 QStringList，每行一个字符串
	* @param image 目标图片（会被修改）
	* @param rect 文本绘制区域
	* @param stringList 要绘制的字符串列表
	* @param font 字体（可选）
	* @param color 文字颜色（可选）
	* @param lineSpacing 行间距（可选，默认2像素）
	*/
	static void drawStringListOnImage(QImage& image, const QRect& rect,
		const QStringList& stringList,
		const QFont& font = QFont(),
		const QColor& color = ::Qt::black,
		int startSpacing = 2,
		int lineSpacing = 2) 
	{
		if (image.isNull() || stringList.isEmpty()) return;

		QPainter painter(&image);
		painter.setRenderHint(QPainter::Antialiasing);
		painter.setFont(font);
		painter.setPen(color);

		// 获取字体度量信息
		QFontMetrics metrics(font);
		int lineHeight = metrics.height() + lineSpacing;  // 单行高度（含间距）

		// 初始绘制位置（左上角）
		int x = rect.x() + startSpacing;
		int y = rect.y() + metrics.ascent() + startSpacing;  // 补偿基线偏移

		// 遍历所有字符串逐行绘制
		for (const QString& str : stringList) {
			// 检查是否超出区域底部
			if (y + metrics.descent() > rect.bottom()) break;

			painter.drawText(x, y, str);
			y += lineHeight;  // 移动到下一行
		}
	}

#pragma endregion

#if 0
	//生成二维码图片
#include "ZXing/Barcode.h"
#include "ZXing/MultiFormatWriter.h"
#include "ZXing/BitMatrix.h"
	static QImage generateQRCodeImage(const QString& text, int width, int height)
	{
		ZXing::MultiFormatWriter writer(ZXing::BarcodeFormat::QRCode);
		//ZXing::WriterOptions options;
		//options.margin = 2;
		//options.errorCorrectionLevel = ZXing::ErrorCorrectionLevel::High;

		try {
			ZXing::BitMatrix bitMatrix = writer.encode(text.toUtf8().toStdString(), width, height);
			QImage qrImage(width, height, QImage::Format_RGB32);
			qrImage.fill(::Qt::white);

			for (int y = 0; y < bitMatrix.height(); ++y) {
				for (int x = 0; x < bitMatrix.width(); ++x) {
					if (bitMatrix.get(x, y)) {
						qrImage.setPixel(x, y, ::Qt::black);
					}
				}
			}
			return qrImage;
		}
		catch (const std::exception& e) {
			qDebug() << "Error generating QR Code:" << e.what();
			return QImage();
		}
	}
#endif // 0

private:

};








#endif // !_TQTGUI_H_
