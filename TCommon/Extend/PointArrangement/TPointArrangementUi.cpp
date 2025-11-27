#include <QMessageBox>
#include "TPointArrangementUi.h"
#include "ui_TPointArrangementUi.h"


TPointArrangementUi::TPointArrangementUi(nArrangementType iType, 
	int iLength,
	int iWidth,
	int iPointRadius,
	TPointArrangement defaultModelArrangement,
	QWidget *parent)
	: QDialog(parent),
	m_iLength(iLength),
	m_iWidth(iWidth),
	m_iPointRadius(iPointRadius)
{
	ui = new Ui::TPointArrangementUi();
	ui->setupUi(this);

	m_iType = iType;
	if (iType == nArrangementType::ArrangementType_AddOne)
		ui->pointCountFrame->hide();

	initWidget(defaultModelArrangement);
}

TPointArrangementUi::~TPointArrangementUi()
{
	delete ui;
}

void TPointArrangementUi::on_okBtn_clicked()
{
	TPointArrangement modelArrangement;
	modelArrangement.iType = m_iType;
	modelArrangement.iPointRowCount = ui->pointRowSBox->value();
	modelArrangement.iPointColumnCount = ui->pointColumnSBox->value();
	modelArrangement.iFirstPointPosition = (nPointPosition)ui->firstPointPositionCombBox->currentIndex();
	QString strSecondPointDirection = ui->secondPointDirectionCombBox->currentText();
	if (strSecondPointDirection == PDToRight)
		modelArrangement.iSecondPointDirection = nPointDirection::PointDirection_ToRight;
	else if (strSecondPointDirection == PDToLeft)
		modelArrangement.iSecondPointDirection = nPointDirection::PointDirection_ToLeft;
	else if (strSecondPointDirection == PDUpward)
		modelArrangement.iSecondPointDirection = nPointDirection::PointDirection_Upward;
	else if (strSecondPointDirection == PDDown)
		modelArrangement.iSecondPointDirection = nPointDirection::PointDirection_Down;
	modelArrangement.iFirstPointX = ui->firstPointXSBox->value();
	modelArrangement.iFirstPointY = ui->firstPointYSBox->value();
	modelArrangement.iXSpacing = ui->xSpacingSBox->value();
	modelArrangement.iYSpacing = ui->ySpacingSBox->value();
	if (m_iType == nArrangementType::ArrangementType_Auto)
		modelArrangement.iPointCount = ui->pointCountSBox->value();
	modelArrangement.iLineFeedWay = (nLineFeedWay)ui->lineFeedWayBox->currentIndex();
	if (check(modelArrangement))
	{
		emit signalModelArrangement(modelArrangement);
		accept();
	}
}

void TPointArrangementUi::on_cancelBtn_clicked()
{
	reject();
}

