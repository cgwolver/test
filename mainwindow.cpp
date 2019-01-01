#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QPushButton>
#include <QtQml\qqmlcontext.h> 
#include <QMessageBox>
#include "capbarevent.h"
#include <QQuickItem>
#include <QtQml\qqmlprivate.h>
#include "buttontable.h"
#include <qpainter.h>
#include <QLibrary>
#include <QGraphicsDropShadowEffect>

//#ifdef _SAFE_BROWSER_ENABLE_GLASS_EFFECT
#include <dwmapi.h>
#pragma comment(lib,"dwmapi.lib")
#pragma comment(lib,"user32.lib")

void setGlassEffect(QWidget *w)
{
#if WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_DESKTOP)

    typedef HRESULT (STDAPICALLTYPE *enableBlurBehindWindow)(HWND, const DWM_BLURBEHIND *);
    typedef HRESULT (STDAPICALLTYPE *extendFrameIntoClientArea)(HWND, MARGINS *);
    enableBlurBehindWindow f1 =
        (enableBlurBehindWindow)QLibrary::resolve("dwmapi", "DwmEnableBlurBehindWindow");
    extendFrameIntoClientArea f2 =
        (extendFrameIntoClientArea)QLibrary::resolve("dwmapi", "DwmExtendFrameIntoClientArea");

    DWM_BLURBEHIND bb = {0};
    MARGINS margins = {-1};
    bb.dwFlags = DWM_BB_ENABLE | DWM_BB_BLURREGION;
    if(f1)f1((HWND)w->winId(), &bb);
    if(f2)f2((HWND)w->winId(), &margins);
#endif
}
void SetWindowBlur(HWND hWnd)
{
#if WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_DESKTOP)

	const HINSTANCE hModule = LoadLibrary(TEXT("user32.dll"));
	if (hModule)
	{
		struct ACCENTPOLICY
		{
			int nAccentState;
			int nFlags;
			int nColor;
			int nAnimationId;
		};
		struct WINCOMPATTRDATA
		{
			int nAttribute;
			PVOID pData;
			ULONG ulDataSize;
		};
		typedef BOOL(WINAPI*pSetWindowCompositionAttribute)(HWND, WINCOMPATTRDATA*);
		const pSetWindowCompositionAttribute SetWindowCompositionAttribute = (pSetWindowCompositionAttribute)GetProcAddress(hModule, "SetWindowCompositionAttribute");
		if (SetWindowCompositionAttribute)
		{
			ACCENTPOLICY policy = { 3, 0, 0, 0 }; // ACCENT_ENABLE_BLURBEHIND=3...
			WINCOMPATTRDATA data = { 19, &policy, sizeof(ACCENTPOLICY) }; // WCA_ACCENT_POLICY=19
			SetWindowCompositionAttribute(hWnd, &data);
		}
		FreeLibrary(hModule);
	}
#endif
}

void SetWidgetDropShadow(QWidget*wgt)
{
	QGraphicsDropShadowEffect *shadow_effect = new QGraphicsDropShadowEffect(wgt);
	shadow_effect->setOffset(15, 15);
	shadow_effect->setColor(Qt::darkGray);
	shadow_effect->setBlurRadius(18);
	wgt->setGraphicsEffect(shadow_effect);

}
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
	_moveState(false)

