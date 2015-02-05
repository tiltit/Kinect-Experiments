#ifndef CANVASDIALOG_H
#define CANVASDIALOG_H

#include <QWidget>
#include <QGLWidget>
#include <QLabel>
#include <QSlider>

#include "colorwheel.h"

#include "TrackWidget.h"

class MainDialog : public QWidget
{
	Q_OBJECT
	public:
		MainDialog();
	private:
		void saveSettings();
    	void loadSettings();
		TrackWidget *trackWidget;
		QLabel *depthStartLabel;
		QLabel *depthEndLabel;
		ColorWheel *colorWheel;
		QSlider *depthStartSlider;
		QSlider *depthEndSlider;

	private slots:
		void closing();
		void onDepthStartSliderChange(int value);
		void onDepthEndSliderChange(int value);
		void onColorWheelColorChange(const QColor color);
};

#endif
