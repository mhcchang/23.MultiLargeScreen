#include "WidgetCam.h"

#include <QTime>

WidgetCam::WidgetCam(QWidget *parent, QString strImg)
	: GLYuvWidget(parent)
{
	ui.setupUi(this);
	m_blRunning = false;
	m_parent = parent;
	m_strImg = strImg;
	m_lblFront = new QLabel(this);
	m_lblFront->setAttribute(Qt::WA_TransparentForMouseEvents, true);
	m_lblFront->setAttribute(Qt::WA_TranslucentBackground, true);
	if (m_strImg != "")
	{
		m_lblFront->raise();
		m_lblFront->setPixmap(QPixmap(m_strImg));
		m_lblFront->setScaledContents(true);
		m_lblFront->show();
	}
	setWindowFlags(Qt::FramelessWindowHint);

	connect(this, &WidgetCam::showYuvSignal, this, &GLYuvWidget::slotShowYuvFrame, Qt::UniqueConnection);
	connect(ui.btnFull, &QPushButton::clicked, this, &WidgetCam::btnFullSlot, Qt::UniqueConnection);
	
	m_nIndex = -1;
	m_manager = cvcam::StreamManager::create();
}

WidgetCam::~WidgetCam()
{
	stopVideo();
}

void WidgetCam::mouseDoubleClickEvent(QMouseEvent* event)
{
	if (event->button() == Qt::LeftButton)
	{
		m_winType = windowFlags();
		//m_rtDefault = this->geometry();
		//this->setWindowFlags(Qt::Dialog | Qt::WindowTitleHint | Qt::WindowCloseButtonHint);
		//this->setWindowModality(Qt::ApplicationModal);
		//this->show();
		btnFullSlot();
	}
	else
		return QWidget::mouseDoubleClickEvent(event);
}

void WidgetCam::closeEvent(QCloseEvent *event)
{
	if (this->windowType() & (Qt::Dialog | Qt::WindowTitleHint | Qt::WindowCloseButtonHint)
		== (Qt::Dialog | Qt::WindowTitleHint | Qt::WindowCloseButtonHint))
	{
		this->setWindowFlags(m_winType);
		this->setParent(m_parent);
		this->setGeometry(m_rtDefault);
		this->move(0, 0);
		this->show();

		event->ignore();
	}
	else
		QWidget::closeEvent(event);
}

void WidgetCam::hideEvent(QHideEvent *event)
{
	stopVideo();
}

void WidgetCam::SetDefaultType()
{

}

void WidgetCam::SetIndex(int nIndex)
{
	m_nIndex = nIndex;
}

void WidgetCam::enterEvent(QEvent *event)
{
	if (m_strImg != "")
	{
		//m_strImg = ":/UI/back/image2.png";
		//m_lblFront->setPixmap(QPixmap(m_strImg));
	}
}

void WidgetCam::leaveEvent(QEvent *event)
{
	if (m_strImg != "")
	{
		//m_strImg = ":/UI/back/image3.png";
		//m_lblFront->setPixmap(QPixmap(m_strImg));
	}
}

void WidgetCam::resizeEvent(QResizeEvent *event)
{
	m_lblFront->setGeometry(this->geometry());
	m_lblFront->move(0, 0);
	m_lblFront->setPixmap(QPixmap(m_strImg));
	return GLYuvWidget::resizeEvent(event);
}

void WidgetCam::setParentWin(QWidget *parent)
{
	m_parent = parent;
	this->setParent(m_parent);
}

void WidgetCam::btnShooterSlot()
{
	//todo
}

void WidgetCam::btnBrightSlot()
{
	//todo
}

void WidgetCam::btnFullSlot()
{
	emit ShowDomeVideoSignal(m_nIndex);
}

void WidgetCam::btnPlaceSlot()
{
	//todo
}

void WidgetCam::btnVideoSlot()
{
	//todo
}

void WidgetCam::btnZoominSlot()
{
	//todo
}

void WidgetCam::startVideo()
{
	m_manager.startDecoding();
	m_blRunning = true;
	cvcam::Frame frame2;
	cvcam::Frame frame;
	QTime tt;
	tt.start();
	while (m_blRunning)
	{
		cvcam::Stream stream = m_manager.at(0);
		cvcam::FrameQueue frameQueue = stream.frameQueue();

		cvcam::Frame frame;
		if (frameQueue.tryPop(frame))
		{
			if (!m_blRunning)
				break;
			if (frame.isValid())
			{
				frame2 = frame;
				//printf("frame: %d id= %d \n", frame.format(), i);

				if (!frame.data(0) || !frame.data(1))
				{
					//printf("m_frame == %p %p \n", frame.data(0), frame.data(1));
					std::this_thread::sleep_for(std::chrono::milliseconds(1));
					continue;
				}

				emit showYuvSignal(frame);

				tt.restart();
				std::this_thread::sleep_for(std::chrono::milliseconds(2));

				frame2 = cvcam::Frame();
			}
		}
		else
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}
	}

	m_manager.stopDecoding();

	//最后清理屏幕
	//cvcam::Frame framenull;
	//emit showYuvSignal(framenull);
	std::this_thread::sleep_for(std::chrono::milliseconds(50));
}

void WidgetCam::stopVideo()
{
	m_blRunning = false;
	m_manager.stopDecoding();
	std::this_thread::sleep_for(std::chrono::milliseconds(50));
	m_manager.close();
	int i = 5;
	while (!m_thread.joinable() && i > 0)
	{
		i--;
		std::this_thread::sleep_for(std::chrono::milliseconds(20));
	}
	if (m_thread.joinable())
		m_thread.join();
}

void WidgetCam::CloseStream()
{
	if (m_manager.isOpen())
		stopVideo();
}

void WidgetCam::SetInfo(QString szInfo)
{
	QString sz = QString::fromUtf8("<html><head/><body><p><span style=\"font-weight:600; color:#9b9b9b; \">%1</span></p></body></html>").arg(szInfo);
	
	ui.lblModal->setText(sz);
}

bool WidgetCam::OpenStream(std::string url)
{
	if (m_manager.isOpen())
		stopVideo();
	cvcam::StreamOpenResultList resultList;
	cvcam::StreamParamsList paramsList;
	m_url = url;
	int i = 0;
	cvcam::StreamParams params =
	{
		cvcam::StreamType::STREAM_TYPE_MAIN,
		url,
		cvcam::CameraType::CAMERA_TYPE_DETAIL,
		i++,
		std::to_string(i),
		NULL
	};
	paramsList.push_back(params);

	int res = m_manager.open(paramsList, resultList);
	if (res != 0)
		return false;
	
	m_thread = std::thread(&WidgetCam::startVideo, this);

	return true;
}

void WidgetCam::mousePressEvent(QMouseEvent* event)
{
	emit IndexChangeSignal(m_nIndex);

	return QWidget::mousePressEvent(event);
}

void WidgetCam::SetDefault(bool blTrue)
{
	if (blTrue)
	{
		m_strImg = ":/UI/back/image2.png";
		m_lblFront->setPixmap(QPixmap(m_strImg));
	}
	else
	{
		m_strImg = ":/UI/back/image3.png";
		m_lblFront->setPixmap(QPixmap(m_strImg));
	}
}