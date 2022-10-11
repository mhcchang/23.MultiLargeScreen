#include "FfmpegThread.h"

#include <QMessageBox>

#define MAX_WIDTH 1280
#define MAX_HIEGHT 720
unsigned char m_buf[3][MAX_WIDTH * MAX_HIEGHT];
#include "moc_FfmpegThread.cpp"

FfmpegThread::FfmpegThread(QObject *parent)
	: m_szPath(""),
	m_blUseGpu(true)
{
}

FfmpegThread::~FfmpegThread()
{
}

//QMainWindow* getMainWindow()
//{
//	//foreach(QWidget *w, qApp->topLevelWidgets())
//	//if (QMainWindow* mainWin = qobject_cast<QMainWindow*>(w))
//	//	return mainWin;
//	return nullptr;
//}

void FfmpegThread::run()
{
	if (m_szPath == "")
		return;

	setObjectName("FfmpegThread");
	AVFormatContext *pFormatCtx;
	av_register_all();//注册所有组件
	avformat_network_init();//初始化网络
	pFormatCtx = avformat_alloc_context();//初始化一个AVFormatContext
	char filepath[] = "C:\\ffmpeg5.8\\bin\\1.wmv";
	int             i, videoindex;
	AVCodecContext  *pCodecCtx;
	AVCodec         *pCodec;
	AVFrame *pFrame, *pFrameYUV;
	uint8_t *out_buffer;
	AVPacket *packet;
	int y_size;
	int ret, got_picture;
	struct SwsContext *img_convert_ctx;

	FILE *fp_yuv = fopen("output.yuv", "wb+");
	AVDictionary* options = NULL;
	av_dict_set(&options, "rtsp_transport", "tcp", 0);
	if (ret = avformat_open_input(&pFormatCtx, m_szPath.toLatin1().data(), NULL, &options) != 0)
	{//打开输入的视频文件
		char errbuf[1024] = { 0, };
		av_strerror(ret, errbuf, 1023);
		printf("Couldn't open input stream %s.\n", errbuf);
		return;
	}
	if (avformat_find_stream_info(pFormatCtx, NULL)<0){//获取视频文件信息
		printf("Couldn't find stream information.\n");
		return;
	}
	videoindex = -1;
	for (i = 0; i<pFormatCtx->nb_streams; i++)
	if (pFormatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO){
		videoindex = i;
		break;
	}

	if (videoindex == -1){
		printf("Didn't find a video stream.\n");
		return;
	}

	pCodecCtx = pFormatCtx->streams[videoindex]->codec;
	pCodec = avcodec_find_decoder(pCodecCtx->codec_id);//查找解码器
	if (pCodec == NULL){
		printf("Codec not found.\n");
		return;
	}

	if (avcodec_open2(pCodecCtx, pCodec, NULL)<0){//打开解码器
		printf("Could not open codec.\n");
		return;
	}

	pFrame = av_frame_alloc();
	pFrameYUV = av_frame_alloc();
	out_buffer = (uint8_t *)av_malloc(avpicture_get_size(AV_PIX_FMT_YUV420P, pCodecCtx->width, pCodecCtx->height));
	avpicture_fill((AVPicture *)pFrameYUV, out_buffer, AV_PIX_FMT_YUV420P, pCodecCtx->width, pCodecCtx->height);
	packet = (AVPacket *)av_malloc(sizeof(AVPacket));
	//Output Info-----------------------------
	printf("--------------- File Information ----------------\n");
	av_dump_format(pFormatCtx, 0, filepath, 0);
	printf("-------------------------------------------------\n");
	img_convert_ctx = sws_getContext(pCodecCtx->width, pCodecCtx->height, pCodecCtx->pix_fmt,
		pCodecCtx->width, pCodecCtx->height, AV_PIX_FMT_YUV420P, SWS_BICUBIC, NULL, NULL, NULL);

//#define __USE_GPU__
//#ifdef __USE_GPU__
	//nvenc begin
	AVCodec *codec;
	AVCodecContext *c = NULL;
	int numBytes = 0;
	if (m_blUseGpu)
	{
		// find the video encoder //
		//codec = avcodec_find_encoder_by_name("hevc_nvenc");
		codec = avcodec_find_decoder_by_name("h264_cuvid");
		//mhchang！！！！
		//codec = avcodec_find_encoder_by_name("h264_cuvid");
		//codec = avcodec_find_encoder(AV_CODEC_ID_H265);
		//codec = avcodec_find_encoder_by_name("h264_nvenc");
		//codec = avcodec_find_encoder(AV_CODEC_ID_H264);
		if (!codec) {
			fprintf(stderr, "Codec not found\n");
			exit(1);
		}

		c = avcodec_alloc_context3(codec);
		if (!c) {
			fprintf(stderr, "Could not allocate video codec context\n");
			exit(1);
		}

		/* put sample parameters */
		c->bit_rate = pCodecCtx->bit_rate;
		/* resolution must be a multiple of two */
		c->width = pCodecCtx->width;
		c->height = pCodecCtx->height;
		c->time_base = pCodecCtx->time_base;
		c->gop_size = pCodecCtx->gop_size;

		c->pix_fmt = AV_PIX_FMT_NV12; // AV_PIX_FMT_YUV420P;
		c->max_b_frames = 0;

		AVDictionary *param = 0;
		//H.264
		if (codec->id == AV_CODEC_ID_H264) {
			av_dict_set(&param, "preset", "medium", 0);
			av_dict_set(&param, "tune", "zerolatency", 0);
		}

		/* open it */
		if (avcodec_open2(c, codec, &param) < 0) {
			fprintf(stderr, "Could not open codec\n");
			system("pause");
			exit(1);
		}

		//nvenc end
		int numBytes = avpicture_get_size(AV_PIX_FMT_NV12, pCodecCtx->width, pCodecCtx->height);
		out_buffer = (uint8_t*)av_realloc(out_buffer, numBytes * sizeof(uint8_t));
	}
//#endif
	int _num = 0;
	//关键帧
	int blNoKeyframe = 0;

	m_blStop = false;
	///nvenc cuvid
	while (!m_blStop && av_read_frame(pFormatCtx, packet) >= 0)
	{//读取一帧压缩数据
		if (packet->stream_index == videoindex)
		{
//#ifdef __USE_GPU__
			if(m_blUseGpu)
				ret = avcodec_decode_video2(c, pFrame, &got_picture, packet);//解码一帧压缩数据
			else
				ret = avcodec_decode_video2(pCodecCtx, pFrame, &got_picture, packet);//

//#else																		 
			//ret = avcodec_decode_video2(pCodecCtx, pFrame, &got_picture, packet);//
//#endif
			if (ret < 0)
			{
				printf("Decode Error.\n");
				return;
			}
			if (got_picture)
			{
				//sws_scale 使用opengl的gpu实现 此处不再使用
				//sws_scale(img_convert_ctx, (const uint8_t* const*)pFrame->data, pFrame->linesize, 0, pCodecCtx->height,
				//	pFrameYUV->data, pFrameYUV->linesize);
				
				y_size = pCodecCtx->width * pCodecCtx->height;
				_num++;
				QString sz;

				//emit 
				//AVPictureType pict_type;
					
				if (blNoKeyframe > 0 && pFrame->key_frame == 1)
				{
					sz.sprintf("%d", pFrame->pict_type);
					//QMessageBox::information(NULL, "Title", sz, QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
					//emit showYuv(pFrame->data[0], pFrame->data[1], pFrame->data[2], pCodecCtx->width, pCodecCtx->height);

					blNoKeyframe++;
					//break;
				}
				else
					;
//#ifdef __USE_GPU__
				if (m_blUseGpu)
				{
					if (AV_PIX_FMT_NV12 == c->pix_fmt)
					{
#define __DRAW_ALL__
#ifdef __DRAW_ALL__
						//全副输入到opengl 利用裁剪方式显示
						int bytes = 0;

						for (int i = 0; i < pCodecCtx->height; i++) { //将y分量拷贝
							::memcpy(out_buffer + bytes, pFrame->data[0] + pFrame->linesize[0] * i, pCodecCtx->width);
							bytes += pCodecCtx->width;
						}
						int uv = pCodecCtx->height >> 1;
						for (int i = 0; i < uv; i++) { //将uv分量拷贝
							::memcpy(out_buffer + bytes, pFrame->data[1] + pFrame->linesize[1] * i, pCodecCtx->width);
							bytes += pCodecCtx->width;
						}
						emit showYuv(out_buffer, out_buffer + pCodecCtx->width * pCodecCtx->height, NULL, pCodecCtx->width, pCodecCtx->height);
#else
						//先裁剪不需要显示的部分， 然后再输入到opengl  memcpy效率会高一些  但显示效果可能差 gl渲染时需要边界外的数据 GL_LINEAR
						int bytes = 0;

						for (int i = 0; i < pCodecCtx->height / 3; i++) { //将y分量拷贝
							::memcpy(out_buffer + bytes, pFrame->data[0] + pFrame->linesize[0] * i + pCodecCtx->width / 3, pCodecCtx->width / 3);
							bytes += (pCodecCtx->width / 3);
						}
						int uv = (pCodecCtx->height / 3 >> 1);
						for (int i = 0; i < uv; i++) { //将uv分量拷贝
							::memcpy(out_buffer + bytes, pFrame->data[1] + pFrame->linesize[1] * i + pCodecCtx->width / 3, pCodecCtx->width / 3);
							bytes += (pCodecCtx->width / 3);
						}
						emit showYuv(out_buffer, NULL, NULL, pCodecCtx->width / 3, pCodecCtx->height / 3);
#endif
						//
					}
					else
						emit showYuv(pFrame->data[0], pFrame->data[1], pFrame->data[2], pCodecCtx->width, pCodecCtx->height);
				}
				else
					emit showYuv(pFrame->data[0], pFrame->data[1], pFrame->data[2], pCodecCtx->width, pCodecCtx->height);
//#endif
				Sleep(1);
					
				//break;
				if (pFrame->pict_type == 1 || pFrame->pict_type == 5)
				sz.sprintf("\n%02d\t", pFrame->pict_type);
				else
					sz.sprintf("%02d\t", pFrame->pict_type);

				printf("Succeed to decode 1 frame!\n");
				//return 0;
			}
		}
		
		Sleep(1);
		
		av_free_packet(packet);
	}
	//flush decoder
	//当av_read_frame()循环退出的时候，实际上解码器中可能还包含剩余的几帧数据。
	//因此需要通过“flush_decoder”将这几帧数据输出。
	//“flush_decoder”功能简而言之即直接调用avcodec_decode_video2()获得AVFrame，而不再向解码器传递AVPacket。//
	while (1) 
	{
		ret = avcodec_decode_video2(pCodecCtx, pFrame, &got_picture, packet);
		if (ret < 0)
			break;
		if (!got_picture)
			break;
		//sws_scale(img_convert_ctx, (const uint8_t* const*)pFrame->data, pFrame->linesize, 0, pCodecCtx->height,
		//	pFrameYUV->data, pFrameYUV->linesize);

		int y_size = pCodecCtx->width*pCodecCtx->height;
		//fwrite(pFrameYUV->data[0], 1, y_size, fp_yuv);    //Y 
		//fwrite(pFrameYUV->data[1], 1, y_size / 4, fp_yuv);  //U
		//fwrite(pFrameYUV->data[2], 1, y_size / 4, fp_yuv);  //V

		printf("Flush Decoder: Succeed to decode 1 frame!\n");
	}

	sws_freeContext(img_convert_ctx);

	//关闭文件以及释放内存
	fclose(fp_yuv);

	av_frame_free(&pFrameYUV);
	av_frame_free(&pFrame);
	avcodec_close(pCodecCtx);
	
	if (m_blUseGpu)
		avcodec_close(c);
	avformat_close_input(&pFormatCtx);

	return;
}

void FfmpegThread::setFilePath(QString szPath)
{
	m_szPath = szPath;
}

void FfmpegThread::setStop()
{
	m_blStop = true;
}
