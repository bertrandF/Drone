/*
 *  This file is part of the CommandStation Project
 *  Copyright (C) 15/04/2014 -- mplayerwidget.cpp -- bertrand
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

#include "mplayerwidget.h"

#include <QDebug>


MPlayerWidget::MPlayerWidget(QWidget *parent) :
    QWidget(parent)
{   
    this->args << "-nogui";
//    this->mplayerArgs << "-noconsolecontrols";
//    this->mplayerArgs << "-nojoystick";
//    this->mplayerArgs << "-nomouseinput";
//    this->mplayerArgs << "-idle";
    this->args << "-slave";
    this->args << "-cache" << "512";
//    this->mplayerArgs << "-noborder";
    this->args << "-wid" << QString::number(this->winId());
    this->args << "-msglevel" << "all=9";

    connect(&(this->mplayer), SIGNAL(readyReadStandardError()), this,\
            SIGNAL(readyReadStandardError()));
    connect(&(this->mplayer), SIGNAL(readyReadStandardOutput()), this,\
            SIGNAL(readyReadStandardOutput()));
    connect(&(this->mplayer), SIGNAL(finished(int,QProcess::ExitStatus)), this,\
            SIGNAL(finished(int,QProcess::ExitStatus)));
    connect(&(this->mplayer), SIGNAL(error(QProcess::ProcessError)), this,\
            SIGNAL(error(QProcess::ProcessError)));
}

MPlayerWidget::~MPlayerWidget()
{
    this->mplayer.kill();
    this->mplayer.waitForFinished(-1);
}
void MPlayerWidget::setMediaFile(QUrl url)
{
    if(!url.isValid()) {
        qWarning() << "WARN: MPlayer invalid URL: '" << url.toString() \
                   << "'." << endl;
        return;
    }
    this->mediaFileURL = url;
}

void MPlayerWidget::start()
{
    this->args << this->mediaFileURL.toDisplayString(QUrl::PreferLocalFile);
    this->mplayer.start("mplayer", this->args);
}

QByteArray MPlayerWidget::readAllStandardError()
{
    return this->mplayer.readAllStandardError();
}

QByteArray MPlayerWidget::readAllStandardOutput()
{
    return this->mplayer.readAllStandardOutput();
}

QByteArray MPlayerWidget::readLineStandardError()
{
    this->mplayer.setReadChannel(QProcess::StandardError);
    return this->mplayer.readLine();
}

QByteArray MPlayerWidget::readLineStandardOutput()
{
    this->mplayer.setReadChannel(QProcess::StandardOutput);
    return this->mplayer.readLine();
}

/* --------------- */
/* --- GETTERS --- */
/* --------------- */
QStringList MPlayerWidget::getArgs() const
{
    return args;
}

QUrl MPlayerWidget::getMediaFileURL() const
{
    return mediaFileURL;
}







