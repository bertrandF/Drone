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
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>

#include "constants.h"
#include "dcp.h"
#include "commandstationparameters.h"

#define DRONESCOMBOBOX_WELCOMEMSG   " -- Please select a drone -- "

ConfigurationPanel::ConfigurationPanel(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ConfigurationPanel)
{
    QNetworkInterface interface;

    ui->setupUi(this);

    this->cmdP = new CommandStationParameters();

    ui->dcpServerInterface->addItem("AddressAny ( * )");
    foreach (interface, QNetworkInterface::allInterfaces()) {
        if(!interface.addressEntries().isEmpty())
        {   
            QString text = interface.name().
                    append(" ( ").
                    append(interface.addressEntries().first().ip().toString()).
                    append(" ) ");
            QVariant userData = QVariant(
                        interface.addressEntries().first().ip().toString() );
            ui->dcpServerInterface->addItem(text, userData);
        }
    }

    this->db = QSqlDatabase::addDatabase("QPSQL", DBREADER_CONNECTIONNAME);
    this->ui->dronesListComboBox->addItem(DRONESCOMBOBOX_WELCOMEMSG);

    connect(this->ui->frontVideoUrl, SIGNAL(textEdited(QString)), this, SLOT(confUpdate(QString)));
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

void ConfigurationPanel::on_getDronesListButton_clicked()
{
    QMessageBox msgBox;
    msgBox.setIcon(QMessageBox::Information);
    msgBox.setMaximumWidth(1000);
    msgBox.setWindowTitle("Error");
    msgBox.setStandardButtons(QMessageBox::Close);
    msgBox.setDefaultButton(QMessageBox::Close);

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
        this->cmdP->dbServerHost = info.addresses().first();
        this->cmdP->dbServerPort = this->ui->dbServerPort->value();
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
        this->cmdP->dbName = this->ui->dbName->text();
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
        this->cmdP->dbUserName = this->ui->dbUserName->text();
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
        this->cmdP->dbUserPassword = this->ui->dbUserPassword->text();
    }

    // ---- CONNECT TO DB ----
    this->db.setHostName(this->cmdP->dbServerHost.toString());
    this->db.setPort(this->cmdP->dbServerPort);
    this->db.setDatabaseName(this->cmdP->dbName);
    this->db.setUserName(this->cmdP->dbUserName);
    this->db.setPassword(this->cmdP->dbUserPassword);

    if(db.open())
    {
        this->ui->dronesListComboBox->clear();
        this->ui->dronesListComboBox->addItem(DRONESCOMBOBOX_WELCOMEMSG);

        QString queryStr("SELECT id, ip, port, info FROM " DCP_DBSTATIONS
                         " WHERE type='drone'");
        QSqlQuery query(db);
        if(query.exec(queryStr))
        {
            while(query.next())
            {
                QMap<QString, QVariant> userData;
                userData.insert("id", QVariant(query.value(0).toInt()));
                userData.insert("ip", QVariant(query.value(1).toString()));
                userData.insert("port", QVariant(query.value(2).toInt()));
                userData.insert("info", QVariant(query.value(3).toString()));

                this->ui->dronesListComboBox->addItem(
                            query.value(0).toString() + " -- " +
                            query.value(3).toString(), userData);
            }
            this->db.close();
        }
        else
        {
            msgBox.setInformativeText("Error while executing query to " \
                                      "retreive drone list:\n" +
                                      query.lastError().driverText() + "\n" +
                                      query.lastError().databaseText() + "\n" +
                                      "Query was:\n" + queryStr);
            msgBox.exec();
            this->db.close();
            return;
        }
    }
    else
    {
        msgBox.setInformativeText("Cannot open DB!\nCheck your configuration.");
        msgBox.exec();
        return;
    }
}

void ConfigurationPanel::on_nextButton_clicked()
{
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
        this->cmdP->frontVideoFeed = QUrl(this->ui->frontVideoUrl->text());
    }

    // ---- DCP Server setup ----
    if(ui->dcpServerInterface->currentIndex() == 0)
       this->cmdP->dcpServerHost = QHostAddress::Any;
    else
    {
        QString addr = this->ui->dcpServerInterface->currentData().toString();
        this->cmdP->dcpServerHost = QHostAddress(addr);
    }
    this->cmdP->dcpServerPort = this->ui->dcpServerPort->value();



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
        this->cmdP->centralStationHost = info.addresses().first();
        this->cmdP->centralStationPort = this->ui->centralStationPort->value();
    }

    // ---- Check that a drone is selected ----
    if(this->ui->dronesListComboBox->currentIndex() == 0)
    {
        msgBox.setInformativeText("Please select a drone.");
        msgBox.exec();
        return;
    }
    else
    {
        QMap<QString, QVariant> userData =
            this->ui->dronesListComboBox->currentData().toMap();
        this->cmdP->droneId     = userData.value("id").toInt();
        this->cmdP->droneHost   = QHostAddress(userData.value("ip").toString());
        this->cmdP->dronePort   = userData.value("port").toInt();
        this->cmdP->droneInfo   = userData.value("info").toString();
    }

    emit signal_configuration_done(this->cmdP);
}
