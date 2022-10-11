/*
 * @brief: qt5 opengl render video
 * @author:
 * @date:
 */

#ifdef _WIN32
#include <Windows.h>
#endif // WIN32
#include <QMutexLocker>
#include "nv12render_gpu.h"

inline bool check(int e, int iLine, const char *szFile) {
    if (e != 0) {
        qDebug() << "General error " << e << " at line " << iLine << " in file " << szFile;
        return false;
    }
    return true;
}

#define ck(call) check(call, __LINE__, __FILE__)

Nv12Render_Gpu::Nv12Render_Gpu(CUcontext ctx):
	context(ctx)
{
	qDebug() << "Nv12Render_Gpu::Nv12Render_Gpu context: " << reinterpret_cast<unsigned long long>(context);
	if (!context)
	{
		ck(cuInit(0));
		CUdevice cuDevice;
		ck(cuDeviceGet(&cuDevice, 0));
		char szDeviceName[80];
		ck(cuDeviceGetName(szDeviceName, sizeof(szDeviceName), cuDevice));
		qDebug() << "GPU in use: " << szDeviceName;
		ck(cuCtxCreate(&context, CU_CTX_SCHED_BLOCKING_SYNC, cuDevice));
		need_destroy_ = true;
	}
}

Nv12Render_Gpu::~Nv12Render_Gpu()
{
	qDebug() << "Nv12Render_Gpu::~Nv12Render_Gpu() in";
	ck(cuGraphicsUnregisterResource(cuda_ybuffer_resource));
	ck(cuGraphicsUnregisterResource(cuda_uvbuffer_resource));
	if (need_destroy_) {
		ck(cuCtxDestroy(context));
	    qDebug() << "Nv12Render_Gpu::~Nv12Render_Gpu() context destroy" << reinterpret_cast<unsigned long long>(context);
	}
	else {
		qDebug() << "Nv12Render_Gpu::~Nv12Render_Gpu() context from out";
	}
    vbo.destroy();
    glDeleteTextures(sizeof(textures) / sizeof(GLuint), textures);
    glDeleteBuffers(sizeof(tex_buffers)/sizeof(GLuint), tex_buffers);
	qDebug() << "Nv12Render_Gpu::~Nv12Render_Gpu() out";
}

Q_GLOBAL_STATIC(QMutex, initMutex)

void Nv12Render_Gpu::initialize(const int width, const int height, const bool horizontal, const bool vertical)
{
	initializeOpenGLFunctions();
	QMutexLocker initLock(initMutex());
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

	if (horizontal) {
		if (vertical) {
			GLfloat points[]{
				-1.0f, 1.0f,
				 1.0f, 1.0f,
				 1.0f, -1.0f,
				-1.0f, -1.0f,

				1.0f,1.0f,
				0.0f,1.0f,
				0.0f,0.0f,
				1.0f,0.0f
			};

			vbo.create();
			vbo.bind();
			vbo.allocate(points, sizeof(points));
		}
		else {
			GLfloat points[]{
				-1.0f, 1.0f,
				 1.0f, 1.0f,
				 1.0f, -1.0f,
				-1.0f, -1.0f,

				1.0f,0.0f,
				0.0f,0.0f,
				0.0f,1.0f,
				1.0f,1.0f
			};

			vbo.create();
			vbo.bind();
			vbo.allocate(points, sizeof(points));
		}
	}
	else {
		if (vertical) {
			GLfloat points[]{
				-1.0f, 1.0f,
				 1.0f, 1.0f,
				 1.0f, -1.0f,
				-1.0f, -1.0f,

				0.0f,1.0f,
				1.0f,1.0f,
				1.0f,0.0f,
				0.0f,0.0f
			};

			vbo.create();
			vbo.bind();
			vbo.allocate(points, sizeof(points));
		}
		else {
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
		}
	}
}

