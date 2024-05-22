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


//���з���
enum nLineFeedWay
{
//�٢ڢ�
//�ܢݢ�
//�ߢ��
#define SameAsPrevious	"SameAsPrevious"

//�٢ڢ�
//�ޢݢ�
//�ߢ��
#define SnakeLike		"SnakeLike"
	nSameAsPrevious = 0,	//����һ��/��һ��
	nSnakeLike,	//����
};


enum nArrangementType
{
	ArrangementType_AddOne,//��һ��ӵ�λ
	ArrangementType_Auto,//�Զ��Ų���λ
};

//��λ�Ų�
struct TPointArrangement
{
	nArrangementType iType = ArrangementType_AddOne;//��λ�Ų���ʽ
	int	iPointRowCount = 5;	//��λ����
	int iPointColumnCount = 9;//��λ����
	nPointPosition iFirstPointPosition = nPointPosition::PointPosition_TopLeft;	//��һ����λλ��
	nPointDirection iSecondPointDirection = nPointDirection::PointDirection_ToRight;//�ڶ�����λ����ڵ�һ����λ�ķ���
	nLineFeedWay	iLineFeedWay = nLineFeedWay::nSameAsPrevious;
	int iFirstPointX = 0;//��һ�����X������
	int iFirstPointY = 0;//��һ�����Y������
	int iXSpacing = 0;//X������
	int iYSpacing = 0;
	int iPointCount = 45;//��λ���� <= iPointRowCount * iPointColimnCount

	//************************************
	// ����: 	int index: ��ǰ������, ��1��ʼ
	// ����: 	int iRowCount: ����
	// ����: 	int iColumnCount: ����
	// ����: 	nPointPosition iFirstPointPosition: ��ʼ���λ��
	// ����: 	nPointDirection iSecondPointDirection: �������λ��
	// ����: 	nLineFeedWay iLineFeedWay: ����ʱ�ķ�ʽ
	// ����:	QList<int>: �����ڽӵ�����
	// ����:	��ȡĳ����������ڽӵ�����
	//************************************
	static QList<int> GetAdjacencyPointIndex(int index, int iRowCount, int iColumnCount,
		nPointPosition iFirstPointPosition = nPointPosition::PointPosition_TopLeft,
		nPointDirection iSecondPointDirection = nPointDirection::PointDirection_ToRight,
		nLineFeedWay iLineFeedWay = nLineFeedWay::nSameAsPrevious);

	//************************************
	// ����: 	int index: ��ǰ������, ��1��ʼ
	// ����: 	int iRowCount: ����
	// ����: 	int iColumnCount: ����
	// ����: 	int & iRow: ���� ��, ��1��ʼ
	// ����: 	int & iColumn: ���� ��, ��1��ʼ
	// ����: 	nPointPosition iFirstPointPosition: ��ʼ���λ��
	// ����: 	nPointDirection iSecondPointDirection: �������λ��
	// ����: 	nLineFeedWay iLineFeedWay: ����ʱ�ķ�ʽ
	// ����:	bool: �ɹ�/ʧ��
	// ����:	��ĳ�����index ��ȡ ĳ�����λ��
	//************************************
	static bool GetPointPosition(int index, int iRowCount, int iColumnCount, int &iRow, int &iColumn,
		nPointPosition iFirstPointPosition = nPointPosition::PointPosition_TopLeft,
		nPointDirection iSecondPointDirection = nPointDirection::PointDirection_ToRight,
		nLineFeedWay iLineFeedWay = nLineFeedWay::nSameAsPrevious);
	
	//************************************
	// ����: 	int & index: ����ֵ ��ǰ������, ��1��ʼ
	// ����: 	int iRowCount: ����
	// ����: 	int iColumnCount: ����
	// ����: 	int iRow: ��, ��1��ʼ
	// ����: 	int iColumn: ��, ��1��ʼ
	// ����: 	nPointPosition iFirstPointPosition: ��ʼ���λ��
	// ����: 	nPointDirection iSecondPointDirection: �������λ��
	// ����: 	nLineFeedWay iLineFeedWay: ����ʱ�ķ�ʽ
	// ����:	bool: �ɹ�/ʧ��
	// ����:	��ĳ���������λ�� ��ȡ ĳ�����index
	//************************************
	static bool GetPointIndex(int &index, int iRowCount, int iColumnCount, int iRow, int iColumn,
		nPointPosition iFirstPointPosition = nPointPosition::PointPosition_TopLeft,
		nPointDirection iSecondPointDirection = nPointDirection::PointDirection_ToRight,
		nLineFeedWay iLineFeedWay = nLineFeedWay::nSameAsPrevious);

	//************2024/05/21**********
	// ����: ��ȡĳ���������λ�� 
	// ����: bool �ɹ�/ʧ��
	// ����: int index: ���
	// ����: int iRowCount: ������
	// ����: int iColumnCount: ������ 
	// ����: int iFirstPointX: ��һ�����X����
	// ����: int iFirstPointY: ��һ�����Y����
	// ����: int iXSpacing: X������
	// ����: int iYSpacing: Y������
	// ����: QPoint & point: ���ص������
	// ����: nPointPosition iFirstPointPosition: ��ʼ���λ��
	// ����: nPointDirection iSecondPointDirection: �������λ��
	// ����: nLineFeedWay iLineFeedWay: ����ʱ�ķ�ʽ
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

//��λ�Ų����ý���
class TPointArrangementUi : public QDialog
{
	Q_OBJECT

public:
	TPointArrangementUi(nArrangementType iType, 
		int iLength,//���س���
		int iWidth,//���ؿ��
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