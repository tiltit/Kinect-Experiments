#include <QApplication>
#include <QWidget>

#include "MainDialog.h"

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);

	MainDialog window;
	//window.resize(250, 150);
	//window.resize(400,400);
	window.setWindowTitle("qTtrack");
	window.show();
	return app.exec();

}

