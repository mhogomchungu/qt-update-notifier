/*
 *
 *  Copyright (c) 2013
 *  name : Francis Banyikwa
 *  email: mhogomchungu@gmail.com
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */


#ifndef LOGWINDOW_H
#define LOGWINDOW_H

#include <QWidget>
#include <QFile>
#include <QString>
#include <QCloseEvent>
#include <QEvent>
#include <QKeyEvent>

#include "utility.h"

namespace Ui {
class logWindow;
}

class logWindow : public QWidget
{
	Q_OBJECT
public:
	explicit logWindow( QString = QString(),QWidget * parent = 0 ) ;
	~logWindow() ;
	void showLogWindow( void ) ;
	void showAptGetWindow( void ) ;
private slots:
	void pbClearLog( void ) ;
	void pbQuit( void ) ;
	void updateLogWindow( void ) ;
	void updateLogWindow_1( void ) ;
private:
	void closeEvent( QCloseEvent * ) ;
	bool eventFilter( QObject * watched,QEvent * event ) ;
	Ui::logWindow * m_ui;
	QString m_logFile ;
	QString m_logPath ;
};

#endif // LOGWINDOW_H