void TPointArrangementUi::initWidget(TPointArrangement defaultModelArrangement)
{
	ui->pointRowSBox->setValue(defaultModelArrangement.iPointRowCount);
	ui->pointColumnSBox->setValue(defaultModelArrangement.iPointColumnCount);
	ui->firstPointXSBox->setValue(defaultModelArrangement.iFirstPointX);
	ui->firstPointYSBox->setValue(defaultModelArrangement.iFirstPointY);
	ui->xSpacingSBox->setValue(defaultModelArrangement.iXSpacing);
	ui->ySpacingSBox->setValue(defaultModelArrangement.iYSpacing);
	ui->pointCountSBox->setValue(defaultModelArrangement.iPointCount);

	ui->firstPointXSBox->setMinimum(std::min(m_iPointRadius, m_iLength - m_iPointRadius));
	ui->firstPointXSBox->setMaximum(std::max(m_iPointRadius, m_iLength - m_iPointRadius));
	ui->firstPointYSBox->setMinimum(std::min(m_iPointRadius, m_iWidth - m_iPointRadius));
	ui->firstPointYSBox->setMaximum(std::max(m_iPointRadius, m_iWidth - m_iPointRadius));
	ui->xSpacingSBox->setMinimum(m_iPointRadius * 2);
	ui->ySpacingSBox->setMinimum(m_iPointRadius * 2);

	ui->firstPointPositionCombBox->addItems(QStringList()
		<< TopLeft
		<< TopRight
		<< BottomLeft
		<< BottomRight);
	ui->firstPointPositionCombBox->setCurrentIndex(defaultModelArrangement.iFirstPointPosition);
	if ((nPointPosition)defaultModelArrangement.iFirstPointPosition == 
		nPointPosition::PointPosition_TopLeft)
		ui->secondPointDirectionCombBox->addItems(QStringList() << PDToRight << PDDown);
	else if ((nPointPosition)defaultModelArrangement.iFirstPointPosition == 
		nPointPosition::PointPosition_TopRight)
		ui->secondPointDirectionCombBox->addItems(QStringList() << PDToLeft << PDDown);
	else if ((nPointPosition)defaultModelArrangement.iFirstPointPosition == 
		nPointPosition::PointPosition_BottomLeft)
		ui->secondPointDirectionCombBox->addItems(QStringList() << PDUpward << PDToRight);
	else if ((nPointPosition)defaultModelArrangement.iFirstPointPosition == 
		nPointPosition::PointPosition_BottomRight)
		ui->secondPointDirectionCombBox->addItems(QStringList() << PDUpward << PDToLeft);
	ui->secondPointDirectionCombBox->setCurrentIndex(0);

	ui->lineFeedWayBox->addItems(QStringList() << SameAsPrevious << SnakeLike);
	connect(ui->firstPointPositionCombBox, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, [&](int iPostion) {
		ui->secondPointDirectionCombBox->clear();
		if ((nPointPosition)iPostion == nPointPosition::PointPosition_TopLeft)
			ui->secondPointDirectionCombBox->addItems(QStringList() << PDToRight << PDDown);
		else if ((nPointPosition)iPostion == nPointPosition::PointPosition_TopRight)
			ui->secondPointDirectionCombBox->addItems(QStringList() << PDToLeft << PDDown);
		else if ((nPointPosition)iPostion == nPointPosition::PointPosition_BottomLeft)
			ui->secondPointDirectionCombBox->addItems(QStringList() << PDUpward << PDToRight);
		else if ((nPointPosition)iPostion == nPointPosition::PointPosition_BottomRight)
			ui->secondPointDirectionCombBox->addItems(QStringList() << PDUpward << PDToLeft);
	}, Qt::QueuedConnection);

	connect(ui->pointRowSBox, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, [&](int iRow) {
		int iColumn = ui->pointColumnSBox->value();
		if (iColumn != 0 && iRow != 0)
		{
			if (ui->pointCountSBox->value() > iRow * iColumn)
				ui->pointCountSBox->setValue(iRow * iColumn);
			ui->pointCountSBox->setMaximum(iRow * iColumn);
		}
	}, Qt::QueuedConnection);
	connect(ui->pointColumnSBox, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, [&](int iColumn) {
		int iRow = ui->pointRowSBox->value();
		if (iColumn != 0 && iRow != 0)
		{
			if (ui->pointCountSBox->value() > iRow * iColumn)
				ui->pointCountSBox->setValue(iRow * iColumn);
			ui->pointCountSBox->setMaximum(iRow * iColumn);
		}
	}, Qt::QueuedConnection);
}

