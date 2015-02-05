#ifndef TRACKWIDGET_H
#define TRACKWIDGET_H

#include <QWidget>
#include <QGLWidget>
#include <QKeyEvent>
#include <QVector>
#include <QColor>

#include <core/core.hpp>
#include <imgproc/imgproc.hpp>
#include <highgui/highgui.hpp>


#include <cvblob.h>

#include "MyFreenect.hpp"
#include "FontRender.h"


class TrackWidget : public QGLWidget
 {
     Q_OBJECT        // must include this if you use Qt signals/slots

 public:
     TrackWidget(QWidget *parent = 0);
    int heightForWidth(int w);
    void setLimitDepthStart(int depth);
    void setLimitDepthEnd(int depth);
    void setSelectedColor(uint8_t r, uint8_t g, uint8_t b);
    QVector<QColor*>* computeGoldenRatioColors(double startHue,double saturation, double value, int size);
    void computeHue();
    MyFreenect *device;

 protected:

    void initializeGL();
    void resizeGL(int w, int h);
    void paintGL();
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void keyPressEvent(QKeyEvent *event);

private:
    
    uint8_t hsvLookup[2048][3];
    cv::Mat rgbMat;
    cv::Mat depthMat;
    cv::Mat depthRgbMat;
    cv::Mat blobMat;
    Freenect::Freenect freenect;
    GLuint glDepthTex;
    GLuint glRgbTex;
    int limitDepthStart;
    int limitDepthEnd;
    uint8_t selectedRed, selectedGreen, selectedBlue;
    cvb::CvTracks tracks;
    IplImage *labelImg;
    QVector<QColor*> *colorVector;
    FontRender *fontRender;



private slots:
        void onTic();
 };

 #endif