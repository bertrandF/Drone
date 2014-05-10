/*
 *  This file is part of the Altimeter project
 *  Copyright (C) 10/05/2014 -- altimeter.cpp -- bertrand
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 * */

#include "altimeter.h"
#include <QBitmap>
#include <QPolygon>

#define WIDGETSIZE      (200)
#define CIRCLEPENWIDTH  (4)

#define HANDHEIGHT      (140)
#define HANDWIDTH       (25)
#define HANDCENTERX     (HANDWIDTH/2)
#define HANDCENTERY     (105)
#define HANDCENTER      (QPoint(HANDCENTERX, HANDCENTERY))

/*
 * NOTE: Using background color for antialiasing helps hidding mask artifacts
 * due to antialiasing.
 * */

Altimeter::Altimeter(QWidget *parent) :
    QGraphicsView(parent),
    feets(0)
{
    QPolygon polygon; // helper

    this->setGeometry(0,0, WIDGETSIZE, WIDGETSIZE);

    // Style Setting
    backgroundBrush = QBrush(Qt::black);//QBrush(QColor(0x53, 0x54, 0x48));
    circlePen       = QPen(Qt::white);
    circlePen.setWidth(CIRCLEPENWIDTH);
    graduationPen   = QPen(Qt::white);
    graduationPen.setWidth(2);
    graduationFont  = QFont("lucida");
    graduationFont.setPointSize(12);
    graduationFont.setBold(true);
    handPenFront    = QPen(Qt::black);
    handPenFront.setWidth(2);
    handPenBack     = QPen(Qt::white);
    handPenBack.setWidth(2);
    handBrushBack   = QBrush(Qt::black);
    handBrushFront  = QBrush(Qt::white);

    // BACKGROUND PIXMAP
    backgroundPixmap = QPixmap(WIDGETSIZE,WIDGETSIZE);
    QPainter painter;
    painter.begin(&(this->backgroundPixmap));
    painter.setRenderHint(QPainter::Antialiasing);
    // Background
    painter.fillRect(QRect(0,0,WIDGETSIZE,WIDGETSIZE), backgroundBrush);
    // Circle
    painter.translate(WIDGETSIZE/2, WIDGETSIZE/2);
    painter.setBrush(Qt::NoBrush);
    painter.setPen(circlePen);
    painter.drawEllipse(QPoint(0,0),
                        (WIDGETSIZE/2-CIRCLEPENWIDTH),
                        (WIDGETSIZE/2-CIRCLEPENWIDTH)
                        );
    // Graduation
    painter.save();
    painter.setFont(graduationFont);
    for(int i=0 ; i<10 ; ++i)
    {
        graduationPen.setWidth(2);
        painter.setPen(graduationPen);
        painter.drawLine(QPoint(0, -(WIDGETSIZE/2-CIRCLEPENWIDTH)+5),
                         QPoint(0, -(WIDGETSIZE/2-CIRCLEPENWIDTH)+13));

        graduationPen.setWidth(1);
        painter.setPen(graduationPen);
        painter.drawText(
                    QPoint(-5, -(WIDGETSIZE/2-CIRCLEPENWIDTH)+28),
                    QString::number(i));
        for(int j=1 ; j<5 ; ++j)
        {
            painter.rotate(7.2);
            painter.drawLine(QPoint(0, -(WIDGETSIZE/2-CIRCLEPENWIDTH)+5),
                             QPoint(0, -(WIDGETSIZE/2-CIRCLEPENWIDTH)+10));
        }
        painter.rotate(7.2);
    }
    painter.restore();
    painter.end();


    // HAND, FEETS
    feetsHand = QPixmap(HANDWIDTH, HANDHEIGHT);
    painter.begin(&(this->feetsHand));
    painter.setRenderHint(QPainter::Antialiasing);
    painter.fillRect(feetsHand.rect(), backgroundBrush);
    // Hand front
    painter.setBrush(handBrushFront);
    painter.setPen(handPenFront);
    painter.drawRect(QRect(HANDCENTERX-2, CIRCLEPENWIDTH+5, 4, 16));
    painter.drawRect(QRect(
                         QPoint(HANDCENTERX-3, CIRCLEPENWIDTH+5+15),
                         QPoint(HANDCENTERX+3, HANDCENTERY))
                     );
    // Hand back
    painter.setBrush(handBrushBack);
    painter.setPen(handPenBack);
    painter.drawRect(HANDCENTERX-3, HANDCENTERY, 6, 25);
    painter.drawEllipse(QPoint(HANDCENTERX, HANDCENTERY+25), 6, 6);
    painter.end();
    // Mask
    feetsHand.setMask(feetsHand.createMaskFromColor(
                          backgroundBrush.color(),
                          Qt::MaskInColor)
                      );


    // HAND, TENTH of FEETS
    tenthHand = QPixmap(HANDWIDTH, HANDHEIGHT);
    painter.begin(&(this->tenthHand));
    painter.setRenderHint(QPainter::Antialiasing);
    painter.fillRect(tenthHand.rect(), backgroundBrush);
    // Hand front
    painter.setBrush(handBrushFront);
    painter.setPen(handPenFront);
    polygon.append(HANDCENTER);
    polygon.append(QPoint(HANDCENTERX-10, HANDCENTERY-25));
    polygon.append(QPoint(HANDCENTERX, HANDCENTERY-75));
    polygon.append(QPoint(HANDCENTERX+10, HANDCENTERY-25));
    painter.drawPolygon(polygon);
    // Hand back
    painter.setBrush(handBrushBack);
    painter.setPen(handPenBack);
    polygon.clear();
    polygon.append(HANDCENTER);
    polygon.append(QPoint(HANDCENTERX-10, HANDCENTERY+25));
    polygon.append(QPoint(HANDCENTERX+10, HANDCENTERY+25));
    painter.drawPolygon(polygon);
    painter.end();
    // Mask
    tenthHand.setMask(tenthHand.createMaskFromColor(
                          backgroundBrush.color(),
                          Qt::MaskInColor)
                      );


    // HAND, HUNDREDS of FEETS
    hundredsHand = QPixmap(HANDWIDTH, HANDHEIGHT);
    painter.begin(&(this->hundredsHand));
    painter.setRenderHint(QPainter::Antialiasing);
    painter.fillRect(hundredsHand.rect(), backgroundBrush);
    // Hand front
    painter.setBrush(handBrushFront);
    painter.setPen(handPenFront);
    painter.drawRect(QRect(
                         HANDCENTERX-2,
                         CIRCLEPENWIDTH+5,
                         4,
                         HANDCENTERY-CIRCLEPENWIDTH-5)
                     );
    polygon.clear();
    polygon.append(QPoint(HANDCENTERX-10, CIRCLEPENWIDTH+5));
    polygon.append(QPoint(HANDCENTERX+10, CIRCLEPENWIDTH+5));
    polygon.append(QPoint(HANDCENTERX, CIRCLEPENWIDTH+5+15));
    painter.drawPolygon(polygon);
    // Hand back
    painter.setBrush(handBrushBack);
    painter.setPen(handPenBack);
    painter.end();
    // Mask
    hundredsHand.setMask(hundredsHand.createMaskFromColor(
                             backgroundBrush.color(),
                             Qt::MaskInColor)
                         );

}

