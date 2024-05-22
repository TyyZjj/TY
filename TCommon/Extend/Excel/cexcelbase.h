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
/// \brief win��excel���ٶ�ȡ��
/// �ο������ĿԴ�룬���Դ���ַ��https://github.com/qtcn/tianchi/blob/v0.0.2-build20130701/include/tianchi/file/tcmsexcel.h
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
	/// @brief ���÷���ĳ���
	enum Alignment
	{
		xlTop = -4160, ///< ����
		xlLeft = -4131, ///< ����
		xlRight = -4152, ///< ����
		xlCenter = -4108, ///< ����
		xlBottom = -4107  ///< ����
	};

	/// @brief ����һ��Microsoft Excel�ļ�
	bool create(const QString& filename = QString());
	/// @brief ��һ��Microsoft Excel�ļ�
	bool open(const QString& filename = QString());
	/// @brief ���Microsoft Excel�ļ�
	void saveAs(const QString& filename);
	void save();
	/// @brief �ر�Microsoft Excel�ļ�
	void close();
	/// @brief �߳���ǰ�򿪵� Microsoft Excel<br>
	/// @brief �����˶���Ը� Excel �Ŀ���Ȩ<br>
	/// @brief Excel �ļ��Ա��ִ򿪣���ɥʧ�˿���Ȩ
	void kick();

	/// @brief ���õ�ǰ�򿪵� Excel �Ƿ�ɼ�
	void setVisible(bool value);
	/// @brief ���� Excel �ĵ��ı���
	void setCaption(const QString& value);

	/// @brief �½�һ�� Excel �ĵ�
	bool addBook();

	//	@brief ����һ��sheetҳ��
	bool addSheet(const QString &strSheetName);

	/// @brief ���ص�ǰ Excel �� Sheet ����
	int sheetCount();

	/// @brief ���ص�ǰ�򿪵� Excel ȫ�� Sheet ��
	QStringList sheetNames();

	/// @brief ���ص�ǰ Sheet.
	bool currentSheet();

	/// @brief ���ò�ָ����ǰ Sheet.
	/// @param [in] ��ǰ Sheet �������� 1 ��ʼ
	bool setCurrentSheet(int index);

	/// @brief ��ǰ�򿪵� Excel �� Sheet ��
	QString currentSheetName();

	/// @brief ��ȡ��Ԫ�� Sheet ������
	/// @param [in] row �кţ��� 1 ��ʼ
	/// @param [in] col �кţ��� 1 ��ʼ
	/// @return ����ָ����Ԫ�������
	QVariant read(int row, int col);

	/// @brief ��ȡ��Ԫ�� Sheet ������
	/// @param [in] row �кţ��� 1 ��ʼ
	/// @param [in] col �кţ��� 1 ��ʼ
	/// @return ����ָ����Ԫ�������
	inline QVariant cell(int row, int col) { return read(row, col); }

	/// @brief д�뵥Ԫ�� Sheet ������
	/// @param [in] row �кţ��� 1 ��ʼ
	/// @param [in] col �кţ��� 1 ��ʼ
	/// @param [in] value ׼��д�������
	void write(int row, int col, const QVariant& value);

	/// @brief ��Ԫ��д��ͼƬ, ����λ�úʹ�С�浥Ԫ�䶯
	/// @param [in] row �кţ��� 1 ��ʼ
	/// @param [in] col �кţ��� 1 ��ʼ
	/// @param [QString] strImgPath ͼƬ·��
	void writePicture(int row, int col, const QString strImgPath, int left, int top, int width, int height);

	bool cellHeight(int row, int col, double& height);
	bool cellWidth(int row, int col, double & width);

	void cellFormat(int row, int col, const QString& format);
	void cellAlign(int row, int col, Alignment hAlign, Alignment vAlign);
	void cellInteriorColor(int row, int col, const QColor& color);

	/// @brief ��ȡ��Ч������Ϣ
	/// @see rowStart() const
	/// @see rowEnd() const
	/// @see colStart() const
	/// @see colEnd() const
	bool usedRange(int& rowStart, int& colStart, int &rowEnd, int &colEnd);

	/// @brief ȫ�ı�����
	QVariant readAll();
	void readAll(QList<QList<QVariant> > &cells);
	bool writeCurrentSheet(const QList<QList<QVariant> > &cells);

	/// @brief ��cell����

	static void convertToColName(int data, QString &res);
	static QString to26AlphabetString(int data);
	static void castListListVariant2Variant(const QList<QList<QVariant> > &cells, QVariant& res);
	static void castVariant2ListListVariant(const QVariant& var, QList<QList<QVariant> > &res);
};

#endif // _CEXCELBASE_H_