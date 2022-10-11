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

class WidgetCam : public GLYuvWidget
{
	Q_OBJECT

public:
	explicit WidgetCam(QWidget *parent = Q_NULLPTR, QString strImg = ":/UI/back/image3.png");
	~WidgetCam();

	void mouseDoubleClickEvent(QMouseEvent* event) Q_DECL_OVERRIDE;
	void closeEvent(QCloseEvent *event) Q_DECL_OVERRIDE;
	void mousePressEvent(QMouseEvent* event) Q_DECL_OVERRIDE;

	void enterEvent(QEvent *event) Q_DECL_OVERRIDE;
	void leaveEvent(QEvent *event) Q_DECL_OVERRIDE;
	void resizeEvent(QResizeEvent *event) Q_DECL_OVERRIDE;

	void hideEvent(QHideEvent *event) Q_DECL_OVERRIDE;

	void SetIndex(int nIndex);
	void SetDefaultType();
	Ui::WidgetCam ui;

	void setParentWin(QWidget *parent);
	bool OpenStream(std::string url);
	void CloseStream();

	void SetInfo(QString sz);
	void SetDefault(bool blTrue);
public slots:
	void btnShooterSlot();
	void btnBrightSlot();
	void btnFullSlot();
	void btnPlaceSlot();
	void btnVideoSlot();
	void btnZoominSlot();
public:
	signals:
		void ShooterSignal(int id);
		void BrightSignal(int id);
		void FullSignal(int id);
		void PlaceSignal(int id);
		void VideoSignal(int id);
		void ZoominSignal(int id);

		void IndexChangeSignal(int nIndex);
		void ShowDomeVideoSignal(int nIndex);
private:
	signals:
		int showYuvSignal(cvcam::Frame frame);
private:
	QWidget * m_parent;
	QRect m_rtDefault;
	Qt::WindowFlags m_winType;

	QString m_strImg;

	QLabel * m_lblFront;

	std::string m_url;
	cvcam::Frame m_frame;
	cvcam::StreamManager m_manager;
	std::thread m_thread;
	volatile bool m_blRunning;

	int m_nIndex;
private:
	void startVideo();
	void stopVideo();
};
