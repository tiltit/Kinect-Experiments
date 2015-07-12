#include <fstream>
#include <iostream>
#include <string>
#include <sstream>
#include <sys/stat.h>
#include <boost/filesystem.hpp>
#include <boost/lexical_cast.hpp>
#include <math.h>

#include "KinectRecord.h"
#include "split.h"

using namespace std;
using namespace cv;
using namespace cvb;

inline bool fileExists (const std::string& name) {
  struct stat buffer;   
  return (stat (name.c_str(), &buffer) == 0); 
}

// Convert Kinect data to meters, see:
// http://openkinect.org/wiki/Imaging_Information
float KinectRecord::convertToMeters(uint16_t rawData)
{
	return 0.1236 * tanf( (float)rawData / 2842.5 + 1.1863);
}

KinectRecord::KinectRecord()
{
	isOpen = false;
}

Frame KinectRecord::getRgbFrameFromCurrenttime( double currenttime, vector<Frame*> rgbFrames ) 
{
    Frame rgbFrame;
    for(vector<Frame*>::iterator it = rgbFrames.begin();it!= rgbFrames.end();++it) {
    	rgbFrame = **it;
    	if(rgbFrame.currenttime > currenttime)
    		break;
    }
    return rgbFrame;
}

int KinectRecord::open(string fileName)
{
	std::vector<Frame*> rgbFrames;
	std::vector<Frame*> depthFrames;
	cout.precision(15);
	boost::filesystem::path p(fileName); 
	recordDirectory = p.parent_path().generic_string();
	cout << recordDirectory << endl;

	string line;
	vector<string> txtData;

	ifstream recordFile(fileName.c_str());

	int cnt = 0;

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
    frames.clear();

    vector<string> frameInfo;
    
    for(vector<string>::iterator it = txtData.begin();it != txtData.end(); ++it) {

    	frameInfo = split(*it, '-');
    	if (frameInfo.size() != 3) {
    		// Some very basic error checking
    		cout << "Corrupt entry" << endl;
    		// return -1;
    	} else {

	    	if(frameInfo[0][0] == 'a')
				continue;	// Don't care mutch for the accelerometer data

			Frame *frame = new Frame;

			frame->type = frameInfo[0][0];
			frame->currenttime = boost::lexical_cast<double>(frameInfo[1]);

			string timestamp = frameInfo[2];
			timestamp.erase(timestamp.find('.'), timestamp.size() - 1);
			frame->timestamp = boost::lexical_cast<unsigned long>(timestamp);
			frame->fileName = recordDirectory + '/' + *it;
			
			
			//cout << "Frame:" << frame->type << " " << frame->currenttime << " " << frame->fileName  << endl;
			switch(frame->type) {
				case 'r' :	rgbFrames.push_back(frame);break;
				case 'd' :	depthFrames.push_back(frame);break;
				default: break;
			}
		}
		
    }

    cout << "RGB Frames:" << '\t' << rgbFrames.size() << endl;
    cout << "Depth Frames:" << '\t' << depthFrames.size() << endl;

	for(vector<Frame*>::iterator it = depthFrames.begin(); it != depthFrames.end(); ++it) {
		cout << "Frame nr:\t" << cnt++ << endl;
		if(fileExists((*it)->fileName)) {
			Frames *f = new Frames;
			f->depthFrame = **it;
			f->rgbFrame = getRgbFrameFromCurrenttime((*it)->currenttime, rgbFrames);
			frames.push_back(f);
			cout << "Depth:\t" << f->depthFrame.fileName << endl;
			cout << "Rgb:\t" << f->rgbFrame.fileName << endl;
		} else {
			cout << "File error!" << endl;
		}
	}

	// Small test
	// for(vector<Frames*>::iterator it = frames.begin(); it != frames.end(); ++it) {
	// 	cout << (*it)->rgbFrame.fileName << endl;
	// 	cout << (*it)->depthFrame.fileName << endl;
	// }

	rgbFrames.clear();
	depthFrames.clear();

    isOpen = true;
    currentFrame = 0;
	return 1;
}

int KinectRecord::close()
{
	frames.clear();
	isOpen = false;
	return 0;
}