{
    setWindowFlags(Qt::FramelessWindowHint);//Qt::SplashScreen);

	qmlRegisterType<CapBarEvent>("qt.capbar.event", 1, 0, "CapbarEvent");
	//qmlRegisterType<LeftBarEvent>("qt.leftbar.event", 1, 0, "LeftbarEvent");
	QQmlContext*ctx = nullptr;
	
	ui->setupUi(this);

    //SetWidgetDropShadow(ui->frame);
    //ui->capBar->setVisible(false);
    //setGlassEffect(this);
	//QtWin::setCompositionEnabled(true);
    //HWND hWnd = (HWND)winId();
    //SetWindowLongA(hWnd,GWL_STYLE,GetWindowLongA(hWnd,GWL_STYLE)|WS_BORDER|WS_THICKFRAME);
	//setAttribute(Qt::WA_TranslucentBackground);//设置窗口背景透明
	SetWindowBlur((HWND)winId());
    setStyleSheet( "background-color: rgba(255, 255, 255,255);");
	ctx = ui->capBar->rootContext();
    QObject* obj = ui->capBar->rootObject()->findChild<QQmlPrivate::QQmlElement<CapBarEvent>*>("capEventObject");
    ui->capBar->setClearColor(QColor(0,0,0,0));
	ui->quickWidget->setClearColor(QColor(192, 192, 192, 0));
	if (obj)
	{
        connect(obj, SIGNAL(close()), this, SLOT(onClose()));
        connect(obj, SIGNAL(minimize()), this, SLOT(onMinimize()));
        connect(obj, SIGNAL(maximize()), this, SLOT(onMaximize()));
        //connect(obj, SIGNAL(dbclicked()),this, SLOT(onCapDbClicked()));
        //connect(obj, SIGNAL(sgclicked()),this,SLOT(onCapSgClicked()));
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

	QObject::connect(ui->listWidget_2,
		SIGNAL(currentItemChanged(QListWidgetItem*, QListWidgetItem*)),
		this,
		SLOT(QListWidget_2_currentItemChanged(QListWidgetItem*, QListWidgetItem*)));

	QObject::connect(ui->tableWidget,
		SIGNAL(currentItemChanged(QTreeWidgetItem*, QTreeWidgetItem*)),
		this,
		SLOT(QTableWidget_currentItemChanged(QTreeWidgetItem*, QTreeWidgetItem*)));

	const wchar_t* text[10] = { L"发现",L"收集",L"关注",L"互动",L"人脉",L"发布",L"项目",L"空间",L"云盘",L"应用"};

	for (int i = 0; i<10; i++)
	{
		QListWidgetItem* lwi = new QListWidgetItem;
		ItemButton*lb = ui->listWidget->addButtonItem(lwi,true);
		lwi->setText(QString::fromWCharArray(text[i]));
	}
	
	for (int i = 0; i<12; i++)
	{
		QListWidgetItem* lwi = new QListWidgetItem;
		ItemButton*lb = ui->listWidget_2->addButtonItem(lwi, false);
		const wchar_t* wc = L"\uE700";
		lwi->setText(QString::fromWCharArray(wc));
	}

	ui->tableWidget->addButtonItem(QString::fromWCharArray(L"工作笔记"), true);

	ui->tableWidget->addButtonItem(QString::fromWCharArray(L"项目博客"), true);

	ui->tableWidget->addButtonItem(QString::fromWCharArray(L"开发者网络"), true);

	ui->tableWidget->addButtonItem(QString::fromWCharArray(L"开源项目"), true);

	ui->tableWidget->addButtonItem(QString::fromWCharArray(L"软件市场"), true);

	ui->tableWidget->addButtonItem(QString::fromWCharArray(L"更多分类"), true);
	ui->tableWidget->setVisible(false);
}

void MainWindow::ListButton_toggled(bool)
{

}
void MainWindow::paintEvent(QPaintEvent *e)
{
    QMainWindow::paintEvent(e);


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
    QRect rtCapBar = ui->capBar->geometry();
	
	rtCapBar.setRight(bound.width());
    ui->capBar->setGeometry(rtCapBar);
	
	QRect rtList = ui->listWidget->geometry();
	rtList.setTop(rtCapBar.bottom()+1);
	rtList.setBottom(bound.height());
    ui->listWidget->setGeometry(rtList);

	QRect rtList2 = ui->listWidget_2->geometry();
	rtList2.setTop(0);
	rtList2.setBottom(bound.height());
	ui->listWidget_2->setGeometry(rtList2);
	
	QRect rtList3 = ui->tableWidget->geometry();
	QSize szList3 = rtList3.size();
	rtList3.setTop(rtCapBar.bottom() + 2);
	rtList3.setBottom(rtList3.top()+ szList3.height()-1);
	ui->tableWidget->setGeometry(rtList3);

	//QSizeF sz = ui->capBar->rootObject()->size();
	//QQuickItem* obj = ui->capBar->rootObject()->findChild<QQuickItem*>("rectObject");
    //obj->setSize(QSizeF(sz.width(),bound.width()));
	
}

void MainWindow::QListWidget_2_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous)
{
	QWidget* pw = ui->listWidget_2->itemWidget(previous);
	if (pw)
	{
		ItemButton* lb = (ItemButton*)pw;
		lb->setChecked(false);
	}
}
 void MainWindow::QListWidget_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous)
 {
    QWidget* pw = ui->listWidget->itemWidget(previous);
    if(pw)
    {
       ItemButton* lb = (ItemButton*)pw;
       lb->setChecked(false);
    }
 }

 void MainWindow::QTableWidget_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous)
 {
	 QTreeWidgetItem* topItem = ui->tableWidget->topLevelItem(0);

	 int ic =  ui->tableWidget->currentColumn();
	 QModelIndex index = ui->tableWidget->indexFromItem(previous,ic);
	 
	 int cc = topItem->columnCount();
	 for (int i = 0; i < cc; i++)
	 {
		 if (i != ic)
		 {
			 QWidget* pw = ui->tableWidget->itemWidget(topItem, i);
			 if (pw)
			 {
				 ItemButton* lb = (ItemButton*)pw;
				 lb->setChecked(false);
			 }
		 }
	 }
 }

 void MainWindow::mouseReleaseEvent(QMouseEvent *event)
 {
	 setMouseTracking(false);
	 releaseMouse();

	 _moveState = false;
 }

 void MainWindow::mousePressEvent(QMouseEvent *event)
 {
	 _mousePt = this->mapToGlobal(event->pos());

	 QRect rect = childWindowRect(ui->capBar);

	 rect.setLeft(0);
	 rect.setRight(width());

	 if (rect.contains(event->pos()))
	 {
		 _movePt = event->globalPos();
		 _moveState = true;
	 }
	// ui->capBar->setMouseTracking(true);
	 setMouseTracking(true);
	 grabMouse();
 }
void MainWindow::mouseDoubleClickEvent(QMouseEvent *event)
{
    QRect rect = childWindowRect(ui->capBar);
	QPoint pos = this->mapFromGlobal(_mousePt);
    //QRect rcMousePt(_mousePt, QSize(1, 1));
    if (rect.contains(pos))
    {
        onMaximize();
    }
}
 void MainWindow::mouseMoveEvent(QMouseEvent *event)
 {
	 Qt::MouseButtons btn = event->buttons();
	 if (btn&Qt::LeftButton)
	 {

	 }

	 if(btn&Qt::LeftButton)
	 {
		 QPoint pt= this->mapFromGlobal(_movePt);
		 QRect rect = childWindowRect(ui->capBar);
		
		 if (_moveState)
		 {
			 QPoint dpt = event->globalPos() - _movePt;
			 int dptx = dpt.x();
			 int dpty = dpt.y();
			 move(pos()+dpt);
		 } 	
	 }

	 _movePt = event->globalPos();
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

