#include "QGLYuv420Widget.h"

#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>
#include <QDebug>
#define VERTEXIN 0
#define TEXTUREIN 1

QGLYuv420Widget::QGLYuv420Widget(QWidget *parent) :
	QOpenGLWidget(parent)
{
	m_blShow = true;
	m_nLastTime = 0;
}

QGLYuv420Widget::~QGLYuv420Widget()
{
	makeCurrent();
	vbo.destroy();
	//textureY->destroy();
	//textureU->destroy();
	//textureV->destroy();
	doneCurrent();
}

void QGLYuv420Widget::slotShowYuv(uchar *ptr, uint width, uint height)
{
	yuvPtr = ptr;
	videoW = width;
	videoH = height;
	update();
}

int QGLYuv420Widget::showFrameSlot(int cameraid, const STRU_PLATFORMFRAME &psf)
{
	if (psf.timestamp >= m_nLastTime)
	{
		m_nLastTime = psf.timestamp;
	}
	else
	{
		g_logger->info(" showFrameSlot  psf.timestamp=%1 m_nLastTime=%2", psf.timestamp, m_nLastTime);
		//QMessageLogger(".\\videoclient_QGLYuv420Widget.log", __LINE__, "showFrameSlot").debug() << " psf.timestamp="
		//	<< psf.timestamp << " m_nLastTime=" << m_nLastTime;
		return 0;
	}
	if (m_blShow)
	{
		slotShowYuv(psf.pFrameData, psf.width, psf.height);
	}
	else
		slotShowYuv(NULL, psf.width, psf.height);

	return 1;
}

void QGLYuv420Widget::initializeGL()
{
	initializeOpenGLFunctions();
	unsigned char* sz = (unsigned char*)glGetString(GL_VERSION);
	if (g_logger)
		g_logger->info("GL_VERSION %1", QString::fromLocal8Bit((char*)sz));

	glEnable(GL_DEPTH_TEST);

	static const GLfloat vertices[]{
		//顶点坐标
		-1.0f,-1.0f,
		-1.0f,+1.0f,
		+1.0f,+1.0f,
		+1.0f,-1.0f,
		//纹理坐标
		0.0f,1.0f,
		0.0f,0.0f,
		1.0f,0.0f,
		1.0f,1.0f,
	};


	vbo.create();
	vbo.bind();
	vbo.allocate(vertices, sizeof(vertices));

	QOpenGLShader *vshader = new QOpenGLShader(QOpenGLShader::Vertex, this);
	const char *vsrc =
		"attribute vec4 vertexIn; \
    attribute vec2 textureIn; \
    varying vec2 textureOut;  \
    void main(void)           \
    {                         \
        gl_Position = vertexIn; \
        textureOut = textureIn; \
    }";
	vshader->compileSourceCode(vsrc);

	QOpenGLShader *fshader = new QOpenGLShader(QOpenGLShader::Fragment, this);
	const char *fsrc = "varying vec2 textureOut; \
    uniform sampler2D tex_y; \
    uniform sampler2D tex_u; \
    uniform sampler2D tex_v; \
    void main(void) \
    { \
        vec3 yuv; \
        vec3 rgb; \
        yuv.x = texture2D(tex_y, textureOut).r; \
        yuv.z = texture2D(tex_u, textureOut).r - 0.5; \
        yuv.y = texture2D(tex_v, textureOut).r - 0.5; \
        rgb = mat3( 1,       1,         1, \
                    0,       -0.39465,  2.03211, \
                    1.13983, -0.58060,  0) * yuv; \
        gl_FragColor = vec4(rgb, 1); \
    }";
//rgb.x = yuv.x + 1.4075 * (yuv.z - 0.5); \
		//rgb.y = yuv.x - 0.3455 * (yuv.y - 0.5) - 0.7169 * (yuv.z - 0.5); \
		//rgb.z = yuv.x + 1.779 * (yuv.y - 0.5); 
	//const char *fsrc = "varying vec2 textureOut; \
 //   uniform sampler2D tex_y; \
 //   uniform sampler2D tex_u; \
 //   uniform sampler2D tex_v; \
 //   void main(void) \
 //   { \
 //       vec3 yuv; \
 //       vec3 rgb; \
 //       yuv.x = texture2D(tex_y, textureOut).r; \
 //       yuv.z = texture2D(tex_u, textureOut).r - 0.5; \
 //       yuv.y = texture2D(tex_v, textureOut).r - 0.5; \
 //       rgb = mat3( 1,       1,         1, \
	//                0,      -0.3455,  1.779, \
 //                   1.4075,	-0.7169,  0) * yuv; \
 //       gl_FragColor = vec4(rgb, 1); \
 //   }";
	fshader->compileSourceCode(fsrc);

	program = new QOpenGLShaderProgram(this);
	program->addShader(vshader);
	program->addShader(fshader);
	program->bindAttributeLocation("vertexIn", VERTEXIN);
	program->bindAttributeLocation("textureIn", TEXTUREIN);
	program->link();
	program->bind();
	program->enableAttributeArray(VERTEXIN);
	program->enableAttributeArray(TEXTUREIN);
	program->setAttributeBuffer(VERTEXIN, GL_FLOAT, 0, 2, 2 * sizeof(GLfloat));
	program->setAttributeBuffer(TEXTUREIN, GL_FLOAT, 8 * sizeof(GLfloat), 2, 2 * sizeof(GLfloat));

	textureUniformY = program->uniformLocation("tex_y");
	textureUniformU = program->uniformLocation("tex_u");
	textureUniformV = program->uniformLocation("tex_v");
	textureY = new QOpenGLTexture(QOpenGLTexture::Target2D);
	textureU = new QOpenGLTexture(QOpenGLTexture::Target2D);
	textureV = new QOpenGLTexture(QOpenGLTexture::Target2D);
	textureY->create();
	textureU->create();
	textureV->create();
	idY = textureY->textureId();
	idU = textureU->textureId();
	idV = textureV->textureId();
	//glClearColor(0.0, 0.0, 0.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
}

