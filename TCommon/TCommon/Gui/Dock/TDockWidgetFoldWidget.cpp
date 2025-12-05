#include <QFrame>
#include <QLabel>
#include <QVariant>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QRegularExpression>
#include "TDockWidgetFoldWidget.h"

TDockWidgetFoldWidget::TDockWidgetFoldWidget(QMainWindow* pMainWindow, 
	nFoldDirection direction /*= nFoldLeft*/,
	const QString& title /*= ""*/, 
	QWidget* parent /*= nullptr*/):
	QWidget(parent),
	m_pMainWindow(pMainWindow), 
	m_iDirection(direction),
	m_strTitle(title)
{
	initWidget();
}

TDockWidgetFoldWidget::~TDockWidgetFoldWidget()
{
}

void TDockWidgetFoldWidget::setControlWidget(QDockWidget* controlWidget)
{
	m_pControlWidget = controlWidget;
}

void TDockWidgetFoldWidget::setParternWidget(QWidget* parternWidget)
{
	m_pParternWidget = parternWidget;
}

void TDockWidgetFoldWidget::hideDock()
{
	if (m_pControlWidget && m_pParternWidget)
	{
		m_pControlWidget->hide();
		m_pParternWidget->show();
	}
}

void TDockWidgetFoldWidget::hideAllDock()
{
	if (m_pControlWidget && m_pParternWidget)
	{
		Qt::DockWidgetArea area = m_pMainWindow->dockWidgetArea(m_pControlWidget);
		QList<QDockWidget*> lstAreaDock = getAreaDockWidget(area);
		for (auto* pDock : lstAreaDock)
		{
			TDockWidgetFoldWidget* pFoldWidget = dynamic_cast<TDockWidgetFoldWidget*>(pDock->widget());
			if (pFoldWidget)
			{
				pFoldWidget->hideDock();
			}
			else
			{
				pDock->hide();
			}
		}
	}
}

