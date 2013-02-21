#ifndef MAINWINDOW_H
#define MAINWINDOW_H

/*
 *
 *  Copyright (c) 2013
 *  name : mhogo mchungu
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

#include <QMainWindow>
#include <QFile>
#include <QString>
#include <QStringList>
#include <QDir>
#include <QIcon>
#include <QAction>
#include <QKeySequence>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QCloseEvent>
#include <QAction>
#include <QFileDialog>
#include <QMetaMethod>
#include <QDebug>
#include <QCoreApplication>
#include <QTimer>
#include <QDateTime>
#include <kstatusnotifieritem.h>
#include <kmenu.h>
#include "check_updates.h"
#include "logwindow.h"
#include <kstandarddirs.h>
#include <cstdlib>
#include <cstdio>

class qtUpdateNotifier : public KStatusNotifierItem
{
	Q_OBJECT
public:
	explicit qtUpdateNotifier();
	~qtUpdateNotifier();
	void start( void ) ;
	void logActivity( QString ) ;
signals:
	void updateLogWindow( void ) ;
public slots:
	void startSynaptic( void ) ;
private slots:
	void run( void ) ;
	void closeApp( int ) ;
	void closeApp( void ) ;
	void updateList( QStringList ) ;
	void threadTerminated( void ) ;
	void threadisFinished( void ) ;
	void closeApplication( void ) ;
	void changeIcon( QString ) ;
	void checkForUpdates( void ) ;
	void checkForUpdatesOnStartUp( void ) ;
	void _activate( QPoint & );
	void _activateRequested( bool state,const QPoint &pos );
	void scheduleUpdates( int ) ;
	void updatesFound( int,QStringList ) ;
	void logWindowShow( void ) ;
	void doneUpdating( void ) ;
private:
	u_int64_t getCurrentTime( void ) ;
	u_int64_t getTimeFromConfigFile( void ) ;
	void createEnvironment( void ) ;
	void writeUpdateTimeToConfigFile( void ) ;
	void showToolTip( QString,QString,QStringList ) ;
	void showToolTip( QString,QString,QString ) ;
	void showToolTip( QString,QString,int ) ;
	void showToolTip( QString,QString ) ;
	void showToolTip( int ) ;
	QString nextUpdateTime( void ) ;
	QString nextUpdateTime( int ) ;
	QString logMsg( int ) ;
	bool m_canCloseApplication ;
	bool m_threadIsRunning ;
	QStringList m_updatesList ;
	check_updates * m_updates ;
	KMenu * m_trayMenu ;
	QTimer * m_timer ;
	QString m_configTime ;
	QString m_configLog ;
	u_int64_t m_sleepDuration ;
	u_int64_t m_currentTime ;
	QString m_configPath ;
};

#endif // MAINWINDOW_H
