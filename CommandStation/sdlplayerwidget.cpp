/*
 *  This file is part of the CommandStation Project
 *  Copyright (C) 15/04/2014 -- sdlplayerwidget.cpp -- bertrand
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

#include "sdlplayerwidget.h"

#include <QDebug>


SDLPlayerWidget::SDLPlayerWidget(QWidget *parent) :
    QWidget(parent)
{   


    this->args << "-w" << QString::number(this->winId());
    this->args << "-r" << "15";
    this->args << "-l" << "200";

    connect(&(this->SDLplayer), SIGNAL(readyReadStandardError()), this,\
            SIGNAL(readyReadStandardError()));
    connect(&(this->SDLplayer), SIGNAL(readyReadStandardOutput()), this,\
            SIGNAL(readyReadStandardOutput()));
    connect(&(this->SDLplayer), SIGNAL(finished(int,QProcess::ExitStatus)), this,\
            SIGNAL(finished(int,QProcess::ExitStatus)));
    connect(&(this->SDLplayer), SIGNAL(error(QProcess::ProcessError)), this,\
            SIGNAL(error(QProcess::ProcessError)));
}

SDLPlayerWidget::~SDLPlayerWidget()
{
    this->SDLplayer.kill();
    this->SDLplayer.waitForFinished(-1);
}
void SDLPlayerWidget::setMediaFile(QUrl url)
{
    if(!url.isValid()) {
        qWarning() << "WARN: player invalid URL: '" << url.toString() \
                   << "'." << endl;
        return;
    }
    this->mediaFileURL = url;
}

void SDLPlayerWidget::start()
{
    this->args << this->mediaFileURL.toDisplayString(QUrl::PreferLocalFile);
    this->SDLplayer.start("SDLplayer", this->args);
}

QByteArray SDLPlayerWidget::readAllStandardError()
{
    return this->SDLplayer.readAllStandardError();
}

QByteArray SDLPlayerWidget::readAllStandardOutput()
{
    return this->SDLplayer.readAllStandardOutput();
}

QByteArray SDLPlayerWidget::readLineStandardError()
{
    this->SDLplayer.setReadChannel(QProcess::StandardError);
    return this->SDLplayer.readLine();
}

QByteArray SDLPlayerWidget::readLineStandardOutput()
{
    this->SDLplayer.setReadChannel(QProcess::StandardOutput);
    return this->SDLplayer.readLine();
}

/* --------------- */
/* --- GETTERS --- */
/* --------------- */
QStringList SDLPlayerWidget::getArgs() const
{
    return args;
}

QUrl SDLPlayerWidget::getMediaFileURL() const
{
    return mediaFileURL;
}







