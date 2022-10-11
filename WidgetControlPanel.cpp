#include "WidgetControlPanel.h"
#include <QPainter>
#include <QDebug>

WidgetControlPanel::WidgetControlPanel(QWidget *parent)
	: QLabel(parent)
{
	setGeometry(QRect(0, 0, 140, 140));
	//ui.setupUi(this);
	m_image.load(":/UI/image/ctlPanel0.png");
	//m_imagerotate.load(":/UI/image/pantransparent03.png");

	//ui.label->setAttribute(Qt::WA_TranslucentBackground, true);

	//setAlignment(Qt::AlignCenter);
	//QPixmap qPixmap = QPixmap::fromImage(m_image);
	//QSize size(140, 140);
	//QPixmap qPixmapScaled = qPixmap.scaled(size, Qt::KeepAspectRatio);
	//setPixmap(qPixmapScaled);

	//QPixmap qPixmapScaled;
	//GetAnchorHeadPixmap(qPixmapScaled, QString(":/UI/image/ctlPanel.png"), 140, 140);
	//setAlignment(Qt::AlignCenter);
	//setPixmap(qPixmapScaled);
	setPixmap(QPixmap::fromImage(m_image));
}

WidgetControlPanel::~WidgetControlPanel()
{
}

void WidgetControlPanel::mousePressEvent(QMouseEvent * event)
{
	CheckPlace(event->pos().x(), event->pos().y());
	qDebug() << "mouse down";
}

void WidgetControlPanel::mouseReleaseEvent(QMouseEvent * event)
{
	emit OnStop();
	qDebug() << "mouse stop";
}

