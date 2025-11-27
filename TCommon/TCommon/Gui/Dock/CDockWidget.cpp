#include <QVariant>
#include <QHBoxLayout>
#include "CDockWidget.h"


CDockWidget::CDockWidget(QMainWindow* parent,
	QWidget* pWidget,
	nFoldDirection direction /*= nFoldLeft*/,
	const QString& title /*= ""*/,
	bool bCanClose /*= false*/,
	Qt::WindowFlags flags /*= Qt::WindowFlags()*/):
	QDockWidget(title, parent, flags),
	m_iDirection(direction),
	m_strTitle(title),
	m_bCanClose(bCanClose),
	m_pParent(parent)
{
	initWidget();
	pWidget->setParent(this);
	setWidget(pWidget);
}


CDockWidget::~CDockWidget()
{
	//releaseFoldDockWidget();
}


void CDockWidget::setTitleBarStyleSheet(const QString& styleSheet)
{
	m_pTitleBar->setStyleSheet(styleSheet);
}

void CDockWidget::hideDock()
{
	if (m_pFoldDockWidget &&
		m_pFoldWidget)
	{
		this->hide();
		m_pFoldDockWidget->show();
		//m_pFoldWidget->show();
	}
}

void CDockWidget::minimizeDock()
{
	if (m_pFoldBtn && !this->isHidden())
		emit m_pFoldBtn->clicked();
}

void CDockWidget::setTitle(const QString& title)
{
	m_strTitle = title;
	setWindowTitle(title);
	if (m_pLabel)
		m_pLabel->setText(m_strTitle);
	if (m_pFoldWidget)
		m_pFoldWidget->setTitle(title);
}

bool CDockWidget::isFoldDockHidden()
{
	return m_pFoldDockWidget->isHidden();
}

bool CDockWidget::isCompleteHidden()
{
	return this->isHidden() && m_pFoldDockWidget->isHidden();
}

void CDockWidget::setHideState(bool bDockHide, bool bFoldDockHide)
{
	this->setHidden(bDockHide);
	m_pFoldDockWidget->setHidden(bFoldDockHide);
}

void CDockWidget::setCompleteHide(bool hide, bool& isHidden, bool& isDockHide, bool& isFoldDockHide)
{
	if (isHidden)
	{
		if (hide)
			return;
		else
		{
			if (!isDockHide)
				this->show();
			else if (!isFoldDockHide)
				m_pFoldDockWidget->show();
			isHidden = false;
		}
	}
	else
	{
		if (hide)
		{
			isDockHide = this->isHidden();
			isFoldDockHide = m_pFoldDockWidget->isHidden();
			this->hide();
			m_pFoldDockWidget->hide();
			isHidden = true;
		}
	}
}