bool TPointArrangementUi::check(const TPointArrangement& modelArrangement)
{
	if (modelArrangement.iPointRowCount == 0 ||
		modelArrangement.iPointColumnCount == 0 ||
		modelArrangement.iPointCount > modelArrangement.iPointRowCount * modelArrangement.iPointColumnCount)
	{
		QMessageBox::warning(nullptr,
			QString::fromUtf8("Warning"),
			QString::fromUtf8("Row/Colomn/Point Count value error!\n行/列/点位总数错误!"));
		return false;
	}
	if (modelArrangement.iFirstPointX < m_iPointRadius ||
		modelArrangement.iFirstPointX > (m_iLength - m_iPointRadius) ||
		modelArrangement.iFirstPointY < m_iPointRadius ||
		modelArrangement.iFirstPointY >(m_iWidth - m_iPointRadius))
	{
		QMessageBox::warning(nullptr,
			QString::fromUtf8("Warning"),
			QString::fromUtf8("First Point X/Y value error, X:[%1, %2], Y:[%1, %2]!\n\
第一个点的X/Y坐标错误!").arg(m_iPointRadius).arg(m_iLength - m_iPointRadius)
.arg(m_iPointRadius).arg(m_iWidth - m_iPointRadius));
		return false;
	}
	if (modelArrangement.iXSpacing < 2 * m_iPointRadius || 
		modelArrangement.iYSpacing < 2 * m_iPointRadius)
	{
		QMessageBox::warning(nullptr,
			QString::fromUtf8("Warning"),
			QString::fromUtf8("X/Y Spacing value is smaller than a point's diameter(%1)!\n\
X/Y间距不足以放两个点位!").arg(2 * m_iPointRadius));
		return false;
	}
	if (modelArrangement.iFirstPointPosition == nPointPosition::PointPosition_TopLeft ||
		modelArrangement.iFirstPointPosition == nPointPosition::PointPosition_TopRight)
	{ 
		int iYSpacingMax = m_iWidth - m_iPointRadius - modelArrangement.iFirstPointY;
		if (modelArrangement.iPointRowCount > 1)
			iYSpacingMax /= (modelArrangement.iPointRowCount - 1);
		//iYSpacingMax -= 2 * m_iPointRadius;
		if (modelArrangement.iYSpacing > iYSpacingMax)
		{
			QMessageBox::warning(nullptr,
				QString::fromUtf8("Warning"),
				QString::fromUtf8("Y Spacing value is too big!(%1)\nY轴间距过大")
.arg(iYSpacingMax));
			return false;
		}
	}
	else if (modelArrangement.iFirstPointPosition == nPointPosition::PointPosition_BottomLeft ||
		modelArrangement.iFirstPointPosition == nPointPosition::PointPosition_BottomRight)
	{
		int iYSpacingMax = modelArrangement.iFirstPointY - m_iPointRadius;
		if (modelArrangement.iPointRowCount > 1)
			iYSpacingMax /= (modelArrangement.iPointRowCount - 1);
		//iYSpacingMax -= 2 * m_iPointRadius;
		if (modelArrangement.iYSpacing > iYSpacingMax)
		{
			QMessageBox::warning(nullptr,
				QString::fromUtf8("Warning"),
				QString::fromUtf8("Y Spacing value is too big!(%1)\nY轴间距过大")
				.arg(iYSpacingMax));
			return false;
		}
	}

	if (modelArrangement.iFirstPointPosition == nPointPosition::PointPosition_TopLeft ||
		modelArrangement.iFirstPointPosition == nPointPosition::PointPosition_BottomLeft)
	{
		int iXSpacingMax = m_iLength - m_iPointRadius - modelArrangement.iFirstPointX;
		if (modelArrangement.iPointColumnCount > 1)
			iXSpacingMax /= (modelArrangement.iPointColumnCount - 1);
		//iXSpacingMax -= 2 * m_iPointRadius;
		if (modelArrangement.iXSpacing > iXSpacingMax)
		{
			QMessageBox::warning(nullptr,
				QString::fromUtf8("Warning"),
				QString::fromUtf8("X Spacing value is too big!(%1)\nY轴间距过大")
				.arg(iXSpacingMax));
			return false;
		}
	}
	else if (modelArrangement.iFirstPointPosition == nPointPosition::PointPosition_TopRight ||
		modelArrangement.iFirstPointPosition == nPointPosition::PointPosition_BottomRight)
	{
		int iXSpacingMax = modelArrangement.iFirstPointX - m_iPointRadius;
		if (modelArrangement.iPointColumnCount > 1)
			iXSpacingMax /= (modelArrangement.iPointColumnCount - 1);
		//iXSpacingMax -= 2 * m_iPointRadius;
		if (modelArrangement.iXSpacing > iXSpacingMax)
		{
			QMessageBox::warning(nullptr,
				QString::fromUtf8("Warning"),
				QString::fromUtf8("X Spacing value is too big!(%1)\nY轴间距过大")
				.arg(iXSpacingMax));
			return false;
		}
	}
	return true;
}

QList<int> TPointArrangement::GetAdjacencyPointIndex(int index, int iRowCount, int iColumnCount,
	nPointPosition iFirstPointPosition /*= nPointPosition::PointPosition_TopLeft*/, 
	nPointDirection iSecondPointDirection /*= nPointDirection::PointDirection_ToRight*/,
	nLineFeedWay iLineFeedWay /*= nLineFeedWay::nSameAsPrevious*/)
{
	QList<int> lstIndex;
	if (index > iRowCount * iColumnCount)
		return lstIndex;

	int iRow(0), iColumn(0);
	bool ret = GetPointPosition(index, iRowCount, iColumnCount, iRow, iColumn, 
		iFirstPointPosition, iSecondPointDirection, iLineFeedWay);
	if (!ret)
		return lstIndex;

	//上
	if (iRow > 1)
	{
		int iRowAdjacency = iRow - 1;
		int iColumnAdjacency = iColumn;
		int indexAdjacency(0);
		ret = GetPointIndex(indexAdjacency, iRowCount, iColumnCount,
			iRowAdjacency, iColumnAdjacency,
			iFirstPointPosition,
			iSecondPointDirection,
			iLineFeedWay);
		if (ret)
			lstIndex.append(indexAdjacency);
	}
	//下
	if (iRow < iRowCount)
	{
		int iRowAdjacency = iRow + 1;
		int iColumnAdjacency = iColumn;
		int indexAdjacency(0);
		ret = GetPointIndex(indexAdjacency, iRowCount, iColumnCount,
			iRowAdjacency, iColumnAdjacency,
			iFirstPointPosition,
			iSecondPointDirection,
			iLineFeedWay);
		if (ret)
			lstIndex.append(indexAdjacency);
	}
	//左
	if (iColumn > 1)
	{
		int iRowAdjacency = iRow;
		int iColumnAdjacency = iColumn - 1;
		int indexAdjacency(0);
		ret = GetPointIndex(indexAdjacency, iRowCount, iColumnCount,
			iRowAdjacency, iColumnAdjacency,
			iFirstPointPosition,
			iSecondPointDirection,
			iLineFeedWay);
		if (ret)
			lstIndex.append(indexAdjacency);
	}
	//右
	if (iColumn < iColumnCount)
	{
		int iRowAdjacency = iRow;
		int iColumnAdjacency = iColumn + 1;
		int indexAdjacency(0);
		ret = GetPointIndex(indexAdjacency, iRowCount, iColumnCount,
			iRowAdjacency, iColumnAdjacency,
			iFirstPointPosition,
			iSecondPointDirection,
			iLineFeedWay);
		if (ret)
			lstIndex.append(indexAdjacency);
	}
	return lstIndex;
}

