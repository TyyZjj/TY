#include "cexcelbase.h"
#include <QFile>
#include <QList>
#include <QDebug>
#include <ole2.h>
#if defined(Q_OS_WIN)
#include <ActiveQt/QAxObject>
#endif // Q_OS_WIN

#define TC_FREE(x)  {delete x; x=nullptr;}

class CExcelBasePrivate
{
	Q_DECLARE_PUBLIC(CExcelBase)
public:
	explicit CExcelBasePrivate(CExcelBase* qptr);
	~CExcelBasePrivate();

	void construct();
	void destory();

	CExcelBase* const q_ptr;

#if defined(Q_OS_WIN)
	QAxObject*  excel;
	QAxObject*  books;
	QAxObject*  book;
	QAxObject*  sheets;
	QAxObject*  sheet;
#endif
	QString     filename;
	QString     sheetName;
};

CExcelBasePrivate::CExcelBasePrivate(CExcelBase *qptr)
: q_ptr(qptr)
#if defined(Q_OS_WIN)
, excel(nullptr)
, books(nullptr)
, book(nullptr)
, sheets(nullptr)
, sheet(nullptr)
#endif // Q_OS_WIN
{
	HRESULT r = OleInitialize(0);
	if (r != S_OK && r != S_FALSE)
		return;
	//qDebug() << "CExcelBasePrivate";
}

CExcelBasePrivate::~CExcelBasePrivate()
{
#if defined(Q_OS_WIN)
	if (excel)
	{
		if (!excel->isNull())
		{
			excel->dynamicCall("Quit()");
		}
	}
	TC_FREE(sheet);
	TC_FREE(sheets);
	TC_FREE(book);
	TC_FREE(books);
	TC_FREE(excel);
	OleUninitialize();
#endif // Q_OS_WIN
}

void CExcelBasePrivate::construct()
{
#if defined(Q_OS_WIN)
	destory();
	excel = new QAxObject(q_ptr);
	excel->setControl("Excel.Application");
	excel->setProperty("Visible", false);
	if (excel->isNull())
	{
		excel->setControl("ET.Application");
	}
	if (!excel->isNull())
	{
		books = excel->querySubObject("Workbooks");
	}
#endif // Q_OS_WIN
}

void CExcelBasePrivate::destory()
{
#if defined(Q_OS_WIN)
	TC_FREE(sheet);
	TC_FREE(sheets);
	if (book != nullptr && !book->isNull())
	{
		book->dynamicCall("Close(Boolean)", false);
	}
	TC_FREE(book);
	TC_FREE(books);
	if (excel != nullptr && !excel->isNull())
	{
		excel->dynamicCall("Quit()");
	}
	TC_FREE(excel);
	filename = "";
	sheetName = "";
#endif // Q_OS_WIN
}


CExcelBase::CExcelBase(QObject* par) :QObject(par)
, d_ptr(new CExcelBasePrivate(this))
{
	//qDebug() << "CExcelBase";
}

CExcelBase::~CExcelBase()
{
	close();
	delete d_ptr;
}

bool CExcelBase::create(const QString& filename)
{
	bool ret = false;
#if defined(Q_OS_WIN)
	Q_D(CExcelBase);
	d->construct();
	if (d->books != nullptr && !d->books->isNull())
	{
		d->books->dynamicCall("Add");
		d->book = d->excel->querySubObject("ActiveWorkBook");
		d->sheets = d->book->querySubObject("WorkSheets");
		currentSheet();
		d->filename = filename;
		ret = true;
	}
#else
	Q_UNUSED(filename)
#endif // Q_OS_WIN
		return ret;
}

