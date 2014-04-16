/*
 *  This file is part of the CommandStation Project
 *  Copyright (C) 15/04/2014 -- horizonwidget.cpp -- bertrand
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

#include "horizonwidget.h"

/*
 * There is and HardCoded margin of 2 pixels in QGraphicsView
 * We need to remove it when scaling so that the image perfectly
 * fits the display rect and do not go under the margin which will
 * unable scrolling.
 */
#define SHITTY_MARGIN   (2)

HorizonWidget::HorizonWidget(QWidget *parent) :
    QGraphicsView(parent)
{
    // TODO: size of QGraphicsView (150x150) is hardcoded -> get it whith size()
    // TODO: We need to draw the Pixmap when the QGraphicsView is shown (before
    // TODO: size() returns bad numbers).
    this->setScene(&(this->scene));
    //this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    //this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    // Background
    this->back.setPixmap(QPixmap(":/images/Horizon.png"));
    this->back.setTransformOriginPoint(475, 1440);
    this->scene.addItem(&(this->back));

    this->centerOn(&(this->back));
    this->show();
}

void HorizonWidget::setPitch(float pitch)
{
    this->centerOn(950/2, pitch*(-8)+1440);
}

void HorizonWidget::setRoll(float roll)
{

}

void HorizonWidget::wheelEvent(QWheelEvent *event)
{
    return;
}

void HorizonWidget::paintEvent(QPaintEvent *event)
{
    QGraphicsView::paintEvent(event);

//    QPainter painter(this);
//    painter.begin(this);
//    painter.drawPixmap(this->mapToScene(120,178).toPoint(), QPixmap(":/images/Horizon_plane.png"));
//    painter.end();
}

void HorizonWidget::drawForeground(QPainter *painter, const QRectF &rect)
{
    //QGraphicsView::drawForeground(painter, rect);
    //QRect exposedRect(graphicsView.mapToScene(0,0).toPoint(), graphicsView.viewport()->rect().size());
    painter->drawPixmap(this->mapToScene(120,178).toPoint(), QPixmap(":/images/Horizon_plane.png"));
}
