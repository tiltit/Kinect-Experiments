#ifndef KINECTRECORD_H
#define KINECTRECORD_H

#include <vector>
#include <string>
#include <iostream>

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

#include <cvblob.h>

struct MiniTrack {
	uint16_t id;
	uint16_t minX;
	uint16_t maxX;
	uint16_t minY;
	uint16_t maxY;
	double centroidX;
	double centroidY;
	uint16_t depth;
};

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
	std::map<int, MiniTrack*> miniTracks;
};

struct TrackingSettings {
	uint16_t clipClose;
	uint16_t clipDistant;
	uint16_t blobFilterSmall;
	uint16_t blobFilterLarge;
	uint16_t blobDistanceFilter;
	uint16_t blobInactiveFilter;
	uint16_t blobActiveFilter;
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
	int getBlobs( Frames *f, cv::Mat& output );
	std::map<int, MiniTrack*> getTracks(Frames *f);
	bool getIsOpen();
	int computeTracks();
	std::vector<std::string> exportTracksToXml();
	static float convertToMeters(uint16_t rawData);

	struct TrackingSettings trackingSettings;

protected:
	void releaseMinitracks(std::map<int, MiniTrack*> &miniTracks);
	int loadPGM16BitBigEndian(std::string filename, cv::Mat& output);
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