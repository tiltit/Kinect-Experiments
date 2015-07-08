#include <QApplication>
#include <QWidget>
#include <QGLWidget>
#include <GL/glu.h>
#include <QSizePolicy>
#include <QTimer>
#include <QColor>

#include <QDebug>

#include <cmath>

#include "glView.h"


using namespace cv;
using namespace cvb;

GlView::GlView(QWidget *parent)
	: QGLWidget(parent) 
{
	record = new KinectRecord();
	computeHue();
	colorVector = computeGoldenRatioColors(0.01,0.5,1,100);
	fontRender = new FontRender("freemono_regular_12.PNG","freemono_regular_12.xml");

	rgbMat = Mat::zeros( Size(640,480), CV_8UC3 );
	depthMat = Mat::zeros( Size(640,480), CV_16UC1 );
	blobMat = Mat::zeros( Size(640,480), CV_8UC1 );
	blobRgbMat = Mat::zeros( Size(640,480), CV_8UC3 );
	depthRgbMat = Mat::zeros( Size(640,480), CV_8UC3 );

	labelImg=cvCreateImage(cvSize(640, 480), IPL_DEPTH_LABEL, 1);

	blobsVisible = false;
	tracksVisibleLeft = false;
	tracksVisibleRight = false;
}

void GlView::viewBlobs(bool visible) 
{ 
	blobsVisible = visible; 
}


QVector<QColor*>* GlView::computeGoldenRatioColors(double startHue,double saturation, double value, int size) 
{
	int i;
	double h = startHue;
	QVector<QColor*> *v = new QVector<QColor*>;

	for(i=0;i!=size;++i) {
		QColor *color = new QColor(255,255,255);
		h = fmod( startHue + (0.618033988749895 * i), 1.0);
		color->setHsvF(h, saturation, value);
		v->push_back(color);
	}

	return v;
}

void GlView::computeHue() {
	int i;
	double hue;
	QColor color(255,255,255);

	for(i=1;i!=1022;++i) {

		//hue = 1.0 / 512 * (i%512);
		hue = 1.0 / 5 * (KinectRecord::convertToMeters(i));
		color.setHsvF(hue, 1.0, 1.0);

		hsvLookup[i][0] = color.blue();
		hsvLookup[i][1] = color.green();
		hsvLookup[i][2] = color.red();
	}
	hsvLookup[0xFFFF][0] = 0;
	hsvLookup[0xFFFF][1] = 0;
	hsvLookup[0xFFFF][2] = 0;
}

