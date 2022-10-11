#ifndef __GLYUV420WIDGET_H__
#define __GLYUV420WIDGET_H__

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLBuffer>
#include <QTimer>
#include <QQueue>
//#include "PubFuns.h"

QT_FORWARD_DECLARE_CLASS(QOpenGLShaderProgram)
QT_FORWARD_DECLARE_CLASS(QOpenGLTexture)

//using namespace MHCPUBLIC_FUNS;

class QGLYuv420Widget : public QOpenGLWidget, protected QOpenGLFunctions
{
	Q_OBJECT
public:
	QGLYuv420Widget(QWidget *parent = 0);
	~QGLYuv420Widget();

public slots:
	//int showFrameSlot(int cameraid, QQueue<PSTRU_PLATFORMFRAME> &queue);
	//int showFrameSlot(int cameraid, const STRU_PLATFORMFRAME &psf);
	void slotShowYuv(uchar *ptr, uint width, uint height); //显示一帧Yuv图像
public:
	volatile bool m_blShow;
protected:
	void initializeGL() Q_DECL_OVERRIDE;
	void paintGL() Q_DECL_OVERRIDE;

private:
	QOpenGLShaderProgram *program;
	QOpenGLBuffer vbo;
	GLuint textureUniformY, textureUniformU, textureUniformV; //opengl中y、u、v分量位置
	QOpenGLTexture *textureY = nullptr;
	QOpenGLTexture *textureU = nullptr;
	QOpenGLTexture *textureV = nullptr;
	GLuint idY, idU, idV; //自己创建的纹理对象ID，创建错误返回0
	uint videoW, videoH;
	uchar *yuvPtr = nullptr;

	ULONGLONG m_nLastTime;
};

#endif //__GLYUV420WIDGET_H__