void WidgetControlPanel::ResizeUI()
{
	QRect rt = this->geometry();
	setAlignment(Qt::AlignCenter);
	QSize sz(qMin(rt.width(), rt.height()), qMin(rt.width(), rt.height()));
	QPixmap qPixmap = QPixmap::fromImage(m_image);
	QPixmap qPixmapScaled = qPixmap.scaled(sz, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
	setPixmap(qPixmapScaled);
}

void WidgetControlPanel::ResizeUI(QSize sz)
{
	setAlignment(Qt::AlignCenter);
	//QSize sz(qMin(rt.width(), rt.height()), qMin(rt.width(), rt.height()));
	QPixmap qPixmap = QPixmap::fromImage(m_image);
	QPixmap qPixmapScaled = qPixmap.scaled(sz, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
	setPixmap(qPixmapScaled);
}

void WidgetControlPanel::resizeEvent(QResizeEvent *event)
{
	ResizeUI();
	repaint();
}

bool WidgetControlPanel::LoadImg(QString apwzFileName, QImage* apImge, int w, int h)
{
	if (NULL == apImge || NULL == apwzFileName || 0 == apwzFileName[0])
	{
		return false;
	}

	apImge->load(apwzFileName);
	if (apImge->isNull() || apImge->width() <= 0 || apImge->height() <= 0)
	{
		return false;
	}

	*apImge = apImge->scaled(QSize(apImge->width(), apImge->height()), Qt::KeepAspectRatio);

	QImage lofixedImage(QSize(w, h), QImage::Format_ARGB32_Premultiplied);

	QPainter painter(&lofixedImage);
	painter.setCompositionMode(QPainter::CompositionMode_Source);
	painter.fillRect(lofixedImage.rect(), Qt::transparent);

	painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
	QPoint loPoint;
	int iWidth = apImge->width();
	int iHeight = apImge->height();
	loPoint.setX((w - apImge->width()) / 2);
	loPoint.setY((h - apImge->height()) / 2);
	painter.drawImage(loPoint, *apImge);
	painter.end();

	*apImge = lofixedImage;
	if (apImge->isNull())
	{
		return false;
	}

	return true;
}

bool WidgetControlPanel::GetAnchorHeadPixmap(QPixmap& aoPixmap, QString apwzHeadPath, int w, int h)
{
	QImage loSrcImge;
	QImage loDesImge;

	if (!LoadImg(apwzHeadPath, &loSrcImge, w, h))
	{
		return false;
	}

	QImage loResultImage = QImage(QSize(w, h), QImage::Format_ARGB32_Premultiplied);

	QPainter painter(&loResultImage);
	painter.setCompositionMode(QPainter::CompositionMode_Source);
	QRect rect = loResultImage.rect();
	painter.fillRect(loResultImage.rect(), Qt::transparent);

	painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
	painter.drawImage(0, 0, loDesImge);
	int width = loDesImge.width();
	int height = loDesImge.height();

	painter.setCompositionMode(QPainter::CompositionMode_SourceAtop);
	painter.drawImage(0, 0, loSrcImge);
	int iWidth = loSrcImge.width();
	int iHeight = loSrcImge.height();

	painter.setCompositionMode(QPainter::CompositionMode_DestinationOver);
	painter.fillRect(loResultImage.rect(), QColor(0, 0, 0, 0));
	painter.end();

	aoPixmap = QPixmap::fromImage(loResultImage);
	return true;
}

void WidgetControlPanel::paintEvent(QPaintEvent * event)
{
	//QRect rt = this->geometry();
	////QPixmap temp(qMin(rt.width(), rt.height()), qMin(rt.width(), rt.height()));
	//QSize sz(qMin(rt.width(), rt.height()), qMin(rt.width(), rt.height()));
	////temp.fill(Qt::transparent);
	//QPixmap qPixmap = QPixmap::fromImage(m_image);
	//QPixmap qPixmapScaled = qPixmap.scaled(sz, Qt::KeepAspectRatio);

	//QPainter p1(this);
	//// CompositionMode_Source将图片绘制进去
	//p1.setCompositionMode(QPainter::CompositionMode_Source);
	//p1.drawPixmap(0, 0, qPixmapScaled);
	//// CompositionMode_DestinationIn设置图片的透明度
	//p1.setCompositionMode(QPainter::CompositionMode_DestinationIn);
	//// 根据QColor中第四个参数设置透明度，此处position的取值范围是0～255
	////p1.fillRect(temp.rect(), QColor(0, 0, 0, this->transparency * 255 / 100));
	//p1.end();

	//QPainter painter(this);
	//painter.setRenderHint(QPainter::Antialiasing, true);
	//painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
	QLabel::paintEvent(event);
}

//int WidgetControlPanel::heightForWidth(int w) const
//{
//	//qDebug() << "GroupBoxHFW::heightForWidth called";
//	return w;
//}
//
//bool WidgetControlPanel::hasHeightForWidth() const
//{
//	//qDebug() << "GroupBoxHFW::hasHeightForWidth called";
//	return true;
//}

/**
* @brief 以按钮圆心为坐标圆点，建立坐标系，求出(targetX, targetY)坐标与x轴的夹角
* @param targetX x坐标
* @param targetY y坐标
* @param r 直径
* @return (targetX, targetY)坐标与x轴的夹角
*/
#define PI (3.14159265)
float calcAngle(float targetX, float targetY, float r)
{
	float x = targetX - r / 2; //len/2 圆点x坐标
	float y = targetY - r / 2; //len/2 圆点y坐标
	double radian;

	if (x != 0)
	{
		float tan = abs(y / x);
		if (x > 0)
		{
			if (y >= 0)
			{
				radian = atan(tan);
			}
			else
			{
				radian = 2 * PI - atan(tan);
			}
		}
		else
		{
			if (y >= 0)
			{
				radian = PI - atan(tan);
			}
			else
			{
				radian = PI + atan(tan);
			}
		}
	}
	else
	{
		if (y > 0)
		{
			radian = PI / 2;
		}
		else
		{
			radian = -PI / 2;
		}
	}
	return (float)((radian * 180) / PI);
}

#if 1
void WidgetControlPanel::CheckPlace(float x, float y)
{
	QRect rt = this->geometry();

	float fang = calcAngle(x, y, qMin(rt.width(), rt.height()));
	::printf("%d \n", (int)(fang / 45.0));
	::printf("------------- CheckPlace x,y=[%d,%d] w,h=[%d,%d]\n", x, y, rt.width(), rt.height());

	QRect rtn;
	int iplace = (fang + 22.5) / 45.0;
	iplace %= 8;

	printf("iplace =%d", iplace);
	m_image.load(QString(":/UI/image/ctlPanel%1.png").arg(iplace));
	setAlignment(Qt::AlignCenter);
	QSize sz(qMin(rt.width(), rt.height()), qMin(rt.width(), rt.height()));
	QPixmap qPixmap = QPixmap::fromImage(m_image);
	QPixmap qPixmapScaled = qPixmap.scaled(sz, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);

	setPixmap(qPixmapScaled);
	switch (iplace)
	{
	case 0:
		emit OnClickRight();
		break;
	case 1:
	{
		emit OnClickBottomRight();
		break;
	}
	case 2:
	{
		emit OnClickBottom();
		break;
	}
	case 3:
	{
		emit OnClickBottomLeft();
		break;
	}
	case 4:
	{
		emit OnClickLeft();
		break;
	}
	case 5:
	{
		emit OnClickTopLeft();
		break;
	}
	case 6:
	{
		emit OnClickTop();
		break;
	}
	case 7:
	{
		emit OnClickTopRight();
		break;
	}
	default:
		break;
	}
}

#else
void WidgetControlPanel::CheckPlace(float x, float y)
{
	QRect rt = this->geometry();

	float fang = calcAngle(x, y, rt.width());
	::printf("%d \n", (int)(fang / 45.0));

	QRect rtn;
	int iplace = (fang + 22.5) / 45.0;
	iplace %= 8;
	switch (iplace)
	{
	case 0:
		ui.label->move((140 - 50), (140 - 60) / 2);
		ui.label->setPixmap(QPixmap::fromImage(m_image));
		emit OnClickRight();
		break;
	case 1:
	{
		ui.label->move(80, (140 - 60) - 4);

		QMatrix matrix;
		matrix.rotate(90.0);//以90度为例
		QImage image = m_imagerotate.transformed(matrix, Qt::FastTransformation);
		ui.label->setPixmap(QPixmap::fromImage(image));

		emit OnClickBottomRight();
		break;
	}
	case 2:
	{
		QMatrix matrix;
		matrix.rotate(90.0);//以90度为例
		QImage image = m_image.transformed(matrix, Qt::FastTransformation);
		ui.label->setPixmap(QPixmap::fromImage(image));

		ui.label->move((140 - 48) / 2, (140 - 60) +4);
		emit OnClickBottom();
		break;
	}
	case 3:
	{
		QMatrix matrix;
		matrix.rotate(180.0);//以90度为例
		QImage image = m_imagerotate.transformed(matrix, Qt::FastTransformation);
		ui.label->setPixmap(QPixmap::fromImage(image));

		ui.label->move(10, (140 - 60)  - 4);
		emit OnClickBottomLeft();
		break;
	}
	case 4:
	{
		ui.label->move(2, (140 - 60) / 2);

		QImage image = m_image.mirrored(true, false);
		ui.label->setPixmap(QPixmap::fromImage(image));
		emit OnClickLeft();
		break;
	}
	case 5:
	{
		QMatrix matrix;
		matrix.rotate(270.0);//以90度为例
		QImage image = m_imagerotate.transformed(matrix, Qt::FastTransformation);
		ui.label->setPixmap(QPixmap::fromImage(image));

		ui.label->move(10, 4);
		emit OnClickTopLeft();
		break;
	}
	case 6:
	{
		QMatrix matrix;
		matrix.rotate(270.0);//以90度为例
		QImage image = m_image.transformed(matrix, Qt::FastTransformation);

		ui.label->setPixmap(QPixmap::fromImage(image));
		ui.label->move((140 - 50) / 2, -2);
		emit OnClickTop();
		break;
	}
	case 7:
	{
		ui.label->setPixmap(QPixmap::fromImage(m_imagerotate));

		ui.label->move(80, 2);
		emit OnClickTopRight();
		break;
	}
	default:
		break;
	}

	/*
	 
//关于镜像旋转，QImage中有对应的接口，可以通过设定水平、垂直方向旋转
QImage image::mirrored(bool horizontal = false, bool vertical = true) const ;
 
 
//水平翻转
    image = image.mirrored(true, false);
    imageLabel->setPixmap(QPixmap::fromImage(image));
//垂直翻转
    image = image.mirrored(false, true);
    imageLabel->setPixmap(QPixmap::fromImage(image));
 
 
//顺时针旋转90度
    QMatrix matrix;
    matrix.rotate(90.0);//以90度为例
    image = image.transformed(matrix,Qt::FastTransformation);
    imageLabel->setPixmap(QPixmap::fromImage(image));
//逆时针旋转45度
    QMatrix matrix;
    matrix.rotate(-45.0);//以45度为例
    image = image.transformed(matrix,Qt::FastTransformation);
    imageLabel->setPixmap(QPixmap::fromImage(image));
	*/
	//ui.control->setGeometry(rtn);
}
#endif