Frames* KinectRecord::getCurrentFrames(void)
{
	return frames.at(currentFrame);
}

Frames* KinectRecord::getNextFrames(void)
{
	if( currentFrame < frames.size()-1 )
		currentFrame++;
	return frames.at(currentFrame);
}

Frames* KinectRecord::getPreviousFrames(void)
{
	if(currentFrame > 0 )
		currentFrame--;
	return frames.at(currentFrame);
}

Frames* KinectRecord::getFrames(int index)
{
	currentFrame = index;
	return frames.at(currentFrame);
}

int KinectRecord::getNumberOfFrames(void)
{
	return frames.size();
}

int KinectRecord::getDepth(Frames *f, Mat& output)
{
	Mat mat;
	mat = imread(f->depthFrame.fileName.c_str(), CV_LOAD_IMAGE_ANYDEPTH | CV_LOAD_IMAGE_GRAYSCALE);
	mat.copyTo(output);
	return 1;
}

int KinectRecord::getRgb(Frames *f, Mat& output)
{
    Mat mat;
    mat = imread(f->rgbFrame.fileName.c_str(), CV_LOAD_IMAGE_COLOR); 
    mat.copyTo(output);
    return 1;
}

int KinectRecord::getBlobs( Frames *f, Mat& output )
{
	Mat blobMat;
	blobMat = imread(f->depthFrame.fileName.c_str(), CV_LOAD_IMAGE_ANYDEPTH | CV_LOAD_IMAGE_GRAYSCALE);

	// Pity the that the folowing two line do not work...
	//threshold(blobMat, blobMat, trackingSettings.clipClose, trackingSettings.clipDistant, CV_THRESH_BINARY);
	//blobMat.convertTo(blobMat, CV_8U, 0.00390625);

	uint16_t* depth = (uint16_t*)(blobMat.data);
	for(int i=0;i!=640*480;++i) {
		if( (depth[i] > trackingSettings.clipClose) && (depth[i] < trackingSettings.clipDistant)) {
			depth[i] = 255;	// This is what is selected
		} else {
			depth[i] = 0; // This is not selected
		}
	}
	blobMat.convertTo(blobMat, CV_8U);	// This function clips the most significant byte unless the scale ratio is specified in the third argument. 

	IplImage *labelImg = cvCreateImage(cvSize(640, 480), IPL_DEPTH_LABEL, 1);

	IplImage *blobIpl = new IplImage(blobMat);
	//CvBlobs blobs;

	cvReleaseBlobs(f->blobs);

	unsigned int result=cvLabel(blobIpl, labelImg, f->blobs);
	cvFilterByArea(f->blobs, trackingSettings.blobFilterSmall, trackingSettings.blobFilterLarge);
	blobMat = cvarrToMat(blobIpl, true);
	blobMat.copyTo(output);
	//cvReleaseBlobs(f->blobs);
	//f->blobs = blobs;
	//cvReleaseBlobs(blobs);
	for (CvBlobs::const_iterator it=f->blobs.begin(); it!=f->blobs.end(); ++it) {
		cout << "Blob #" << it->second->label << ": Area=" << it->second->area << endl;
	}
	cout << endl;

	cvReleaseImage(&labelImg);
	//cvReleaseImage(& blobIpl);
	
	return result;
}

map<int, MiniTrack*> KinectRecord::getTracks(Frames *f)
{

}

bool KinectRecord::getIsOpen()
{
	return isOpen;
}

void KinectRecord::releaseMinitracks(map<int, MiniTrack*> &miniTracks)
{
	for (map<int, MiniTrack*>::iterator it=miniTracks.begin(); it!=miniTracks.end(); it++) {
		MiniTrack *miniTrack;
		miniTrack = (*it).second;
		delete miniTrack;
	}
	miniTracks.clear();
}

