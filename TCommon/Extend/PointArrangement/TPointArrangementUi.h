#ifndef _TPOINTARRANGEMENTUI_H_
#define _TPOINTARRANGEMENTUI_H_
#include <QPoint>
#include <QDialog>
namespace Ui { class TPointArrangementUi; };

enum nPointPosition
{
#define TopLeft		"TopLeft"
#define TopRight	"TopRight"
#define BottomLeft	"BottomLeft"
#define BottomRight	"BottomRight"
	PointPosition_TopLeft = 0,
	PointPosition_TopRight,
	PointPosition_BottomLeft,
	PointPosition_BottomRight,
};

enum nPointDirection
{
#define PDToRight	"ToRight"
#define PDToLeft	"ToLeft"
#define PDUpward	"Upward"
#define PDDown		"Down"
	PointDirection_ToRight = 0,
	PointDirection_ToLeft,
	PointDirection_Upward,
	PointDirection_Down,
};


//换行方法
enum nLineFeedWay
{
//①②③
//④⑤⑥
//⑦⑧⑨
#define SameAsPrevious	"SameAsPrevious"

//①②③
//⑥⑤④
//⑦⑧⑨
#define SnakeLike		"SnakeLike"
	nSameAsPrevious = 0,	//和上一行/列一样
	nSnakeLike,	//蛇形
};


enum nArrangementType
{
	ArrangementType_AddOne,//逐一添加点位
	ArrangementType_Auto,//自动排布点位
};

//点位排布
struct TPointArrangement
{
	nArrangementType iType = ArrangementType_AddOne;//点位排布方式
	int	iPointRowCount = 5;	//点位行数
	int iPointColumnCount = 9;//点位列数
	nPointPosition iFirstPointPosition = nPointPosition::PointPosition_TopLeft;	//第一个点位位置
	nPointDirection iSecondPointDirection = nPointDirection::PointDirection_ToRight;//第二个点位相对于第一个点位的方向
	nLineFeedWay	iLineFeedWay = nLineFeedWay::nSameAsPrevious;
	int iFirstPointX = 0;//第一个点的X轴坐标
	int iFirstPointY = 0;//第一个点的Y轴坐标
	int iXSpacing = 0;//X方向间隔
	int iYSpacing = 0;
	int iPointCount = 45;//点位总数 <= iPointRowCount * iPointColimnCount

	//************************************
	// 参数: 	int index: 当前点的序号, 从1开始
	// 参数: 	int iRowCount: 行数
	// 参数: 	int iColumnCount: 列数
	// 参数: 	nPointPosition iFirstPointPosition: 起始点的位置
	// 参数: 	nPointDirection iSecondPointDirection: 结束点的位置
	// 参数: 	nLineFeedWay iLineFeedWay: 换行时的方式
	// 返回:	QList<int>: 所有邻接点的序号
	// 功能:	获取某个点的所有邻接点的序号
	//************************************
	static QList<int> GetAdjacencyPointIndex(int index, int iRowCount, int iColumnCount,
		nPointPosition iFirstPointPosition = nPointPosition::PointPosition_TopLeft,
		nPointDirection iSecondPointDirection = nPointDirection::PointDirection_ToRight,
		nLineFeedWay iLineFeedWay = nLineFeedWay::nSameAsPrevious);

	//************************************
	// 参数: 	int index: 当前点的序号, 从1开始
	// 参数: 	int iRowCount: 行数
	// 参数: 	int iColumnCount: 列数
	// 参数: 	int & iRow: 返回 行, 从1开始
	// 参数: 	int & iColumn: 返回 列, 从1开始
	// 参数: 	nPointPosition iFirstPointPosition: 起始点的位置
	// 参数: 	nPointDirection iSecondPointDirection: 结束点的位置
	// 参数: 	nLineFeedWay iLineFeedWay: 换行时的方式
	// 返回:	bool: 成功/失败
	// 功能:	由某个点的index 获取 某个点的位置
	//************************************
	static bool GetPointPosition(int index, int iRowCount, int iColumnCount, int &iRow, int &iColumn,
		nPointPosition iFirstPointPosition = nPointPosition::PointPosition_TopLeft,
		nPointDirection iSecondPointDirection = nPointDirection::PointDirection_ToRight,
		nLineFeedWay iLineFeedWay = nLineFeedWay::nSameAsPrevious);
	
	//************************************
	// 参数: 	int & index: 返回值 当前点的序号, 从1开始
	// 参数: 	int iRowCount: 行数
	// 参数: 	int iColumnCount: 列数
	// 参数: 	int iRow: 行, 从1开始
	// 参数: 	int iColumn: 列, 从1开始
	// 参数: 	nPointPosition iFirstPointPosition: 起始点的位置
	// 参数: 	nPointDirection iSecondPointDirection: 结束点的位置
	// 参数: 	nLineFeedWay iLineFeedWay: 换行时的方式
	// 返回:	bool: 成功/失败
	// 功能:	由某个点的行列位置 获取 某个点的index
	//************************************
	static bool GetPointIndex(int &index, int iRowCount, int iColumnCount, int iRow, int iColumn,
		nPointPosition iFirstPointPosition = nPointPosition::PointPosition_TopLeft,
		nPointDirection iSecondPointDirection = nPointDirection::PointDirection_ToRight,
		nLineFeedWay iLineFeedWay = nLineFeedWay::nSameAsPrevious);

	//************2024/05/21**********
	// 功能: 获取某个点的坐标位置 
	// 返回: bool 成功/失败
	// 参数: int index: 序号
	// 参数: int iRowCount: 总行数
	// 参数: int iColumnCount: 总列数 
	// 参数: int iFirstPointX: 第一个点的X坐标
	// 参数: int iFirstPointY: 第一个点的Y坐标
	// 参数: int iXSpacing: X方向间距
	// 参数: int iYSpacing: Y方向间距
	// 返回: QPoint & point: 返回点的坐标
	// 参数: nPointPosition iFirstPointPosition: 起始点的位置
	// 参数: nPointDirection iSecondPointDirection: 结束点的位置
	// 参数: nLineFeedWay iLineFeedWay: 换行时的方式
	//************************************
	static bool getPoint(int index, int iRowCount, int iColumnCount,
		int iFirstPointX, int iFirstPointY,
		int iXSpacing, int iYSpacing,
		QPoint &point,
		nPointPosition iFirstPointPosition = nPointPosition::PointPosition_TopLeft,
		nPointDirection iSecondPointDirection = nPointDirection::PointDirection_ToRight,
		nLineFeedWay iLineFeedWay = nLineFeedWay::nSameAsPrevious);
};
Q_DECLARE_METATYPE(TPointArrangement);

//点位排布设置界面
class TPointArrangementUi : public QDialog
{
	Q_OBJECT

public:
	TPointArrangementUi(nArrangementType iType, 
		int iLength,//像素长度
		int iWidth,//像素宽度
		int iPointRadius,
		TPointArrangement defaultModelArrangement,
		QWidget *parent = Q_NULLPTR);
	~TPointArrangementUi();

Q_SIGNALS:
	void signalModelArrangement(TPointArrangement modelArrangement);

public Q_SLOTS:
	void on_okBtn_clicked();
	void on_cancelBtn_clicked();

private:
	void initWidget(TPointArrangement defaultModelArrangement);
	bool check(const TPointArrangement& modelArrangement);

private:
	Ui::TPointArrangementUi *ui;

	int m_iLength;
	int m_iWidth;
	int m_iPointRadius;
	nArrangementType	m_iType;
};
#endif //_TPOINTARRANGEMENTUI_H_