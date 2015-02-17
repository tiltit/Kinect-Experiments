#ifndef KINECTRECORD_H
#define KINECTRECORD_H

#include <vector>
#include <string>
#include <iostream>

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

#include <cvblob.h>

struct Frame
{
	char type;
	double currenttime;
	unsigned long timestamp;
	std::string fileName;
};

struct TrackingSettings {
	uint16_t clipClose;
	uint16_t clipDistant;
	uint16_t blobFilterSmall;
	uint16_t blobFilterLarge;
	uint16_t blobDistanceFilter;
	uint16_t blobInactiveFilter;
};

class KinectRecord 
{
public:
	KinectRecord();
	int open(std::string fileName);
	int close();
	Frame* getCurrentFrame(void);
	Frame* getNextFrame(void);
	Frame* getPreviousFrame(void);
	Frame* getFrame(int index);
	int getNumberOfFrames(void);
	int getDepth(Frame *frame, cv::Mat& output);
	int getRgb(Frame *frame, cv::Mat& output);
	int getBlobs(Frame *frame, cv::Mat& output);
	bool getIsOpen();

	struct TrackingSettings trackingSettings;

protected:
	int currentFrame;
	int nbrOfFrames;
	std::vector<Frame*> rgbFrames;
	std::vector<Frame*> depthFrames;
	bool isOpen;
	std::string recordDirectory;
private:
	cv::Mat rgbMat;
	cv::Mat depthMat;
	cv::Mat blobMat;
};

#endif