bool TPointArrangement::GetPointPosition(int index, int iRowCount, int iColumnCount, 
	int &iRow, int &iColumn, 
	nPointPosition iFirstPointPosition /*= nPointPosition::PointPosition_TopLeft*/, 
	nPointDirection iSecondPointDirection /*= nPointDirection::PointDirection_ToRight*/,
	nLineFeedWay iLineFeedWay /*= nLineFeedWay::nSameAsPrevious*/)
{
	if (index > iRowCount * iColumnCount ||
		iRowCount <= 0 ||
		iColumnCount <= 0 ||
		index <= 0)
		return false;

	if (iLineFeedWay == nLineFeedWay::nSameAsPrevious)
	{
		if (iFirstPointPosition ==
			nPointPosition::PointPosition_TopLeft)
		{
			if (iSecondPointDirection ==
				nPointDirection::PointDirection_ToRight)
			{
				iColumn = (index - 1) % iColumnCount + 1;
				iRow = (index - 1) / iColumnCount + 1;
			}
			else if (iSecondPointDirection ==
				nPointDirection::PointDirection_Down)
			{
				iRow = (index - 1) % iRowCount + 1;
				iColumn = (index - 1) / iRowCount + 1;
			}
		}
		else if (iFirstPointPosition ==
			nPointPosition::PointPosition_TopRight)
		{
			if (iSecondPointDirection ==
				nPointDirection::PointDirection_ToLeft)
			{
				iColumn = iColumnCount - (index - 1) % iColumnCount;
				iRow = (index - 1) / iColumnCount + 1;
			}
			else if (iSecondPointDirection ==
				nPointDirection::PointDirection_Down)
			{
				iRow = (index - 1) % iRowCount + 1;
				iColumn = iColumnCount - (index - 1) / iRowCount;
			}
		}
		else if (iFirstPointPosition ==
			nPointPosition::PointPosition_BottomLeft)
		{
			if (iSecondPointDirection ==
				nPointDirection::PointDirection_Upward)
			{
				iRow = iRowCount - (index - 1) % iRowCount;
				iColumn = (index - 1) / iRowCount + 1;
			}
			else if (iSecondPointDirection ==
				nPointDirection::PointDirection_ToRight)
			{
				iColumn = (index - 1) % iColumnCount + 1;
				iRow = iRowCount - (index - 1) / iColumnCount;
			}
		}
		else if (iFirstPointPosition ==
			nPointPosition::PointPosition_BottomRight)
		{
			if (iSecondPointDirection ==
				nPointDirection::PointDirection_ToLeft)
			{
				iColumn = iColumnCount - (index - 1) % iColumnCount;
				iRow = iRowCount - (index - 1) / iColumnCount;
			}
			else if (iSecondPointDirection ==
				nPointDirection::PointDirection_Upward)
			{
				iRow = iRowCount - (index - 1) % iRowCount;
				iColumn = iColumnCount - (index - 1) / iRowCount;
			}
		}
	}
	else if (iLineFeedWay == nLineFeedWay::nSnakeLike)
	{
		if (iFirstPointPosition ==
			nPointPosition::PointPosition_TopLeft)
		{
			if (iSecondPointDirection ==
				nPointDirection::PointDirection_ToRight)
			{
				bool sequence = (index - 1) / iColumnCount % 2;
				if (!sequence)
					iColumn = (index - 1) % iColumnCount + 1;
				else
					iColumn = iColumnCount - (index - 1) % iColumnCount;
				iRow = (index - 1) / iColumnCount + 1;
			}
			else if (iSecondPointDirection ==
				nPointDirection::PointDirection_Down)
			{
				bool sequence = (index - 1) / iRowCount % 2;
				if (!sequence)
					iRow = (index - 1) % iRowCount + 1;
				else
					iRow = iRowCount - (index - 1) % iRowCount;
				iColumn = (index - 1) / iRowCount + 1;
			}
		}
		else if (iFirstPointPosition ==
			nPointPosition::PointPosition_TopRight)
		{
			if (iSecondPointDirection ==
				nPointDirection::PointDirection_ToLeft)
			{
				bool sequence = (index - 1) / iColumnCount % 2;
				if (!sequence)
					iColumn = iColumnCount - (index - 1) % iColumnCount;
				else
					iColumn = (index - 1) % iColumnCount + 1;
				iRow = (index - 1) / iColumnCount + 1;
			}
			else if (iSecondPointDirection ==
				nPointDirection::PointDirection_Down)
			{
				bool sequence = (index - 1) / iRowCount % 2;
				if (!sequence)
					iRow = (index - 1) % iRowCount + 1;
				else
					iRow = iRowCount - (index - 1) % iRowCount;
				iColumn = iColumnCount - (index - 1) / iRowCount;
			}
		}
		else if (iFirstPointPosition ==
			nPointPosition::PointPosition_BottomLeft)
		{
			if (iSecondPointDirection ==
				nPointDirection::PointDirection_Upward)
			{
				bool sequence = (index - 1) / iRowCount % 2;
				if (!sequence)
					iRow = iRowCount - (index - 1) % iRowCount;
				else
					iRow = (index - 1) % iRowCount + 1;
				iColumn = (index - 1) / iRowCount + 1;
			}
			else if (iSecondPointDirection ==
				nPointDirection::PointDirection_ToRight)
			{
				bool sequence = (index - 1) / iColumnCount % 2;
				if (!sequence)
					iColumn = (index - 1) % iColumnCount + 1;
				else
					iColumn = iColumnCount - (index - 1) % iColumnCount;
				iRow = iRowCount - (index - 1) / iColumnCount;
			}
		}
		else if (iFirstPointPosition ==
			nPointPosition::PointPosition_BottomRight)
		{
			if (iSecondPointDirection ==
				nPointDirection::PointDirection_ToLeft)
			{
				bool sequence = (index - 1) / iColumnCount % 2;
				if (!sequence)
					iColumn = iColumnCount - (index - 1) % iColumnCount;
				else
					iColumn = (index - 1) % iColumnCount + 1;
				iRow = iRowCount - (index - 1) / iColumnCount;
			}
			else if (iSecondPointDirection ==
				nPointDirection::PointDirection_Upward)
			{
				bool sequence = (index - 1) / iRowCount % 2;
				if (!sequence)
					iRow = iRowCount - (index - 1) % iRowCount;
				else
					iRow = (index - 1) % iRowCount + 1;
				iColumn = iColumnCount - (index - 1) / iRowCount;
			}
		}
	}

	return true;
}

