/*
 *  This file is part of the Drone project
 *  Copyright (C) 11/05/2014 -- videotab.cpp -- bertrand
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

#include "videotab.h"
#include "ui_videotab.h"

#include <QDebug>

VideoTab::VideoTab(QString mediaFile, QWidget *parent) :
    QWidget(parent),
    mediaFile(mediaFile),
    ui(new Ui::videoTab)
{
    ui->setupUi(this);

    player = ui->videoWidget;

    connect(this->player, SIGNAL(error(QProcess::ProcessError)), this,\
            SLOT(mplayerProcessError(QProcess::ProcessError)));
    connect(this->player, SIGNAL(finished(int,QProcess::ExitStatus)), this,\
            SLOT(mplayerProcessFinished(int,QProcess::ExitStatus)));
    connect(this->player, SIGNAL(readyReadStandardError()), this,\
            SLOT(mplayerReadyReadError()));
    // The following line spams the log text box, so unconnected.
    //connect(this->mpw, SIGNAL(readyReadStandardOutput()), this, SLOT(mplayerReadyReadOutput()));

    player->setMediaFile(mediaFile);
    player->start();
}

VideoTab::~VideoTab()
{
    delete ui;
}


void VideoTab::mplayerReadyReadError()
{
    QTextEdit*  log = this->ui->logBox;
    QByteArray  line;
    QColor prevColor = log->textColor();

    log->setTextColor(Qt::red);
    line = player->readLineStandardError();
    while(line.length() > 0) {
        line.remove(line.length()-1, 1); // remove '\n'
        log->append(QString(line));
        line = player->readLineStandardError();
    }
    log->setTextColor(prevColor);
}

void VideoTab::mplayerReadyReadOutput()
{
    QTextEdit*  log = this->ui->logBox;
    QByteArray  line;
    QColor prevColor = log->textColor();

    log->setTextColor(Qt::green);
    line = player->readLineStandardOutput();
    while(line.length() > 0) {
        line.remove(line.length()-1, 1); // remove '\n'
        log->append(QString(line));
        line = player->readLineStandardOutput();
    }
    log->setTextColor(prevColor);
}

void VideoTab::mplayerProcessError(QProcess::ProcessError err)
{
    switch (err) {
    case QProcess::Crashed:
        qWarning("WARN: MPlayer Crashed. Attempting restart...");
        this->player->start();
        break;
    case QProcess::FailedToStart:
        qWarning("WARN: MPlayer Failed to start. Attempting restart...");
        this->player->start();
        break;
    case QProcess::ReadError:
    case QProcess::WriteError:
        qWarning("WARN: MPlayer IO error.");
        break;
    case QProcess::Timedout:
        qWarning("WARN: MPlayer start timed out. Attempting restart...");
        this->player->start();
        break;
    case QProcess::UnknownError:
        qWarning("WARN: MPlayer unknown error.");
        break;
    default:
        qWarning() << "WARN: MPlayer unknown QProcessError (" \
                   << QString::number(err) << ")." << endl;
        break;
    }
}

void VideoTab::mplayerProcessFinished(int exitCode, \
                                            QProcess::ExitStatus exitStatus)
{
    switch (exitStatus) {
    case QProcess::NormalExit:
        qWarning("WARN: MPlayer Stopped.");
        break;
    case QProcess::CrashExit:
        qWarning("WARN: MPlayer crash exited.");
        break;
    default:
        qWarning() << "WARN: MPlayer unknown ExitStatus (" << exitStatus \
                   << ")." << endl;
        break;
    }
    qWarning() << "WARN: MPlayer exit code: (" << exitCode << ")." << endl;
}
