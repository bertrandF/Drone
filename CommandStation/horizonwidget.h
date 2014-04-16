/*
 *  This file is part of the CommandStation Project
 *  Copyright (C) 15/04/2014 -- horizonwidget.h -- bertrand
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

#ifndef HORIZONWIDGET_H
#define HORIZONWIDGET_H

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>

class HorizonWidget : public QGraphicsView
{
    Q_OBJECT
public:
    explicit HorizonWidget(QWidget *parent = 0);

signals:

public slots:
    void setPitch(float pitch);
    void setRoll(float roll);

protected:
    void wheelEvent(QWheelEvent *event);
    void paintEvent(QPaintEvent *event);
    void drawForeground(QPainter* painter, const QRectF &rect);

private:
    QGraphicsScene scene;
    QGraphicsPixmapItem back;
    QGraphicsPixmapItem plane;

};

#endif // HORIZONWIDGET_H