void TDockWidgetFoldWidget::initWidget()
{
	QGridLayout* pGridLayout = new QGridLayout(this);
	pGridLayout->setContentsMargins(0, 0, 0, 0);

	QFrame *pFrame = new QFrame(this);
	pFrame->setObjectName(QString::fromUtf8("frame"));
	pFrame->setStyleSheet(QString::fromUtf8("QFrame#frame{\n"
		"border: 1px solid;"
		"background-color:#f0f0f2;\n"
		"}"));
	pFrame->setFrameShape(QFrame::StyledPanel);
	pFrame->setFrameShadow(QFrame::Raised);

	m_pLabel = new QLabel(pFrame);
	QPushButton* pBtn = nullptr;
	if (m_iDirection == nFoldDirection::nFoldLeft || 
		m_iDirection == nFoldDirection::nFoldRight)
	{
		QSizePolicy sizePolicy(QSizePolicy::Maximum, QSizePolicy::Fixed);
		sizePolicy.setHorizontalStretch(0);
		sizePolicy.setVerticalStretch(0);
		sizePolicy.setHeightForWidth(this->sizePolicy().hasHeightForWidth());
		this->setSizePolicy(sizePolicy);
		this->setMinimumSize(QSize(0, 0));
		this->setMaximumSize(QSize(28, 16777215));
		//this->setFixedHeight(160);

		QVBoxLayout *pVLayout = new QVBoxLayout(pFrame);
		pVLayout->setContentsMargins(0, 0, 0, 0);
		QSpacerItem *pVSpacer1 = new QSpacerItem(20, 20, QSizePolicy::Fixed, QSizePolicy::Fixed);
		pVLayout->addItem(pVSpacer1);

		m_pLabel->setObjectName(QString::fromUtf8("label"));
		m_pLabel->setText(m_strTitle.split("", QString::SkipEmptyParts).join("\n"));
		m_pLabel->setAlignment(Qt::AlignHCenter | Qt::AlignTop);
		m_pLabel->adjustSize();
		pVLayout->addWidget(m_pLabel);
		
		pBtn = new QPushButton(pFrame);
		pBtn->setFlat(true);
		if (m_iDirection == nFoldDirection::nFoldLeft)
			pBtn->setIcon(QIcon(":/TabShell/fold_right.png"));
		else
			pBtn->setIcon(QIcon(":/TabShell/fold_left.png"));
		pVLayout->addWidget(pBtn);

		QSpacerItem* pVSpacer2 = new QSpacerItem(20, 10, QSizePolicy::Fixed, QSizePolicy::Expanding);
		pVLayout->addItem(pVSpacer2);
		m_pSpacerItem = pVSpacer2;

		pGridLayout->addWidget(pFrame);
	}
	else if (m_iDirection == nFoldDirection::nFoldTop ||
		m_iDirection == nFoldDirection::nFoldBottom)
	{
		QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Maximum);
		sizePolicy.setHorizontalStretch(0);
		sizePolicy.setVerticalStretch(0);
		sizePolicy.setHeightForWidth(this->sizePolicy().hasHeightForWidth());
		this->setSizePolicy(sizePolicy);
		this->setMinimumSize(QSize(0, 0));
		this->setMaximumSize(QSize(16777215, 28));
		//this->setFixedWidth(160);

		QHBoxLayout* pHLayout = new QHBoxLayout(pFrame);
		pHLayout->setContentsMargins(0, 0, 0, 0);
		QSpacerItem* pHSpacer1 = new QSpacerItem(20, 20, QSizePolicy::Fixed, QSizePolicy::Fixed);
		pHLayout->addItem(pHSpacer1);

		m_pLabel->setText(m_strTitle);
		m_pLabel->adjustSize();
		pHLayout->addWidget(m_pLabel);

		pBtn = new QPushButton(pFrame);
		pBtn->setFlat(true);
		if (m_iDirection == nFoldDirection::nFoldTop)
			pBtn->setIcon(QIcon(":/TabShell/fold_down.png"));
		else
			pBtn->setIcon(QIcon(":/TabShell/fold_up.png"));
		pHLayout->addWidget(pBtn);

		QSpacerItem* pHSpacer2 = new QSpacerItem(20, 20, QSizePolicy::Expanding, QSizePolicy::Fixed);
		pHLayout->addItem(pHSpacer2);
		m_pSpacerItem = pHSpacer2;

		pGridLayout->addWidget(pFrame);
	}
	this->adjustSize();

	connect(pBtn, &QPushButton::clicked, this, [&]() {
		hideAllDock();
		if (m_pControlWidget)
			m_pControlWidget->raise();
		}, Qt::QueuedConnection);
}

QList<QDockWidget*> TDockWidgetFoldWidget::getAreaDockWidget(Qt::DockWidgetArea area)
{
	QList<QDockWidget*> lstAreaDock;
	if (m_pMainWindow == nullptr)
		return lstAreaDock;

	for (QDockWidget* dock : m_pMainWindow->findChildren<QDockWidget*>())
	{
		if (dock->property(PROPERTY_DOCK_TYPE).toInt() == DOCK_TYPE_FOLD_BTN &&
			m_pMainWindow->dockWidgetArea(dock) == area)
		{
			lstAreaDock.append(dock);
		}
	}
	return lstAreaDock;
}

void TDockWidgetFoldWidget::setTitle(const QString& title)
{
	m_strTitle = title;
	if (m_iDirection == nFoldDirection::nFoldLeft ||
		m_iDirection == nFoldDirection::nFoldRight)
	{
		if(m_pLabel)
			m_pLabel->setText(m_strTitle.split("", QString::SkipEmptyParts).join("\n"));
	}
	else if (m_iDirection == nFoldDirection::nFoldTop ||
		m_iDirection == nFoldDirection::nFoldBottom)
	{
		if (m_pLabel)
			m_pLabel->setText(m_strTitle);
	}
}
