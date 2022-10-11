#include "MultiLargeScreen.h"

void test111()
{
	float al = -1081.0;
	float al1 = fmod(al, 360);
	printf("%f \n", al1);
}

bool PTZControl()
{
	float azimuthHAngel = 350;
	float hfov = 40;
	float azimuthVAngel = -10;
	float vfov = 20;
	float minHFov = (azimuthHAngel - hfov / 2.0);
	float maxHFov = (hfov / 2.0 + azimuthHAngel);

	minHFov = fmod(minHFov, 360);
	maxHFov = fmod(maxHFov, 360);
	if (minHFov < 0)
		minHFov += 360;
	if (maxHFov < 0)
		maxHFov += 360;
	float minVFov = (azimuthVAngel - vfov / 2.0);
	float maxVFov = (vfov / 2.0 + azimuthVAngel);

//------
	float pan = 321;
	float tilt = -2;
	float zoom = 20;
	
	float m_fMinHFov = 280;
	float m_fMaxHFov = 280 + 42;

	float m_fMinVFov = -2;
	float m_fMaxVFov = -2 + 15;

	float fpan = pan;
	if (fpan >= 360.0)
		fpan = fmod(pan, 360);
	if (fpan > 180)
		fpan = fpan - 360;
	float minhfov = m_fMinHFov;
	float maxhfov = m_fMaxHFov;
	if (m_fMinHFov > 180)
		minhfov = m_fMinHFov - 360;
	if (m_fMaxHFov > 180)
		maxhfov = m_fMaxHFov - 360;

	if (fpan < minhfov)
		fpan = -1;
	else if (fpan > maxhfov)
		fpan = 1;
	else
	{
		fpan = (fpan * 2 - maxhfov - minhfov) / (maxhfov - minhfov);
	}

	float ftilt = 0.0f;
	if (tilt < m_fMinVFov)
		ftilt = -1;
	else if (tilt > m_fMaxVFov)
		ftilt = 1;
	else
		ftilt = (tilt * 2 - m_fMaxVFov - m_fMinVFov) / (m_fMaxVFov - m_fMinVFov);
	float fzoom = zoom / 40;

	float pana = 0;
	pan = -0.179991;
	pana = (280 + 42 / 2.0 * pan);

	return true;
}

MultiLargeScreen::MultiLargeScreen(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);

	ui.lblTitle->setAttribute(Qt::WA_TranslucentBackground);
	ui.labelstatus->setAttribute(Qt::WA_TranslucentBackground);
	
//	lblImg = ui.label;
//	image.load(":/MultiLargeScreen/uires/borde3x.png");

	//QImage image1 = image.scaled(rt.width(), rt.height(), Qt::IgnoreAspectRatio);
	//lblImg->setPixmap(QPixmap::fromImage(image1));

	//QRect rt1 = lblImg->geometry();

	//QImage image1 = image.scaled(rt1.width(), rt1.height(), Qt::IgnoreAspectRatio);
	//lblImg->setPixmap(QPixmap::fromImage(image1));//ÏÔÊ¾
	this->showMaximized();

	//test111();

	//PTZControl();
	//getchar();
}

void MultiLargeScreen::resizeEvent(QResizeEvent *event)
{
//	QRect rt1 = lblImg->geometry(); // this->rect();
	
//	QImage image1 = image.scaled(rt1.width(), rt1.height(), Qt::IgnoreAspectRatio);
//	lblImg->setPixmap(QPixmap::fromImage(image1));//ÏÔÊ¾

	//m_lblSetWidget->raise();
	//update();
}
