/*
 *  This file is part of the CommandStation Project
 *  Copyright (C) 15/04/2014 -- commandpanel.h -- bertrand
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

#ifndef COMMANDPANEL_H
#define COMMANDPANEL_H

#include <QWidget>
#include <QProcess>
#include <QTextEdit>
#include <QSqlDatabase>

#include <dcpserverbackendremote.h>

#include "commandstationparameters.h"
#include "mplayerwidget.h"



namespace Ui {
class CommandPanel;
}

class CommandPanel : public QWidget
{
    Q_OBJECT

public:
    explicit CommandPanel(CommandStationParameters *cmdP, QWidget *parent = 0);
    ~CommandPanel();

public slots:
    void    mplayerProcessFinished      (int exitCode, \
                                            QProcess::ExitStatus exitStatus);
    void    mplayerProcessError         (QProcess::ProcessError err);
    void    mplayerReadyReadError       ();
    void    mplayerReadyReadOutput      ();
    void    srvBackendStatusChanged     (
            enum DCPServerCommandStatus status);

private:
    Ui::CommandPanel    *ui;
    MPlayerWidget       *mpw;
    QTextEdit           *logBox;
    CommandStationParameters    *cmdP;
    QSqlDatabase        db;
    DCPServerCommand *srvBack;
};

#endif // COMMANDPANEL_H
