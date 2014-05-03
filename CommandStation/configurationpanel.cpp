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
#include <QDateTime>

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
}

ConfigurationPanel::~ConfigurationPanel()
{
    delete ui;
}

void ConfigurationPanel::on_getConfigFromDBButton_clicked()
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

        // ---- get drones list ----
        QString queryStr("SELECT id, ip, port, date, info FROM " DCP_DBSTATIONS
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
                userData.insert("date", QVariant(query.value(3).toDateTime()));
                userData.insert("info", QVariant(query.value(4).toString()));

                this->ui->dronesListComboBox->addItem(
                            query.value(0).toString() + " -- " +
                            query.value(4).toString(), userData);
            }
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

        // ---- get central station ----
        queryStr.clear();
        queryStr = "SELECT id, ip, port, date, info FROM " DCP_DBSTATIONS
                " WHERE type='central'";
        query.clear();
        if(query.exec(queryStr))
        {
            if(query.next())
            {
                ui->centralIPText->setText(query.value(1).toString());
                ui->centralPortText->setText(query.value(2).toString());
                ui->centralDateText->setText(query.value(3).toString());
                ui->centralInfoText->setText(query.value(4).toString());
            }
            else
            {
                msgBox.setInformativeText("No central station registered in database.");
                msgBox.exec();
                this->db.close();
                return;
            }
        }
        else
        {
            msgBox.setInformativeText("Error while executing query to " \
                                      "retreive central station:\n" +
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
    this->db.close();
}

void ConfigurationPanel::on_dronesListComboBox_currentIndexChanged()
{
    if(ui->dronesListComboBox->currentIndex() != 0)
    {
        QMap<QString, QVariant> userData =
            this->ui->dronesListComboBox->currentData().toMap();
        this->ui->droneIPText->setText(userData.value("ip").toString());
        this->ui->dronePortText->setText(userData.value("port").toString());
        this->ui->droneDateText->setText(userData.value("date").toString());
        this->ui->droneInfoText->setText(userData.value("info").toString());
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
    if(ui->commandInfoText->text().isEmpty())
    {
        msgBox.setInformativeText("Please set Informative text for command Station.");
        msgBox.exec();
        return;
    }
    else
    {
        this->cmdP->dcpServerInfo = ui->commandInfoText->text();
    }

    // ---- Central Station setup ----
    if(this->ui->centralIPText->text().isEmpty())
    {
        msgBox.setInformativeText("Please fetch data from Database first.");
        msgBox.exec();
        return;
    }
    else
    {
        this->cmdP->centralStationHost = QHostAddress(ui->centralIPText->text());
        this->cmdP->centralStationPort = ui->centralPortText->text().toInt();
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
