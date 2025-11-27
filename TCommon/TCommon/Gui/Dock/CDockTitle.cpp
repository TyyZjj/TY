#include "CDockTitle.h"
#include "ui_CDockTitle.h"

CDockTitle::CDockTitle(QString strTitle, QWidget* pChild, bool bCanClose /*= true*/, QWidget *parent)
	: QWidget(parent)
	, ui(new Ui::CDockTitleClass())
{
	ui->setupUi(this);

	//this->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
	//ui->widget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);

	ui->lblTitle->setText(strTitle);
	pChild->setParent(ui->widget);
	ui->gridLayout_widget->addWidget(pChild);
	//adjustSize();
	
	if (!bCanClose)
		ui->btnClose->hide();

	initWidget();
}

CDockTitle::~CDockTitle()
{
	delete ui;
}

void CDockTitle::hideContent(bool isHide /*= true*/)
{
	if (isHide &&
		ui->btnExpand->arrowType() == Qt::ArrowType::DownArrow)
		emit ui->btnExpand->clicked();
	else if (!isHide &&
		ui->btnExpand->arrowType() == Qt::ArrowType::LeftArrow)
		emit ui->btnExpand->clicked();

	
}

void CDockTitle::setTitle(QString strTitle)
{
	ui->lblTitle->setText(strTitle);
}

void CDockTitle::initWidget()
{
	connect(ui->btnExpand, &QPushButton::clicked, this, [&]() {
		if (ui->btnExpand->arrowType() == Qt::LeftArrow)
		{
			ui->widget->show();
			ui->btnExpand->setArrowType(Qt::DownArrow);
		}
		else if (ui->btnExpand->arrowType() == Qt::DownArrow)
		{
			ui->widget->hide();
			ui->btnExpand->setArrowType(Qt::LeftArrow);
		}

		this->updateGeometry();
		// 通知父布局更新
		if (QWidget* parent = this->parentWidget()) {
			parent->layout()->activate();
			parent->updateGeometry();
		}
		});

	connect (ui->btnClose, &QPushButton::clicked, this, [&]() {
		emit signalClose();//交由外部决定是释放还是隐藏
		});
}
