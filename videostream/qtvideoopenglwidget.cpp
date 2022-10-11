#include "qtvideoopenglwidget.h"

QtVideoOpenGlWidget::QtVideoOpenGlWidget(QWidget *parent)
	: QGLYuv420Widget(parent)
	//: QWidget(parent)
{
	btnFull = NULL;
	btnClose = NULL;
	if (parent)
	{
		btnFull = new QPushButton(this);
		btnFull->setObjectName(QStringLiteral("pushButton"));
		btnFull->setGeometry(QRect(0, 0, 28, 28));
		btnFull->setMinimumSize(QSize(28, 28));
		btnFull->setMaximumSize(QSize(28, 28));
		QIcon icon;
		icon.addFile(QStringLiteral(":/res/resources/Aero_WinFlip_2d.png"), QSize(), QIcon::Normal, QIcon::On);
		btnFull->setIcon(icon);
		btnFull->setIconSize(QSize(24, 24));
		btnFull->setCheckable(false);
		btnFull->setFlat(true);
		btnClose = new QPushButton(this);
		btnClose->setObjectName(QStringLiteral("btnClose"));
		btnClose->setGeometry(QRect(30, 0, 28, 28));
		btnClose->setMinimumSize(QSize(28, 28));
		btnClose->setMaximumSize(QSize(28, 28));
		QIcon icon1;
		icon1.addFile(QStringLiteral(":/res/resources/cross.png"), QSize(), QIcon::Normal, QIcon::On);
		btnClose->setIcon(icon1);
		btnClose->setIconSize(QSize(24, 24));
		btnClose->setCheckable(false);
		btnClose->setFlat(true);
		btnFull->show();
		btnClose->show();

		connect(btnFull, &QPushButton::clicked, this, &QtVideoOpenGlWidget::btnFullClicked);
		connect(btnClose, &QPushButton::clicked, this, &QtVideoOpenGlWidget::btnCloseClicked);
	}
	this->setAttribute(Qt::WA_DeleteOnClose, true);
	//this->setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint & ~Qt::WindowMinMaxButtonsHint);

	m_vpThread = NULL;
//	m_qvw = NULL;
//	m_mainWin = NULL;
}

//QtVideoOpenGlWidget::QtVideoOpenGlWidget(QWidget *parent, MainWindow* mainWin)
//	: QGLYuv420Widget(mainWin)
//{
//	setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog);
//	setWindowModality(Qt::WindowModal);
//
//	btnFull = NULL;
//	btnClose = NULL;
//	if (parent)
//	{
//		btnFull = new QPushButton(this);
//		btnFull->setObjectName(QStringLiteral("pushButton"));
//		btnFull->setGeometry(QRect(0, 0, 28, 28));
//		btnFull->setMinimumSize(QSize(28, 28));
//		btnFull->setMaximumSize(QSize(28, 28));
//		QIcon icon;
//		icon.addFile(QStringLiteral(":/res/resources/Aero_WinFlip_2d.png"), QSize(), QIcon::Normal, QIcon::On);
//		btnFull->setIcon(icon);
//		btnFull->setIconSize(QSize(24, 24));
//		btnFull->setCheckable(false);
//		btnFull->setFlat(true);
//		btnClose = new QPushButton(this);
//		btnClose->setObjectName(QStringLiteral("btnClose"));
//		btnClose->setGeometry(QRect(30, 0, 28, 28));
//		btnClose->setMinimumSize(QSize(28, 28));
//		btnClose->setMaximumSize(QSize(28, 28));
//		QIcon icon1;
//		icon1.addFile(QStringLiteral(":/res/resources/cross.png"), QSize(), QIcon::Normal, QIcon::On);
//		btnClose->setIcon(icon1);
//		btnClose->setIconSize(QSize(24, 24));
//		btnClose->setCheckable(false);
//		btnClose->setFlat(true);
//		btnFull->show();
//		btnClose->show();
//
//		connect(btnFull, &QPushButton::clicked, this, &QtVideoOpenGlWidget::btnFullClicked);
//		connect(btnClose, &QPushButton::clicked, this, &QtVideoOpenGlWidget::btnCloseClicked);
//	}
//	this->setAttribute(Qt::WA_DeleteOnClose, true);
//	//this->setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint & ~Qt::WindowMinMaxButtonsHint);
//
//	m_vpThread = NULL;
//	m_qvw = NULL;
//	m_mainWin = mainWin;
//}

QtVideoOpenGlWidget::~QtVideoOpenGlWidget()
{
	if (m_vpThread)
		disconnect(m_vpThread, &VideoPlatformThread::showFrameSignal, this, 0);
	if (btnFull)
	{
		delete btnFull;
	}
	if (btnClose)
		delete btnClose;
	emit closeChildSignal();
}

void QtVideoOpenGlWidget::btnFullClicked()
{
	if (!m_vpThread)
		return;
	emit ShowDlgVideoSignal(m_vpThread, m_blCloseVideo);
}

void QtVideoOpenGlWidget::closeChildSlot()
{
	//disconnect(m_qvw, &QtVideoOpenGlWidget::closeChildSignal, this, 0);
	//m_qvw = NULL;
}

void QtVideoOpenGlWidget::btnCloseClicked()
{
	//if (m_qvw)
	//{
	//	m_qvw->hide();
	//	disconnect(m_vpThread, &VideoPlatformThread::showFrameSignal, m_qvw, 0);
	////	m_qvw->btnCloseClicked();
	//	m_qvw = NULL;
	//}

	m_vpThread = NULL;
	closeVideo();	
}

void QtVideoOpenGlWidget::closeVideo()
{
	m_blShow = false;
	m_vpThread = NULL;
	repaint();
}

bool QtVideoOpenGlWidget::setVideoSource(int nVideo)
{
	return false;
}

void QtVideoOpenGlWidget::setVideoSource(VideoPlatformThread * vpthread, bool blCloseVideo)
{
	m_blCloseVideo = blCloseVideo;
	closeVideo();
//	m_vpThread = vpthread;
	m_vpThread = vpthread;

	m_blShow = true;
//	connect(this, &QtVideoOpenGlWidget::ShowDlgVideoSignal, m_mainWin, &MainWindow::ShowDlgVideoSlot);
}

bool QtVideoOpenGlWidget::close()
{
	QWidget::close();
	return true;
}

void QtVideoOpenGlWidget::showBtns(bool blShow)
{
	if (blShow)
	{
		if (btnFull)
			btnFull->show();
		if (btnClose)
			btnClose->show();
	}
	else
	{
		if (btnFull)
			btnFull->hide();
		if (btnClose)
			btnClose->hide();
	}
}