void Nv12Render_Gpu::initializeWH(const int width, const int height)
{
	GLuint id[2];
	glGenTextures(2, id);
	idY = id[0];
	idUV = id[1];
	std::copy(std::begin(id), std::end(id), std::begin(textures));

    glBindTexture(GL_TEXTURE_2D, idY);
    glTexImage2D(GL_TEXTURE_2D,0,GL_RED, width, height,0,GL_RED,GL_UNSIGNED_BYTE,nullptr);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_2D, 0);

    glBindTexture(GL_TEXTURE_2D,idUV);
    glTexImage2D(GL_TEXTURE_2D,0,GL_RG, width >> 1, height >> 1,0,GL_RG,GL_UNSIGNED_BYTE,nullptr);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_2D, 0);

    glGenBuffers(2, tex_buffers);
    ybuffer_id = tex_buffers[0];
    uvbuffer_id = tex_buffers[1];

    glBindBuffer(GL_PIXEL_UNPACK_BUFFER_ARB, ybuffer_id);
    glBufferData(GL_PIXEL_UNPACK_BUFFER_ARB, width * height * sizeof(char), nullptr, GL_STREAM_DRAW_ARB);
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER_ARB, 0);

    glBindBuffer(GL_PIXEL_UNPACK_BUFFER_ARB, uvbuffer_id);
    glBufferData(GL_PIXEL_UNPACK_BUFFER_ARB, width * height* sizeof(char) / 2, nullptr, GL_STREAM_DRAW_ARB);
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER_ARB, 0);

	ck(cuCtxSetCurrent(context));
	ck(cuGraphicsGLRegisterBuffer(&cuda_ybuffer_resource, ybuffer_id, CU_GRAPHICS_REGISTER_FLAGS_WRITE_DISCARD));
	ck(cuGraphicsGLRegisterBuffer(&cuda_uvbuffer_resource, uvbuffer_id, CU_GRAPHICS_REGISTER_FLAGS_WRITE_DISCARD));
}

