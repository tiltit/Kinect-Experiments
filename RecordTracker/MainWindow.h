#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "ui_mainwindow.h"
#include <QWheelEvent>

typedef struct TrackingSettings {
	uint16_t clipClose;
	uint16_t clipDistant;
	uint16_t blobFilterSmall;
	uint16_t blobFilterLarge;
	uint16_t blobDistanceFilter;
	uint16_t blobInactiveFilter;
};


class MainWindow : public QMainWindow, private Ui::UiMainWindow
{
	Q_OBJECT
	public:
		MainWindow(QMainWindow *parent = 0);
		int close();

	protected:
		void closeEvent(QCloseEvent *event);

	private:
		struct TrackingSettings trackingSettings;
		void loadSettings();
		void saveSettings();
		
	private slots:
		void actionOpenFolderTriggered();
		void previousFrameButtonClicked();
		void nextFrameButtonClicked();
		void onDepthMinSliderChange(int value);
		void onDepthMaxSliderChange(int value);
		void onFrameSliderChange(int value);
		void onColorWheelColorChange(QColor);
		void onFrameNumberValueChanged (int value);
		void onDepthMinSpinBoxValueChanged(int value);
		void onDepthMaxSpinBoxValueChanged(int value);

		void onBlobFilterSmallSpinBoxValueChanged( int value );
		void onBlobFilterLargeSpinBoxValueChanged( int value );

		void onBlobDistanceFilterSliderChange( int value );
		void onBlobInactiveFilterSliderChange( int value );

		void onBlobDistanceFilterSpinBoxValueChanged( int value );
		void onBlobInactiveFilterSpinBoxValueChanged( int value );
}; 

#endif