#pragma once
#pragma execution_character_set("utf-8")
#include <QWidget>
#include <QMouseEvent>
#include "ui_WidgetCam.h"
#include "GLYuvWidget.h"
#include <QLabel>
#include <QResizeEvent>
#include <QImage>

#include <thread>
#include "streamsdk.h"
class WidgetVideo : public GLYuvWidget
{
	Q_OBJECT

public:
	explicit WidgetVideo(QWidget *parent = Q_NULLPTR, QString strImg = ":/UI/back/refcvcam.png");
	~WidgetVideo();

	void mouseDoubleClickEvent(QMouseEvent* event) Q_DECL_OVERRIDE;
	void mousePressEvent(QMouseEvent* event) Q_DECL_OVERRIDE;
	
	void closeEvent(QCloseEvent *event) Q_DECL_OVERRIDE;

	void resizeEvent(QResizeEvent *event) Q_DECL_OVERRIDE;

	void SetDefaultType();

	void setParentWin(QWidget *parent);
	bool OpenStream(std::string url, QString strImg = "");
	void CloseStream();

public:
	signals:
		void ShooterSignal(int id);
private:
	signals:
		int showYuvSignal(cvcam::Frame frame);

public:
	QString m_strImg;
	QLabel * m_lblFront;
private:
	QWidget * m_parent;

	QLabel * lblImgBack;
	//QImage image;
	QRect m_rtDefault;
	Qt::WindowFlags m_winType;

	std::string m_url;
	cvcam::Frame m_frame;
	cvcam::StreamManager m_manager;
	std::thread m_thread;
	volatile bool m_blRunning;

private:
	void startVideo();
	void stopVideo();
};