void Nv12Render_Gpu::render(cvcam::Frame &frame)
{
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	glDepthMask(false);
	if (!frame.isValid())
	{
		return;
	}

	//只有在获得流之后才能知道宽高 
	if (m_width != frame.width() || m_height != frame.height())
	{
		m_width = frame.width();
		m_height = frame.height();
		initializeWH(frame.width(), frame.height());
	}

	
	//printf("frame: format[%d], width[%d], height[%d], linesize0[%d], linesize1[%d], data0[%p], data1[%p] \n",
	//	frame.format(), m_width, m_height, frame.linesize(0), frame.linesize(1), frame.data(0), frame.data(1));
	ck(cuCtxSetCurrent(context));
	CUdeviceptr d_ybuffer;
	size_t d_y_size;
	ck(cuGraphicsMapResources(1, &cuda_ybuffer_resource, 0));
	ck(cuGraphicsResourceGetMappedPointer(&d_ybuffer, &d_y_size, cuda_ybuffer_resource));
	CUDA_MEMCPY2D m = { 0 };
	m.srcMemoryType = CU_MEMORYTYPE_DEVICE;
	m.srcDevice = reinterpret_cast<CUdeviceptr>(frame.data(0));
	m.srcPitch = frame.linesize(0);
	m.dstMemoryType = CU_MEMORYTYPE_DEVICE;
	m.dstDevice = d_ybuffer;
	m.dstPitch = m_width;// d_y_size / frame.height();
	m.WidthInBytes = m_width; //frame.linesize(0);
	m.Height = frame.height();
	ck(cuMemcpy2DAsync(&m, 0));

	ck(cuGraphicsUnmapResources(1, &cuda_ybuffer_resource, 0));

	CUdeviceptr d_uvbuffer;
	size_t d_uv_size;
	ck(cuGraphicsMapResources(1, &cuda_uvbuffer_resource, 0));
	ck(cuGraphicsResourceGetMappedPointer(&d_uvbuffer, &d_uv_size, cuda_uvbuffer_resource));
	m.srcMemoryType = CU_MEMORYTYPE_DEVICE;
	m.srcDevice = reinterpret_cast<CUdeviceptr>(frame.data(1));
	m.srcPitch = frame.linesize(1);
	m.dstMemoryType = CU_MEMORYTYPE_DEVICE;
	m.dstDevice = d_uvbuffer;
	m.dstPitch = m_width; // d_uv_size / (frame.height() >> 1);
	m.WidthInBytes = m_width; //frame.linesize(1);
	m.Height = (frame.height() >> 1);
	ck(cuMemcpy2DAsync(&m, 0));
	ck(cuGraphicsUnmapResources(1, &cuda_uvbuffer_resource, 0));
	ck(cuCtxSetCurrent(nullptr));

	program.bind();
	vbo.bind();
	program.enableAttributeArray("vertexIn");
	program.enableAttributeArray("textureIn");
	program.setAttributeBuffer("vertexIn", GL_FLOAT, 0, 2, 2 * sizeof(GLfloat));
	program.setAttributeBuffer("textureIn", GL_FLOAT, 2 * 4 * sizeof(GLfloat), 2, 2 * sizeof(GLfloat));

	glActiveTexture(GL_TEXTURE0 + 1);
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER_ARB, ybuffer_id);
	glBindTexture(GL_TEXTURE_2D, idY);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, frame.width(), frame.height(), GL_RED, GL_UNSIGNED_BYTE, nullptr);
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER_ARB, 0);

	glActiveTexture(GL_TEXTURE0 + 0);
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER_ARB, uvbuffer_id);
	glBindTexture(GL_TEXTURE_2D, idUV);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, frame.width() >> 1, frame.height() >> 1, GL_RG, GL_UNSIGNED_BYTE, nullptr);
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER_ARB, 0);

	program.setUniformValue("textureY", 1);
	program.setUniformValue("textureUV", 0);

	glScissor(0, 0, frame.width(), frame.height());

	glDrawArrays(GL_QUADS, 0, 4);
	program.disableAttributeArray("vertexIn");
	program.disableAttributeArray("textureIn");
	vbo.release();
	program.release();
}

