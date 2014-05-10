/*
 *  This file is part of the Compass project
 *  Copyright (C) 06/05/2014 -- compass.h -- bertrand
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

#ifndef COMPASS_H
#define COMPASS_H

#include <QGraphicsView>
#include <QPaintEvent>


class Compass : public QGraphicsView
{
    Q_OBJECT
public:
    explicit Compass(QWidget *parent = 0);

    void setBearing(double bearing);
    inline double getBearing()
        { return this->bearing; }

signals:

public slots:

protected:
    void paintEvent(QPaintEvent *event);

private:
    void paint(QPainter *painter, QPaintEvent *event);

    QBrush  background;
    QPen    circlePen;
    QPen    graduationsPen;
    QPen    textPen;
    QFont   textFont;
    QPen    planePen;

    QVector<QLineF> plane;

    double bearing;
};

#endif // COMPASS_H
