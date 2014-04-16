/*
 *  This file is part of the CommandStation Project
 *  Copyright (C) 15/04/2014 -- mplayerwidget.h -- bertrand
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

#ifndef MPLAYERWIDGET_H
#define MPLAYERWIDGET_H

#include <QWidget>
#include <QUrl>
#include <QStringList>
#include <QProcess>

class MPlayerWidget : public QWidget
{
    Q_OBJECT
public:
    explicit    MPlayerWidget(QWidget *parent = 0);
    ~MPlayerWidget();

    void        setMediaFile(QUrl url);
    void        start();
    QByteArray  readAllStandardError();
    QByteArray  readAllStandardOutput();
    QByteArray  readLineStandardError();
    QByteArray  readLineStandardOutput();

    QStringList getArgs() const;
    QUrl        getMediaFileURL() const;

signals:
    void        readyReadStandardError();
    void        readyReadStandardOutput();
    void        finished(int,QProcess::ExitStatus);
    void        error(QProcess::ProcessError);

public slots:

private:
    QUrl        mediaFileURL;
    QStringList args;
    QProcess    mplayer;

};

#endif // MPLAYERWIDGET_H