bool TPointArrangement::GetPointIndex(int &index, int iRowCount, int iColumnCount, int iRow, int iColumn, 
	nPointPosition iFirstPointPosition /*= nPointPosition::PointPosition_TopLeft*/, 
	nPointDirection iSecondPointDirection /*= nPointDirection::PointDirection_ToRight*/,
	nLineFeedWay iLineFeedWay /*= nLineFeedWay::nSameAsPrevious*/)
{
	if (iRow > iRowCount ||
		iColumn > iColumnCount ||
		iRowCount <= 0 ||
		iColumnCount <= 0)
		return false;

	index = 0;

	if (iLineFeedWay == nLineFeedWay::nSameAsPrevious)
	{
		if (iFirstPointPosition ==
			nPointPosition::PointPosition_TopLeft)
		{
			if (iSecondPointDirection ==
				nPointDirection::PointDirection_ToRight)
			{
				index = (iRow - 1) * iColumnCount;
				index += iColumn;
			}
			else if (iSecondPointDirection ==
				nPointDirection::PointDirection_Down)
			{
				index = (iColumn - 1) * iRowCount;
				index += iRow;
			}
		}
		else if (iFirstPointPosition ==
			nPointPosition::PointPosition_TopRight)
		{
			if (iSecondPointDirection ==
				nPointDirection::PointDirection_ToLeft)
			{
				index = (iRow - 1) * iColumnCount;
				index += iColumnCount - iColumn + 1;
			}
			else if (iSecondPointDirection ==
				nPointDirection::PointDirection_Down)
			{
				index = (iColumnCount - iColumn) * iRowCount;
				index += iRow;
			}
		}
		else if (iFirstPointPosition ==
			nPointPosition::PointPosition_BottomLeft)
		{
			if (iSecondPointDirection ==
				nPointDirection::PointDirection_Upward)
			{
				index = (iColumn - 1) * iRowCount;
				index += iRowCount - iRow + 1;
			}
			else if (iSecondPointDirection ==
				nPointDirection::PointDirection_ToRight)
			{
				index = (iRowCount - iRow) * iColumnCount;
				index += iColumn;
			}
		}
		else if (iFirstPointPosition ==
			nPointPosition::PointPosition_BottomRight)
		{
			if (iSecondPointDirection ==
				nPointDirection::PointDirection_ToLeft)
			{
				index = (iRowCount - iRow) * iColumnCount;
				index += iColumnCount - iColumn + 1;
			}
			else if (iSecondPointDirection ==
				nPointDirection::PointDirection_Upward)
			{
				index = (iColumnCount - iColumn) * iRowCount;
				index += iRowCount - iRow + 1;
			}
		}
	}
	else if (iLineFeedWay == nLineFeedWay::nSnakeLike)
	{
		if (iFirstPointPosition ==
			nPointPosition::PointPosition_TopLeft)
		{
			if (iSecondPointDirection ==
				nPointDirection::PointDirection_ToRight)
			{
				index = (iRow - 1) * iColumnCount;
				if (iRow % 2)
					index += iColumn;
				else
					index += (iColumnCount - iColumn) + 1;
			}
			else if (iSecondPointDirection ==
				nPointDirection::PointDirection_Down)
			{
				index = (iColumn - 1) * iRowCount;
				if (iColumn % 2)
					index += iRow;
				else
					index += (iRowCount - iRow) + 1;
			}
		}
		else if (iFirstPointPosition ==
			nPointPosition::PointPosition_TopRight)
		{
			if (iSecondPointDirection ==
				nPointDirection::PointDirection_ToLeft)
			{
				index = (iRow - 1) * iColumnCount;
				if (iRow % 2)
					index += (iColumnCount - iColumn) + 1;
				else
					index += iColumn;
			}
			else if (iSecondPointDirection ==
				nPointDirection::PointDirection_Down)
			{
				index = (iColumnCount - iColumn) * iRowCount;
				if ((iColumnCount - iColumn) % 2)
					index += (iRowCount - iRow) + 1;
				else
					index += iRow;
			}
		}
		else if (iFirstPointPosition ==
			nPointPosition::PointPosition_BottomLeft)
		{
			if (iSecondPointDirection ==
				nPointDirection::PointDirection_Upward)
			{
				index = (iColumn - 1) * iRowCount;
				if (iColumn % 2)
					index += (iRowCount - iRow) + 1;
				else
					index += iRow;
			}
			else if (iSecondPointDirection ==
				nPointDirection::PointDirection_ToRight)
			{
				index = (iRowCount - iRow) * iColumnCount;
				if ((iRowCount - iRow) % 2)
					index += (iColumnCount - iColumn) + 1;
				else
					index += iColumn;
			}
		}
		else if (iFirstPointPosition ==
			nPointPosition::PointPosition_BottomRight)
		{
			if (iSecondPointDirection ==
				nPointDirection::PointDirection_ToLeft)
			{
				index = (iRowCount - iRow) * iColumnCount;
				if ((iRowCount - iRow) % 2)
					index += iColumn;
				else
					index += (iColumnCount - iColumn) + 1;
			}
			else if (iSecondPointDirection ==
				nPointDirection::PointDirection_Upward)
			{
				index = (iColumnCount - iColumn) * iRowCount;
				if ((iColumnCount - iColumn) % 2)
					index += iRow;
				else
					index += (iRowCount - iRow) + 1;
			}
		}
	}
	return true;
}

