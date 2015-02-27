#ifndef FONTRENDER_H
#define FONTRENDER_H

#include <QObject>
#include <QtXml>
#include <QMap>
#include <QString>
#include <QPoint>
#include <QRect>
#include <QByteArray>
#include <GL/glu.h>
#include <QColor>

struct Glyph {
	int width;
	int height;
	QPoint offset;
	QRect rect;
	QByteArray data;
};

class FontRender : public QObject
 {
    Q_OBJECT

	public:
		FontRender(QString fontImageFileName, QString fontXmlFileName, QObject *parent = 0);
		void print(int x_offset, int y_offset, QString s);
		void setColor(QRgb);
		void setColor(QColor color);
		void setColor(uint8_t r, uint8_t g, uint8_t b);

	protected:

	private:
		QMap<unsigned char, Glyph*> glyphMap;
		QString imageFileName;
		QString xmlFileName;
		QColor textColor;

	private slots:
 };

#endif