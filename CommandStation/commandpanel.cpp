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
#include "videotab.h"

#include <QtGlobal>
#include <QThread>
#include <QtSql/QSqlDatabase>
#include <QSqlError>
#include <QMessageBox>

#include <dcpservercommand.h>


CommandPanel::CommandPanel(CommandStationParameters *cmdP, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CommandPanel),
    cmdP(cmdP)
{
    /* ----- UI SETUP ----- */
    ui->setupUi(this);

    this->show(); // Need this to raise the panel at the foreground !!!

    this->logBox = this->ui->logTextBox;

    ui->compass->setBearing(74);
    ui->artificialHorizon->setRollPitch(33, 10);
    ui->altimeter->setAltitude(265);

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

    /* ----- VIDEO FEED ----- */
    foreach (QString video, cmdP->droneVideos) {
        if(video.isEmpty())
            continue;
        VideoTab *tab = new VideoTab(video);
        ui->videosTabWidget->addTab(tab, video);
    }

    /* ----- MOUSE POS ----- */
    this->initPos = QCursor::pos();

    /* ----- WARNING ----- */
    QMessageBox msgBox;
    msgBox.setIcon(QMessageBox::Warning);
    msgBox.setMaximumWidth(1000);
    msgBox.setWindowTitle("!!! WARNING !!!");
    msgBox.setStandardButtons(QMessageBox::Close);
    msgBox.setDefaultButton(QMessageBox::Close);
    msgBox.setInformativeText("Implementation of aileron command from mouse pos"
                              "is FALSE. Must re-implement.");
}

CommandPanel::~CommandPanel()
{
    delete ui;
}

#define MOUSE_MOV_STEP  (10)
void CommandPanel::mouseMoveEvent(QMouseEvent *event)
{
    if(event->button() == Qt::RightButton) {
        QPoint currMov = this->initPos - event->globalPos();
        currMov /= MOUSE_MOV_STEP;

        /* TODO: MAKE A REAL IMPLEMENTATION */
        qint8 aileronR = currMov.x() + currMov.y();
        qint8 aileronL = -currMov.x() + currMov.y();

        this->commandServer->sendCommandAilerons(aileronR, aileronL, 0);
    }
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
    while(this->commandServer->getStatus() == Disconnecting)
        QCoreApplication::processEvents();

    this->commandServer->sayByeBye();
    this->logBox->append("Waiting state STOPPED ...");
    while(this->commandServer->getStatus() == Stopping)
        QCoreApplication::processEvents();
    this->logBox->append("Exit process done ...");
}