void CDockWidget::initWidget()
{
	setProperty(PROPERTY_DOCK_TYPE, DOCK_TYPE_FOLD_DOCK);
	m_pTitleBar = new QWidget(this);
	QHBoxLayout* pHLayout = new QHBoxLayout;
	pHLayout->setMargin(0);

	m_pFoldBtn = new QPushButton(m_pTitleBar);
	m_pFoldBtn->setFlat(true);
	m_pLabel = new QLabel(m_pTitleBar);
	m_pLabel->setText(m_strTitle);
	QSpacerItem *pSpaceItem = new QSpacerItem(30, 10, QSizePolicy::Expanding, QSizePolicy::Preferred);
	pHLayout->addWidget(m_pLabel);
	pHLayout->addItem(pSpaceItem);
	if (m_bCanClose)
	{
		m_pCloseBtn = new QPushButton(m_pTitleBar);
		m_pCloseBtn->setFlat(true);
		m_pCloseBtn->setIcon(QIcon(":/TabShell/close.png"));
		pHLayout->addWidget(m_pCloseBtn);

		connect(m_pCloseBtn, &QPushButton::clicked, this, [&]() {
			releaseFoldDockWidget();
			close();
			emit signalClose(m_strTitle);
			});
	}
	pHLayout->addWidget(m_pFoldBtn);
	m_pTitleBar->setLayout(pHLayout);
	setTitleBarWidget(m_pTitleBar);

	connect(m_pFoldBtn, &QPushButton::clicked, this, [&]() {
		if (m_pFoldDockWidget &&
			m_pFoldWidget)
		{
			//判断同一侧是否有其他QDockWidget
			{
				Qt::DockWidgetArea area = m_pParent->dockWidgetArea(this);
				int iCurDockIndx(-1);
				QList<QList<QDockWidget*>> lstAreaDock = getAreaDockWidget(area, &iCurDockIndx);
				if (lstAreaDock.isEmpty() || iCurDockIndx == -1)
				{
					return;//非正常情况
				}
				if (lstAreaDock.count() == 1)//所有的dock堆叠在一起
				{
					hideAllDock(lstAreaDock);
				}
				else
				{
					QDockWidget* pDock(nullptr);
					if (iCurDockIndx)
					{
						auto lstDock = lstAreaDock[iCurDockIndx - 1];
						pDock = lstDock.last();
					}

					else
					{
						auto lstDock = lstAreaDock[iCurDockIndx + 1];
						pDock = lstDock.last();
					}
					auto& lstDock = lstAreaDock[iCurDockIndx];
					if (pDock)
					{
						for (auto dock : lstDock)
						{
							m_pParent->tabifyDockWidget(pDock, dock);
							pDock->raise();
						}
					}
				}
			}
		}
		});

	resetDirection(m_iDirection);

	connect(this, &QDockWidget::topLevelChanged, this, [&](bool floating) {
		if (floating)
		{
			m_pFoldBtn->hide();
			releaseFoldDockWidget();
		}
		else
		{
			Qt::DockWidgetArea area = m_pParent->dockWidgetArea(this);
			if (area == Qt::DockWidgetArea::LeftDockWidgetArea)
			{
				resetDirection(nFoldDirection::nFoldLeft);
			}
			else if (area == Qt::DockWidgetArea::RightDockWidgetArea)
			{
				resetDirection(nFoldDirection::nFoldRight);
			}
			else if (area == Qt::DockWidgetArea::TopDockWidgetArea)
			{
				resetDirection(nFoldDirection::nFoldTop);
			}
			else if (area == Qt::DockWidgetArea::BottomDockWidgetArea)
			{
				resetDirection(nFoldDirection::nFoldBottom);
			}
			if (m_pFoldWidget)//如果拖动到一侧时, 已经存在了其他的fold dock按钮时, 需要隐藏此一侧的所有fold dock按钮
			{
				m_pFoldWidget->hideAllDock();
			}
		}
		}, Qt::QueuedConnection);

	//setStyleSheet(
	//	"QDockWidget {"
	//	"   border: 1px solid #FF0000;"  // 边框颜色和宽度
	//	"   border-radius: 4px;"         // 圆角
	//	"}"
	//	"QDockWidget::title {"
	//	"   background: #E0E0E0;"        // 标题栏背景色
	//	"   padding: 4px;"
	//	"}"
	//);
}


void CDockWidget::releaseFoldDockWidget()
{
	if (m_pFoldWidget)
	{
		m_pFoldWidget->setParent(nullptr);
		delete m_pFoldWidget;
		m_pFoldWidget = nullptr;
	}
	if (m_pFoldDockWidget)
	{
		m_pFoldDockWidget->setParent(nullptr);
		delete m_pFoldDockWidget;
		m_pFoldDockWidget = nullptr;
	}
}

