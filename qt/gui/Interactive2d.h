/*  Copyright (C) 2001 Le Coq Loïc
*    This file is part of QCAS.
*
*    QCAS is free software: you can redistribute it and/or modify
*    it under the terms of the GNU General Public License as published by
*    the Free Software Foundation, either version 3 of the License, or
*    (at your option) any later version.
*
*    QCAS is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*    GNU General Public License for more details.
*
*    You should have received a copy of the GNU General Public License
*    along with Foobar.  If not, see <http://www.gnu.org/licenses/>
*/

#ifndef INTERACTIVE2D_H
#define INTERACTIVE2D_H
#include <QObject>
#include <QStyle>
#include <QWindowsStyle>

#include <QWidget>
#include "gui/CentralTabWidget.h"
class MainWindow;
class QToolBar;
class QPushButton;
class QToolButton;
class QAction;
class MainWindow;
class IconSize : public QWindowsStyle
 {
   Q_OBJECT
public:
    explicit IconSize(QWindowsStyle *parent = 0);
    int pixelMetric(PixelMetric metric, const QStyleOption * option = 0, const QWidget * widget = 0 ) const;
signals:
public slots:
};

class Interactive2d:public QWidget,MainSheet{
    Q_OBJECT
public:
Interactive2d(MainWindow *parent=0);
private:
    MainWindow* mainWindow;
    QToolBar* toolBar;
    QWidget* canvas;
    QToolButton* buttonPt;
    QToolButton* buttonLine;
    QToolButton* buttonCircle;
    QAction* singlept;
    QAction * inter;
    QAction* midpoint;
    QAction* line;
    QAction* halfline;
    QAction* segment;
    QAction* circle2pt;
    QAction* circle3pt;


    void initGui();

};


#endif // INTERACTIVE2D_H
