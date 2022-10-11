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
		//��������
		-1.0f,-1.0f,
		-1.0f,+1.0f,
		+1.0f,+1.0f,
		+1.0f,-1.0f,
		//��������
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
	//    m.perspective(60.0f, 4.0f/3.0f, 0.1f, 100.0f );//͸�Ӿ��������ı仯��ͼ�θ��ű仯����Ļƽ�����ľ����ӵ㣨����ͷ��,��Ҫ��ͼ��������Ļ����һ�����롣
	//    m.ortho(-2,+2,-2,+2,-10,10);//���ü�ƽ����һ������,�������½ǵ���ά�ռ������ǣ�left,bottom,-near��,���Ͻǵ��ǣ�right,top,-near�����Դ˴�Ϊ������ʾz�����Ϊ10��
	//Զ�ü�ƽ��Ҳ��һ������,���½ǵ�ռ������ǣ�left,bottom,-far��,���Ͻǵ��ǣ�right,top,-far�����Դ˴�Ϊ������ʾz����СΪ-10��
	//��ʱ�������Ļ�������Ļˮƽ���м䣬ֻ��ǰ�����ҵľ��������ơ�
	glActiveTexture(GL_TEXTURE0);  //��������ԪGL_TEXTURE0,ϵͳ�����
	glBindTexture(GL_TEXTURE_2D, idY); //��y�����������id�����������Ԫ
									   //ʹ���ڴ��е����ݴ���������y������������
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, videoW, videoH, 0, GL_RED, GL_UNSIGNED_BYTE, yuvPtr);
	// https:/ /blog.csdn.net/xipiaoyouzi/article/details/53584798 �����������
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glActiveTexture(GL_TEXTURE1); //��������ԪGL_TEXTURE1
	//glBindTexture(GL_TEXTURE1, idU);
	glBindTexture(GL_TEXTURE_2D, idU);
	//ʹ���ڴ��е����ݴ���������u������������
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, videoW >> 1, videoH >> 1, 0, GL_RED, GL_UNSIGNED_BYTE, yuvPtr + videoW * videoH);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glActiveTexture(GL_TEXTURE2); //��������ԪGL_TEXTURE2
	glBindTexture(GL_TEXTURE_2D, idV);
	//ʹ���ڴ��е����ݴ���������v������������
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, videoW >> 1, videoH >> 1, 0, GL_RED, GL_UNSIGNED_BYTE, yuvPtr + videoW * videoH * 5 / 4);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	//ָ��y����Ҫʹ����ֵ
	glUniform1i(textureUniformY, 0);
	//ָ��u����Ҫʹ����ֵ
	glUniform1i(textureUniformU, 1);
	//ָ��v����Ҫʹ����ֵ
	glUniform1i(textureUniformV, 2);
	//ʹ�ö������鷽ʽ����ͼ��
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
}
