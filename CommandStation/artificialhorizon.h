/*
 *  This file is part of the ArtificialHorizon project
 *  Copyright (C) 04/05/2014 -- artificialhorizon.h -- bertrand
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

#ifndef ARTIFICIALHORIZON_H
#define ARTIFICIALHORIZON_H

#include <QGraphicsView>
#include <QPainter>
#include <QPaintEvent>
#include <QBrush>
#include <QPen>
#include <QBitmap>

class ArtificialHorizon : public QGraphicsView
{
    Q_OBJECT
public:
    explicit ArtificialHorizon(QWidget *parent = 0);

signals:

public slots:
    void    setRollPitch(double roll, double pitch);

protected:
    void paintEvent(QPaintEvent *event);

private:
    void paint(QPainter *painter, QPaintEvent *event);

    QBrush  backgroundBrush;
    QPen    circlePen;
    QBrush  skyBrush;
    QBrush  groundBrush;
    QPen    linePen;
    QBrush  triangleBrush;

    QPixmap foregroundPixmap;
    QBitmap foregroundMask;
    QPixmap movingPixmap;
    QPen    indicatorPen;
    QBrush  indicatorBrush;
    QPointF indicatorTriangle[3];
    QLineF  indicatorLines[5];
    QPen    scalePen;
    QLineF  scaleLines[12];

    double roll;
    double pitch;
};

#endif // ARTIFICIALHORIZON_H
