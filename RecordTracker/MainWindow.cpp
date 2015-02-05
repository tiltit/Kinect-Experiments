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

	//dialog->setDirectory("/home/oliver");

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

void MainWindow::onFrameNumberValueChanged ( int i )
{
	frameSlider->setValue(i);
	glView->showFrame(i - 1);
}

void MainWindow::onDepthMinSliderChange( int value )
{
	glView->setLimitDepthMin(value);
	glView->updateGL();
}

void MainWindow::onDepthMaxSliderChange( int value )
{
	glView->setLimitDepthMax(value);
	glView->updateGL();

}

void MainWindow::onColorWheelColorChange( QColor color )
{
	glView->setSelectedColor(color);
	glView->updateGL();
}

