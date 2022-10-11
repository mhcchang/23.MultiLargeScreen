#ifndef NV12FBORENDER_H
#define NV12FBORENDER_H
#include <QOpenGLFunctions>
#include <QOpenGLExtraFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>
#include <cuda_gl_interop.h>
#include <QtGui/QOpenGLContext>
#include <QtGui/QOpenGLFramebufferObject>
class Nv12FBORender : public QOpenGLFunctions
{
public:
	Nv12FBORender() = default;
	Nv12FBORender(const Nv12FBORender&) = delete;
	~Nv12FBORender();
	void initialize();
	void render(uchar*nv12Ptry, uchar*nv12Ptruv, int w, int h);

	int m_nthreadid;
private:
	QOpenGLShaderProgram program;
	GLuint idY, idUV;
	QOpenGLBuffer vbo;

	GLuint ids[2];

	cudaGraphicsResource_t * cudaResource = 0;
	cudaGraphicsResource_t * cudaResource1 = 0;
	//GLuint textureID[2];
	cudaArray* devArray[2];

	void Nv12FBORender::createTextureSrc(GLuint *tex_screen, unsigned int size_x, unsigned int size_y);

	//QOpenGLFramebufferObject fbo;
	//QOpenGLFramebufferObjectFormat format;
};

#endif // NV12FBORENDER_H