bool CExcelBase::open(const QString& filename)
{
	bool ret = false;
	qDebug() << "open 0";
#if defined(Q_OS_WIN)
	Q_D(CExcelBase);
	qDebug() << "open 1";
	d->construct();
	qDebug() << "open 2";
	if (d->books != nullptr && !d->books->isNull())
	{
		qDebug() << "open 3";
		d->book = d->books->querySubObject("Open(QString, QVariant)", filename, 0);
		qDebug() << "open 4";
		ret = d->book != nullptr && !d->book->isNull();
		if (ret)
		{
			qDebug() << "open 5";
			d->sheets = d->book->querySubObject("WorkSheets");
			d->filename = filename;
			qDebug() << "open 6";
			currentSheet();
			qDebug() << "open 7";
		}
	}
#else
	Q_UNUSED(filename)
#endif // Q_OS_WIN
		return ret;
}

void CExcelBase::saveAs(const QString& filename)
{
#if defined(Q_OS_WIN)
	Q_D(CExcelBase);
	if (d->book != nullptr && !d->book->isNull())
	{
		d->filename = filename;
		QString strPath = d->filename;
		strPath = strPath.replace('/', '\\');
		qDebug() << strPath;
		//d->book->dynamicCall("SaveAs(const QString&,int,const QString&,const QString&,bool,bool)", strPath
		//	, 56, QString(""), QString(""), false, false);
		d->book->dynamicCall("SaveAs(const QString&)", strPath);
	}
#else
	Q_UNUSED(filename)
#endif // Q_OS_WIN
}
///
/// \brief 保存excel
///
void CExcelBase::save()
{
#if defined(Q_OS_WIN)
	Q_D(CExcelBase);
	if (d->filename.isEmpty())
		return;
	//saveAs(d->filename);
	if (d->book != nullptr && !d->book->isNull())
		d->book->dynamicCall("Save()");
#else
	Q_UNUSED(filename)
#endif // Q_OS_WIN
}

void CExcelBase::close()
{
#if defined(Q_OS_WIN)
	Q_D(CExcelBase);
	d->destory();
#endif // Q_OS_WIN
}

void CExcelBase::kick()
{
#if defined(Q_OS_WIN)
	Q_D(CExcelBase);

	if (d->excel != nullptr && !d->excel->isNull())
	{
		d->excel->setProperty("Visible", true);
	}
	TC_FREE(d->sheet);
	TC_FREE(d->sheets);
	TC_FREE(d->book);
	TC_FREE(d->books);
	TC_FREE(d->excel);
	d->destory();
#endif // Q_OS_WIN
}

QStringList CExcelBase::sheetNames()
{
	QStringList ret;
#if defined(Q_OS_WIN)
	Q_D(CExcelBase);
	if (d->sheets != nullptr && !d->sheets->isNull())
	{
		int sheetCount = d->sheets->property("Count").toInt();
		for (int i = 1; i <= sheetCount; i++)
		{
			QAxObject* sheet = d->sheets->querySubObject("Item(int)", i);
			if (nullptr == sheet || sheet->isNull())
				continue;
			ret.append(sheet->property("Name").toString());
			delete sheet;
			sheet = nullptr;
		}
	}
#endif // Q_OS_WIN
	return ret;
}

QString CExcelBase::currentSheetName()
{
	Q_D(CExcelBase);
	return d->sheetName;
}

void CExcelBase::setVisible(bool value)
{
#if defined(Q_OS_WIN)
	Q_D(CExcelBase);
	if (d->excel != nullptr && !d->excel->isNull())
	{
		d->excel->setProperty("Visible", value);
	}
#else
	Q_UNUSED(value)
#endif // Q_OS_WIN
}

void CExcelBase::setCaption(const QString& value)
{
#if defined(Q_OS_WIN)
	Q_D(CExcelBase);
	if (d->excel != nullptr && !d->excel->isNull())
	{
		d->excel->setProperty("Caption", value);
	}
#else
	Q_UNUSED(value)
#endif // Q_OS_WIN
}

