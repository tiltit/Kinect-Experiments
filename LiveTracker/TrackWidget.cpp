#include <QApplication>
#include <QWidget>
#include <QGLWidget>
#include <GL/glu.h>
#include <QSizePolicy>
#include <QTimer>
#include <QColor>

#include <QDebug>

#include <cmath>

#include "TrackWidget.h"
#include "libfreenect.hpp"


using namespace cv;
using namespace cvb;

TrackWidget::TrackWidget(QWidget *parent)
	: QGLWidget(parent) 
{
	//QSizePolicy policy(QSizePolicy::Preferred, QSizePolicy::Preferred);
	//policy.setHeightForWidth(true);
	//this->setSizePolicy(policy);
	colorVector = computeGoldenRatioColors(0.01,0.5,1,100);
	fontRender = new FontRender("freemono_regular_12.PNG","freemono_regular_12.xml");


	rgbMat = Mat::zeros( Size(640,480), CV_8UC3 );
	depthMat = Mat::zeros( Size(640,480), CV_16UC1 );
	blobMat = Mat::zeros( Size(640,480), CV_8UC1 );
	depthRgbMat = Mat::zeros( Size(640,480), CV_8UC3 );

	labelImg=cvCreateImage(cvSize(640, 480), IPL_DEPTH_LABEL, 1);

	device = &freenect.createDevice<MyFreenect>(0);

	computeHue();

	device->startVideo();
	device->startDepth();

	QTimer *timer = new QTimer(this);

	connect(timer, SIGNAL(timeout()), this, SLOT(onTic()));
	timer->start(0);
}

QVector<QColor*>* TrackWidget::computeGoldenRatioColors(double startHue,double saturation, double value, int size) 
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

void TrackWidget::computeHue() {
	int i;
	double hue;
	QColor color(255,255,255);

	for(i=1;i!=2048;++i) {
		hue = 1.0 / 512 * (i%512);
		color.setHsvF(hue, 1.0, 1.0);

		hsvLookup[i][0] = color.blue();
		hsvLookup[i][1] = color.green();
		hsvLookup[i][2] = color.red();
	}
	hsvLookup[2047][0] = 0;
	hsvLookup[2047][1] = 0;
	hsvLookup[2047][2] = 0;

}

void TrackWidget::initializeGL()
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

void TrackWidget::resizeGL(int w, int h) 
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

void TrackWidget::paintGL() 
{
	glBindTexture(GL_TEXTURE_2D, glRgbTex);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, rgbMat.cols, rgbMat.rows, 0, GL_BGR, GL_UNSIGNED_BYTE, rgbMat.data);
	//glTexImage2D(GL_TEXTURE_2D, 0, 3, 640, 480, 0, GL_RGB, GL_UNSIGNED_BYTE, test);

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




	QColor *color;
	glDisable(GL_TEXTURE_2D);
	for (CvTracks::iterator it=tracks.begin(); it!=tracks.end(); it++)
    {
    	CvTrack *track = (*it).second;
    	color = colorVector->at(track->id % 100);
    	glLineWidth(2);
		
		glColor3f(color->redF(), color->greenF(), color->blueF());

    	glBegin(GL_LINE_STRIP);
    	
 		glVertex2f(track->minx, track->miny); 
 		glVertex2f(track->maxx, track->miny);
 		glVertex2f(track->maxx, track->maxy); 
 		glVertex2f(track->minx, track->maxy);
 		glVertex2f(track->minx, track->miny);
 		glEnd();
 		fontRender->setColor(QColor(255,255,0));
 		fontRender->print(track->minx + 5, track->miny + 5, QString::number(track->id));
    }
    glEnable(GL_TEXTURE_2D); 

	

	this->swapBuffers();
}

void TrackWidget::mousePressEvent(QMouseEvent *event) 
{
	event->ignore();
}

void TrackWidget::mouseMoveEvent(QMouseEvent *event) 
{
	qDebug() << "Mouse move:\t" << event->x() << " " << event->y();
}

void TrackWidget::keyPressEvent(QKeyEvent* event) 
{
	printf("key pressed: %d\n", event->key());
	switch(event->key()) {
	case Qt::Key_Escape:
		 QApplication::quit();
		break;
	default:
		event->ignore();
		break;
	}
}

int TrackWidget::heightForWidth(int w) 
{
	qDebug() << w;
	return w;
}

void TrackWidget::onTic() 
{
	int i;

	bool gotRgb = device->getVideo(rgbMat);
	bool gotDepth = device->getDepth(depthMat);

	if(gotDepth) {
		uint16_t* depth = (uint16_t*)(depthMat.data);
		for(i=0;i!=640*480;++i) {

			if( (depth[i] > limitDepthStart) && (depth[i] < limitDepthEnd)) {
				depthRgbMat.data[i*3] = selectedBlue;
				depthRgbMat.data[i*3+1] = selectedGreen;
				depthRgbMat.data[i*3+2] = selectedRed;
				blobMat.data[i]=255;
			} else {
				depthRgbMat.data[i*3] = hsvLookup[depth[i]][0];
				depthRgbMat.data[i*3+1] = hsvLookup[depth[i]][1];
				depthRgbMat.data[i*3+2] = hsvLookup[depth[i]][2];
				blobMat.data[i]=0;
			}
		}
		//cvThreshold(&depthMat, &blobMat, limitDepthStart, limitDepthEnd, CV_THRESH_BINARY);

		IplImage *blobIpl = new IplImage(blobMat);
		
		CvBlobs blobs;
		unsigned int result=cvLabel(blobIpl, labelImg, blobs);
		// Filter Blobs that are either to big or to small
		cvFilterByArea(blobs, 500, 50000);
		IplImage *rgbIpl = new IplImage(rgbMat);

		//cvRenderBlobs(labelImg, blobs, rgbIpl, rgbIpl);
		/// \fn cvUpdateTracks(CvBlobs const &b, CvTracks &t, const double thDistance, const unsigned int thInactive, const unsigned int thActive=0)
		cvUpdateTracks(blobs, tracks, 100., 5);
		//cvRenderTracks(tracks, rgbIpl, rgbIpl, CV_TRACK_RENDER_ID|CV_TRACK_RENDER_BOUNDING_BOX);

		cvReleaseBlobs(blobs);

	}

	if( gotRgb || gotDepth ) {
		this->updateGL();
	}
}


void TrackWidget::setLimitDepthStart(int depth)
{
	limitDepthStart = depth;
}

void TrackWidget::setLimitDepthEnd(int depth)
{
	limitDepthEnd = depth;
}


void TrackWidget::setSelectedColor(uint8_t r, uint8_t g, uint8_t b)
{
	selectedRed = r;
	selectedGreen = g;
	selectedBlue = b;
}