void Nv12Render_Gpu::render(unsigned char* nv12_y, unsigned char* nv12_uv, const int width, const int height)
{
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glDisable(GL_DEPTH_TEST); 
	glDisable(GL_CULL_FACE);
	glDepthMask(false);
	if (!nv12_y || !nv12_uv)
	{
		printf("!nv12_y || !nv12_uv \n");
		return;
	}
#if 0
	ck(cuCtxSetCurrent(context));
	CUdeviceptr d_ybuffer;
	size_t d_y_size;
	ck(cuGraphicsMapResources(1, &cuda_ybuffer_resource, 0));
	ck(cuGraphicsResourceGetMappedPointer(&d_ybuffer, &d_y_size, cuda_ybuffer_resource));
	CUDA_MEMCPY2D m = { 0 };
	m.srcMemoryType = CU_MEMORYTYPE_DEVICE;
	m.srcDevice = reinterpret_cast<CUdeviceptr>(nv12_y);
	m.srcPitch = width;
	m.dstMemoryType = CU_MEMORYTYPE_DEVICE;
	m.dstDevice = d_ybuffer;
	m.dstPitch = d_y_size / height;
	m.WidthInBytes = width;
	m.Height = height;
	ck(cuMemcpy2DAsync(&m, 0));
	ck(cuGraphicsUnmapResources(1, &cuda_ybuffer_resource, 0));

	CUdeviceptr d_uvbuffer;
	size_t d_uv_size;
	ck(cuGraphicsMapResources(1, &cuda_uvbuffer_resource, 0));
	ck(cuGraphicsResourceGetMappedPointer(&d_uvbuffer, &d_uv_size, cuda_uvbuffer_resource));
	m.srcMemoryType = CU_MEMORYTYPE_DEVICE;
	m.srcDevice = reinterpret_cast<CUdeviceptr>(nv12_uv);
	m.srcPitch = width;
	m.dstMemoryType = CU_MEMORYTYPE_DEVICE;
	m.dstDevice = d_uvbuffer;
	m.dstPitch = d_uv_size / (height >> 1);
	m.WidthInBytes = width;
	m.Height = (height >> 1);
	ck(cuMemcpy2DAsync(&m, 0));
	ck(cuGraphicsUnmapResources(1, &cuda_uvbuffer_resource, 0));
	ck(cuCtxSetCurrent(nullptr));
#endif
	program.bind();
	vbo.bind();
	program.enableAttributeArray("vertexIn");
	program.enableAttributeArray("textureIn");
	program.setAttributeBuffer("vertexIn", GL_FLOAT, 0, 2, 2 * sizeof(GLfloat));
	program.setAttributeBuffer("textureIn", GL_FLOAT, 2 * 4 * sizeof(GLfloat), 2, 2 * sizeof(GLfloat));

	glActiveTexture(GL_TEXTURE0 + 1);
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER_ARB, ybuffer_id);
	
	glBindTexture(GL_TEXTURE_2D, idY);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RED, GL_UNSIGNED_BYTE, nullptr);
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER_ARB, 0);

	glActiveTexture(GL_TEXTURE0 + 0);
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER_ARB, uvbuffer_id);
	glBindTexture(GL_TEXTURE_2D, idUV);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width >> 1, height >> 1, GL_RG, GL_UNSIGNED_BYTE, nullptr);
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER_ARB, 0);

	program.setUniformValue("textureY", 1);
	program.setUniformValue("textureUV", 0);
	glDrawArrays(GL_QUADS, 0, 4);
	program.disableAttributeArray("vertexIn");
	program.disableAttributeArray("textureIn");
	vbo.release();
	program.release();
}

void Nv12Render_Gpu::render(unsigned char* nv12_dPtr, const int width, const int height)
{    
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glDisable(GL_DEPTH_TEST); 
	glDisable(GL_CULL_FACE);
	glDepthMask(false);
    if(!nv12_dPtr)
    {
        return;
    }

    ck(cuCtxSetCurrent(context));
    CUdeviceptr d_ybuffer;
    size_t d_y_size;
    ck(cuGraphicsMapResources(1, &cuda_ybuffer_resource, 0));
    ck(cuGraphicsResourceGetMappedPointer(&d_ybuffer, &d_y_size, cuda_ybuffer_resource));
    CUDA_MEMCPY2D m = { 0 };
    m.srcMemoryType = CU_MEMORYTYPE_DEVICE;
    m.srcDevice = reinterpret_cast<CUdeviceptr>(nv12_dPtr);
    m.srcPitch = width;
    m.dstMemoryType = CU_MEMORYTYPE_DEVICE;
    m.dstDevice = d_ybuffer;
    m.dstPitch = d_y_size / height;
    m.WidthInBytes = width;
    m.Height = height;
    ck(cuMemcpy2DAsync(&m, 0));
    ck(cuGraphicsUnmapResources(1, &cuda_ybuffer_resource, 0));

    CUdeviceptr d_uvbuffer;
    size_t d_uv_size;
    ck(cuGraphicsMapResources(1, &cuda_uvbuffer_resource, 0));
    ck(cuGraphicsResourceGetMappedPointer(&d_uvbuffer, &d_uv_size, cuda_uvbuffer_resource));
    m.srcMemoryType = CU_MEMORYTYPE_DEVICE;
    m.srcDevice = reinterpret_cast<CUdeviceptr>(nv12_dPtr + width * height);
    m.srcPitch = width;
    m.dstMemoryType = CU_MEMORYTYPE_DEVICE;
    m.dstDevice = d_uvbuffer;
    m.dstPitch = d_uv_size / (height>>1);
    m.WidthInBytes = width;
    m.Height = (height>>1);
	ck(cuMemcpy2DAsync(&m, 0));
    ck(cuGraphicsUnmapResources(1, &cuda_uvbuffer_resource, 0));
	ck(cuCtxSetCurrent(nullptr));

    program.bind();
    vbo.bind();
    program.enableAttributeArray("vertexIn");
    program.enableAttributeArray("textureIn");
    program.setAttributeBuffer("vertexIn",GL_FLOAT, 0, 2, 2*sizeof(GLfloat));
    program.setAttributeBuffer("textureIn",GL_FLOAT,2 * 4 * sizeof(GLfloat),2,2*sizeof(GLfloat));

    glActiveTexture(GL_TEXTURE0 + 1);
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER_ARB, ybuffer_id);
    glBindTexture(GL_TEXTURE_2D,idY);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RED, GL_UNSIGNED_BYTE, nullptr);
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER_ARB, 0);

    glActiveTexture(GL_TEXTURE0 + 0);
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER_ARB, uvbuffer_id);
    glBindTexture(GL_TEXTURE_2D,idUV);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width >> 1,height >> 1, GL_RG, GL_UNSIGNED_BYTE, nullptr);
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER_ARB, 0);

    program.setUniformValue("textureY",1);
    program.setUniformValue("textureUV",0);
    glDrawArrays(GL_QUADS,0,4);
    program.disableAttributeArray("vertexIn");
    program.disableAttributeArray("textureIn");
    vbo.release();
    program.release();
}

