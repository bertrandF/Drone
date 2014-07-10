/*
 *  This file is part of the Drone project
 *  Copyright (C) 11/05/2014 -- videotab.h -- bertrand
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

#ifndef VIDEOTAB_H
#define VIDEOTAB_H

#include <QWidget>

#include "sdlplayerwidget.h"

namespace Ui {
class videoTab;
}

class VideoTab : public QWidget
{
    Q_OBJECT

public:
    explicit VideoTab(QString mediaFile, QWidget *parent = 0);
    ~VideoTab();

    SDLPlayerWidget *player;
    QString         mediaFile;

public slots:
    void    playerProcessFinished   (int exitCode,
                                     QProcess::ExitStatus exitStatus);
    void    playerProcessError      (QProcess::ProcessError err);
    void    playerReadyReadError    ();
    void    playerReadyReadOutput   ();

private:
    Ui::videoTab *ui;
};

#endif // VIDEOTAB_H
