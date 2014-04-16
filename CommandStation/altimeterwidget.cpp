/*
 *  This file is part of the CommandStation Project
 *  Copyright (C) 15/04/2014 -- altimeterwidget.cpp -- bertrand
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

#include "altimeterwidget.h"

#include <QGraphicsPixmapItem>
#include <QDebug>

/*
 * There is and HardCoded margin of 2 pixels in QGraphicsView
 * We need to remove it when scaling so that the image perfectly
 * fits the display rect and do not go under the margin which will
 * unable scrolling.
 */
#define SHITTY_MARGIN   (2)

#define MTR_TO_FT       (3.2808399)

AltimeterWidget::AltimeterWidget(QWidget *parent) :
    QGraphicsView(parent),
    scene(),
    back(),
    tenthousands(),
    thousands(),
    hundreds()
{
    // TODO: size of QGraphicsView (150x150) is hardcoded -> get it whith size()
    // TODO: We need to draw the Pixmap when the QGraphicsView is shown (before
    // TODO: size() returns bad numbers).
    this->setScene(&(this->scene));
    this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    // Background
    this->back.setPixmap(QPixmap(":/images/Altimeter_background.png")\
                .scaled(QSize(150-SHITTY_MARGIN,150-SHITTY_MARGIN)));
    this->scene.addItem(&(this->back));


    // Ten Thousands
    this->tenthousands.setPixmap(QPixmap(":/images/Altimeter_tenthousands.png")\
                .scaled(QSize(14,75)));
    this->tenthousands.setPos(74-5,74-55);
    this->tenthousands.setTransformOriginPoint(5,55);
    this->tenthousands.setRotation(0);
    this->scene.addItem(&(this->tenthousands));

    // Thousands
    this->thousands.setPixmap(QPixmap(":/images/Altimeter_thousands.png")\
                .scaled(QSize(20,66)));
    this->thousands.setPos(74-10,74-50);
    this->thousands.setTransformOriginPoint(10,50);
    this->thousands.setRotation(0);
    this->scene.addItem(&(this->thousands));

    // Hundreds
    this->hundreds.setPixmap(QPixmap(":/images/Altimeter_hundreds.png")\
                .scaled(QSize(12,80)));
    this->hundreds.setPos(74-5,74-55);
    this->hundreds.setTransformOriginPoint(5,55);
    this->hundreds.setRotation(0);
    this->scene.addItem(&(this->hundreds));

    this->setFrameShape(QFrame::NoFrame);
    this->show();
}

void AltimeterWidget::setAltitudeFt(long ft)
{
    qreal a;

    a = (ft*360/1000) % 360;
    this->hundreds.setRotation(a);

    a = (ft*360/10000) % 360;
    this->thousands.setRotation(a);

    a = (ft*360/100000) % 360;
    this->tenthousands.setRotation(a);
}

void AltimeterWidget::setAltitudeMtr(long mtr)
{
    this->setAltitudeFt(mtr * MTR_TO_FT);
}

void AltimeterWidget::wheelEvent(QWheelEvent *event)
{
    return;
}