void QGLYuv420Widget::paintGL()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	//	glClearColor(128.0f, 128.0f, 128.0f, 1.0f);
	if (!yuvPtr || !m_blShow)
	{
		return;
	}
	//    QMatrix4x4 m;
	//    m.perspective(60.0f, 4.0f/3.0f, 0.1f, 100.0f );//透视矩阵随距离的变化，图形跟着变化。屏幕平面中心就是视点（摄像头）,需要将图形移向屏幕里面一定距离。
	//    m.ortho(-2,+2,-2,+2,-10,10);//近裁剪平面是一个矩形,矩形左下角点三维空间坐标是（left,bottom,-near）,右上角点是（right,top,-near）所以此处为负，表示z轴最大为10；
	//远裁剪平面也是一个矩形,左下角点空间坐标是（left,bottom,-far）,右上角点是（right,top,-far）所以此处为正，表示z轴最小为-10；
	//此时坐标中心还是在屏幕水平面中间，只是前后左右的距离已限制。
	glActiveTexture(GL_TEXTURE0);  //激活纹理单元GL_TEXTURE0,系统里面的
	glBindTexture(GL_TEXTURE_2D, idY); //绑定y分量纹理对象id到激活的纹理单元
									   //使用内存中的数据创建真正的y分量纹理数据
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, videoW, videoH, 0, GL_RED, GL_UNSIGNED_BYTE, yuvPtr);
	// https:/ /blog.csdn.net/xipiaoyouzi/article/details/53584798 纹理参数解析
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glActiveTexture(GL_TEXTURE1); //激活纹理单元GL_TEXTURE1
	//glBindTexture(GL_TEXTURE1, idU);
	glBindTexture(GL_TEXTURE_2D, idU);
	//使用内存中的数据创建真正的u分量纹理数据
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, videoW >> 1, videoH >> 1, 0, GL_RED, GL_UNSIGNED_BYTE, yuvPtr + videoW * videoH);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glActiveTexture(GL_TEXTURE2); //激活纹理单元GL_TEXTURE2
	glBindTexture(GL_TEXTURE_2D, idV);
	//使用内存中的数据创建真正的v分量纹理数据
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, videoW >> 1, videoH >> 1, 0, GL_RED, GL_UNSIGNED_BYTE, yuvPtr + videoW * videoH * 5 / 4);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	//指定y纹理要使用新值
	glUniform1i(textureUniformY, 0);
	//指定u纹理要使用新值
	glUniform1i(textureUniformU, 1);
	//指定v纹理要使用新值
	glUniform1i(textureUniformV, 2);
	//使用顶点数组方式绘制图形
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
}
