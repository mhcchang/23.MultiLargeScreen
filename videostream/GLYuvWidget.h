#ifndef GLYUVWIDGET_H
#define GLYUVWIDGET_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLBuffer>
#include <QTimer>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>
//#include <QDebug>
#include <QGLFormat>
#include <QLabel>

#include "nv12fborender.h"
#include "nv12render_gpu.h"
#include "streamsdk.h"

#define _RENDERGPU__ 

QT_FORWARD_DECLARE_CLASS(QOpenGLShaderProgram)
QT_FORWARD_DECLARE_CLASS(QOpenGLTexture)

//#pragma execution_character_set("utf-8")

//extern const int C_SHOWGRAYNUM;
struct STRectMsg{
	QPoint startpoint;
	QPoint endpoint;
	QString szMsg;
	Qt::GlobalColor col;
};

class GLYuvWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
	Q_OBJECT
public:
	GLYuvWidget(QWidget *parent = 0);
	~GLYuvWidget();
	int setThreadId(int threadid);
	void setShowVideo(bool blShowVideo) { m_blShowVideo = blShowVideo; };

	void setUseGpuFlag(bool blUseGpu) { m_blUseGpu = blUseGpu; };
public slots:
	void slotShowYuv(uchar *ptry, uchar *ptru, uchar *ptrv, uint width, uint height); //��ʾһ֡Yuvͼ��
	void slotShowYuvFrame(cvcam::Frame frame);
protected slots:
	void on_timeout();

protected:
	void initializeGL() Q_DECL_OVERRIDE;
	void paintGL() Q_DECL_OVERRIDE;
	//void paintEvent(QPaintEvent *e); // Q_DECL_OVERRIDE;
	void resizeGL(int w, int h) Q_DECL_OVERRIDE;
	void mouseDoubleClickEvent(QMouseEvent *event) Q_DECL_OVERRIDE;

private:
	QOpenGLShaderProgram *program;
	QOpenGLBuffer vbo;
	GLuint textureUniformY, textureUniformU, textureUniformV; //opengl��y��u��v����λ��
	QOpenGLTexture *textureY = nullptr, *textureU = nullptr, *textureV = nullptr;
	GLuint idY = 0, idU = 0, idV = 0; //�Լ��������������ID���������󷵻�0
	uint videoW, videoH;
	uchar *yuvPtry = nullptr;
	uchar *yuvPtru = nullptr;
	uchar *yuvPtrv = nullptr;
	QTimer timer;
	volatile bool m_blShow;
	volatile bool m_blUseGpu;

	//QPainter painter;
	QLabel *lb;
	void beforePainte();
	void drawVideo();
	//nv12
	GLuint idUV;
	void drawVideoNv12();

	GLuint m_Base;                                  //��������������ʾ�б�Ŀ�ʼλ��  
	GLfloat m_Cnt1;                                 //�����ƶ�������1  
	GLfloat m_Cnt2;                                 //�����ƶ�������2  
	GLuint m_Texture;                            //������������  
	void glPrint(GLuint x, GLuint y, const char *string, int set);
	void buildFont();
	void paintFont();
	QString text;
	int iii;
	int m_threadid;
	volatile bool m_blShowVideo;
	STRectMsg stRectMsgs[16];

#ifdef _RENDERGPU__
	cvcam::Frame m_frame;
	//Nv12Render * m_render;
	//Nv12FBORender * m_render;
	Nv12Render_Gpu* m_render;
#else
	Nv12FBORender * m_render;
#endif
};

#endif // GLYUVWIDGET_H