bool CExcelBase::addBook()
{
	bool ret = false;
#if defined(Q_OS_WIN)
	Q_D(CExcelBase);
	if (d->excel != nullptr && !d->excel->isNull())
	{
		TC_FREE(d->sheet);
		TC_FREE(d->sheets);
		TC_FREE(d->book);
		TC_FREE(d->books);
		d->books = d->excel->querySubObject("WorkBooks");
		ret = d->books != nullptr && !d->books->isNull();
	}
#endif // Q_OS_WIN
	return ret;
}

bool CExcelBase::currentSheet()
{
	bool ret = false;
#if defined(Q_OS_WIN)
	Q_D(CExcelBase);
	TC_FREE(d->sheet);
	if (d->excel != nullptr && !d->excel->isNull())
	{
		TC_FREE(d->sheet);
		d->sheet = d->excel->querySubObject("ActiveWorkBook");
		ret = d->sheet != nullptr && !d->sheet->isNull();
		d->sheetName = ret ? d->sheet->property("Name").toString() : "";
	}
#endif // Q_OS_WIN
	return ret;
}

bool CExcelBase::setCurrentSheet(int index)
{
	bool ret = false;
#if defined(Q_OS_WIN)
	Q_D(CExcelBase);
	if (d->sheets != nullptr && !d->sheets->isNull())
	{
		TC_FREE(d->sheet);
		d->sheet = d->sheets->querySubObject("Item(int)", index);
		ret = d->sheet != nullptr && !d->sheet->isNull();
		if (ret)
		{
			d->sheet->dynamicCall("Activate(void)");
		}
		d->sheetName = ret ? d->sheet->property("Name").toString() : "";
	}
#else
	Q_UNUSED(index)
#endif // Q_OS_WIN
		return ret;
}

bool CExcelBase::addSheet(const QString &strSheetName)
{
#if defined(Q_OS_WIN)
	Q_D(CExcelBase);
	if (d->sheets != nullptr && !d->sheets->isNull())
	{
		QAxObject* newSheet = d->sheets->querySubObject("Add()");
		if (newSheet != nullptr)
		{
			return newSheet->setProperty("Name", strSheetName);
		}
	}
#endif // Q_OS_WIN
	return false;
}

int CExcelBase::sheetCount()
{
	int ret = 0;
#if defined(Q_OS_WIN)
	Q_D(CExcelBase);
	if (d->sheets != nullptr && !d->sheets->isNull())
	{
		ret = d->sheets->property("Count").toInt();
	}
#endif // Q_OS_WIN
	return ret;
}

void CExcelBase::cellFormat(int row, int col, const QString& format)
{
#if defined(Q_OS_WIN)
	Q_D(CExcelBase);
	if (d->sheet != nullptr && !d->sheet->isNull())
	{
		QAxObject* range = d->sheet->querySubObject("Cells(int, int)", row, col);
		if (range)
		{
			range->setProperty("NumberFormatLocal", format);
			delete range;
			range = nullptr;
		}
	}
#else
	Q_UNUSED(row)
		Q_UNUSED(col)
		Q_UNUSED(format)
#endif // Q_OS_WIN
}

bool CExcelBase::cellHeight(int row, int col, double& height)
{
#if defined(Q_OS_WIN)
	Q_D(CExcelBase);
	if (d->sheet != nullptr && !d->sheet->isNull())
	{
		bool isOk(false);
		QAxObject* range = d->sheet->querySubObject("Cells(int, int)", row, col);
		if (range)
		{
			height = range->property("Height").toDouble(&isOk);
			delete range;
			range = nullptr;
			if (isOk)
				return true;
		}
	}
#else
Q_UNUSED(row)
Q_UNUSED(col)
Q_UNUSED(value)
#endif
	height = 0;
	return false;
}


bool CExcelBase::cellWidth(int row, int col, double & width)
{
#if defined(Q_OS_WIN)
	Q_D(CExcelBase);
	if (d->sheet != nullptr && !d->sheet->isNull())
	{
		bool isOk(false);
		QAxObject* range = d->sheet->querySubObject("Cells(int, int)", row, col);
		if (range)
		{
			width = range->property("Width").toDouble(&isOk);
			delete range;
			range = nullptr;
			if (isOk)
				return true;
		}
	}
	
#else
	Q_UNUSED(row)
		Q_UNUSED(col)
		Q_UNUSED(value)
#endif
	width = 0;
	return false;
}