void CDockWidget::initFoldDockWidget(nFoldDirection direction)
{
	releaseFoldDockWidget();

	m_pFoldDockWidget = new QDockWidget(m_pParent);
	QWidget* emptyWidget = new QWidget(m_pFoldDockWidget);
	m_pFoldDockWidget->setTitleBarWidget(emptyWidget);
	m_pFoldDockWidget->setProperty(PROPERTY_DOCK_TYPE, DOCK_TYPE_FOLD_BTN);
	m_pFoldDockWidget->setFeatures(QDockWidget::NoDockWidgetFeatures/*m_pFoldDockWidget->features()
		& ~QDockWidget::DockWidgetClosable
		& ~QDockWidget::DockWidgetMovable
		& ~QDockWidget::DockWidgetFloatable*/);
	m_pFoldDockWidget->hide();
	m_pFoldWidget = new CDockWidgetFoldWidget(m_pParent, m_iDirection, m_strTitle, m_pFoldDockWidget);
	m_pFoldWidget->setControlWidget(m_pFoldDockWidget);
	m_pFoldWidget->setParternWidget(this);
	m_pFoldDockWidget->setWidget(m_pFoldWidget);
	//m_pFoldWidget->hide();

	if (m_pParent)
	{
		if (m_iDirection == nFoldDirection::nFoldLeft)
		{
			m_pParent->addDockWidget(Qt::LeftDockWidgetArea, m_pFoldDockWidget);
		}
		else if (m_iDirection == nFoldDirection::nFoldRight)
		{
			m_pParent->addDockWidget(Qt::RightDockWidgetArea, m_pFoldDockWidget);
		}
		else if (m_iDirection == nFoldDirection::nFoldTop)
		{
			m_pParent->addDockWidget(Qt::TopDockWidgetArea, m_pFoldDockWidget);
		}
		else if (m_iDirection == nFoldDirection::nFoldBottom)
		{
			m_pParent->addDockWidget(Qt::BottomDockWidgetArea, m_pFoldDockWidget);
		}
	}
}

void CDockWidget::resetDirection(nFoldDirection direction)
{
	m_pFoldBtn->show();
	m_iDirection = direction;
	if (m_iDirection == nFoldDirection::nFoldLeft)
	{
		m_pFoldBtn->setIcon(QIcon(":/TabShell/fold_left.png"));
	}
	else if (m_iDirection == nFoldDirection::nFoldRight)
	{
		m_pFoldBtn->setIcon(QIcon(":/TabShell/fold_right.png"));
	}
	else if (m_iDirection == nFoldDirection::nFoldTop)
	{
		m_pFoldBtn->setIcon(QIcon(":/TabShell/fold_up.png"));
	}
	else if (m_iDirection == nFoldDirection::nFoldBottom)
	{
		m_pFoldBtn->setIcon(QIcon(":/TabShell/fold_down.png"));
	}

	initFoldDockWidget(direction);
}

QList<QList<QDockWidget*>> CDockWidget::getAreaDockWidget(Qt::DockWidgetArea area, int* pCurDockIndx /*= nullptr*/)
{
	QList<QList<QDockWidget*>> lstAreaDock;
	if (m_pParent == nullptr)
		return lstAreaDock;

	for (QDockWidget* dock : m_pParent->findChildren<QDockWidget*>())
	{
		if (dock->property(PROPERTY_DOCK_TYPE).toInt() == DOCK_TYPE_FOLD_DOCK &&
			m_pParent->dockWidgetArea(dock) == area)
		{
			if (lstAreaDock.isEmpty())
			{
				lstAreaDock.append(QList<QDockWidget*>() << dock);
				if (dock == this && pCurDockIndx)
					*pCurDockIndx = 0;
			}
			else
			{
				bool isFind(false);
				for (int index = 0; index < lstAreaDock.count(); index++)
				{
					QList<QDockWidget*>& lstDick = lstAreaDock[index];
					QList<QDockWidget*> lstTabifiedDock = m_pParent->tabifiedDockWidgets(lstDick.first());
					for (auto & pDock : lstTabifiedDock)
					{
						if (pDock == dock)
						{
							lstDick.append(dock);
							isFind = true;
							if (dock == this && pCurDockIndx)
								*pCurDockIndx = index;
							break;
						}
					}
					if (isFind)
					{
						break;
					}
				}
				if (!isFind)
				{
					lstAreaDock.append(QList<QDockWidget*>() << dock);
					if (dock == this && pCurDockIndx)
						*pCurDockIndx = lstAreaDock.count() - 1;
				}
			}
		}
	}

	return lstAreaDock;
}

void CDockWidget::hideAllDock(QList<QList<QDockWidget*>> lstAreaDock /*= QList<QList<QDockWidget*>>()*/)
{
	if (lstAreaDock.isEmpty())
	{
		Qt::DockWidgetArea area = m_pParent->dockWidgetArea(this);
		lstAreaDock = getAreaDockWidget(area);
	}
	for (auto lstDock : lstAreaDock)
	{
		for (auto dock : lstDock)
		{
			CDockWidget* pDock = dynamic_cast<CDockWidget*>(dock);
			if (pDock)
			{
				pDock->hideDock();
			}
		}
	}
}
