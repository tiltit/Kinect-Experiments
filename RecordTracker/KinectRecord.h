#ifndef KINECTRECORD_H
#define KINECTRECORD_H

#include <vector>
#include <string>
#include <iostream>

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

struct Frame
{
	char type;
	double currenttime;
	unsigned long timestamp;
	std::string fileName;
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
	bool getIsOpen();

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
};

#endif