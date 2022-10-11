#include "WidgetVideo.h"

#include <QTime>

WidgetVideo::WidgetVideo(QWidget *parent, QString strImg)
	: m_strImg(strImg), GLYuvWidget(parent)
{
	m_blRunning = false;
	m_parent = parent;
	//m_strImg = ":/UI/back/refcvcam.png";
	m_lblFront = nullptr;
	if (m_strImg != "")
	{
		m_lblFront = new QLabel(this);
		m_lblFront->setAttribute(Qt::WA_TransparentForMouseEvents, true);
		m_lblFront->setAttribute(Qt::WA_TranslucentBackground, true);
		m_lblFront->raise();
		m_lblFront->setPixmap(QPixmap(m_strImg));
		m_lblFront->setScaledContents(true);
		m_lblFront->show();
	}
	setWindowFlags(Qt::FramelessWindowHint);

	connect(this, &WidgetVideo::showYuvSignal, this, &GLYuvWidget::slotShowYuvFrame, Qt::UniqueConnection);

	m_manager = cvcam::StreamManager::create();
}

WidgetVideo::~WidgetVideo()
{
	stopVideo();
}

void WidgetVideo::mouseDoubleClickEvent(QMouseEvent* event)
{
	//if (event->button() == Qt::LeftButton)
	//{
	//	m_winType = windowFlags();
	//	m_rtDefault = this->geometry();
	//	this->setWindowFlags(Qt::Dialog | Qt::WindowTitleHint | Qt::WindowCloseButtonHint);
	//	this->setWindowModality(Qt::ApplicationModal);
	//	this->show();
	//}
	//else
		return QWidget::mouseDoubleClickEvent(event);
}

void WidgetVideo::mousePressEvent(QMouseEvent* event)
{
	//if (event->button() == Qt::LeftButton)
	//{
	//	m_winType = windowFlags();
	//	m_rtDefault = this->geometry();
	//	this->setWindowFlags(Qt::Dialog | Qt::WindowTitleHint | Qt::WindowCloseButtonHint);
	//	this->setWindowModality(Qt::ApplicationModal);
	//	this->show();
	//}
	//else
	return QWidget::mousePressEvent(event);
}

void WidgetVideo::closeEvent(QCloseEvent *event)
{
	//if (this->windowType() & (Qt::Dialog | Qt::WindowTitleHint | Qt::WindowCloseButtonHint)
	//	== (Qt::Dialog | Qt::WindowTitleHint | Qt::WindowCloseButtonHint))
	//{
	//	this->setWindowFlags(m_winType);
	//	this->setParent(m_parent);
	//	this->setGeometry(m_rtDefault);
	//	this->move(0, 0);
	//	this->show();

	//	event->ignore();
	//}
	//else
		QWidget::closeEvent(event);
}

void WidgetVideo::SetDefaultType()
{

}

void WidgetVideo::resizeEvent(QResizeEvent *event)
{
	if (m_lblFront)
	{
		QRect rt = this->geometry();
		m_lblFront->setGeometry(rt);
		m_lblFront->move(0, 0);
		m_lblFront->setPixmap(QPixmap(m_strImg));
	}
	return GLYuvWidget::resizeEvent(event);
}

void WidgetVideo::setParentWin(QWidget *parent)
{
	m_parent = parent;
	this->setParent(m_parent);
}
#if 1
void WidgetVideo::startVideo()
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
				//frameX[i] = frame;
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
	std::this_thread::sleep_for(std::chrono::milliseconds(50));
}
#else
void WidgetVideo::startVideo()
{
	m_manager.startDecoding();
	m_blRunning = true;
	//cvcam::Frame frame2;
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
				//frameX[i] = frame;
				//frame2 = frame;
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

				//frame2 = cvcam::Frame();
			}
		}
		else
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}
	}

	m_manager.stopDecoding();
	std::this_thread::sleep_for(std::chrono::milliseconds(50));
}
#endif
void WidgetVideo::stopVideo()
{
	m_blRunning = false;
	std::this_thread::sleep_for(std::chrono::milliseconds(200));
	m_manager.close();
	int i = 10;
	while (!m_thread.joinable() && i > 0)
	{
		i--;
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}
	if (m_thread.joinable())
		m_thread.join();
}

bool WidgetVideo::OpenStream(std::string url, QString strImg)
{
	if (m_strImg != strImg && strImg != "")
	{
		m_strImg = strImg;
		m_lblFront->setPixmap(QPixmap(m_strImg));
		m_lblFront->raise();
	}

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
	
	m_thread = std::thread(&WidgetVideo::startVideo, this);

	return true;
}

void WidgetVideo::CloseStream()
{
	stopVideo();
}