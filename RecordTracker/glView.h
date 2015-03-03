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


enum class Display { VIDEO, DEPTH, BLOBS };

class GlView : public QGLWidget
 {
     Q_OBJECT        // must include this if you use Qt signals/slots

 public:
    GlView(QWidget *parent = 0);
    void setSelectedColor(QColor color);
    QVector<QColor*>* computeGoldenRatioColors(double startHue,double saturation, double value, int size);
    void computeHue();
    int openRecord(QString fileName);
    int getRecordLength() const { return record->getNumberOfFrames(); }
    void showFrame(int frameIndex);
    void showNextFrame();
    void showPreviousFrame();
    void refreshFrames();
    void viewBlobs(bool visible);

    void setLeftDisplay(Display source);
    void setRightDisplay(Display source);
    void setTracksVisibleLeft(bool visible);
    void setTracksVisibleRight(bool visible);

    KinectRecord *record;


 protected:
    void initializeGL();
    void resizeGL(int w, int h);
    void drawBlobs(int xOffset);
    void drawTracks(int xOffset);
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
    cv::Mat blobRgbMat;
    cvb::CvBlobs blobs;
    GLuint glDepthTex;
    GLuint glRgbTex;
    QColor selectedColor;
    cvb::CvTracks tracks;
    IplImage *labelImg;
    QVector<QColor*> *colorVector;
    FontRender *fontRender;
    Frames *currentFrames;
    bool blobsVisible;
    Display leftDisplay;
    Display rightDisplay;
    bool tracksVisibleLeft;
    bool tracksVisibleRight;


private slots:

 };

 #endif