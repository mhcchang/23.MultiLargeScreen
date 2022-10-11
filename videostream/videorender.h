/*
 * @Author: your name
 * @Date: 2020-08-02 11:07:26
 * @LastEditTime: 2020-08-15 14:44:17
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: \vs_code\Nv12Render_Gpu\videorender.h
 */ 
#ifndef VIDEORENDER_H
#define VIDEORENDER_H

#ifdef linux
#  define VIDEORENDERSHARED_EXPORT __attribute__((visibility("default")))
#else
#  define VIDEORENDERSHARED_EXPORT __declspec(dllexport)
#endif

class VIDEORENDERSHARED_EXPORT VideoRender
{
public:
    virtual ~VideoRender(){}
    /**
	 * @description: 初始化opengl上下文，编译链接shader;如果是GPU直接与OOPENGL对接数据，则会分配GPU内存或注册资
	 * @param width 视频宽度
	 * @param height 视频高度
	 * @param horizontal 是否水平镜像
	 * @param vertical 是否垂直镜像
     */
    virtual void initialize(const int width, const int height, const bool horizontal = false, const bool vertical = false) = 0;
    /**
	 * @description: 渲染一帧数据
	 * @param buffer 设备指针
	 * @param width 帧宽度
	 * @param height 帧高度
     */
    virtual void render(unsigned char* buffer, const int width, const int height) = 0;
    /**
	 * @description: 渲染一帧分离在2个planr的nv12数据
	 * @param planr 2个平面地址的指针数组。按照默认格式排序0(Y分量)、1(UV分量)，2个平面
	 * @param line_size 二维图片的每行字节大小，2个平面，也是GPU内存的nPitch，顺序和planr保持一致
	 * @param width 视频帧宽度
	 * @param height 视频帧高度
     */
    virtual void render(unsigned char* planr[], int line_size[], const int width, const int height) = 0;
    /**
     * @description: 异步加载数据到纹理
     * @param buffer 连续内存地址
     * @param width 视频帧宽度
     * @param height 视频帧高度
     */
    virtual void upLoad(unsigned char* buffer, const int width, const int height) = 0;
    /**
     * @description: 异步加载数一个分散在多个planr的数据到纹理
     * @param planr 多个平面地址的指针数组。按照默认格式排序，如YUV为0(Y分量)、1(U分量)、2(V分量); NV12为0(Y分量)、1(UV分量)
     * @param line_size 二维图片的每行字节大小，也是GPU内存的nPitch
     * @param width 视频帧宽度
     * @param height 视频帧高度
     */
    virtual void upLoad(unsigned char* planr[], const int line_size[], const int width, const int height) = 0;
    /**
     * @description: 异步绘制纹理数据
     */
    virtual void draw() = 0;
};

extern "C"
{
    VIDEORENDERSHARED_EXPORT VideoRender* createRender(void *ctx);
}
#endif // VIDEORENDER_H
