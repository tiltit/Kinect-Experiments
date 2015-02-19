#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "ui_mainwindow.h"
#include <QWheelEvent>
#include <QActionGroup>

class MainWindow : public QMainWindow, private Ui::UiMainWindow
{
	Q_OBJECT
	public:
		MainWindow(QMainWindow *parent = 0);

	protected:
		void closeEvent(QCloseEvent *event);

	private:
		void loadSettings();
		void saveSettings();

		QActionGroup *rightViewGroup;
		QActionGroup *leftViewGroup;
		
	private slots:
		void actionOpenFolderTriggered();
		void onDepthMinSliderChange(int value);
		void onDepthMaxSliderChange(int value);
		void onFrameSliderChange(int value);
		void onColorWheelColorChange(QColor);

		void onBlobFilterSmallSpinBoxValueChanged( int value );
		void onBlobFilterLargeSpinBoxValueChanged( int value );

		void onBlobDistanceFilterSliderChange( int value );

		void onBlobInactiveFilterSliderChange( int value );

		void onRightViewGroupTriggered( QAction * action);
		void onLeftViewGroupTriggered( QAction * action);
		void onComputeButtonClicked();
}; 

#endif