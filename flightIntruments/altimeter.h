/*
 *  This file is part of the Drone project
 *  Copyright (C) 10/05/2014 -- altimeter.h -- bertrand
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

#ifndef ALTIMETER_H
#define ALTIMETER_H

#include <QGraphicsView>
#include <QPixmap>
#include <QBrush>
#include <QPen>
#include <QFont>

class Altimeter : public QGraphicsView
{
    Q_OBJECT
public:
    explicit Altimeter(QWidget *parent = 0);

signals:

public slots:
    void setAltitude(double feets);

protected:
    void paintEvent(QPaintEvent *event);

private:
    void paint(QPainter *painter, QPaintEvent *event);

    double feets;

    QBrush  backgroundBrush;
    QPen    circlePen;
    QPixmap backgroundPixmap;
    QPen    graduationPen;
    QFont   graduationFont;

    QPen    handPenFront;
    QPen    handPenBack;
    QBrush  handBrushFront;
    QBrush  handBrushBack;
    QPixmap feetsHand;
    QPixmap tenthHand;
    QPixmap hundredsHand;


};

#endif // ALTIMETER_H