void Nv12Render_Gpu::render(unsigned char* planr[], int line_size[], const int width, const int height)
{
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glDisable(GL_DEPTH_TEST);
    if(!planr)
    {
        return;
    }

    ck(cuCtxSetCurrent(context));
    CUdeviceptr d_ybuffer;
    size_t d_y_size;
    ck(cuGraphicsMapResources(1, &cuda_ybuffer_resource, 0));
    ck(cuGraphicsResourceGetMappedPointer(&d_ybuffer, &d_y_size, cuda_ybuffer_resource));
    CUDA_MEMCPY2D m = { 0 };
    m.srcMemoryType = CU_MEMORYTYPE_DEVICE;
    m.srcDevice = reinterpret_cast<CUdeviceptr>(planr[0]);
    m.srcPitch = line_size[0];
    m.dstMemoryType = CU_MEMORYTYPE_DEVICE;
    m.dstDevice = d_ybuffer;
    m.dstPitch = d_y_size / height;
    m.WidthInBytes = width;
    m.Height = height;
	ck(cuMemcpy2DAsync(&m, 0));
    ck(cuGraphicsUnmapResources(1, &cuda_ybuffer_resource, 0));

    CUdeviceptr d_uvbuffer;
    size_t d_uv_size;
    ck(cuGraphicsMapResources(1, &cuda_uvbuffer_resource, 0));
    ck(cuGraphicsResourceGetMappedPointer(&d_uvbuffer, &d_uv_size, cuda_uvbuffer_resource));
    m.srcMemoryType = CU_MEMORYTYPE_DEVICE;
    m.srcDevice = reinterpret_cast<CUdeviceptr>(planr[1]);
    m.srcPitch = line_size[1];
    m.dstMemoryType = CU_MEMORYTYPE_DEVICE;
    m.dstDevice = d_uvbuffer;
    m.dstPitch = d_uv_size / (height>>1);
    m.WidthInBytes = width;
    m.Height = (height>>1);
	ck(cuMemcpy2DAsync(&m, 0));
    ck(cuGraphicsUnmapResources(1, &cuda_uvbuffer_resource, 0));
	ck(cuCtxSetCurrent(nullptr));

    program.bind();
    vbo.bind();
    program.enableAttributeArray("vertexIn");
    program.enableAttributeArray("textureIn");
    program.setAttributeBuffer("vertexIn",GL_FLOAT, 0, 2, 2*sizeof(GLfloat));
    program.setAttributeBuffer("textureIn",GL_FLOAT,2 * 4 * sizeof(GLfloat),2,2*sizeof(GLfloat));

    glActiveTexture(GL_TEXTURE0 + 1);
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER_ARB, ybuffer_id);
    glBindTexture(GL_TEXTURE_2D,idY);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RED, GL_UNSIGNED_BYTE, nullptr);
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER_ARB, 0);

    glActiveTexture(GL_TEXTURE0 + 0);
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER_ARB, uvbuffer_id);
    glBindTexture(GL_TEXTURE_2D,idUV);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width >> 1,height >> 1, GL_RG, GL_UNSIGNED_BYTE, nullptr);
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER_ARB, 0);

    program.setUniformValue("textureY",1);
    program.setUniformValue("textureUV",0);
    glDrawArrays(GL_QUADS,0,4);
    program.disableAttributeArray("vertexIn");
    program.disableAttributeArray("textureIn");
    vbo.release();
    program.release();
}