void GlView::initializeGL()
{
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClearDepth(1.0);
	glDepthFunc(GL_LESS);
	glDepthMask(GL_FALSE);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);
	glDisable(GL_ALPHA_TEST);
	glEnable(GL_TEXTURE_2D);
	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glShadeModel(GL_FLAT);

	glClearColor(.3, .3, .3, 0);
 	glClear(GL_COLOR_BUFFER_BIT);

	glGenTextures(1, &glDepthTex);
	glBindTexture(GL_TEXTURE_2D, glDepthTex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glGenTextures(1, &glRgbTex);
	glBindTexture(GL_TEXTURE_2D, glRgbTex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glEnable(GL_BLEND);
}

void GlView::resizeGL(int w, int h) 
{
	glViewport(0,0,w,h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho (0, 1280, 480, 0, -1.0f, 1.0f);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glEnable(GL_BLEND);
	resize(w,w/8*3);
}

void GlView::retrieveImageData()
{
	if( (leftDisplayMode == DisplayMode::VIDEO) || (rightDisplayMode == DisplayMode::VIDEO)) {
		record->getRgb(currentFrames, rgbMat);	
	}
	if( (leftDisplayMode == DisplayMode::DEPTH) || (rightDisplayMode == DisplayMode::DEPTH)) {
		record->getDepth(currentFrames, depthMat);
	}
	if((leftDisplayMode == DisplayMode::BLOBS) || (rightDisplayMode == DisplayMode::BLOBS)) {
		record->getBlobs(currentFrames, blobMat);
	}
}

void GlView::drawBlobs(int xOffset)
{
	glDisable(GL_TEXTURE_2D);

	for(CvBlobs::iterator it=currentFrames->blobs.begin(); it!=currentFrames->blobs.end(); ++it) {
		CvBlob *blob = (*it).second;
		glLineWidth(2);
		glColor3f(1.0, 1.0, 1.0);

		glBegin(GL_LINE_STRIP);

		glVertex2f(blob->minx + xOffset, blob->miny); 
		glVertex2f(blob->maxx + xOffset, blob->miny);
		glVertex2f(blob->maxx + xOffset, blob->maxy); 
		glVertex2f(blob->minx + xOffset, blob->maxy);
		glVertex2f(blob->minx + xOffset, blob->miny);

		glEnd();
		fontRender->setColor(QColor(0,255,255));
		fontRender->print(blob->minx + xOffset, blob->miny - 20, QString::number(blob->area));
	}

	glEnable(GL_TEXTURE_2D);
}

void GlView::drawTracks(int xOffset)
{
	QColor *color;
	glDisable(GL_TEXTURE_2D);
	//glDisable(GL_TEXTURE_2D);
	for(std::map<int, MiniTrack*>::iterator it=currentFrames->miniTracks.begin(); it!=currentFrames->miniTracks.end();++it) {
		MiniTrack *miniTrack = (*it).second;
		
		//qDebug() << miniTrack->centroidX;
		color = colorVector->at(miniTrack->id % 100);
		glLineWidth(2);
		glColor3f(color->redF(), color->greenF(), color->blueF());
		glBegin(GL_LINE_STRIP);
		glVertex2f(miniTrack->centroidX + xOffset, miniTrack->centroidY - 5); 
		glVertex2f(miniTrack->centroidX + xOffset, miniTrack->centroidY + 5); 
		glEnd();
		glBegin(GL_LINE_STRIP);
		glVertex2f(miniTrack->centroidX-5 + xOffset, miniTrack->centroidY); 
		glVertex2f(miniTrack->centroidX+5 + xOffset, miniTrack->centroidY); 
		glEnd();
		fontRender->setColor(QColor(255,0,255));
		fontRender->print(miniTrack->minX + xOffset + 5, miniTrack->minY + 5, QString::number(miniTrack->id));

	}
	glEnable(GL_TEXTURE_2D);
}

void GlView::paintGL() 
{
	if(record->getIsOpen()) {

		if( (leftDisplayMode == DisplayMode::DEPTH) || (rightDisplayMode == DisplayMode::DEPTH)) {
			uint16_t* depth = (uint16_t*)(depthMat.data);
			for(int i=0;i!=640*480;++i) {
				// if( (depth[i] > limitDepthMin) && (depth[i] < limitDepthMax)) {
				if( (depth[i] > record->trackingSettings.clipClose) && (depth[i] < record->trackingSettings.clipDistant)) {
					// This is what is selected
					depthRgbMat.data[i*3] = selectedColor.blue();
				 	depthRgbMat.data[i*3+1] = selectedColor.green();
				 	depthRgbMat.data[i*3+2] = selectedColor.red();
				 	//blobMat.data[i]=255;
				} else {
					// This is what is cliped

					// depthRgbMat.data[i*3] = depth[i] >> 3;
					// depthRgbMat.data[i*3+1] = depth[i] >> 3;
					// depthRgbMat.data[i*3+2] = depth[i] >> 3;

					depthRgbMat.data[i*3] = hsvLookup[depth[i]][0];
					depthRgbMat.data[i*3+1] = hsvLookup[depth[i]][1];
					depthRgbMat.data[i*3+2] = hsvLookup[depth[i]][2];

				}
			}
		}

		if( (leftDisplayMode == DisplayMode::BLOBS) || (rightDisplayMode == DisplayMode::BLOBS) ) {
			uint8_t* blobData = (uint8_t*)(blobMat.data);
			for(int i=0;i!=640*480;++i) {
				if( blobData[i] > 0 ) {
					blobRgbMat.data[i*3] = selectedColor.blue();
				 	blobRgbMat.data[i*3+1] = selectedColor.green();
				 	blobRgbMat.data[i*3+2] = selectedColor.red();
				} else {
					blobRgbMat.data[i*3] = 0;
					blobRgbMat.data[i*3+1] = 0;
					blobRgbMat.data[i*3+2] = 0;
				}
			}
		}

		// Left View
		glBindTexture(GL_TEXTURE_2D, glRgbTex);
		
		//glTexImage2D(GL_TEXTURE_2D, 0, 3, rgbMat.cols, rgbMat.rows, 0, GL_BGR, GL_UNSIGNED_BYTE, rgbMat.data);
		switch(leftDisplayMode) {
			case DisplayMode::VIDEO:
				glTexImage2D(GL_TEXTURE_2D, 0, 3, rgbMat.cols, rgbMat.rows, 0, GL_BGR, GL_UNSIGNED_BYTE, rgbMat.data);
				break;
			case DisplayMode::DEPTH: 
				glTexImage2D(GL_TEXTURE_2D, 0, 3, depthRgbMat.cols, depthRgbMat.rows, 0, GL_BGR, GL_UNSIGNED_BYTE, depthRgbMat.data);
				break;
			case DisplayMode::BLOBS: 
				glTexImage2D(GL_TEXTURE_2D, 0, 3, blobRgbMat.cols, blobRgbMat.rows, 0, GL_BGR, GL_UNSIGNED_BYTE, blobRgbMat.data);
				break;
		}
		glBegin(GL_TRIANGLE_FAN);
		glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
		glTexCoord2f(0, 0); glVertex3f(0,0,0);
		glTexCoord2f(1, 0); glVertex3f(640,0,0);
		glTexCoord2f(1, 1); glVertex3f(640,480,0);
		glTexCoord2f(0, 1); glVertex3f(0,480,0);
		glEnd();

		if(leftDisplayMode == DisplayMode::BLOBS) {
			drawBlobs(0);
		}

		if(tracksVisibleLeft) {
			drawTracks(0);
		}

		// Right View
		glBindTexture(GL_TEXTURE_2D, glDepthTex);
		switch(rightDisplayMode) {
			case DisplayMode::VIDEO:
				glTexImage2D(GL_TEXTURE_2D, 0, 3, rgbMat.cols, rgbMat.rows, 0, GL_BGR, GL_UNSIGNED_BYTE, rgbMat.data);
				break;
			case DisplayMode::DEPTH: 
				glTexImage2D(GL_TEXTURE_2D, 0, 3, depthRgbMat.cols, depthRgbMat.rows, 0, GL_BGR, GL_UNSIGNED_BYTE, depthRgbMat.data);
				break;
			case DisplayMode::BLOBS: 
				glTexImage2D(GL_TEXTURE_2D, 0, 3, blobRgbMat.cols, blobRgbMat.rows, 0, GL_BGR, GL_UNSIGNED_BYTE, blobRgbMat.data);
				break;
		}
		//glTexImage2D(GL_TEXTURE_2D, 0, 3, depthRgbMat.cols, depthRgbMat.rows, 0, GL_BGR, GL_UNSIGNED_BYTE, depthRgbMat.data);
		glBegin(GL_TRIANGLE_FAN);
		glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
		glTexCoord2f(0, 0); glVertex3f(640,0,0);
		glTexCoord2f(1, 0); glVertex3f(1280,0,0);
		glTexCoord2f(1, 1); glVertex3f(1280,480,0);
		glTexCoord2f(0, 1); glVertex3f(640,480,0);
		glEnd();

		if(rightDisplayMode == DisplayMode::BLOBS) {
			drawBlobs(640);
		}

		if(tracksVisibleRight) {
			drawTracks(640);
		}

	}

	this->swapBuffers();
}

// void GlView::mousePressEvent(QMouseEvent *event) 
// {
// 	//event->ignore();
// }

// void GlView::mouseMoveEvent(QMouseEvent *event) 
// {
// 	qDebug() << "Mouse move:\t" << event->x() << " " << event->y();
// }

// void GlView::keyPressEvent(QKeyEvent* event) 
// {
// 	printf("key pressed: %d\n", event->key());
// 	switch(event->key()) {
// 	case Qt::Key_Escape:
// 		 QApplication::quit();
// 		break;
// 	default:
// 		event->ignore();
// 		break;
// 	}
// }

void GlView::setSelectedColor(QColor color)
{
	selectedColor = color;
}

int GlView::openRecord(QString fileName)
{

	if(record->open(fileName.toStdString())) {
		//currentFrame = record->getCurrentFrame();
		currentFrames = record->getCurrentFrames();
		retrieveImageData();
		qDebug() << "done loop";
		this->updateGL();
	} else {
		return -1;
	}
	return 0;
}

void GlView::showFrame(int frameIndex)
{
	currentFrames = record->getFrames(frameIndex);
	retrieveImageData();
	this->updateGL();
}

void GlView::refreshFrames()
{
	if( record->getIsOpen() && ((leftDisplayMode == DisplayMode::BLOBS) || (rightDisplayMode == DisplayMode::BLOBS)) ) {
		retrieveImageData();
	}
	this->updateGL();
}

void GlView::showPreviousFrame()
{
	currentFrames = record->getPreviousFrames();
	retrieveImageData();
	this->updateGL();
}

void GlView::showNextFrame()
{
	currentFrames = record->getNextFrames();
	retrieveImageData();
	this->updateGL();
}

void GlView::setLeftDisplayMode(DisplayMode source)
{
	leftDisplayMode = source;
}

void GlView::setRightDisplayMode(DisplayMode source)
{
	rightDisplayMode = source;
}

void GlView::setTracksVisibleLeft(bool visible)
{
	tracksVisibleLeft = visible;
}

void GlView::setTracksVisibleRight(bool visible)
{
	tracksVisibleRight = visible;
}
