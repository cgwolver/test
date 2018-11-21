#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QPushButton>
#include <QtQml\qqmlcontext.h> 
#include <QMessageBox>
#include "capbarevent.h"
#include <QQuickItem>
#include <QtQml\qqmlprivate.h>
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
	setWindowFlags(Qt::FramelessWindowHint);//Qt::SplashScreen);

	qmlRegisterType<CapBarEvent>("qt.capbar.event", 1, 0, "CapbarEvent");
	//qmlRegisterType<LeftBarEvent>("qt.leftbar.event", 1, 0, "LeftbarEvent");
	QQmlContext*ctx = nullptr;
	
	ui->setupUi(this);
	
	ctx = ui->capBar->rootContext();
    QObject* obj = ui->capBar->rootObject()->findChild<QQmlPrivate::QQmlElement<CapBarEvent>*>("capEventObject");


	
	if (obj)
	{
		connect(obj, SIGNAL(close(void)), this, SLOT(onClose(void)));
		connect(obj, SIGNAL(minimize(void)), this, SLOT(onMinimize(void)));
		connect(obj, SIGNAL(maximize(void)), this, SLOT(onMaximize(void)));
	}
    QObject::connect(ui->listWidget,
                     SIGNAL(itemEntered(QListWidgetItem*)),
                     this,
                     SLOT(QListWidget_itemEntered(QListWidgetItem*)));

    //QObject::connect(ui->listWidget,
    //                 SIGNAL(itemChanged(QListWidgetItem*)),
    //                 this,
    //                 SLOT(QListWidget_itemChanged(QListWidgetItem*)));

    QObject::connect(ui->listWidget,
                     SIGNAL(currentItemChanged(QListWidgetItem*,QListWidgetItem*)),
                     this,
                     SLOT(QListWidget_currentItemChanged(QListWidgetItem*,QListWidgetItem*)));

	const wchar_t* text[6] = { L"推荐",L"关注",L"收藏",L"学习",L"工作",L"市场" };

	for (int i = 0; i<6; i++)
	{
		ListButton*lb = ui->listWidget->addButtonItem(QString("Push Button %1").arg(i + 1),true);
		lb->lvi->setText(QString::fromWCharArray(text[i]));
	}
	
    //ListButton*lbx = ui->listWidget2->addButtonItem(QString::fromWCharArray(L"科技树"),false);
}

void MainWindow::ListButton_toggled(bool)
{

}

MainWindow::~MainWindow()
{
    delete ui;
}
void MainWindow::QListWidget_itemEntered(QListWidgetItem *item)
{

    //item->setTextColor(QColor(255,0,0,255));
}
void MainWindow::QListWidget_itemChanged(QListWidgetItem *item)
{
	
}
void MainWindow::resizeEvent(QResizeEvent *event)
{
	QMainWindow::resizeEvent(event);
	QRect bound = geometry();
    QRect rt = ui->capBar->geometry();
    rt.setRight(bound.width());
    ui->capBar->setGeometry(rt);
    rt = ui->listWidget->geometry();
    int y = rt.top();
    rt.setBottom(bound.height()-2);
    ui->listWidget->setGeometry(rt);
	//QSizeF sz = ui->capBar->rootObject()->size();
	//QQuickItem* obj = ui->capBar->rootObject()->findChild<QQuickItem*>("rectObject");
    //obj->setSize(QSizeF(sz.width(),bound.width()));
	
}
 void MainWindow::QListWidget_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous)
 {
    QWidget* pw = ui->listWidget->itemWidget(previous);
    if(pw)
    {
       ListButton* lb = (ListButton*)pw;
       lb->setChecked(false);
    }
 }
 void MainWindow::mouseReleaseEvent(QMouseEvent *event)
 {
	 setMouseTracking(false);
	 releaseMouse();
 }

 void MainWindow::mousePressEvent(QMouseEvent *event)
 {
	 _mousePt = event->pos();
	 setMouseTracking(true);
	 grabMouse();
 }

 void MainWindow::mouseMoveEvent(QMouseEvent *event)
 {
	 Qt::MouseButtons btn = event->buttons();
	 if (btn&Qt::LeftButton)
	 {
		 //QMessageBox qmb(QMessageBox::Information,"","");
		 //qmb.exec();
	 }

	 //if(btn&Qt::LeftButton)
	 {
		 QRect rect = childWindowRect(ui->capBar);
		 QRect rcMousePt(_mousePt, QSize(1, 1));
		 if (rect.contains(rcMousePt))
		 {
			 move(event->pos() - _mousePt + pos());
		 }
	 }
 }
 QRect MainWindow::childWindowRect(QWidget*child)
 {
	 QRect rec = child->geometry();
	 QPoint topLeft = child->mapTo(this, QPoint(0, 0));
	 QSize size = rec.size();
	 QPoint bottomRight = child->mapTo(this, QPoint(size.width(), size.height()));
	 QRect rectWindow(topLeft, bottomRight);
	 return rectWindow;
 }
 void MainWindow::onClose()
 {
    qApp->exit();
 }
 void MainWindow::onMaximize()
 {
	 if (isMaximized())
	 {
		 showNormal();
	 }
	 else
	 {
		 showMaximized();
	 }
 }
 void MainWindow::onMinimize()
 {
	 showMinimized();
 }
