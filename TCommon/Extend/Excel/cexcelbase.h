#ifndef _CEXCELBASE_H_
#define _CEXCELBASE_H_
#include <QObject>
#include <ActiveQt/QAxObject>
#include <QString>
#include <QStringList>
#include <QVariant>
#include <QColor>
#include<ole2.h>
#pragma comment(lib,"Qt5AxBase")
#pragma comment(lib,"Qt5AxContainer")

class CExcelBasePrivate;
///
/// \brief win下excel快速读取类
/// 参考天池项目源码，天池源码地址：https://github.com/qtcn/tianchi/blob/v0.0.2-build20130701/include/tianchi/file/tcmsexcel.h
///
class CExcelBase : public QObject
{
public:
	CExcelBase(QObject* par = nullptr);
	~CExcelBase();
private:
	Q_DISABLE_COPY(CExcelBase)
		Q_DECLARE_PRIVATE(CExcelBase)
		CExcelBasePrivate* const d_ptr;
public:
	/// @brief 设置方向的常数
	enum Alignment
	{
		xlTop = -4160, ///< 靠上
		xlLeft = -4131, ///< 靠左
		xlRight = -4152, ///< 靠右
		xlCenter = -4108, ///< 居中
		xlBottom = -4107  ///< 靠下
	};

	/// @brief 创建一个Microsoft Excel文件
	bool create(const QString& filename = QString());
	/// @brief 打开一个Microsoft Excel文件
	bool open(const QString& filename = QString());
	/// @brief 另存Microsoft Excel文件
	void saveAs(const QString& filename);
	void save();
	/// @brief 关闭Microsoft Excel文件
	void close();
	/// @brief 踢出当前打开的 Microsoft Excel<br>
	/// @brief 放弃此对象对该 Excel 的控制权<br>
	/// @brief Excel 文件仍保持打开，但丧失了控制权
	void kick();

	/// @brief 设置当前打开的 Excel 是否可见
	void setVisible(bool value);
	/// @brief 设置 Excel 文档的标题
	void setCaption(const QString& value);

	/// @brief 新建一本 Excel 文档
	bool addBook();

	//	@brief 新增一个sheet页面
	bool addSheet(const QString &strSheetName);

	/// @brief 返回当前 Excel 的 Sheet 数量
	int sheetCount();

	/// @brief 返回当前打开的 Excel 全部 Sheet 名
	QStringList sheetNames();

	/// @brief 返回当前 Sheet.
	bool currentSheet();

	/// @brief 设置并指定当前 Sheet.
	/// @param [in] 当前 Sheet 索引，从 1 开始
	bool setCurrentSheet(int index);

	/// @brief 当前打开的 Excel 的 Sheet 名
	QString currentSheetName();

	/// @brief 读取单元格 Sheet 的内容
	/// @param [in] row 行号，从 1 开始
	/// @param [in] col 列号，从 1 开始
	/// @return 返回指定单元格的内容
	QVariant read(int row, int col);

	/// @brief 读取单元格 Sheet 的内容
	/// @param [in] row 行号，从 1 开始
	/// @param [in] col 列号，从 1 开始
	/// @return 返回指定单元格的内容
	inline QVariant cell(int row, int col) { return read(row, col); }

	/// @brief 写入单元格 Sheet 的内容
	/// @param [in] row 行号，从 1 开始
	/// @param [in] col 列号，从 1 开始
	/// @param [in] value 准备写入的内容
	void write(int row, int col, const QVariant& value);

	/// @brief 单元格写入图片, 并且位置和大小随单元变动
	/// @param [in] row 行号，从 1 开始
	/// @param [in] col 列号，从 1 开始
	/// @param [QString] strImgPath 图片路径
	void writePicture(int row, int col, const QString strImgPath, int left, int top, int width, int height);

	bool cellHeight(int row, int col, double& height);
	bool cellWidth(int row, int col, double & width);

	void cellFormat(int row, int col, const QString& format);
	void cellAlign(int row, int col, Alignment hAlign, Alignment vAlign);
	void cellInteriorColor(int row, int col, const QColor& color);

	/// @brief 获取有效区域信息
	/// @see rowStart() const
	/// @see rowEnd() const
	/// @see colStart() const
	/// @see colEnd() const
	bool usedRange(int& rowStart, int& colStart, int &rowEnd, int &colEnd);

	/// @brief 全文本操作
	QVariant readAll();
	void readAll(QList<QList<QVariant> > &cells);
	bool writeCurrentSheet(const QList<QList<QVariant> > &cells);

	/// @brief 单cell操作

	static void convertToColName(int data, QString &res);
	static QString to26AlphabetString(int data);
	static void castListListVariant2Variant(const QList<QList<QVariant> > &cells, QVariant& res);
	static void castVariant2ListListVariant(const QVariant& var, QList<QList<QVariant> > &res);
};

#endif // _CEXCELBASE_H_