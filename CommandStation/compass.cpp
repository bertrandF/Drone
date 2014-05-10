/*
 *  This file is part of the Compass project
 *  Copyright (C) 06/05/2014 -- compass.cpp -- bertrand
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

#include "compass.h"

#include <QPainter>
#include <QLinearGradient>
#include <QBrush>
#include <QPaintEvent>
#include <QSize>

// This is the size, (no scale sry !)
#define COMPASSSIZE (200)

#define CIRCLEPENWIDTH      (4)

Compass::Compass(QWidget *parent) :
    QGraphicsView(parent),
    bearing(60)
{
    QGraphicsView::setGeometry(0, 0, COMPASSSIZE, COMPASSSIZE);

    background      = QBrush(QColor(0x53, 0x54, 0x48));
    circlePen       = QPen(Qt::black);
    circlePen.setWidth(CIRCLEPENWIDTH);
    graduationsPen  = QPen(Qt::white);
    graduationsPen.setWidth(2);
    textPen         = QPen(Qt::white);
    textFont        = QFont("lucida");
    textFont.setPointSize(10);
    textFont.setBold(true);
    planePen        = QPen(QColor(0xff, 0xa5, 0x00));
    planePen.setWidth(2);

    plane.append(QLineF( 0 , -70 , 0 , -55 ));
    plane.append(QLineF( 0 , -55 , 10 , -35 ));
    plane.append(QLineF( 10 , -35 , 50 , 0 ));
    plane.append(QLineF( 50 , 0 , 50 , 13 ));
    plane.append(QLineF( 50 , 13 , 8 , 0 ));
    plane.append(QLineF( 8 , 0 , 5 , 35 ));
    plane.append(QLineF( 5 , 35 , 23 , 50 ));
    plane.append(QLineF( 23 , 50 , 23 , 60 ));
    plane.append(QLineF( 23 , 60 , 2 , 55 ));
    plane.append(QLineF( 2 , 55 , 0 , 65 ));
    plane.append(QLineF( 0 , -70 , 0 , -55 ));
    plane.append(QLineF( 0 , -55 , -10 , -35 ));
    plane.append(QLineF( -10 , -35 , -50 , 0 ));
    plane.append(QLineF( -50 , 0 , -50 , 13 ));
    plane.append(QLineF( -50 , 13 , -8 , 0 ));
    plane.append(QLineF( -8 , 0 , -5 , 35 ));
    plane.append(QLineF( -5 , 35 , -23 , 50 ));
    plane.append(QLineF( -23 , 50 , -23 , 60 ));
    plane.append(QLineF( -23 , 60 , -2 , 55 ));
    plane.append(QLineF( -2 , 55 , 0 , 65 ));

}

void Compass::setBearing(double bearing)
{
    if(bearing >= 0  && bearing <= 360 )
        this->bearing = bearing;
    this->viewport()->update();
}

void Compass::paintEvent(QPaintEvent *event)
{
    QPainter painter;
    painter.begin(this->viewport());
    painter.setRenderHint(QPainter::Antialiasing);
    paint(&painter, event);
    painter.end();
}

void Compass::paint(QPainter *painter, QPaintEvent *event)
{
    // Background + Circle
    painter->fillRect(event->rect(), background);
    painter->setPen(circlePen);
    painter->translate(COMPASSSIZE/2, COMPASSSIZE/2);
    painter->drawEllipse(QPoint(0, 0), COMPASSSIZE/2-CIRCLEPENWIDTH,
                         COMPASSSIZE/2-CIRCLEPENWIDTH);

    // Plane
    painter->setPen(planePen);
    painter->drawLines(plane);

    // Graduations (points)
    painter->rotate(-bearing);
    painter->save();
    painter->setPen(graduationsPen);
    painter->rotate(5);
    for(int i=5 ; i<360 ; i+=10)
    {
        painter->drawPoint(85, 0);
        painter->rotate(10);
    }
    painter->restore();

    // Graduations (lines)
    painter->save();
    painter->setPen(graduationsPen);
    for(int i=0 ; i<=360 ; i+=10)
    {
        painter->drawLine(QPointF(85,0), QPointF(93,0));
        painter->rotate(10);
    }
    painter->restore();

    // Text
    painter->save();
    painter->setPen(textPen);
    painter->setFont(textFont);
    painter->drawText(QPoint(-5,-70), QString("N"));
    painter->rotate(90);
    painter->drawText(QPoint(-5,-70), QString("E"));
    painter->rotate(90);
    painter->drawText(QPoint(-5,-70), QString("S"));
    painter->rotate(90);
    painter->drawText(QPoint(-5,-70), QString("W"));
    painter->restore();
}
