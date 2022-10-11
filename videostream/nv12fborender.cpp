
#include "nv12fborender.h"
#include <QOpenGLTexture>
#include <QDebug>

void Nv12FBORender::createTextureSrc(GLuint *tex_screen, unsigned int size_x, unsigned int size_y)
{
	// create a texture
	glGenTextures(1, tex_screen);
	glBindTexture(GL_TEXTURE_2D, *tex_screen);

	// set basic parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	// buffer data
#ifndef USE_TEXTURE_RGBA8UI
	printf("Creating a Texture render target GL_RGBA16F_ARB\n");
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F_ARB, size_x, size_y, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
#else
	printf("Creating a Texture render target GL_RGBA8UI_EXT\n");
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8UI_EXT, size_x, size_y, 0, GL_RGBA_INTEGER_EXT, GL_UNSIGNED_BYTE, NULL);
#endif
	
	// register this texture with CUDA
	cudaError_t err = cudaGraphicsGLRegisterImage(cudaResource, *tex_screen,
		GL_TEXTURE_2D, cudaGraphicsRegisterFlagsWriteDiscard);
	if (err != cudaSuccess)
	{
		//std::cout << "cudaGraphicsGLRegisterImage: " << err << "Line: " << __LINE__;
		//return -1;
	}
}

void Nv12FBORender::initialize()
{
	initializeOpenGLFunctions();
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

	program.addCacheableShaderFromSourceCode(QOpenGLShader::Vertex, vsrc);
	program.addCacheableShaderFromSourceCode(QOpenGLShader::Fragment, fsrc);
	program.link();

	GLfloat points[]{
		-1.0f, 1.0f,		1.0f, 1.0f,
		1.0f, -1.0f,		-1.0f, -1.0f,

		0.0f,0.0f,		1.0f,0.0f,
		1.0f,1.0f,		0.0f,1.0f
	};

	vbo.create();
	vbo.bind();
	vbo.allocate(points, sizeof(points));

	glGenTextures(2, ids);
	idY = ids[0];
	idUV = ids[1];

	glGenFramebuffers(2, ids);
	////绑定
	glBindFramebuffer(GL_FRAMEBUFFER, ids[0]);
	glBindFramebuffer(GL_FRAMEBUFFER, ids[1]);

	//createTextureSrc(&ids[0], 3840, 2160);
	//cudaGraphicsMapFlagsReadOnly //cudaGraphicsRegisterFlagsWriteDiscard
	//cudaError_t err = cudaGraphicsGLRegisterImage(cudaResource, ids[0], GL_TEXTURE_2D, cudaGraphicsRegisterFlagsWriteDiscard);
	//if (err != cudaSuccess)
	//{
	//	//std::cout << "cudaGraphicsGLRegisterImage: " << err << "Line: " << __LINE__;
	//	//return -1;
	//}

	//err = cudaGraphicsGLRegisterImage(cudaResource1, ids[1], GL_TEXTURE_2D, cudaGraphicsRegisterFlagsWriteDiscard);
	//if (err != cudaSuccess)
	//{
	//	//std::cout << "cudaGraphicsGLRegisterImage: " << err << "Line: " << __LINE__;
	//	//return -1;
	//}
	//告诉CUDA运行时映射这个共享资源
	//cudaGraphicsMapResources(2, cudaResource, 0);
}

Nv12FBORender::~Nv12FBORender()
{
	glDeleteFramebuffers(2, ids);
	glDeleteTextures(2, ids);
}

