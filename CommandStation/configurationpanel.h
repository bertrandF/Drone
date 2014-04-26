/*
 *  This file is part of the CommandStation Project
 *  Copyright (C) 15/04/2014 -- configurationpanel.h -- bertrand
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

#ifndef CONFIGURATIONPANEL_H
#define CONFIGURATIONPANEL_H

#include <QWidget>
#include <QNetworkInterface>

#include "commandstationparameters.h"

namespace Ui {
class ConfigurationPanel;
}

class ConfigurationPanel : public QWidget
{
    Q_OBJECT

public slots:
    void on_nextButton_clicked();
    void on_getDronesListButton_clicked();
    void confUpdate(QString);

signals:
    void signal_configuration_done(CommandStationParameters *cmdP);

public:
    explicit ConfigurationPanel(QWidget *parent = 0);
    ~ConfigurationPanel();

private:
    Ui::ConfigurationPanel  *ui;
    CommandStationParameters *cmdP;

    class ComboBoxItem : public QString, public QNetworkInterface
    {
    public:
        ComboBoxItem(QString str, QNetworkInterface interface) :
            QString(str),
            QNetworkInterface(interface)
        {}
    };

    QList<ComboBoxItem*> interfaces;
};

#endif // CONFIGURATIONPANEL_H
