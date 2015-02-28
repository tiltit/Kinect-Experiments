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

	connect(blobInactiveFilterSpinBox, SIGNAL(valueChanged(int)), this, SLOT(onBlobInactiveFilterSpinBoxChange(int)));
	connect(blobActiveFilterSpinBox, SIGNAL(valueChanged(int)), this, SLOT(onBlobActiveFilterSpinBoxChange(int)));

	connect(computeButton, SIGNAL(clicked()), this, SLOT(onComputeButtonClicked()));

	rightViewGroup = new QActionGroup(this);
	rightViewGroup->addAction(actionDepthRight);
	rightViewGroup->addAction(actionBlobsRight);
	rightViewGroup->addAction(actionVideoRight);
	actionDepthRight->setChecked(true);
	connect(rightViewGroup, SIGNAL(triggered(QAction*)), this, SLOT(onRightViewGroupTriggered(QAction*)));
	glView->setRightDisplay(Display::DEPTH);

	leftViewGroup = new QActionGroup(this);
	leftViewGroup->addAction(actionDepthLeft);
	leftViewGroup->addAction(actionBlobsLeft);
	leftViewGroup->addAction(actionVideoLeft);
	actionVideoLeft->setChecked(true);
	connect(leftViewGroup, SIGNAL(triggered(QAction*)), this, SLOT(onLeftViewGroupTriggered(QAction*)));
	glView->setLeftDisplay(Display::VIDEO);

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
	glView->record->trackingSettings.blobDistanceFilter = (setting != "") ? setting.toInt() : blobDistanceFilterSpinBox->value();

	setting = settings.value("blobInactiveFilter").toString();
	glView->record->trackingSettings.blobInactiveFilter = (setting != "") ? setting.toInt() : blobInactiveFilterSpinBox->value();

	setting = settings.value("blobActiveFilter").toString();
	glView->record->trackingSettings.blobActiveFilter = (setting != "") ? setting.toInt() : blobActiveFilterSpinBox->value();

	depthMinSlider->setValue(glView->record->trackingSettings.clipClose);	
	depthMaxSlider->setValue(glView->record->trackingSettings.clipDistant);
	blobFilterSmallSpinBox->setValue(glView->record->trackingSettings.blobFilterSmall);
	blobFilterLargeSpinBox->setValue(glView->record->trackingSettings.blobFilterLarge);
	blobDistanceFilterSlider->setValue(glView->record->trackingSettings.blobDistanceFilter);
	blobInactiveFilterSpinBox->setValue(glView->record->trackingSettings.blobInactiveFilter);

	colorWheel->setColor(QColor(
		settings.value("colorWheelRed").toString().toInt(),
		settings.value("colorWheelGreen").toString().toInt(),
		settings.value("colorWheelBlue").toString().toInt()
	));

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
	settings.setValue("blobActiveFilter", QString::number(glView->record->trackingSettings.blobActiveFilter));
	
	settings.setValue("colorWheelRed", QString::number(colorWheel->color().red()));
	settings.setValue("colorWheelGreen", QString::number(colorWheel->color().green()));
	settings.setValue("colorWheelBlue", QString::number(colorWheel->color().blue()));
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
		computeButton->setEnabled(true);
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
	glView->refreshFrames();
}

void MainWindow::onDepthMaxSliderChange( int value )
{
	glView->record->trackingSettings.clipDistant = value;
	glView->refreshFrames();

}

void MainWindow::onColorWheelColorChange( QColor color )
{
	glView->setSelectedColor(color);
	glView->refreshFrames();
}

void MainWindow::onBlobFilterSmallSpinBoxValueChanged( int value )
{
	glView->record->trackingSettings.blobFilterSmall = value;
	glView->refreshFrames();
}

void MainWindow::onBlobFilterLargeSpinBoxValueChanged( int value )
{
	glView->record->trackingSettings.blobFilterLarge = value;
	glView->refreshFrames();
}

void MainWindow::onBlobDistanceFilterSliderChange( int value )
{
	glView->record->trackingSettings.blobDistanceFilter = value;
	blobDistanceFilterSpinBox->setValue(value);
	glView->refreshFrames();
}

void MainWindow::onBlobInactiveFilterSpinBoxChange( int value )
{
	glView->record->trackingSettings.blobInactiveFilter = value;
	glView->refreshFrames();
}

void MainWindow::onBlobActiveFilterSpinBoxChange( int value )
{
	glView->record->trackingSettings.blobActiveFilter = value;
	glView->refreshFrames();
}

void MainWindow::onRightViewGroupTriggered(QAction * action)
{
	if (actionDepthRight->isChecked()) {
		glView->setRightDisplay(Display::DEPTH);
	} else if (actionBlobsRight->isChecked()) {
		glView->setRightDisplay(Display::BLOBS);
	} else if (actionVideoRight->isChecked()) {
		glView->setRightDisplay(Display::VIDEO);
	}
	glView->refreshFrames();
}

void MainWindow::onLeftViewGroupTriggered(QAction * action)
{
	if (actionDepthLeft->isChecked()) {
		glView->setLeftDisplay(Display::DEPTH);
	} else if (actionBlobsLeft->isChecked()) {
		glView->setLeftDisplay(Display::BLOBS);
	} else if (actionVideoLeft->isChecked()) {
		glView->setLeftDisplay(Display::VIDEO);
	}
	glView->refreshFrames();
}

void MainWindow::onComputeButtonClicked()
{
	statusbar->showMessage("Computing tracks");
	glView->record->computeTracks();
	statusbar->showMessage("Finished computing tracks");
}