void Altimeter::setAltitude(double feets)
{
    if(feets>0)
    {
        this->feets = feets;
        this->viewport()->update();
    }
}

void Altimeter::paintEvent(QPaintEvent *event)
{
    QPainter painter;
    painter.begin(this->viewport());
    painter.setRenderHint(QPainter::Antialiasing);
    paint(&painter, event);
    painter.end();
}

void Altimeter::paint(QPainter *painter, QPaintEvent *event)
{
    painter->drawPixmap(0,0, backgroundPixmap);
    painter->translate(WIDGETSIZE/2, WIDGETSIZE/2);

    // Feets
    painter->save();
    painter->rotate(feets * 36);
    painter->drawPixmap(
                -HANDWIDTH/2,
                -(WIDGETSIZE/2-CIRCLEPENWIDTH)-5,
                feetsHand
                );
    painter->restore();

    // Tenth of feets
    painter->save();
    painter->rotate(feets * 3.6);
    painter->drawPixmap(
                -HANDWIDTH/2,
                -(WIDGETSIZE/2-CIRCLEPENWIDTH)-5,
                tenthHand
                );
    painter->restore();

    // Hundreds of feets
    painter->save();
    painter->rotate(feets * 0.36);
    painter->drawPixmap(
                -HANDWIDTH/2,
                -(WIDGETSIZE/2-CIRCLEPENWIDTH)-5,
                hundredsHand
                );
    painter->restore();

    // Center circle
    painter->setBrush(handBrushBack);
    painter->drawEllipse(QPoint(0,0), 6, 6);
}
