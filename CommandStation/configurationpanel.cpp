/*
 *  This file is part of the CommandStation Project
 *  Copyright (C) 15/04/2014 -- configurationpanel.cpp -- bertrand
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

#include "configurationpanel.h"
#include "ui_configurationpanel.h"

#include <QMessageBox>
#include <QHostInfo>

#include "commandstationparameters.h"

ConfigurationPanel::ConfigurationPanel(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ConfigurationPanel)
{
    ui->setupUi(this);

    connect(this->ui->frontVideoUrl, SIGNAL(textEdited(QString)), this, SLOT(confUpdate(QString)));
    connect(this->ui->dcpServerHost, SIGNAL(textEdited(QString)), this, SLOT(confUpdate(QString)));
    connect(this->ui->dbServerHost, SIGNAL(textEdited(QString)), this, SLOT(confUpdate(QString)));
    connect(this->ui->dbName, SIGNAL(textEdited(QString)), this, SLOT(confUpdate(QString)));
    connect(this->ui->dbUserName, SIGNAL(textEdited(QString)), this, SLOT(confUpdate(QString)));
    connect(this->ui->dbUserPassword, SIGNAL(textEdited(QString)), this, SLOT(confUpdate(QString)));
}

ConfigurationPanel::~ConfigurationPanel()
{
    delete ui;
}

void ConfigurationPanel::confUpdate(QString)
{

}

void ConfigurationPanel::on_nextButton_clicked()
{
    CommandStationParameters cmdP;

    QMessageBox msgBox;
    msgBox.setIcon(QMessageBox::Information);
    msgBox.setMaximumWidth(1000);
    msgBox.setWindowTitle("Error");
    msgBox.setStandardButtons(QMessageBox::Close);
    msgBox.setDefaultButton(QMessageBox::Close);

    // ---- FRONT VIDEO FEED URL ----
    if(this->ui->frontVideoUrl->text().isEmpty())
    {
        msgBox.setInformativeText("Front Video Feed URL is empty !");
        msgBox.exec();
        return;
    }
    else
    {
        QUrl frontVideoFeed(this->ui->frontVideoUrl->text());
        if(!frontVideoFeed.isValid())
        {
            msgBox.setInformativeText("Front Video Feed URL is not valid !");
            msgBox.exec();
            return;
        }
        cmdP.frontVideoFeed = QUrl(this->ui->frontVideoUrl->text());
    }

    // ---- DCP Server setup ----
    if(this->ui->dcpServerHost->text().isEmpty())
    {
        msgBox.setInformativeText("DCP Server IP is empty !");
        msgBox.exec();
        return;
    }
    else
    {
        // TODO: only allow IP or change Widget for selecting interface
        QHostInfo info = QHostInfo::fromName(this->ui->dcpServerHost->text());
        if(info.addresses().isEmpty())
        {
            msgBox.setInformativeText("DCP Server IP is not valid !");
            msgBox.exec();
            return;
        }
        cmdP.dcpServerHost = info.addresses().first();
        cmdP.dcpServerPort = this->ui->dcpServerPort->value();
    }

    // ---- DB Server setup ----
    // Host
    if(this->ui->dbServerHost->text().isEmpty())
    {
        msgBox.setInformativeText("DB Server IP is empty !");
        msgBox.exec();
        return;
    }
    else
    {
        QHostInfo info = QHostInfo::fromName(this->ui->dbServerHost->text());
        if(info.addresses().isEmpty())
        {
            msgBox.setInformativeText("DB Server IP is not valid !");
            msgBox.exec();
            return;
        }
        cmdP.dbServerHost = info.addresses().first();
        cmdP.dbServerPort = this->ui->dbServerPort->value();
    }
    // DB Name
    if(this->ui->dbName->text().isEmpty())
    {
        msgBox.setInformativeText("DB Name is empty !");
        msgBox.exec();
        return;
    }
    else
    {
        cmdP.dbName = this->ui->dbName->text();
    }
    // User name
    if(this->ui->dbUserName->text().isEmpty())
    {
        msgBox.setInformativeText("DB User Name is empty !");
        msgBox.exec();
        return;
    }
    else
    {
        cmdP.dbUserName = this->ui->dbUserName->text();
    }
    // User Password
    if(this->ui->dbUserPassword->text().isEmpty())
    {
        msgBox.setInformativeText("DB User Password is empty !");
        msgBox.exec();
        return;
    }
    else
    {
        cmdP.dbUserPassword = this->ui->dbUserPassword->text();
    }

    // ---- Central Station setup ----
    if(this->ui->centralStationHost->text().isEmpty())
    {
        msgBox.setInformativeText("Central Station Host is empty !");
        msgBox.exec();
        return;
    }
    else
    {
        QHostInfo info = QHostInfo::fromName(this->ui->centralStationHost->text());
        if(info.addresses().isEmpty())
        {
            msgBox.setInformativeText("Central Station host is not valid !");
            msgBox.exec();
            return;
        }
        cmdP.centralStationHost = info.addresses().first();
        cmdP.centralStationPort = this->ui->centralStationPort->value();
    }


    emit signal_configuration_done(cmdP);
}
