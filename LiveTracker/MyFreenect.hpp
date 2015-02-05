#ifndef MYFREENECT_HPP
#define MYFREENECT_HPP

#include "libfreenect.hpp"
#include <iostream>
#include <vector>
#include <cmath>
#include <pthread.h>
// #include <cv.hpp>
// #include <cxcore.hpp>

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

class Mutex {
public:
	Mutex() {
		pthread_mutex_init( &m_mutex, NULL );
	}
	void lock() {
		pthread_mutex_lock( &m_mutex );
	}
	void unlock() {
		pthread_mutex_unlock( &m_mutex );
	}
private:
	pthread_mutex_t m_mutex;
};


class MyFreenect : public Freenect::FreenectDevice {
  public:
	MyFreenect(freenect_context *_ctx, int _index);

	void VideoCallback(void* _rgb, uint32_t timestamp);
	void DepthCallback(void* _depth, uint32_t timestamp);

	bool getVideo(cv::Mat& output);
	bool getDepth(cv::Mat& output);

  private:
	std::vector<uint8_t> m_buffer_depth;
	std::vector<uint8_t> m_buffer_rgb;
	std::vector<uint16_t> m_gamma;
	cv::Mat depthMat;
	cv::Mat rgbMat;
	cv::Mat ownMat;
	Mutex m_rgb_mutex;
	Mutex m_depth_mutex;
	bool m_new_rgb_frame;
	bool m_new_depth_frame;
	bool m_new_depth_frame_for_own;

	uint16_t startDepthLimit;
	uint16_t endDepthLimit;

	//CvTracks tracks;

};

#endif