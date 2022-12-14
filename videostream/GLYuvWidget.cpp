#include "glyuvwidget.h"

#include <QtOpenGL>

#define VERTEXIN 0
#define TEXTUREIN 1

GLYuvWidget::GLYuvWidget(QWidget *parent) :
	QOpenGLWidget(parent),
	timer(this),
	m_blShow(false),
	m_blUseGpu(true)
{
	text = "QPainter provides highly optimized functions to do "
		"most of the drawing GUI programs require. It can drawRect drawText ";

	CUcontext ctx = nullptr;
	m_render = new Nv12Render_Gpu(ctx);
}

GLYuvWidget::~GLYuvWidget()
{
	glDeleteLists(m_Base, 256);

	doneCurrent();
}

int GLYuvWidget::setThreadId(int threadid)
{
	m_threadid = threadid;
	
	return threadid;
}

void GLYuvWidget::slotShowYuvFrame(cvcam::Frame frame)
{
	m_blShow = true;

	m_frame = frame;
	update();
}

void GLYuvWidget::slotShowYuv(uchar *ptry, uchar *ptru, uchar *ptrv, uint width, uint height)
{
	m_blShow = true;
	yuvPtry = ptry;
	yuvPtru = ptru;
	yuvPtrv = ptrv;
	videoW = width;
	videoH = height;

	//qDebug() << "videoW=" << videoW;
	update();
}

void GLYuvWidget::initializeGL()
{
	//QSurfaceFormat format;
	//format.setDepthBufferSize(24);
	//format.setStencilBufferSize(8);
	//format.setVersion(3, 2);
	//format.setProfile(QSurfaceFormat::CoreProfile);
	//this->setFormat(format);

	if (m_blUseGpu) 
	{
		//#ifdef __USE_GPU__
		//
		
#ifdef _RENDERGPU__
		
		//m_render->initialize(3840, 2160);
		m_render->initialize(3840, 2160);
#else
		m_render = new Nv12FBORender();
#endif
		//m_render->initialize(1920, 1080);
		//m_render->m_nthreadid = m_threadid;
		return;
	}
//#else
/*	else 
	{
		initializeOpenGLFunctions();

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
				yuv.y = texture2D(tex_u, textureOut).r - 0.5; \
				yuv.z = texture2D(tex_v, textureOut).r - 0.5; \
				rgb = mat3( 1,       1,         1, \
				0,       -0.39465,  2.03211, \
				1.13983, -0.58060,  0) * yuv; \
				gl_FragColor = vec4(rgb, 1); \
			}";
		fshader->compileSourceCode(fsrc);

		program = new QOpenGLShaderProgram(this);
		program->addShader(vshader);
		program->addShader(fshader);
		program->bindAttributeLocation("vertexIn", VERTEXIN);
		program->bindAttributeLocation("textureIn", TEXTUREIN);
		program->link();

		//-0000000000000000000000
		glShadeModel(GL_SMOOTH);                            //????????????  
		glClearDepth(1.0);                                  //????????????  
		glEnable(GL_DEPTH_TEST);                            //????????????  
		glDepthFunc(GL_LEQUAL);                             //??????????????????  
		glBlendFunc(GL_SRC_ALPHA, GL_ONE);                  //????????????  
		glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);  //??????????????????????  

		buildFont();                                        //????????  
		//-------------------------
		beforePainte();
	}*/
//#endif
}

void GLYuvWidget::buildFont()                            //????????????  
{
	float cx, cy;                                       //??????????x??y????  
	m_Base = glGenLists(256);                           //????256??????????  
	glBindTexture(GL_TEXTURE_2D, m_Texture);         //????????????  

	for (int i = 0; i<256; i++)                           //????256??????????  
	{
		cx = float(i % 16) / 16.0f;                         //??????????x????  
		cy = float(i / 16) / 16.0f;                         //??????????y????  

		glNewList(m_Base + i, GL_COMPILE);                //????????????????  
		glBegin(GL_QUADS);                          //????????????????????????  
		glTexCoord2f(cx, 1 - cy - 0.0625f);
		glVertex2i(0, 0);
		glTexCoord2f(cx + 0.0625f, 1 - cy - 0.0625f);
		glVertex2i(16, 0);
		glTexCoord2f(cx + 0.0625f, 1 - cy);
		glVertex2i(16, 16);
		glTexCoord2f(cx, 1 - cy);
		glVertex2i(0, 16);
		glEnd();                                    //??????????????????  
		glTranslated(10, 0, 0);                     //????????????????????????10??????  
		glEndList();                                    //????????????????  
	}
}

