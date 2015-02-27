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

struct Frames
{
	Frame rgbFrame;
	Frame depthFrame;
	cvb::CvBlobs blobs;
	cvb::CvTracks tracks;
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
	Frame getRgbFrameFromCurrenttime(double currenttime, std::vector<Frame*> rgbFrames );
	int open(std::string fileName);
	int close();
	Frames* getCurrentFrames(void);
	Frames* getNextFrames(void);
	Frames* getPreviousFrames(void);
	Frames* getFrames(int index);
	int getNumberOfFrames(void);
	int getDepth(Frames *f, cv::Mat& output);
	int getRgb(Frames *f, cv::Mat& output);
	int getBlobs(Frames *f, cv::Mat& output, cvb::CvBlobs& blobs);
	bool getIsOpen();
	int computeTracks();

	struct TrackingSettings trackingSettings;

protected:
	int currentFrame;
	int nbrOfFrames;
	std::vector<Frames*> frames;
	bool isOpen;
	std::string recordDirectory;
private:
	cv::Mat rgbMat;
	cv::Mat depthMat;
	cv::Mat blobMat;
};

#endif