void CExcelBase::cellInteriorColor(int row, int col, const QColor& color)
{
#if defined(Q_OS_WIN)
	Q_D(CExcelBase);
	if (d->sheet != nullptr && !d->sheet->isNull())
	{
		QAxObject* range = d->sheet->querySubObject("Cells(int, int)", row, col);
		QAxObject* interior = range->querySubObject("Interior");
		bool ret = interior->setProperty("Color", color);
		delete range;
		range = nullptr;
	}
#else
	Q_UNUSED(row)
		Q_UNUSED(col)
		Q_UNUSED(format)
#endif // Q_OS_WIN
}

void CExcelBase::cellAlign(int row, int col, Alignment hAlign, Alignment vAlign)
{
#if defined(Q_OS_WIN)
	Q_D(CExcelBase);
	if (d->sheet != nullptr && !d->sheet->isNull())
	{
		QAxObject* range = d->sheet->querySubObject("Cells(int, int)", row, col);
		range->setProperty("HorizontalAlignment", hAlign);
		range->setProperty("VerticalAlignment", vAlign);
		delete range;
		range = nullptr;
	}
#else
	Q_UNUSED(row)
		Q_UNUSED(col)
		Q_UNUSED(hAlign)
		Q_UNUSED(vAlign)
#endif // Q_OS_WIN
}

QVariant CExcelBase::read(int row, int col)
{
	QVariant ret;
#if defined(Q_OS_WIN)
	Q_D(CExcelBase);
	if (d->sheet != nullptr && !d->sheet->isNull())
	{
		QAxObject* range = d->sheet->querySubObject("Cells(int, int)", row, col);
		//ret = range->property("Value");
		ret = range->dynamicCall("Value()");
		delete range;
		range = nullptr;
	}
#else
	Q_UNUSED(row)
		Q_UNUSED(col)
#endif // Q_OS_WIN
		return ret;
}

void CExcelBase::write(int row, int col, const QVariant& value)
{
#if defined(Q_OS_WIN)
	Q_D(CExcelBase);
	if (d->sheet != nullptr && !d->sheet->isNull())
	{
		QAxObject* range = d->sheet->querySubObject("Cells(int, int)", row, col);
		range->dynamicCall("SetValue(const QString&)", value);
		delete range;
		range = nullptr;
	}
#else
	Q_UNUSED(row)
		Q_UNUSED(col)
		Q_UNUSED(value)
#endif // Q_OS_WIN
}

void CExcelBase::writePicture(int row, int col, const QString strImgPath, int left, int top, int width, int height)
{
#if defined(Q_OS_WIN)
	Q_D(CExcelBase);
	if (d->sheet != nullptr && !d->sheet->isNull())
	{
		QAxObject* pShape = d->sheet->querySubObject("Shapes");
		pShape->dynamicCall("AddPicture(QString&,bool,bool,double,double,double,double)", strImgPath, true, true, left, top, width, height);
		delete pShape;
		pShape = nullptr;
	}
#else
	Q_UNUSED(row)
		Q_UNUSED(col)
		Q_UNUSED(value)
#endif
}