void GLYuvWidget::paintFont()                              //????????????????????????
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //??????????????????
	glLoadIdentity();                                   //??????????????????????

	glBindTexture(GL_TEXTURE_2D, m_Texture);         //??????????????
	glTranslatef(0.0f, 0.0f, -5.0f);                    //????????5.0????
	glRotatef(45.0f, 0.0f, 0.0f, 1.0f);                 //??z??????45??
	glRotatef(m_Cnt1*30.0f, 1.0f, 1.0f, 0.0f);          //??(1,1,0)??????
	glDisable(GL_BLEND);                                //????????
	glColor3f(1.0f, 1.0f, 1.0f);                        //??????????????
	glBegin(GL_QUADS);                                  //??????????????
	glTexCoord2d(0.0f, 0.0f);
	glVertex2f(-1.0f, 1.0f);
	glTexCoord2d(1.0f, 0.0f);
	glVertex2f(1.0f, 1.0f);
	glTexCoord2d(1.0f, 1.0f);
	glVertex2f(1.0f, -1.0f);
	glTexCoord2d(0.0f, 1.0f);
	glVertex2f(-1.0f, -1.0f);
	glEnd();

	glRotatef(90.0f, 1.0f, 1.0f, 0.0);                  //??(1,1,0)??????90??
	glBegin(GL_QUADS);                                  //??????????????????????????????
	glTexCoord2d(0.0f, 0.0f);
	glVertex2f(-1.0f, 1.0f);
	glTexCoord2d(1.0f, 0.0f);
	glVertex2f(1.0f, 1.0f);
	glTexCoord2d(1.0f, 1.0f);
	glVertex2f(1.0f, -1.0f);
	glTexCoord2d(0.0f, 1.0f);
	glVertex2f(-1.0f, -1.0f);
	glEnd();

	glEnable(GL_BLEND);                                 //????????
	glLoadIdentity();                                   //????????
	//????????????????????
	glColor3f(1.0f*float(cos(m_Cnt1)), 1.0*float(sin(m_Cnt2)),
		1.0f - 0.5f*float(cos(m_Cnt1 + m_Cnt2)));
	glPrint(int((280 + 250 * cos(m_Cnt1))),
		int(235 + 200 * sin(m_Cnt2)), "NeHe", 0);
	glColor3f(1.0*float(sin(m_Cnt2)), 1.0f - 0.5f*float(cos(m_Cnt1 + m_Cnt2)),
		1.0f*float(cos(m_Cnt1)));
	glPrint(int((280 + 230 * cos(m_Cnt2))),
		int(235 + 200 * sin(m_Cnt1)), "OpenGL", 1);
	glColor3f(0.0f, 0.0f, 1.0f);
	glPrint(int(240 + 200 * cos((m_Cnt1 + m_Cnt2) / 5)), 2,
		"Giuseppe D'Agata", 0);
	glColor3f(1.0f, 1.0f, 1.0f);
	glPrint(int(242 + 200 * cos((m_Cnt1 + m_Cnt2) / 5)), 2,
		"Giuseppe D'Agata", 0);

	m_Cnt1 += 0.01f;                                   //??????????????????
	m_Cnt2 += 0.0081f;
}

void GLYuvWidget::beforePainte()
{
	glEnable(GL_DEPTH_TEST);

	static const GLfloat vertices[]{
		//????????
		-1.0f, -1.0f,
			-1.0f, +1.0f,
			+1.0f, +1.0f,
			+1.0f, -1.0f,
			//????????
			0.0f, 1.0f,
			0.0f, 0.0f,
			1.0f, 0.0f,
			1.0f, 1.0f,
	};

	vbo.create();
	vbo.bind();
	vbo.allocate(vertices, sizeof(vertices));

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
	glClearColor(0.0, 0.0, 0.0, 0.0);
	program->disableAttributeArray(VERTEXIN);
	program->disableAttributeArray(TEXTUREIN);
	program->release();
	//connect(&timer, SIGNAL(timeout()), this, SLOT(on_timeout()));
	//timer.setInterval(25);
	//timer.start();
}

