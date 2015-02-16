#include "MainWindow.h"

#include <QFileDialog>
#include <QStringList>
#include <QString>
#include <QFileInfo>
#include <QSettings>



MainWindow::MainWindow(QMainWindow *parent)
	: QMainWindow(parent)
{
	setupUi(this);
	connect(actionOpen_Folder, SIGNAL(triggered()), this, SLOT(actionOpenFolderTriggered()));
	connect(previousFrameButton, SIGNAL(clicked()), this, SLOT(previousFrameButtonClicked()));
	connect(nextFrameButton, SIGNAL(clicked()), this, SLOT(nextFrameButtonClicked()));
	connect(depthMinSlider, SIGNAL(valueChanged(int)), this, SLOT(onDepthMinSliderChange(int)));
	connect(depthMaxSlider, SIGNAL(valueChanged(int)), this, SLOT(onDepthMaxSliderChange(int)));

	connect(frameSlider, SIGNAL(valueChanged(int)), this, SLOT(onFrameSliderChange(int)));

	connect(colorWheel, SIGNAL(colorChange(QColor)), this, SLOT(onColorWheelColorChange(QColor)));
	connect(frameNumberSpinBox, SIGNAL(valueChanged(int)), this, SLOT(onFrameNumberValueChanged(int)));
	connect(depthMinSpinBox, SIGNAL(valueChanged(int)), this, SLOT(onDepthMinSpinBoxValueChanged(int)));
	connect(depthMaxSpinBox, SIGNAL(valueChanged(int)), this, SLOT(onDepthMaxSpinBoxValueChanged(int)));

	connect(blobFilterSmallSpinBox, SIGNAL(valueChanged(int)), this, SLOT(onBlobFilterSmallSpinBoxValueChanged(int)));
	connect(blobFilterLargeSpinBox, SIGNAL(valueChanged(int)), this, SLOT(onBlobFilterLargeSpinBoxValueChanged(int)));

	connect(blobDistanceFilterSlider, SIGNAL(valueChanged(int)), this, SLOT(onBlobDistanceFilterSliderChange(int)));
	connect(blobInactiveFilterSlider, SIGNAL(valueChanged(int)), this, SLOT(onBlobInactiveFilterSliderChange(int)));

	connect(blobDistanceFilterSpinBox, SIGNAL(valueChanged(int)), this, SLOT(onBlobDistanceFilterSpinBoxValueChanged(int)));
	connect(blobInactiveFilterSpinBox, SIGNAL(valueChanged(int)), this, SLOT(onBlobInactiveFilterSpinBoxValueChanged(int)));


	loadSettings();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
	saveSettings();
	qDebug() << "Closing main window" << endl;
}

void MainWindow::loadSettings()
{
	QString setting;
	QSettings settings("foobar","kinect-record-tracker");

	setting = settings.value("clipClose").toString();
	trackingSettings.clipClose = (setting != "") ? setting.toInt() : 0;

	setting = settings.value("clipDistant").toString();
	trackingSettings.clipDistant = (setting != "") ? setting.toInt() : 0;

	setting = settings.value("blobFilterSmall").toString();
	trackingSettings.blobFilterSmall = (setting != "") ? setting.toInt() : blobFilterSmallSpinBox->value();

	setting = settings.value("blobFilterLarge").toString();
	trackingSettings.blobFilterLarge = (setting != "") ? setting.toInt() : blobFilterLargeSpinBox->value();

	setting = settings.value("blobDistanceFilter").toString();
	trackingSettings.blobDistanceFilter = (setting != "") ? setting.toInt() : 0;

	setting = settings.value("blobInactiveFilter").toString();
	trackingSettings.blobInactiveFilter = (setting != "") ? setting.toInt() : 0;

	depthMinSpinBox->setValue(trackingSettings.clipClose);
	depthMinSlider->setValue(trackingSettings.clipClose);
	depthMaxSpinBox->setValue(trackingSettings.clipDistant);
	depthMaxSlider->setValue(trackingSettings.clipDistant);
	blobFilterSmallSpinBox->setValue(trackingSettings.blobFilterSmall);
	blobFilterLargeSpinBox->setValue(trackingSettings.blobFilterLarge);
	blobDistanceFilterSlider->setValue(trackingSettings.blobDistanceFilter);
	blobDistanceFilterSpinBox->setValue(trackingSettings.blobDistanceFilter);
	blobInactiveFilterSlider->setValue(trackingSettings.blobInactiveFilter);
	blobInactiveFilterSpinBox->setValue(trackingSettings.blobInactiveFilter);

}

