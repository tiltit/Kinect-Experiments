/**
 *	Reders fonts created with font builder
 **/
#include <iostream>
#include <QDebug>
#include <QDomDocument>
#include <QImage>

#include "FontRender.h"

FontRender::FontRender(QString fontImageFileName, QString fontXmlFileName, QObject *parent) 
	: QObject(parent) 
{
	
	qDebug() << "FontRender Object Created";

	textColor = QColor(255,255,255);

	imageFileName = fontImageFileName;
	xmlFileName = fontXmlFileName;

	QFile file(fontXmlFileName);
	if (!file.open(QIODevice::ReadOnly)) 
		return;
	QByteArray xmlText = file.readAll();
	file.close();

	QDomDocument doc;
	doc.setContent(xmlText);

	QDomElement docElem = doc.documentElement();
	int height = docElem.attribute("height").toInt();

	QDomNode n = docElem.firstChild();
	while(!n.isNull()) {
    	QDomElement e = n.toElement(); // try to convert the node to an element.
    	if(!e.isNull()) {
        	QByteArray code = e.attribute("code").toLatin1();

        	int width = e.attribute("width").toInt();

        	QStringList offset = e.attribute("offset").split(" ");
        	QStringList rect = e.attribute("rect").split(" ");

        	glyphMap[code[0]] = new Glyph;
        	glyphMap[code[0]]->height = height;
        	glyphMap[code[0]]->width = width;
        	glyphMap[code[0]]->offset = QPoint(offset.at(0).toInt(), offset.at(1).toInt());
        	glyphMap[code[0]]->rect = QRect(rect.at(0).toInt(), rect.at(1).toInt(), rect.at(2).toInt(), rect.at(3).toInt());
    	}
    	n = n.nextSibling();
	}

	QImage fontMapImage(fontImageFileName);

	QMap<unsigned char, Glyph*>::iterator i;
	for(i = glyphMap.begin(); i!= glyphMap.end(); ++i) {
		QImage image = fontMapImage.copy(i.value()->rect);

		int x, y;
		for(y=0;y!=image.height();++y) {
			for(x=0;x!=image.width();++x) {
				QRgb pixel = image.pixel(x,y);
				i.value()->data.append(qAlpha(pixel));
			}
		}
	}

}

void FontRender::print(int x_offset, int y_offset, QString s) 
{
	int x, y;
	int i=0;
	int j=0;
	Glyph *glyph;

	QByteArray ba = s.toLatin1();
	const char *data = ba.data();
	i=0;
	while(data[i]!='\0') {
		j = 0;
		glyph = glyphMap[data[i]];
		for(y=y_offset+glyph->offset.y();y!=y_offset+glyph->offset.y()+glyph->rect.height();++y) {
			for(x=x_offset+glyph->offset.x();x!=x_offset+glyph->offset.x()+glyph->rect.width();++x) {
				glColor4f(textColor.redF(), textColor.greenF(), textColor.blueF(), (unsigned char)glyph->data[j++] / 255.0);
				glBegin(GL_TRIANGLE_FAN);
				glVertex2f(x, y);
				glVertex2f(x, y+1);
				glVertex2f(x+1, y+1);
				glVertex2f(x+1, y);
				glEnd();
			}
		}
		x_offset+=glyph->width;
		i++;
	}
}

void FontRender::setColor(QColor color) {
	textColor = color;
}

void FontRender::setColor(uint8_t r, uint8_t g, uint8_t b) {
	textColor = QColor(r,g,b);
}