void GLYuvWidget::on_timeout()
{
	if (m_blShow)
		update();
}

void drawString(const char* str) {
	static int isFirstCall = 1;
	static GLuint lists;

	if (isFirstCall) { // ????????????????????????????
		// ????????ASCII????????????????????
		isFirstCall = 0;

		// ????MAX_CHAR????????????????????
		lists = glGenLists(255);

		// ??????????????????????????????????????????
		wglUseFontBitmaps(wglGetCurrentDC(), 0, 255, lists);
	}
	// ????????????????????????????????????????
	for (; *str != '\0'; ++str)
		glCallList(lists + *str);
}

void GLYuvWidget::drawVideo()
{
/*	static const GLfloat vertices[]{
		//????????
		-1.0f, -1.0f,
			-1.0f, +1.0f,
			+1.0f, +1.0f,
			+1.0f, -1.0f,
			//????????
			0.0f, 1.0f,
			0.0f, 0.0f,
			1.0f, 0.0f,
			1.0f, 1.0f,
	};

	vbo.create();
	vbo.bind();
	vbo.allocate(vertices, sizeof(vertices));
	vbo.write(0, vertices, sizeof(vertices));

	program->bind();
	program->enableAttributeArray(VERTEXIN);
	program->enableAttributeArray(TEXTUREIN);
	program->setAttributeBuffer(VERTEXIN, GL_FLOAT, 0, 2, 2 * sizeof(GLfloat));
	program->setAttributeBuffer(TEXTUREIN, GL_FLOAT, 8 * sizeof(GLfloat), 2, 2 * sizeof(GLfloat));

	textureUniformY = program->uniformLocation("tex_y");
	textureUniformU = program->uniformLocation("tex_u");
	textureUniformV = program->uniformLocation("tex_v");

	//    QMatrix4x4 m;
	//    m.perspective(60.0f, 4.0f/3.0f, 0.1f, 100.0f );//??????????????????????????????????????????????????????????????????,????????????????????????????????
	//    m.ortho(-2,+2,-2,+2,-10,10);//????????????????????,????????????????????????????left,bottom,-near??,????????????right,top,-near????????????????????z????????10??
	//??????????????????????,????????????????????left,bottom,-far??,????????????right,top,-far????????????????????z????????-10??
	//????????????????????????????????????????????????????????????

	glActiveTexture(GL_TEXTURE0);  //????????????GL_TEXTURE0,??????????
	glBindTexture(GL_TEXTURE_2D, idY); //????y????????????id????????????????
	//??????????????????????????y????????????
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, videoW, videoH, 0, GL_RED, GL_UNSIGNED_BYTE, yuvPtry);
	//https: //blog.csdn.net/xipiaoyouzi/article/details/53584798 ????????????
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glActiveTexture(GL_TEXTURE1); //????????????GL_TEXTURE1
	glBindTexture(GL_TEXTURE_2D, idU);
	//??????????????????????????u????????????
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, videoW >> 1, videoH >> 1, 0, GL_RED, GL_UNSIGNED_BYTE, yuvPtru);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glActiveTexture(GL_TEXTURE2); //????????????GL_TEXTURE2
	glBindTexture(GL_TEXTURE_2D, idV);
	//??????????????????????????v????????????
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, videoW >> 1, videoH >> 1, 0, GL_RED, GL_UNSIGNED_BYTE, yuvPtrv);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	//????y??????????????
	glUniform1i(textureUniformY, 0);
	//????u??????????????
	glUniform1i(textureUniformU, 1);
	//????v??????????????
	glUniform1i(textureUniformV, 2);
	//????????????????????????
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

	textureY->release();
	textureU->release();
	textureV->release();

	vbo.release();
	program->disableAttributeArray(VERTEXIN);
	program->disableAttributeArray(TEXTUREIN);
	program->release();*/
}