void MainWindow::saveSettings()
{
	QString setting;
	QSettings settings("foobar","kinect-record-tracker");

	settings.setValue("clipClose", QString::number(trackingSettings.clipClose));
	settings.setValue("clipDistant", QString::number(trackingSettings.clipDistant));
	settings.setValue("blobFilterSmall", QString::number(trackingSettings.blobFilterSmall));
	settings.setValue("blobFilterLarge", QString::number(trackingSettings.blobFilterLarge));
	settings.setValue("blobDistanceFilter", QString::number(trackingSettings.blobDistanceFilter));
	settings.setValue("blobInactiveFilter", QString::number(trackingSettings.blobInactiveFilter));
}

void MainWindow::actionOpenFolderTriggered() 
{
	QString path;

	QFileDialog *dialog = new QFileDialog();
	//dialog->setFileMode(QFileDialog::ExistingFile);
	dialog->setNameFilter("RECORD TXT Files (*.txt)");
	dialog->setOption(QFileDialog::ReadOnly);
	//dialog->setOption(QFileDialog::DontUseNativeDialog);

	QSettings settings("foobar","kinect-record-tracker");
	path = settings.value("loadpath").toString ();
	if(path != "") {
		dialog->setDirectory(path);
	}

	QStringList fileNames;
	if (dialog->exec()) {
		fileNames = dialog->selectedFiles();
		QString fileName = fileNames.at(0);
		
		QFileInfo fileInfo(fileName);
		path = fileInfo.absoluteDir().path();

		settings.setValue("loadpath", path);
		
		glView->openRecord(fileName);
		qDebug() << "File Opened";

		nextFrameButton->setEnabled(true);
		previousFrameButton->setEnabled(true);
		frameNumberSpinBox->setEnabled(true);
		frameNumberSpinBox->setMaximum(glView->getRecordLength());
		frameSlider->setEnabled(true);
		frameSlider->setMaximum(glView->getRecordLength());
	}
}

/**
 *    Slots
 */

void MainWindow::previousFrameButtonClicked()
{
	frameSlider->setValue(frameSlider->value()-1);
	frameNumberSpinBox->setValue(frameNumberSpinBox->value()+1);
	glView->showPreviousFrame();
}

void MainWindow::nextFrameButtonClicked()
{
	frameSlider->setValue(frameSlider->value()+1);
	frameNumberSpinBox->setValue(frameNumberSpinBox->value()+1);
	glView->showNextFrame();
}

void MainWindow::onFrameSliderChange( int value )
{
	frameNumberSpinBox->setValue(value);
	glView->showFrame(value - 1);
}

void MainWindow::onFrameNumberValueChanged ( int value )
{
	frameSlider->setValue(value);
	glView->showFrame(value - 1);
}

void MainWindow::onDepthMinSliderChange( int value )
{
	trackingSettings.clipClose = value;
	depthMinSpinBox->setValue(value);
	glView->setLimitDepthMin(value);
	glView->updateGL();
}

void MainWindow::onDepthMinSpinBoxValueChanged( int value )
{
	trackingSettings.clipClose = value;
	depthMinSlider->setValue(value);
	glView->setLimitDepthMin(value);
	glView->updateGL();
}

void MainWindow::onDepthMaxSliderChange( int value )
{
	trackingSettings.clipDistant = value;
	depthMaxSpinBox->setValue(value);
	glView->setLimitDepthMax(value);
	glView->updateGL();

}

void MainWindow::onDepthMaxSpinBoxValueChanged( int value )
{
	trackingSettings.clipDistant = value;
	depthMaxSlider->setValue(value);
	glView->setLimitDepthMin(value);
	glView->updateGL();
}

void MainWindow::onColorWheelColorChange( QColor color )
{
	glView->setSelectedColor(color);
	glView->updateGL();
}

void MainWindow::onBlobFilterSmallSpinBoxValueChanged( int value )
{
	trackingSettings.blobFilterSmall = value;
}

void MainWindow::onBlobFilterLargeSpinBoxValueChanged( int value )
{
	trackingSettings.blobFilterLarge = value;
}

void MainWindow::onBlobDistanceFilterSliderChange( int value )
{
	trackingSettings.blobDistanceFilter = value;
	blobDistanceFilterSpinBox->setValue(value);
}

void MainWindow::onBlobInactiveFilterSliderChange( int value )
{
	trackingSettings.blobInactiveFilter = value;
	blobInactiveFilterSpinBox->setValue(value);
}

void MainWindow::onBlobDistanceFilterSpinBoxValueChanged( int value )
{
	trackingSettings.blobDistanceFilter = value;
	blobDistanceFilterSlider->setValue(value);
}

void MainWindow::onBlobInactiveFilterSpinBoxValueChanged( int value )
{
	trackingSettings.blobInactiveFilter = value;
	blobInactiveFilterSlider->setValue(value);
}