void Nv12Render_Gpu::upLoad(unsigned char* nv12_dPtr, const int width, const int height)
{
	if (!nv12_dPtr)
	{
		return;
	}
	QMutexLocker lock(&mtx);
	ck(cuCtxSetCurrent(context));
	CUdeviceptr d_ybuffer;
	size_t d_y_size;
	ck(cuGraphicsMapResources(1, &cuda_ybuffer_resource, 0));
	ck(cuGraphicsResourceGetMappedPointer(&d_ybuffer, &d_y_size, cuda_ybuffer_resource));
	CUDA_MEMCPY2D m = { 0 };
	m.srcMemoryType = CU_MEMORYTYPE_DEVICE;
	m.srcDevice = reinterpret_cast<CUdeviceptr>(nv12_dPtr);
	m.srcPitch = width;
	m.dstMemoryType = CU_MEMORYTYPE_DEVICE;
	m.dstDevice = d_ybuffer;
	m.dstPitch = d_y_size / height;
	m.WidthInBytes = width;
	m.Height = height;
	ck(cuMemcpy2DAsync(&m, 0));
	ck(cuGraphicsUnmapResources(1, &cuda_ybuffer_resource, 0));

	CUdeviceptr d_uvbuffer;
	size_t d_uv_size;
	ck(cuGraphicsMapResources(1, &cuda_uvbuffer_resource, 0));
	ck(cuGraphicsResourceGetMappedPointer(&d_uvbuffer, &d_uv_size, cuda_uvbuffer_resource));
	m.srcMemoryType = CU_MEMORYTYPE_DEVICE;
	m.srcDevice = reinterpret_cast<CUdeviceptr>(nv12_dPtr + width * height);
	m.srcPitch = width;
	m.dstMemoryType = CU_MEMORYTYPE_DEVICE;
	m.dstDevice = d_uvbuffer;
	m.dstPitch = d_uv_size / (height >> 1);
	m.WidthInBytes = width;
	m.Height = (height >> 1);
	ck(cuMemcpy2DAsync(&m, 0));
	ck(cuGraphicsUnmapResources(1, &cuda_uvbuffer_resource, 0));
	ck(cuCtxSetCurrent(nullptr));

	glActiveTexture(GL_TEXTURE0 + 1);
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER_ARB, ybuffer_id);
	glBindTexture(GL_TEXTURE_2D, idY);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RED, GL_UNSIGNED_BYTE, nullptr);
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER_ARB, 0);

	glActiveTexture(GL_TEXTURE0 + 0);
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER_ARB, uvbuffer_id);
	glBindTexture(GL_TEXTURE_2D, idUV);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width >> 1, height >> 1, GL_RG, GL_UNSIGNED_BYTE, nullptr);
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER_ARB, 0);
}