void GLYuvWidget::drawVideoNv12()
{
/*	initializeOpenGLFunctions();
	const char *vsrc =
		"attribute vec4 vertexIn; \
             attribute vec4 textureIn; \
             varying vec4 textureOut;  \
             void main(void)           \
             {                         \
                 gl_Position = vertexIn; \
                 textureOut = textureIn; \
             }";

	const char *fsrc =
		"varying mediump vec4 textureOut;\n"
		"uniform sampler2D textureY;\n"
		"uniform sampler2D textureUV;\n"
		"void main(void)\n"
		"{\n"
		"vec3 yuv; \n"
		"vec3 rgb; \n"
		"yuv.x = texture2D(textureY, textureOut.st).r - 0.0625; \n"
		"yuv.y = texture2D(textureUV, textureOut.st).r - 0.5; \n"
		"yuv.z = texture2D(textureUV, textureOut.st).g - 0.5; \n"
		"rgb = mat3( 1,       1,         1, \n"
		"0,       -0.39465,  2.03211, \n"
		"1.13983, -0.58060,  0) * yuv; \n"
		"gl_FragColor = vec4(rgb, 1); \n"
		"}\n";

	program->addCacheableShaderFromSourceCode(QOpenGLShader::Vertex, vsrc);
	program->addCacheableShaderFromSourceCode(QOpenGLShader::Fragment, fsrc);
	program->link();

	GLfloat points[]{
		-1.0f, 1.0f,
		1.0f, 1.0f,
		1.0f, -1.0f,
		-1.0f, -1.0f,

		0.0f,0.0f,
		1.0f,0.0f,
		1.0f,1.0f,
		0.0f,1.0f
	};

	vbo.create();
	vbo.bind();
	vbo.allocate(points, sizeof(points));

	GLuint ids[2];
	glGenTextures(2, ids);
	idY = ids[0];
	idUV = ids[1];


	glClearColor(0.5f, 0.5f, 0.7f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glDisable(GL_DEPTH_TEST);

	program->bind();
	vbo.bind();
	program->enableAttributeArray("vertexIn");
	program->enableAttributeArray("textureIn");
	program->setAttributeBuffer("vertexIn", GL_FLOAT, 0, 2, 2 * sizeof(GLfloat));
	program->setAttributeBuffer("textureIn", GL_FLOAT, 2 * 4 * sizeof(GLfloat), 2, 2 * sizeof(GLfloat));

	glActiveTexture(GL_TEXTURE0 + 1);
	glBindTexture(GL_TEXTURE_2D, idY);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, videoW, videoH, 0, GL_RED, GL_UNSIGNED_BYTE, yuvPtry);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glActiveTexture(GL_TEXTURE0 + 0);
	glBindTexture(GL_TEXTURE_2D, idUV);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RG, videoW >> 1, videoH >> 1, 0, GL_RG, GL_UNSIGNED_BYTE, yuvPtry);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	program->setUniformValue("textureUV", 0);
	program->setUniformValue("textureY", 1);
	glDrawArrays(GL_QUADS, 0, 4);
	program->disableAttributeArray("vertexIn");
	program->disableAttributeArray("textureIn");
	program->release();*/
}

/*
void GLYuvWidget::paintGL()
{
	//QPainter painter;
	//painter.begin(this);
	//painter.fillRect(0, 0, 128, 128, Qt::green);
	//painter.beginNativePainting();
	//////	//??????1????????????QOpenGLVertexArrayObject??QOpenGLBuffer??QOpenGLShaderProgram??QOpenGLTexture????
	// 2??????????????????????????????????????

	//glClearColor(20.0, 20.0, 50.0, 1.0f);
	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//glEnable(GL_BLEND);
	////glFrontFace(GL_CW);
	////glCullFace(GL_FRONT);
	////glEnable(GL_CULL_FACE);
	//glEnable(GL_DEPTH_TEST);
	//glEnable(GL_TEXTURE_2D);

	drawVideo();

//
//	glDisable(GL_BLEND);
//	glDisable(GL_DEPTH_TEST);
//////	glDisable(GL_STENCIL_TEST);
////	//glDisable(GL_SCISSOR_TEST);
////	glBindTexture(GL_TEXTURE_2D, 0);
////	//glActiveTexture(GL_TEXTURE0);
////	//
//	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//	glClearColor(1, 1, 1, 0);
//
//	painter.endNativePainting();
//	painter.setPen(Qt::blue);
//	painter.drawText(20, 50, "This is a Text!");
//	painter.end();
}
*/

