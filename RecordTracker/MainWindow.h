#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "ui_mainwindow.h"
#include <QWheelEvent>


class MainWindow : public QMainWindow, private Ui::UiMainWindow
{
	Q_OBJECT
	public:
		MainWindow(QMainWindow *parent = 0);

	private:
		
		
	private slots:
		void actionOpenFolderTriggered();
		void previousFrameButtonClicked();
		void nextFrameButtonClicked();
		void onDepthMinSliderChange(int value);
		void onDepthMaxSliderChange(int value);
		void onFrameSliderChange(int value);
		void onColorWheelColorChange(QColor);
		void onFrameNumberValueChanged (int i);
};

#endif