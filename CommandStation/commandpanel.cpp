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

#include "constants.h"
#include "commandpanel.h"
#include "ui_commandpanel.h"

#include <QtGlobal>
#include <QThread>
#include <QtSql/QSqlDatabase>
#include <QSqlError>

#include <dcpservercommand.h>


CommandPanel::CommandPanel(CommandStationParameters *cmdP, QWidget *parent) :
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
    this->mpw->setMediaFile(cmdP->frontVideoFeed);
    this->mpw->start();

    this->show(); // Need this to raise the panel at the foreground !!!

    this->logBox = this->ui->logTextBox;

    ui->compassWidget->setBearing(153);
    ui->altimeterWidget->setAltitudeFt(1586);

    /* ----- DCP SERVER ----- */
    QUdpSocket *sock = new QUdpSocket();
    sock->bind(cmdP->dcpServerHost, cmdP->dcpServerPort);
    this->commandServer = new DCPServerCommand(sock);
    this->commandServer->setDroneHost(cmdP->droneHost, cmdP->dronePort);
    this->commandServer->setCentralStationHost(cmdP->centralStationHost,
                                               cmdP->centralStationPort);
    connect(this->commandServer, SIGNAL(statusChanged(DCPServerCommandStatus)),
            this, SLOT(serverStatusChanged(DCPServerCommandStatus)));
    this->commandServer->sayHello(cmdP->dcpServerInfo);


    /* ----- DB SERVER ----- */
    this->db = QSqlDatabase::database(DBREADER_CONNECTIONNAME);

    if(!this->db.isOpen() && !db.open())
    {
        qCritical() << "CommandPanel: Cannot open DB.";
        qCritical() << "CommandPanel: " << db.lastError().driverText();
        qCritical() << "CommandPanel: " << db.lastError().databaseText();
    }

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

void CommandPanel::serverStatusChanged(
        enum DCPServerCommandStatus status)
{
    switch (status)
    {
    case Init:
        break;
    case SayingHello:
        break;
    case NotConnected:
        this->commandServer->connectToDrone(this->cmdP->droneId);
        break;
    case Connecting:
        break;
    case Connected:
        this->commandServer->log(DCPCommandLog::Info, "What is the Fuck !");
        //QThread::sleep(3);
        //this->commandServer->disconnectFromDrone();
        break;
    case Disconnecting:
        break;
    case Disconnected:
        //this->commandServer->sayByeBye();
        break;
    case Stopping:
        break;
    case Stopped:
        break;
    default:
        break;
    }
}

void CommandPanel::quit()
{
    this->logBox->setTextColor(Qt::green);
    this->logBox->append("\nEXITING ...");
    this->commandServer->disconnectFromDrone();
    this->logBox->append("Waiting state DICONNECTED ...");
    while(this->commandServer->getStatus() < Disconnected)
        QCoreApplication::processEvents();

    this->commandServer->sayByeBye();
    this->logBox->append("Waiting state STOPPED ...");
    while(this->commandServer->getStatus() < Stopped)
        QCoreApplication::processEvents();
    this->logBox->append("Exit process done ...");
}