void Nv12FBORender::render(uchar* nv12Ptry, uchar* nv12Ptruv, int w, int h)
{
	//cudaGraphicsSubResourceGetMappedArray(&devArray[0], *cudaResource, 0, 0);
	//cudaMemcpyToArray(devArray[0], 0, 0, (void*)nv12Ptry, w * h * sizeof(uchar), cudaMemcpyDeviceToDevice);
	//cudaGraphicsUnmapResources(1, cudaResource, 0);

	//cudaGraphicsSubResourceGetMappedArray(&devArray[1], *cudaResource1, 0, 0);
	//cudaMemcpyToArray(devArray[1], 0, 0, (void*)nv12Ptruv, w * h * sizeof(uchar) / 2, cudaMemcpyDeviceToDevice);
	//cudaGraphicsUnmapResources(1, cudaResource1, 0);

	if (!nv12Ptry || !nv12Ptruv)
		return;
	GLenum tex_buffers[] =
	{
		GL_COLOR_ATTACHMENT0,
		GL_COLOR_ATTACHMENT1 }; // ,
	//	GL_COLOR_ATTACHMENT2,
	//	GL_COLOR_ATTACHMENT3,
	//	GL_COLOR_ATTACHMENT4,
	//	GL_COLOR_ATTACHMENT5,
	//	GL_COLOR_ATTACHMENT6,
	//	GL_COLOR_ATTACHMENT7,
	//	GL_COLOR_ATTACHMENT8,
	//	GL_COLOR_ATTACHMENT9,
	//	GL_COLOR_ATTACHMENT10,
	//	GL_COLOR_ATTACHMENT11,
	//	GL_COLOR_ATTACHMENT12,
	//	GL_COLOR_ATTACHMENT13,
	//	GL_COLOR_ATTACHMENT14,
	//	GL_COLOR_ATTACHMENT15
	//};

	glClearColor(0.5f, 0.5f, 0.7f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glDisable(GL_DEPTH_TEST);

	glEnable(GL_SCISSOR_TEST);

	program.bind();
	vbo.bind();
	program.enableAttributeArray("vertexIn");
	program.enableAttributeArray("textureIn");
	program.setAttributeBuffer("vertexIn", GL_FLOAT, 0, 2, 2 * sizeof(GLfloat));
	program.setAttributeBuffer("textureIn", GL_FLOAT, 2 * 4 * sizeof(GLfloat), 2, 2 * sizeof(GLfloat));

	//fbo
	///////////	glBindFramebuffer(GL_FRAMEBUFFER, idY);
	////把2D纹理图像关联到FBO   第一个参数必须是GL_FRAMEBUFFER 第二个参数是关联纹理图像的关联点
	////第三个参数一般都是GL_TEXTURE_2D,第四个参数是纹理对象的ID号,最后一个参数是要被关联的纹理的mipmap等级
//	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, idY, 0);
	//fbo end

	glActiveTexture(GL_TEXTURE0 + 1);
	glBindTexture(GL_TEXTURE_2D, idY);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, w, h, 0, GL_RED, GL_UNSIGNED_BYTE, nv12Ptry);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
//	glFramebufferTexture2D(GL_FRAMEBUFFER, tex_buffers[GL_COLOR_ATTACHMENT0], GL_TEXTURE_2D, ids[0], 0);

	////fbo   !!!!! 使用fbo时 如果在同一个父窗体下  且不是同时生成的子窗体  会出现后面
	///////////	glBindFramebuffer(GL_FRAMEBUFFER, idUV);
//	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, idUV, 0);
	////fbo end
	glActiveTexture(GL_TEXTURE0 + 0);
	glBindTexture(GL_TEXTURE_2D, idUV);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RG, w >> 1, h >> 1, 0, GL_RG, GL_UNSIGNED_BYTE, nv12Ptruv);

	//glTexSubImage2D(GL_TEXTURE_2D, 0, 100, 100, 300, 300, GL_RG, GL_UNSIGNED_BYTE, nv12Ptr + w * h);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	//glFramebufferTexture2D(GL_FRAMEBUFFER, tex_buffers[GL_COLOR_ATTACHMENT1], GL_TEXTURE_2D, ids[1], 0);

	program.setUniformValue("textureUV", 0);
	program.setUniformValue("textureY", 1);

	//glScissor(0, 0, w / 6, h / 6);
	//test
	//第一次绘制到fbo
	QOpenGLContext *ctx = QOpenGLContext::currentContext();
	if (!ctx)
		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	//	return;
	//QOpenGLExtraFunctions extensions(ctx);
	//// 设置目标渲染buffer
	//extensions.glDrawBuffers(2, tex_buffers);

	//glScissor(1920/6, 1080/6, 200, 200);
	//glViewport(0, 0, w / 2, h / 2);

	//第一次绘制到fbo
	//glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	
	//解除fbo绑定
///////////	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	//第二次绘制到屏幕

	glScissor(0, 0, w, h);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

	//glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

	glDisable(GL_SCISSOR_TEST);

	program.disableAttributeArray("vertexIn");
	program.disableAttributeArray("textureIn");
	program.release();

	glUseProgram(0);
}