bool CExcelBase::usedRange(int& rowStart, int& colStart, int& rowEnd, int& colEnd)
{
	bool ret = false;
#if defined(Q_OS_WIN)
	Q_D(CExcelBase);
	if (d->sheet != nullptr && !d->sheet->isNull())
	{
		QAxObject* urange = d->sheet->querySubObject("UsedRange");
		rowStart = urange->property("Row").toInt();
		colStart = urange->property("Column").toInt();
		rowEnd = urange->querySubObject("Rows")->property("Count").toInt();
		colEnd = urange->querySubObject("Columns")->property("Count").toInt();
		delete urange;
		urange = nullptr;
		ret = true;
	}
#else
	Q_UNUSED(rowStart)
		Q_UNUSED(colStart)
		Q_UNUSED(rowEnd)
		Q_UNUSED(colEnd)
#endif // Q_OS_WIN
		return ret;
}
///
/// \brief 读取整个sheet
/// \return
///
QVariant CExcelBase::readAll()
{
	QVariant var;
#if defined(Q_OS_WIN)
	Q_D(CExcelBase);
	if (d->sheet != nullptr && !d->sheet->isNull())
	{
		QAxObject *usedRange = d->sheet->querySubObject("UsedRange");
		if (nullptr == usedRange || usedRange->isNull())
		{
			return var;
		}
		var = usedRange->dynamicCall("Value");
		delete usedRange;
		usedRange = nullptr;
	}
#endif
	return var;
}
///
/// \brief 读取整个sheet的数据，并放置到cells中
/// \param cells
///
void CExcelBase::readAll(QList<QList<QVariant> > &cells)
{
#if defined(Q_OS_WIN)
	castVariant2ListListVariant(readAll(), cells);
#else
	Q_UNUSED(cells);
#endif

}

///
/// \brief 写入一个表格内容
/// \param cells
/// \return 成功写入返回true
/// \see readAllSheet
///
bool CExcelBase::writeCurrentSheet(const QList<QList<QVariant> > &cells)
{
#if defined(Q_OS_WIN)
	Q_D(CExcelBase);
	if (cells.size() <= 0)
		return false;
	if (nullptr == d->sheet || d->sheet->isNull())
		return false;
	int row = cells.size();
	int col = cells.at(0).size();
	QString rangStr;
	convertToColName(col, rangStr);
	rangStr += QString::number(row);
	rangStr = "A1:" + rangStr;
	qDebug() << rangStr;
	QAxObject *range = d->sheet->querySubObject("Range(const QString&)", rangStr);
	if (nullptr == range || range->isNull())
	{
		return false;
	}
	bool succ = true;
	QVariant var;
	castListListVariant2Variant(cells, var);
	range->dynamicCall("Value", var);
	delete range;
	range = nullptr;
	return succ;
#else
	return true;
#endif
}
///
/// \brief 把列数转换为excel的字母列号
/// \param data 大于0的数
/// \return 字母列号，如1->A 26->Z 27 AA
///
void CExcelBase::convertToColName(int data, QString &res)
{
	Q_ASSERT(data > 0 && data<65535);
	int tempData = data / 26;
	if (tempData > 0)
	{
		int mode = data % 26;
		convertToColName(mode, res);
		convertToColName(tempData, res);
	}
	else
	{
		res = (to26AlphabetString(data) + res);
	}
}
///
/// \brief 数字转换为26字母
///
/// 1->A 26->Z
/// \param data
/// \return
///
QString CExcelBase::to26AlphabetString(int data)
{
	QChar ch = data + 0x40;//A对应0x41
	return QString(ch);
}
///
/// \brief QList<QList<QVariant> >转换为QVariant
/// \param cells
/// \return
///
void CExcelBase::castListListVariant2Variant(const QList<QList<QVariant> > &cells, QVariant &res)
{
	QVariantList vars;
	const int rows = cells.size();
	for (int i = 0; i < rows; ++i)
	{
		vars.append(QVariant(cells[i]));
	}
	res = QVariant(vars);
}
///
/// \brief 把QVariant转为QList<QList<QVariant> >
/// \param var
/// \param res
///
void CExcelBase::castVariant2ListListVariant(const QVariant &var, QList<QList<QVariant> > &res)
{
	QVariantList varRows = var.toList();
	if (varRows.isEmpty())
	{
		return;
	}
	const int rowCount = varRows.size();
	QVariantList rowData;
	for (int i = 0; i < rowCount; ++i)
	{
		rowData = varRows[i].toList();
		res.push_back(rowData);
	}
}