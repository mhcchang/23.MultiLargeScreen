#ifndef __FFMPEGTHREAD_H__
#define __FFMPEGTHREAD_H__

#include <QThread>
#include <QString>
#include <QObject>

//½âÂë
extern "C"
{
	#include "libavcodec/avcodec.h"
	#include "libavformat/avformat.h"
	#include "libswscale/swscale.h"
	#include "libavdevice/avdevice.h"
}

class FfmpegThread : public QThread
{
	Q_OBJECT
public:
	explicit FfmpegThread(QObject *parent = 0);
	~FfmpegThread();
	
	virtual void run() Q_DECL_OVERRIDE;
signals:
	int showYuv(uchar *ptry, uchar *ptru, uchar *ptrv, int width, int height);
public:
	void setFilePath(QString szPath);
//	void setW(GLYuvWidget *w);
	void setUseGpuFlag(bool blUseGpu) { m_blUseGpu = blUseGpu; };

	void setStop();
private:
	QString m_szPath;
//	GLYuvWidget * m_w;
	volatile bool m_blStop;
	volatile bool m_blUseGpu;
	//void resultReady(int value);
};
#endif // __FFMPEGTHREAD_H__
