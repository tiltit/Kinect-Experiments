#include <QApplication>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QDebug>
#include <QPixmap>
#include <QColor>
#include <QSizePolicy>
#include <QLabel>
#include <QDebug>
#include <QWidget>
#include <QSettings>

#include "MainDialog.h"

using namespace cv;


MainDialog::MainDialog()
{
	QVBoxLayout *vBoxMain = new QVBoxLayout(this);
	QHBoxLayout *vBoxDepthH = new QHBoxLayout(this);

	QGridLayout *grid1 = new QGridLayout();

	trackWidget = new TrackWidget;

	// Set Strong focus to capture key events
	trackWidget->setFocusPolicy(Qt::StrongFocus);

	vBoxMain->addWidget(trackWidget);

	// Depth Slider select start
	depthStartLabel = new QLabel();
	depthStartLabel->setNum(0);
	grid1->addWidget(depthStartLabel, 0, 0);

	depthEndLabel = new QLabel();
	depthEndLabel->setNum(0);
	grid1->addWidget(depthEndLabel, 1, 0);

	depthStartSlider = new QSlider(Qt::Horizontal);
	depthStartSlider->setMaximum(2047);
	grid1->addWidget(depthStartSlider, 0, 1);

	depthEndSlider = new QSlider(Qt::Horizontal);
	depthEndSlider->setMaximum(2047);
	grid1->addWidget(depthEndSlider, 1, 1);

	vBoxDepthH->addLayout(grid1);

	colorWheel = new ColorWheel();
	colorWheel->setMaximumSize(100,100);
	vBoxDepthH->addWidget(colorWheel);

	vBoxMain->addLayout(vBoxDepthH);

	setLayout(vBoxMain);
	trackWidget->setMinimumSize (640, 240);
	//trackWidget->resize(1280, 480);

	connect(qApp, SIGNAL(aboutToQuit()), this, SLOT(closing()));
	connect(depthStartSlider, SIGNAL(sliderMoved(int)), this, SLOT(onDepthStartSliderChange(int)));
	connect(depthEndSlider, SIGNAL(sliderMoved(int)), this, SLOT(onDepthEndSliderChange(int)));
	connect(colorWheel, SIGNAL(colorChange(QColor)), this, SLOT(onColorWheelColorChange(QColor)));

	loadSettings();
}

void MainDialog::onDepthStartSliderChange(int value)
{
	qDebug() << value;
	trackWidget->setLimitDepthStart(value);
	depthStartLabel->setNum(value);
}

void MainDialog::onDepthEndSliderChange(int value)
{
	trackWidget->setLimitDepthEnd(value);
	depthEndLabel->setNum(value);
}

void MainDialog::onColorWheelColorChange(const QColor color)
{
	trackWidget->setSelectedColor(color.red(), color.green(), color.blue());
}

void MainDialog::closing() {
    trackWidget->device->stopVideo();
    trackWidget->device->stopDepth();
    saveSettings();
    qDebug() << "Quiting";
}

void MainDialog::saveSettings()
{
	QSettings settings("foobar","qtTrack");
	settings.setValue("blob/color", colorWheel->color());
	settings.setValue("blob/sliderStart", depthStartSlider->sliderPosition());
	settings.setValue("blob/sliderEnd", depthEndSlider->sliderPosition());
}

void MainDialog::loadSettings()
{
	QSettings settings("foobar","qtTrack");
	colorWheel->setColor(settings.value("blob/color").value<QColor>());
	depthStartSlider->setValue(settings.value("blob/sliderStart").toInt());
	depthEndSlider->setValue(settings.value("blob/sliderEnd").toInt());
	trackWidget->setLimitDepthStart(depthStartSlider->sliderPosition());
	trackWidget->setLimitDepthEnd(depthEndSlider->sliderPosition());
}