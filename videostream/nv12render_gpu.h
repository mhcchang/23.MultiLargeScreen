/*
 * @brief: qt5 opengl render video
 * @author:
 * @date:
 */

#pragma once
#include <QOpenGLShaderProgram>
#include <QOpenGLFunctions>
#include <QOpenGLBuffer>
#include <QOpenGLTexture>
#include <QMutex>
#include <cuda.h>
#include <cudaGL.h>
#include "videorender.h"

#include "streamsdk.h"

class Nv12Render_Gpu : public QOpenGLFunctions, public VideoRender
{
public:
    Nv12Render_Gpu(CUcontext ctx);
    ~Nv12Render_Gpu();
    /**
     * @description: 初始化渲染器
     * @param width 视频宽度
     * @param height 视频宽度
     * @param horizontal 是否水平镜像
     * @param vertical 是否垂直镜像
     */
    void initialize(const int width, const int height, const bool horizontal = false, const bool vertical = false) override;

	void initializeWH(const int width, const int height);

	void render(cvcam::Frame &frame);
    /**
     * @description: 渲染一帧数据
     * @param buffer 设备指针
     * @param width 帧宽度
     * @param height 帧高度
     */
    void render(unsigned char* nv12_y, unsigned char* nv12_uv, const int width, const int height);
    /**
     * @description: 渲染一帧分离在2个planr的nv12数据
     * @param planr 2个平面地址的指针数组。按照默认格式排序0(Y分量)、1(UV分量)，2个平面
     * @param line_size 二维图片的每行字节大小，2个平面，也是GPU内存的nPitch，顺序和planr保持一致
     * @param width 视频帧宽度
     * @param height 视频帧高度
     */

	void render(unsigned char* nv12_dPtr, const int width, const int height) override;
	/**
	 * @description: 渲染一帧分离在2个planr的nv12数据
	 * @param planr 2个平面地址的指针数组。按照默认格式排序0(Y分量)、1(UV分量)，2个平面
	 * @param line_size 二维图片的每行字节大小，2个平面，也是GPU内存的nPitch，顺序和planr保持一致
	 * @param width 视频帧宽度
	 * @param height 视频帧高度
	 */

    void render(unsigned char* planr[], int line_size[], const int width, const int height) override;
	/**
	 * @description: 异步加载数据到纹理
	 * @param buffer 连续内存地址
	 * @param width 视频帧宽度
	 * @param height 视频帧高度
	 */
	void upLoad(unsigned char* nv12_dPtr, const int width, const int height) override;
	/**
	 * @description: 异步加载数一个分散在多个planr的数据到纹理
	 * @param planr 多个平面地址的指针数组。按照默认格式排序，如YUV为0(Y分量)、1(U分量)、2(V分量); NV12为0(Y分量)、1(UV分量)
	 * @param line_size 二维图片的每行字节大小，也是GPU内存的nPitch
	 * @param width 视频帧宽度
	 * @param height 视频帧高度
	 */
	void upLoad(unsigned char* planr[], const int line_size[], const int width, const int height) override;
	/**
	 * @description: 异步绘制纹理数据
	 */
	void draw() override;

private:
	QMutex mtx;
    CUgraphicsResource cuda_ybuffer_resource, cuda_uvbuffer_resource;
    CUcontext context = nullptr;
    QOpenGLShaderProgram program;
    GLuint idY,idUV, textures[2];
    GLuint ybuffer_id, uvbuffer_id, tex_buffers[2];
    QOpenGLBuffer vbo;
	bool need_destroy_ = false;

	int m_width = 0;
	int m_height = 0;
};