int iwidth = 10;
int iheight = 50;

void GLYuvWidget::paintGL()
{
//#ifdef __USE_GPU__
	//if (m_blUseGpu && m_frame.isValid())
	if (m_blUseGpu && m_frame.isValid())
	{
		QPainter painter;
		painter.begin(this);
		painter.beginNativePainting();
		//drawVideoNv12();
	//	makeCurrent();
		//m_render->render(yuvPtry, yuvPtru, videoW, videoH);
		//m_render->render(yuvPtry, yuvPtru, videoW, videoH);
		//printf("%p %p \n", m_frame.data(0), m_frame.data(1));
		
		//printf("be %llu \n", ::GetTickCount64());
		QTime tt;
		tt.start();

		m_render->render(m_frame);

		//qDebug("render elapsed: %d ms", tt.elapsed());
		//printf("af %llu \n", ::GetTickCount64());
	//	doneCurrent();
		painter.endNativePainting();
		painter.end();

		m_frame = cvcam::Frame();
		return;
	//	makeCurrent();
	}
//#else

	////	//??????1????????????QOpenGLVertexArrayObject??QOpenGLBuffer??QOpenGLShaderProgram??QOpenGLTexture????2??????????????????????????????????????
/*
	if (m_blShow)
	{
		QPainter painter;
		painter.begin(this);
		painter.beginNativePainting();
		glClearColor(0.0, 0.0, 0.0, 1.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glEnable(GL_STENCIL_TEST);
		glEnable(GL_DEPTH_TEST);

		drawVideo();

		glActiveTexture(GL_TEXTURE4);
		glDisableClientState(GL_VERTEX_ARRAY);
		glDisableClientState(GL_COLOR_ARRAY);

		//	glDisable(GL_BLEND);
		glDisable(GL_DEPTH_TEST);
		glDisable(GL_STENCIL_TEST);
		//glDisable(GL_TEXTURE_2D);

		glDisableClientState(GL_VERTEX_ARRAY);
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		glBindTexture(GL_TEXTURE_2D, 0);
		glDisable(GL_TEXTURE_2D);
		painter.endNativePainting();
		qDebug() << "painter.end " << iii;
		painter.end();
		glEnable(GL_DEPTH_TEST);
	}
	else
	{
	
	}
	*/
}

//??????????
void GLYuvWidget::glPrint(GLuint x, GLuint y,           
	const char *string, int set)
{
	if (set > 1)                                        //??????????????1  
	{
		set = 1;                                        //????????1  
	}

	glBindTexture(GL_TEXTURE_2D, m_Texture);            //??????????????  
	glDisable(GL_DEPTH_TEST);                           //????????????  
	glMatrixMode(GL_PROJECTION);                        //????????????  
	glPushMatrix();                                     //??????????????????  
	glLoadIdentity();                                   //????????????  
	glOrtho(0, 640, 0, 480, -1, 1);                     //????????????????????  
	glMatrixMode(GL_MODELVIEW);                         //????????????????  
	glPushMatrix();                                     //??????????????????????  
	glLoadIdentity();                                   //????????????????  

	glTranslated(x, y, 0);                              //????????????????(x,y)????  
	glListBase(m_Base - 32 + (128 * set));              //??????????  
	glCallLists(strlen(string), GL_BYTE, string);       //????????????????  
	glMatrixMode(GL_PROJECTION);                        //????????????  
	glPopMatrix();                                      //????????????????  
	glMatrixMode(GL_MODELVIEW);                         //????????????????  
	glPopMatrix();                                      //??????????  
	glEnable(GL_DEPTH_TEST);                            //????????????  
}

void GLYuvWidget::resizeGL(int w, int h)
{
	if (h == 0)// ??????????
	{
		h = 1;// ????????1
	}

	if (!m_blShow)
		return;
	//????????
//	glViewport(0, 0, w, h);
}

void GLYuvWidget::mouseDoubleClickEvent(QMouseEvent *event)
{
	//QPoint pos = event->pos();

	//text = QString("mouse point %1 %2 ?? %3?? ????XXX").arg(QString::number(pos.x(), 10), QString::number(pos.y(), 10), QString::number(m_threadid + 1, 10));

	QWidget::mouseDoubleClickEvent(event);
}
