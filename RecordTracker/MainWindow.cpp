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

	connect(depthMinSlider, SIGNAL(valueChanged(int)), this, SLOT(onDepthMinSliderChange(int)));
	connect(depthMinSlider, SIGNAL(valueChanged(int)), depthMinSpinBox, SLOT(setValue(int)));
	connect(depthMinSpinBox, SIGNAL(valueChanged(int)), depthMinSlider, SLOT(setValue(int)));
	
	connect(depthMaxSlider, SIGNAL(valueChanged(int)), this, SLOT(onDepthMaxSliderChange(int)));
	connect(depthMaxSlider, SIGNAL(valueChanged(int)), depthMaxSpinBox, SLOT(setValue(int)));
	connect(depthMaxSpinBox, SIGNAL(valueChanged(int)), depthMaxSlider, SLOT(setValue(int)));

	connect(frameSlider, SIGNAL(valueChanged(int)), this, SLOT(onFrameSliderChange(int)));
	connect(frameSlider, SIGNAL(valueChanged(int)), frameNumberSpinBox, SLOT(setValue(int)));
	connect(frameNumberSpinBox, SIGNAL(valueChanged(int)), frameSlider, SLOT(setValue(int)));

	connect(colorWheel, SIGNAL(colorChange(QColor)), this, SLOT(onColorWheelColorChange(QColor)));

	connect(blobFilterSmallSpinBox, SIGNAL(valueChanged(int)), this, SLOT(onBlobFilterSmallSpinBoxValueChanged(int)));
	connect(blobFilterLargeSpinBox, SIGNAL(valueChanged(int)), this, SLOT(onBlobFilterLargeSpinBoxValueChanged(int)));

	connect(blobDistanceFilterSlider, SIGNAL(valueChanged(int)), this, SLOT(onBlobDistanceFilterSliderChange(int)));
	connect(blobDistanceFilterSlider, SIGNAL(valueChanged(int)), blobDistanceFilterSpinBox, SLOT(setValue(int)));
	connect(blobDistanceFilterSpinBox, SIGNAL(valueChanged(int)), blobDistanceFilterSlider, SLOT(setValue(int)));	

	connect(blobInactiveFilterSlider, SIGNAL(valueChanged(int)), this, SLOT(onBlobInactiveFilterSliderChange(int)));
	connect(blobInactiveFilterSlider, SIGNAL(valueChanged(int)), blobInactiveFilterSpinBox, SLOT(setValue(int)));
	connect(blobInactiveFilterSpinBox, SIGNAL(valueChanged(int)), blobInactiveFilterSlider, SLOT(setValue(int)));

	rightViewGroup = new QActionGroup(this);
	rightViewGroup->addAction(actionDepthRight);
	rightViewGroup->addAction(actionBlobsRight);
	rightViewGroup->addAction(actionVideoRight);
	actionDepthRight->setChecked(true);
	connect(rightViewGroup, SIGNAL(triggered(QAction*)), this, SLOT(onRightViewGroupTriggered(QAction*)));

	leftViewGroup = new QActionGroup(this);
	leftViewGroup->addAction(actionDepthLeft);
	leftViewGroup->addAction(actionBlobsLeft);
	leftViewGroup->addAction(actionVideoLeft);
	actionDepthLeft->setChecked(true);
	connect(leftViewGroup, SIGNAL(triggered(QAction*)), this, SLOT(onLeftViewGroupTriggered(QAction*)));

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
	glView->record->trackingSettings.clipClose = (setting != "") ? setting.toInt() : 0;

	setting = settings.value("clipDistant").toString();
	glView->record->trackingSettings.clipDistant = (setting != "") ? setting.toInt() : 0;

	setting = settings.value("blobFilterSmall").toString();
	glView->record->trackingSettings.blobFilterSmall = (setting != "") ? setting.toInt() : blobFilterSmallSpinBox->value();

	setting = settings.value("blobFilterLarge").toString();
	glView->record->trackingSettings.blobFilterLarge = (setting != "") ? setting.toInt() : blobFilterLargeSpinBox->value();

	setting = settings.value("blobDistanceFilter").toString();
	glView->record->trackingSettings.blobDistanceFilter = (setting != "") ? setting.toInt() : 0;

	setting = settings.value("blobInactiveFilter").toString();
	glView->record->trackingSettings.blobInactiveFilter = (setting != "") ? setting.toInt() : 0;

	depthMinSlider->setValue(glView->record->trackingSettings.clipClose);	
	depthMaxSlider->setValue(glView->record->trackingSettings.clipDistant);
	blobFilterSmallSpinBox->setValue(glView->record->trackingSettings.blobFilterSmall);
	blobFilterLargeSpinBox->setValue(glView->record->trackingSettings.blobFilterLarge);
	blobDistanceFilterSlider->setValue(glView->record->trackingSettings.blobDistanceFilter);
	blobInactiveFilterSlider->setValue(glView->record->trackingSettings.blobInactiveFilter);

}

void MainWindow::saveSettings()
{
	QString setting;
	QSettings settings("foobar","kinect-record-tracker");

	settings.setValue("clipClose", QString::number(glView->record->trackingSettings.clipClose));
	settings.setValue("clipDistant", QString::number(glView->record->trackingSettings.clipDistant));
	settings.setValue("blobFilterSmall", QString::number(glView->record->trackingSettings.blobFilterSmall));
	settings.setValue("blobFilterLarge", QString::number(glView->record->trackingSettings.blobFilterLarge));
	settings.setValue("blobDistanceFilter", QString::number(glView->record->trackingSettings.blobDistanceFilter));
	settings.setValue("blobInactiveFilter", QString::number(glView->record->trackingSettings.blobInactiveFilter));
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

		frameNumberSpinBox->setEnabled(true);
		frameNumberSpinBox->setMaximum(glView->getRecordLength());
		frameSlider->setEnabled(true);
		frameSlider->setMaximum(glView->getRecordLength());
	}
}

/**
 *    Slots
 */

void MainWindow::onFrameSliderChange( int value )
{
	glView->showFrame(value - 1);
}

void MainWindow::onDepthMinSliderChange( int value )
{
	glView->record->trackingSettings.clipClose = value;
	glView->updateGL();
}

void MainWindow::onDepthMaxSliderChange( int value )
{
	glView->record->trackingSettings.clipDistant = value;
	glView->updateGL();

}

void MainWindow::onColorWheelColorChange( QColor color )
{
	glView->setSelectedColor(color);
	glView->updateGL();
}

void MainWindow::onBlobFilterSmallSpinBoxValueChanged( int value )
{
	glView->record->trackingSettings.blobFilterSmall = value;
}

void MainWindow::onBlobFilterLargeSpinBoxValueChanged( int value )
{
	glView->record->trackingSettings.blobFilterLarge = value;
}

void MainWindow::onBlobDistanceFilterSliderChange( int value )
{
	glView->record->trackingSettings.blobDistanceFilter = value;
	blobDistanceFilterSpinBox->setValue(value);
}

void MainWindow::onBlobInactiveFilterSliderChange( int value )
{
	glView->record->trackingSettings.blobInactiveFilter = value;
	blobInactiveFilterSpinBox->setValue(value);
}

void MainWindow::onRightViewGroupTriggered(QAction * action)
{
	
}

void MainWindow::onLeftViewGroupTriggered(QAction * action)
{
	
}