bool TPointArrangement::getPoint(int index, int iRowCount, int iColumnCount,
	int iFirstPointX, int iFirstPointY,
	int iXSpacing, int iYSpacing,
	QPoint &point,
	nPointPosition iFirstPointPosition /*= nPointPosition::PointPosition_TopLeft*/,
	nPointDirection iSecondPointDirection /*= nPointDirection::PointDirection_ToRight*/,
	nLineFeedWay iLineFeedWay /*= nLineFeedWay::nSameAsPrevious*/)
{
	if (index > iRowCount * iColumnCount)
		return false;
	if (index == 1)
	{
		point.rx() = iFirstPointX;
		point.ry() = iFirstPointY;
		return true;
	}

	if (iLineFeedWay == nLineFeedWay::nSameAsPrevious)
	{
		if (iFirstPointPosition ==
			nPointPosition::PointPosition_TopLeft)
		{
			if (iSecondPointDirection ==
				nPointDirection::PointDirection_ToRight)
			{
				point.rx() = iFirstPointX;
				point.rx() += ((index - 1) % iColumnCount) * iXSpacing;
				point.ry() = iFirstPointY;
				point.ry() += ((index - 1) / iColumnCount) * iYSpacing;
				return true;
			}
			else if (iSecondPointDirection ==
				nPointDirection::PointDirection_Down)
			{
				point.rx() = iFirstPointX;
				point.rx() += ((index - 1) / iRowCount) * iXSpacing;
				point.ry() = iFirstPointY;
				point.ry() += ((index - 1) % iRowCount) * iYSpacing;
				return true;
			}
		}
		else if (iFirstPointPosition ==
			nPointPosition::PointPosition_TopRight)
		{
			if (iSecondPointDirection ==
				nPointDirection::PointDirection_ToLeft)
			{
				point.rx() = iFirstPointX;
				point.rx() -= ((index - 1) % iColumnCount) * iXSpacing;
				point.ry() = iFirstPointY;
				point.ry() += ((index - 1) / iColumnCount) * iYSpacing;
				return true;
			}
			else if (iSecondPointDirection ==
				nPointDirection::PointDirection_Down)
			{
				point.rx() = iFirstPointX;
				point.rx() -= ((index - 1) / iRowCount) * iXSpacing;
				point.ry() = iFirstPointY;
				point.ry() += ((index - 1) % iRowCount) * iYSpacing;
				return true;
			}
		}
		else if (iFirstPointPosition ==
			nPointPosition::PointPosition_BottomLeft)
		{
			if (iSecondPointDirection ==
				nPointDirection::PointDirection_Upward)
			{
				point.rx() = iFirstPointX;
				point.rx() += ((index - 1) / iRowCount) * iXSpacing;
				point.ry() = iFirstPointY;
				point.ry() -= ((index - 1) % iRowCount) * iYSpacing;
				return true;
			}
			else if (iSecondPointDirection ==
				nPointDirection::PointDirection_ToRight)
			{
				point.rx() = iFirstPointX;
				point.rx() += ((index - 1) % iColumnCount) * iXSpacing;
				point.ry() = iFirstPointY;
				point.ry() -= ((index - 1) / iColumnCount) * iYSpacing;
				return true;
			}
		}
		else if (iFirstPointPosition ==
			nPointPosition::PointPosition_BottomRight)
		{
			if (iSecondPointDirection ==
				nPointDirection::PointDirection_ToLeft)
			{
				point.rx() = iFirstPointX;
				point.rx() -= ((index - 1) % iColumnCount) * iXSpacing;
				point.ry() = iFirstPointY;
				point.ry() -= ((index - 1) / iColumnCount) * iYSpacing;
				return true;
			}
			else if (iSecondPointDirection ==
				nPointDirection::PointDirection_Upward)
			{
				point.rx() = iFirstPointX;
				point.rx() -= ((index - 1) / iRowCount) * iXSpacing;
				point.ry() = iFirstPointY;
				point.ry() -= ((index - 1) % iRowCount) * iYSpacing;
				return true;
			}
		}
	}
	else if (iLineFeedWay == nLineFeedWay::nSnakeLike)
	{
		if (iFirstPointPosition ==
			nPointPosition::PointPosition_TopLeft)
		{
			if (iSecondPointDirection ==
				nPointDirection::PointDirection_ToRight)
			{
				bool sequence = (index - 1) / iColumnCount % 2;
				int iXMax = iFirstPointX;
				iXMax += (iColumnCount - 1) * iXSpacing;
				if (!sequence)
				{
					point.rx() = iFirstPointX;
					point.rx() += ((index - 1) % iColumnCount) * iXSpacing;
				}
				else
				{
					point.rx() = iXMax;
					point.rx() -= ((index - 1) % iColumnCount) * iXSpacing;
				}
				point.ry() = iFirstPointY;
				point.ry() += ((index - 1) / iColumnCount) * iYSpacing;
				return true;
			}
			else if (iSecondPointDirection ==
				nPointDirection::PointDirection_Down)
			{
				bool sequence = (index - 1) / iRowCount % 2;
				int iYMax = iFirstPointY;
				iYMax += (iRowCount - 1) * iYSpacing;
				point.rx() = iFirstPointX;
				point.rx() += ((index - 1) / iRowCount) * iXSpacing;
				if (!sequence)
				{
					point.ry() = iFirstPointY;
					point.ry() += ((index - 1) % iRowCount) * iYSpacing;
				}
				else
				{
					point.ry() = iYMax;
					point.ry() -= ((index - 1) % iRowCount) * iYSpacing;
				}
				return true;
			}
		}
		else if (iFirstPointPosition ==
			nPointPosition::PointPosition_TopRight)
		{
			if (iSecondPointDirection ==
				nPointDirection::PointDirection_ToLeft)
			{
				bool sequence = (index - 1) / iColumnCount % 2;
				int iXMin = iFirstPointX;
				iXMin -= (iColumnCount - 1) * iXSpacing;
				if (!sequence)
				{
					point.rx() = iFirstPointX;
					point.rx() -= ((index - 1) % iColumnCount) * iXSpacing;
				}
				else
				{
					point.rx() = iXMin;
					point.rx() += ((index - 1) % iColumnCount) * iXSpacing;
				}
				point.ry() = iFirstPointY;
				point.ry() += ((index - 1) / iColumnCount) * iYSpacing;
				return true;
			}
			else if (iSecondPointDirection ==
				nPointDirection::PointDirection_Down)
			{
				bool sequence = (index - 1) / iRowCount % 2;
				int iYMax = iFirstPointY;
				iYMax += (iRowCount - 1) * iYSpacing;
				point.rx() = iFirstPointX;
				point.rx() -= ((index - 1) / iRowCount) * iXSpacing;
				if (!sequence)
				{
					point.ry() = iFirstPointY;
					point.ry() += ((index - 1) % iRowCount) * iYSpacing;
				}
				else
				{
					point.ry() = iYMax;
					point.ry() -= ((index - 1) % iRowCount) * iYSpacing;
				}
				return true;
			}
		}
		else if (iFirstPointPosition ==
			nPointPosition::PointPosition_BottomLeft)
		{
			if (iSecondPointDirection ==
				nPointDirection::PointDirection_Upward)
			{
				bool sequence = (index - 1) / iRowCount % 2;
				int iYMin = iFirstPointY;
				iYMin -= (iRowCount - 1) * iYSpacing;
				point.rx() = iFirstPointX;
				point.rx() += ((index - 1) / iRowCount) * iXSpacing;
				if (!sequence)
				{
					point.ry() = iFirstPointY;
					point.ry() -= ((index - 1) % iRowCount) * iYSpacing;
				}
				else
				{
					point.ry() = iYMin;
					point.ry() += ((index - 1) % iRowCount) * iYSpacing;
				}
				return true;
			}
			else if (iSecondPointDirection ==
				nPointDirection::PointDirection_ToRight)
			{
				bool sequence = (index - 1) / iColumnCount % 2;
				int iXMax = iFirstPointX;
				iXMax += (iColumnCount - 1) * iXSpacing;
				if (!sequence)
				{
					point.rx() = iFirstPointX;
					point.rx() += ((index - 1) % iColumnCount) * iXSpacing;
				}
				else
				{
					point.rx() = iXMax;
					point.rx() -= ((index - 1) % iColumnCount) * iXSpacing;
				}
				point.ry() = iFirstPointY;
				point.ry() -= ((index - 1) / iColumnCount) * iYSpacing;
				return true;
			}
		}
		else if (iFirstPointPosition ==
			nPointPosition::PointPosition_BottomRight)
		{
			if (iSecondPointDirection ==
				nPointDirection::PointDirection_ToLeft)
			{
				bool sequence = (index - 1) / iColumnCount % 2;
				int iXMin = iFirstPointX;
				iXMin -= (iColumnCount - 1) * iXSpacing;
				if (!sequence)
				{
					point.rx() = iFirstPointX;
					point.rx() -= ((index - 1) % iColumnCount) * iXSpacing;
				}
				else
				{
					point.rx() = iXMin;
					point.rx() += ((index - 1) % iColumnCount) * iXSpacing;
				}
				point.ry() = iFirstPointY;
				point.ry() -= ((index - 1) / iColumnCount) * iYSpacing;
				return true;
			}
			else if (iSecondPointDirection ==
				nPointDirection::PointDirection_Upward)
			{
				bool sequence = (index - 1) / iRowCount % 2;
				int iYMin = iFirstPointY;
				iYMin -= (iRowCount - 1) * iYSpacing;
				point.rx() = iFirstPointX;
				point.rx() -= ((index - 1) / iRowCount) * iXSpacing;
				if (!sequence)
				{
					point.ry() = iFirstPointY;
					point.ry() -= ((index - 1) % iRowCount) * iYSpacing;
				}
				else
				{
					point.ry() = iYMin;
					point.ry() += ((index - 1) % iRowCount) * iYSpacing;
				}
				return true;
			}
		}
	}
	return false;
}
