/*
 *  This file is part of the CommandStation Project
 *  Copyright (C) 15/04/2014 -- mainwindow.cpp -- bertrand
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

#include <QMessageBox>
#include <QSizePolicy>
#include <QSpacerItem>
#include <QGridLayout>

#include "constants.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    this->confPanel = new ConfigurationPanel(this->centralWidget());
    this->cmdPanel  = NULL;

    connect(this->confPanel, SIGNAL(signal_configuration_done(CommandStationParameters*)),\
                this, SLOT(doneConfiguration(CommandStationParameters*)));
}

MainWindow::~MainWindow()
{
    delete ui;
}

/* ------------- */
/* --- SLOTS --- */
/* ------------- */
void MainWindow::on_actionAbout_triggered()
{
    QSizePolicy sPolicy;
    sPolicy.setHorizontalPolicy(QSizePolicy::Expanding);
    sPolicy.setVerticalPolicy(QSizePolicy::Expanding);

    QMessageBox msgBox(this);
    msgBox.setIcon(QMessageBox::Information);
    msgBox.setMaximumWidth(1000);
    msgBox.setWindowTitle("About");
    msgBox.setInformativeText("Drone Command Station\n"\
                              "Version: " VERSION_MAJOR "."\
                              VERSION_MINOR);
    msgBox.setStandardButtons(QMessageBox::Close);
    msgBox.setDefaultButton(QMessageBox::Close);
    msgBox.setSizePolicy(sPolicy);
    msgBox.adjustSize();


    msgBox.exec();
}

void MainWindow::doneConfiguration(CommandStationParameters *cmdP)
{
    delete this->confPanel;
    this->confPanel = NULL;

    this->cmdPanel = new CommandPanel(cmdP, this->centralWidget());
}
