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

#include "FontRender.h"
#include "KinectRecord.h"


class GlView : public QGLWidget
 {
     Q_OBJECT        // must include this if you use Qt signals/slots

 public:
    GlView(QWidget *parent = 0);
    void setLimitDepthMin(int depth);
    void setLimitDepthMax(int depth);
    void setSelectedColor(QColor color);
    QVector<QColor*>* computeGoldenRatioColors(double startHue,double saturation, double value, int size);
    void computeHue();
    int openRecord(QString fileName);
    int getRecordLength() const { return record->getNumberOfFrames(); }
    void showFrame(int frameIndex);
    void showNextFrame();
    void showPreviousFrame();


 protected:
    void initializeGL();
    void resizeGL(int w, int h);
    void paintGL();
    //void mousePressEvent(QMouseEvent *event);
    //void mouseMoveEvent(QMouseEvent *event);
    //void keyPressEvent(QKeyEvent *event);


private:
    void retrieveImageData(void);
    uint8_t hsvLookup[0xFFFF][3];
    cv::Mat rgbMat;
    cv::Mat depthMat;
    cv::Mat depthRgbMat;
    cv::Mat blobMat;
    GLuint glDepthTex;
    GLuint glRgbTex;
    int limitDepthMin;
    int limitDepthMax;
    QColor selectedColor;
    cvb::CvTracks tracks;
    IplImage *labelImg;
    QVector<QColor*> *colorVector;
    FontRender *fontRender;
    KinectRecord *record;
    Frame *currentFrame;

private slots:

 };

 #endif