int KinectRecord::computeTracks()
{
	if(isOpen) {
		cout << "Computing tracks" << endl;
		CvTracks tracks;
		MiniTrack *miniTrack;
		for(vector<Frames*>::iterator it = frames.begin(); it != frames.end(); ++it) {
			Mat blobMat, blobMat8;

			blobMat = imread((*it)->depthFrame.fileName.c_str(), CV_LOAD_IMAGE_ANYDEPTH | CV_LOAD_IMAGE_GRAYSCALE);
			blobMat8 = Mat::zeros( Size(640,480), CV_8UC1 );

			uint16_t* depth = (uint16_t*)(blobMat.data);
			uint8_t* depth8 = (uint8_t*)(blobMat8.data);

			for(int i=0;i!=640*480;++i) {
				if( (depth[i] > trackingSettings.clipClose) && (depth[i] < trackingSettings.clipDistant)) {
					depth8[i] = 255;	// This is what is selected
				} else {
					depth8[i] = 0; // This is not selected
				}
			}

			IplImage *labelImg = cvCreateImage(cvSize(640, 480), IPL_DEPTH_LABEL, 1);

			IplImage *blobIpl = new IplImage(blobMat8);

			cvReleaseBlobs((*it)->blobs);

			unsigned int result=cvLabel(blobIpl, labelImg, (*it)->blobs);
			if(result) 
				cout << "cvLabel result:\t" << result << endl;

			cvFilterByArea((*it)->blobs, trackingSettings.blobFilterSmall, trackingSettings.blobFilterLarge);

			cvReleaseImage(&labelImg);

			cvUpdateTracks((*it)->blobs, tracks, (double)trackingSettings.blobDistanceFilter, trackingSettings.blobInactiveFilter, trackingSettings.blobActiveFilter);
			//cvUpdateTracks((*it)->blobs, tracks, (double)trackingSettings.blobDistanceFilter, trackingSettings.blobInactiveFilter);

			if((*it)->miniTracks.size() > 0) {
				releaseMinitracks((*it)->miniTracks);
			}

			for (CvTracks::iterator trackIt=tracks.begin(); trackIt!=tracks.end(); trackIt++)
			{
				CvTrack *track = (*trackIt).second;
				miniTrack = new MiniTrack;
				
				miniTrack->id = track->id;

				miniTrack->minX = track->minx;
				miniTrack->minY = track->miny;
				miniTrack->maxX = track->maxx;
				miniTrack->maxY = track->maxy;

				miniTrack->centroidX = track->centroid.x;
				miniTrack->centroidY = track->centroid.y;

				miniTrack->depth = depth[((int)track->centroid.y * 640) + (int)track->centroid.x];

				(*it)->miniTracks[track->label] = miniTrack;
			}
		}
		cvReleaseTracks(tracks);
		cout << "Finished computing tracks" << endl;
		return 1;
	} else {
		return 0;
	}
}

vector<string> KinectRecord::exportTracksToXml()
{
	stringstream ss;
	vector<string> ret;
	ss << "<?xml version=\"1.0\" encoding=\"utf-8\"?>";
	cout << ss << endl;
	ret.push_back(ss.str());
	ss.str(std::string());
	ss.clear();
	for(vector<Frames*>::iterator it = frames.begin(); it != frames.end(); ++it) {
		if((*it)->miniTracks.size() > 0) {	
			//cout << "<Frame currenttime=\"" << (*it)->depthFrame.currenttime << "\">" << endl;
			ss << "<Frame currenttime=\"" << setprecision(16) << (*it)->depthFrame.currenttime << "\">";
			cout << ss.str() << endl;
			ret.push_back(ss.str());
			ss.str(std::string());
			ss.clear();
			for(std::map<int, MiniTrack*>::iterator it_track=(*it)->miniTracks.begin(); it_track!=(*it)->miniTracks.end();++it_track) {
				MiniTrack *miniTrack = (*it_track).second;
				ss << setprecision(16) << "\t<Track id=\"" << miniTrack->id << "\" X=\"" << miniTrack->centroidX << "\" Y=\"" << miniTrack->centroidY << "\" Z=\"" << miniTrack->depth << "\" />";
				cout << ss.str() << endl;
				ret.push_back(ss.str());
				ss.str(std::string());
				ss.clear();
			}
			ss << "</Frame>";
			cout << ss.str() << endl;
			ret.push_back(ss.str());
			ss.str(std::string());
			ss.clear();
		}
	}
	return ret;
}
