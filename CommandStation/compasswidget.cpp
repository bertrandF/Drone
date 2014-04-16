/*
 *  This file is part of the CommandStation Project
 *  Copyright (C) 15/04/2014 -- compasswidget.cpp -- bertrand
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

#include "compasswidget.h"

#include <QSize>

/*
 * There is and HardCoded margin of 2 pixels in QGraphicsView
 * We need to remove it when scaling so that the image perfectly
 * fits the display rect and do not go under the margin which will
 * unable scrolling.
 */
#define SHITTY_MARGIN   (2)

CompassWidget::CompassWidget(QWidget *parent) :
    QGraphicsView(parent),
    scene(),
    back(),
    wheel(),
    plane()
{
    // TODO: size of QGraphicsView (150x150) is hardcoded -> get it whith size()
    // TODO: We need to draw the Pixmap when the QGraphicsView is shown (before
    // TODO: size() returns bad numbers).
    this->setScene(&(this->scene));
    this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    // Background
    this->back.setPixmap(QPixmap(":/images/Compass_background.png")\
                .scaled(QSize(150-SHITTY_MARGIN,150-SHITTY_MARGIN)));
    this->scene.addItem(&(this->back));

    // Wheel
    this->wheel.setPixmap(QPixmap(":/images/Compass_wheel.png")\
                .scaled(QSize(128,128)));
    this->wheel.setPos(74-64,74-64);
    this->wheel.setTransformOriginPoint(64,64);
    //this->wheel.setRotation(0);
    this->scene.addItem(&(this->wheel));

    // Plane
    this->plane.setPixmap(QPixmap(":/images/Compass_plane.png")\
                .scaled(QSize(53,79)));
    this->plane.setPos(74-26,74-53);
    this->scene.addItem(&(this->plane));

    this->setFrameShape(QFrame::NoFrame);
    this->show();
}


void CompassWidget::setBearing(float bearing)
{
    this->wheel.setRotation(bearing);
}

void CompassWidget::wheelEvent(QWheelEvent *event)
{
    return;
}
