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
	depthRgbMat = Mat::zeros( Size(640,480), CV_8UC3 );

	labelImg=cvCreateImage(cvSize(640, 480), IPL_DEPTH_LABEL, 1);
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

	for(i=1;i!=0xFFFF;++i) {
		hue = (1.0 / 0xFFFF) * i;
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
	record->getRgb(currentFrame, rgbMat);
	record->getDepth(currentFrame, depthMat);
	//
	//record->getBlobs(currentFrame, blobMat);
}

void GlView::paintGL() 
{
	if(record->getIsOpen()) {
		uint16_t* depth = (uint16_t*)(depthMat.data);
		int i;
		for(i=0;i!=640*480;++i) {
			 if( (depth[i] > limitDepthMin) && (depth[i] < limitDepthMax)) {
			 	depthRgbMat.data[i*3] = selectedColor.blue();
			 	depthRgbMat.data[i*3+1] = selectedColor.green();
			 	depthRgbMat.data[i*3+2] = selectedColor.red();
			 	//blobMat.data[i]=255;
			} else {
				depthRgbMat.data[i*3] = hsvLookup[depth[i]][0];
				depthRgbMat.data[i*3+1] = hsvLookup[depth[i]][1];
				depthRgbMat.data[i*3+2] = hsvLookup[depth[i]][2];
				//blobMat.data[i]=0;
			}
		}

		glBindTexture(GL_TEXTURE_2D, glRgbTex);
		glTexImage2D(GL_TEXTURE_2D, 0, 3, rgbMat.cols, rgbMat.rows, 0, GL_BGR, GL_UNSIGNED_BYTE, rgbMat.data);
		glBegin(GL_TRIANGLE_FAN);
		glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
		glTexCoord2f(0, 0); glVertex3f(0,0,0);
		glTexCoord2f(1, 0); glVertex3f(640,0,0);
		glTexCoord2f(1, 1); glVertex3f(640,480,0);
		glTexCoord2f(0, 1); glVertex3f(0,480,0);
		glEnd();

		glBindTexture(GL_TEXTURE_2D, glDepthTex);
		glTexImage2D(GL_TEXTURE_2D, 0, 3, depthRgbMat.cols, depthRgbMat.rows, 0, GL_BGR, GL_UNSIGNED_BYTE, depthRgbMat.data);
		glBegin(GL_TRIANGLE_FAN);
		glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
		glTexCoord2f(0, 0); glVertex3f(640,0,0);
		glTexCoord2f(1, 0); glVertex3f(1280,0,0);
		glTexCoord2f(1, 1); glVertex3f(1280,480,0);
		glTexCoord2f(0, 1); glVertex3f(640,480,0);
		glEnd();

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

void GlView::setLimitDepthMax(int depth)
{
	limitDepthMax = depth;
}

void GlView::setLimitDepthMin(int depth)
{
	limitDepthMin = depth;
}


void GlView::setSelectedColor(QColor color)
{
	selectedColor = color;
}

int GlView::openRecord(QString fileName)
{

	if(record->open(fileName.toStdString())) {
		//currentFrame = record->getFrame(0);
		currentFrame = record->getCurrentFrame();
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
	currentFrame = record->getFrame(frameIndex);
	retrieveImageData();
	this->updateGL();
}

void GlView::showPreviousFrame()
{
	currentFrame = record->getPreviousFrame();
	retrieveImageData();
	this->updateGL();
}

void GlView::showNextFrame()
{
	currentFrame = record->getNextFrame();
	retrieveImageData();
	this->updateGL();
}

