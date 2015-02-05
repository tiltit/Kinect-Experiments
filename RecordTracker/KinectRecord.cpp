#include <fstream>
#include <sstream>
#include <iostream>
#include <boost/filesystem.hpp>
#include <boost/lexical_cast.hpp>

#include "KinectRecord.h"
#include "split.h"

using namespace std;
using namespace cv;


KinectRecord::KinectRecord()
{
	isOpen = false;
}

int KinectRecord::open(string fileName)
{
	cout.precision(15);
	boost::filesystem::path p(fileName); 
	recordDirectory = p.parent_path().generic_string();
	cout << recordDirectory << endl;

	string line;
	vector<string> txtData;

	ifstream recordFile(fileName.c_str());

	if(!recordFile)
    {
        std::cout<<"Error opening output file"<< std::endl;
        return -1;
    }

    while (getline(recordFile, line))
        txtData.push_back(line);

    recordFile.close();

    rgbFrames.clear();
    depthFrames.clear();

    vector<string> frameInfo;

    for(vector<string>::iterator it = txtData.begin();it != txtData.end(); ++it) {

    	frameInfo = split(*it, '-');
    	if (frameInfo.size() != 3) {
    		// Some very basic error checking
    		cout << "Corrupt file" << endl;
    		return -1;
    	}

    	if(frameInfo[0][0] == 'a')
    		continue;	// Don't care mutch for the accelerometer data

    	Frame *frame = new Frame;

    	frame->type = frameInfo[0][0];
    	frame->currenttime = boost::lexical_cast<double>(frameInfo[1]);

    	string timestamp = frameInfo[2];
    	timestamp.erase(timestamp.find('.'), timestamp.size() - 1);
		frame->timestamp = boost::lexical_cast<unsigned long>(timestamp);
		frame->fileName = recordDirectory + '/' + *it;

		cout << "Frame:" << frame->type << " " << frame->currenttime << " " << frame->fileName  << endl;
		switch(frame->type) {
			case 'r' :	rgbFrames.push_back(frame);break;
			case 'd' :	depthFrames.push_back(frame);break;
			default: break;
		}
    }

    cout << "RGB Frames:" << '\t' << rgbFrames.size() << endl;
    cout << "Depth Frames:" << '\t' << depthFrames.size() << endl;

    isOpen = true;
    currentFrame = 0;
	return 1;
}

int KinectRecord::close()
{
	rgbFrames.clear();
	depthFrames.clear();
	isOpen = false;
	return 0;
}

Frame* KinectRecord::getCurrentFrame(void)
{
	return depthFrames.at(currentFrame);
}

Frame* KinectRecord::getNextFrame(void)
{
	if(currentFrame < depthFrames.size()-1 )
		currentFrame++;
	return depthFrames.at(currentFrame);
}

Frame* KinectRecord::getPreviousFrame(void)
{
	if(currentFrame > 0 )
		currentFrame--;
	return depthFrames.at(currentFrame);
}

Frame* KinectRecord::getFrame(int index)
{
	currentFrame = index;
	return depthFrames.at(currentFrame);
}

int KinectRecord::getNumberOfFrames(void)
{
	return depthFrames.size();
}

int KinectRecord::getDepth(Frame *frame, Mat& output)
{

	// if(depthMat != NULL)
	// 	delete depthMat;
	// depthMat = new cv::Mat(Size(640,480),CV_16UC1);

 //    *depthMat = imread(frame->fileName, CV_LOAD_IMAGE_GRAYSCALE);
 //    return depthMat;

	Mat mat;
	mat = imread(frame->fileName.c_str(), CV_LOAD_IMAGE_ANYDEPTH | CV_LOAD_IMAGE_GRAYSCALE);
	mat.copyTo(output);
	return 1;
}

int KinectRecord::getRgb(Frame *frame, Mat& output)
{
	// if(rgbMat != NULL) {
	// 	delete rgbMat;
	// }
	// rgbMat = new cv::Mat(Size(640,480),CV_8UC3);
    vector<Frame*>::iterator it;
    Frame *rgbFrame;
    for(it = rgbFrames.begin();it!= rgbFrames.end();++it) {
    	rgbFrame = *it;
    	if(rgbFrame->currenttime > frame->currenttime)
    		break;
    }
    //rgbMat = imread(rgbFrame->fileName, CV_LOAD_IMAGE_COLOR);
    Mat mat;
    mat = imread(rgbFrame->fileName.c_str(), CV_LOAD_IMAGE_COLOR); 
    mat.copyTo(output);
    return 1;
}

bool KinectRecord::getIsOpen()
{
	return isOpen;
}