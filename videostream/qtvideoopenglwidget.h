#pragma once


#include "QGLYuv420Widget.h"
#include "VideoPlatformThread.h"
#include <QDesktopWidget>
#include <QPushButton>
#include <QApplication>
//#include "mainwindow.h"
//
//class MainWindow;
class QtVideoOpenGlWidget : public QGLYuv420Widget
{
	Q_OBJECT

public:
	explicit QtVideoOpenGlWidget(QWidget *parent);

	~QtVideoOpenGlWidget();
//	MainWindow* m_mainWin;
public:
	QPushButton *btnClose;
	QPushButton *btnFull;

	//对应的视频流线程
	VideoPlatformThread * m_vpThread;
public slots:
	void btnFullClicked();
	void btnCloseClicked();

	void closeVideo();
	void closeChildSlot();
public:
	bool setVideoSource(int nVideo);
	void setVideoSource(VideoPlatformThread * vpthread, bool blCloseVideo = true);
	bool close();
	void showBtns(bool blShow);
	//void setMainWin(MainWindow* mainWin) {
	//	m_mainWin = mainWin;
	//}
	bool m_blCloseVideo;
public:
	signals:
		 void closeChildSignal();
		 void ShowDlgVideoSignal(VideoPlatformThread * vpthread, bool blCloseVideo = true);
};
