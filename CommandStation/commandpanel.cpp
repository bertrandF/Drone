/*
 *  This file is part of the CommandStation Project
 *  Copyright (C) 15/04/2014 -- commandpanel.cpp -- bertrand
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

#include <QDebug>
#include <iostream>
#include <cstdio>

#include "commandpanel.h"
#include "ui_commandpanel.h"

#include <QThread>
#include <QtSql/QSqlDatabase>

#include <dcpserverbackendremote.h>
#include <dcpserver.h>


CommandPanel::CommandPanel(CommandStationParameters cmdP, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CommandPanel),
    cmdP(cmdP)
{
    /* ----- UI SETUP ----- */
    ui->setupUi(this);

    this->mpw = this->ui->frontVideoWidget;
    connect(this->mpw, SIGNAL(error(QProcess::ProcessError)), this,\
            SLOT(mplayerProcessError(QProcess::ProcessError)));
    connect(this->mpw, SIGNAL(finished(int,QProcess::ExitStatus)), this,\
            SLOT(mplayerProcessFinished(int,QProcess::ExitStatus)));
    connect(this->mpw, SIGNAL(readyReadStandardError()), this,\
            SLOT(mplayerReadyReadError()));
    // The following line spams the log text box, so unconnected.
    //connect(this->mpw, SIGNAL(readyReadStandardOutput()), this, SLOT(mplayerReadyReadOutput()));
    //this->mpw->setMediaFile(QUrl::fromLocalFile("/home/bertrand/Desktop/Goodfellas.avi"));
    this->mpw->setMediaFile(cmdP.frontVideoFeed);
    this->mpw->start();

    this->show(); // Need this to raise the panel at the foreground !!!

    this->logBox = this->ui->logTextBox;

    ui->compassWidget->setBearing(153);
    ui->altimeterWidget->setAltitudeFt(1586);

    /* ----- DCP SERVER ----- */
    QUdpSocket *sock = new QUdpSocket();
    qDebug() << "bind= " << sock->bind(cmdP.dcpServerHost, cmdP.dcpServerPort);
    this->dcpServer = new DCPServer(sock);
    DCPServerBackendRemote *srvBack = new DCPServerBackendRemote();
    srvBack->registerWithServer(this->dcpServer);
    srvBack->setCentralStationHost(cmdP.centralStationHost,
                                   cmdP.centralStationPort);
    srvBack->sayHello("Welcome to my world !");


    /* ----- DB SERVER ----- */
    QSqlDatabase db = QSqlDatabase::addDatabase("QPSQL");
    db.setHostName(cmdP.dbServerHost.toString());
    db.setPort(cmdP.dbServerPort);
    db.setDatabaseName(cmdP.dbName);
    db.setUserName(cmdP.dbUserName);
    db.setPassword(cmdP.dbUserPassword);

    if(db.open())
    {
        qDebug() << db.tables();

    }
    else
        qDebug("DB error !!!");

}

CommandPanel::~CommandPanel()
{
    delete ui;
}


/* ------------- */
/* --- SLOTS --- */
/* ------------- */
//void CommandPanel::stdoutReadyRead()
//{
//    QTextEdit*  log = this->logBox;
//    QByteArray line;
//    QColor prevColor = log->textColor();

//    log->setTextColor(Qt::green);
//    line = this->stdoutFile.readLine();
//    while(line.length() > 0) {
//        line.remove(line.length()-1, 1); // remove '\n'
//        log->append(QString(line));
//        line = this->stdoutFile.readLine();
//    }
//    log->setTextColor(prevColor);
//}

void CommandPanel::mplayerReadyReadError()
{
    QTextEdit*  log = this->logBox;
    QByteArray  line;
    QColor prevColor = log->textColor();

    log->setTextColor(Qt::red);
    line = this->mpw->readLineStandardError();
    while(line.length() > 0) {
        line.remove(line.length()-1, 1); // remove '\n'
        log->append(QString(line));
        line = this->mpw->readLineStandardError();
    }
    log->setTextColor(prevColor);
}

void CommandPanel::mplayerReadyReadOutput()
{
    QTextEdit*  log = this->logBox;
    QByteArray  line;
    QColor prevColor = log->textColor();

    log->setTextColor(Qt::green);
    line = this->mpw->readLineStandardOutput();
    while(line.length() > 0) {
        line.remove(line.length()-1, 1); // remove '\n'
        log->append(QString(line));
        line = this->mpw->readLineStandardOutput();
    }
    log->setTextColor(prevColor);
}

void CommandPanel::mplayerProcessError(QProcess::ProcessError err)
{
    switch (err) {
    case QProcess::Crashed:
        qWarning("WARN: MPlayer Crashed. Attempting restart...");
        this->mpw->start();
        break;
    case QProcess::FailedToStart:
        qWarning("WARN: MPlayer Failed to start. Attempting restart...");
        this->mpw->start();
        break;
    case QProcess::ReadError:
    case QProcess::WriteError:
        qWarning("WARN: MPlayer IO error.");
        break;
    case QProcess::Timedout:
        qWarning("WARN: MPlayer start timed out. Attempting restart...");
        this->mpw->start();
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

void CommandPanel::mplayerProcessFinished(int exitCode, \
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