void Nv12Render_Gpu::upLoad(unsigned char* planr[], const int line_size[], const int width, const int height)
{
	if (!planr)
	{
		return;
	}
	QMutexLocker lock(&mtx);
	ck(cuCtxSetCurrent(context));
	CUdeviceptr d_ybuffer;
	size_t d_y_size;
	ck(cuGraphicsMapResources(1, &cuda_ybuffer_resource, 0));
	ck(cuGraphicsResourceGetMappedPointer(&d_ybuffer, &d_y_size, cuda_ybuffer_resource));
	CUDA_MEMCPY2D m = { 0 };
	m.srcMemoryType = CU_MEMORYTYPE_DEVICE;
	m.srcDevice = reinterpret_cast<CUdeviceptr>(planr[0]);
	m.srcPitch = line_size[0];
	m.dstMemoryType = CU_MEMORYTYPE_DEVICE;
	m.dstDevice = d_ybuffer;
	m.dstPitch = d_y_size / height;
	m.WidthInBytes = width;
	m.Height = height;
	ck(cuMemcpy2DAsync(&m, 0));
	ck(cuGraphicsUnmapResources(1, &cuda_ybuffer_resource, 0));

	CUdeviceptr d_uvbuffer;
	size_t d_uv_size;
	ck(cuGraphicsMapResources(1, &cuda_uvbuffer_resource, 0));
	ck(cuGraphicsResourceGetMappedPointer(&d_uvbuffer, &d_uv_size, cuda_uvbuffer_resource));
	m.srcMemoryType = CU_MEMORYTYPE_DEVICE;
	m.srcDevice = reinterpret_cast<CUdeviceptr>(planr[1]);
	m.srcPitch = line_size[1];
	m.dstMemoryType = CU_MEMORYTYPE_DEVICE;
	m.dstDevice = d_uvbuffer;
	m.dstPitch = d_uv_size / (height >> 1);
	m.WidthInBytes = width;
	m.Height = (height >> 1);
	ck(cuMemcpy2DAsync(&m, 0));
	ck(cuGraphicsUnmapResources(1, &cuda_uvbuffer_resource, 0));
	ck(cuCtxSetCurrent(nullptr));

	glActiveTexture(GL_TEXTURE0 + 1);
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER_ARB, ybuffer_id);
	glBindTexture(GL_TEXTURE_2D, idY);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RED, GL_UNSIGNED_BYTE, nullptr);
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER_ARB, 0);

	glActiveTexture(GL_TEXTURE0 + 0);
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER_ARB, uvbuffer_id);
	glBindTexture(GL_TEXTURE_2D, idUV);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width >> 1, height >> 1, GL_RG, GL_UNSIGNED_BYTE, nullptr);
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER_ARB, 0);
}

void Nv12Render_Gpu::draw()
{
	QMutexLocker lock(&mtx);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	glDepthMask(false);

	program.bind();
	vbo.bind();
	program.enableAttributeArray("vertexIn");
	program.enableAttributeArray("textureIn");
	program.setAttributeBuffer("vertexIn", GL_FLOAT, 0, 2, 2 * sizeof(GLfloat));
	program.setAttributeBuffer("textureIn", GL_FLOAT, 2 * 4 * sizeof(GLfloat), 2, 2 * sizeof(GLfloat));

	glActiveTexture(GL_TEXTURE0 + 1);
	glBindTexture(GL_TEXTURE_2D, idY);

	glActiveTexture(GL_TEXTURE0 + 0);
	glBindTexture(GL_TEXTURE_2D, idUV);

	program.setUniformValue("textureY", 1);
	program.setUniformValue("textureUV", 0);
	glDrawArrays(GL_QUADS, 0, 4);
	program.disableAttributeArray("vertexIn");
	program.disableAttributeArray("textureIn");
	vbo.release();
	program.release();
}

VideoRender* createRender(void *ctx)
{
    return new Nv12Render_Gpu((CUcontext)ctx);
}
