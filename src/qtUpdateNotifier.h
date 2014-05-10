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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>
#include <QStringList>
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
#include <cstdlib>
#include <cstdio>
#include <QTranslator>
#include <QLibraryInfo>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>

#include "logwindow.h"
#include "instance.h"
#include "desktop_file.h"
#include "task.h"
#include "configuredialog.h"
#include "qt-update-install-path.h"
#include "utility.h"
#include "settings.h"
#include "statusicon.h"

class qtUpdateNotifier : public statusicon
{
	Q_OBJECT
public:
	explicit qtUpdateNotifier() ;
	~qtUpdateNotifier() ;
	void start( void ) ;
	void logActivity( const QString& ) ;
	void logActivity_1( const QString& ) ;
	void setDebug( bool ) ;
	static bool autoStartEnabled( void ) ;
	static int instanceAlreadyRunning( void ) ;
signals:
	void updateLogWindow( void ) ;
	void configOptionsChanged_1( void ) ;
	void msg( QString ) ;
public slots:
	void startUpdater( void ) ;
	void setUpdateInterval( int ) ;
private slots:
	void run( void ) ;
	void closeApp( int ) ;
	void closeApp( void ) ;
	void changeIcon( QString ) ;
	void checkForUpdates( void ) ;
	void manualCheckForUpdates( void ) ;
	void automaticCheckForUpdates( void ) ;
	void checkForUpdatesOnStartUp( void ) ;
	void scheduleUpdates( int ) ;
	void updateStatus( int,QStringList ) ;
	void logWindowShow( void ) ;
	void aptGetLogWindow( void) ;
	void doneUpdating( void ) ;
	void toggleAutoStart( bool ) ;
	void openConfigureDialog( void ) ;
	void checkOutDatedPackages( QStringList ) ;
	void startTimer( void ) ;
	void startTimer_1( void ) ;
	void configOptionsChanged( void ) ;
	void autoRefreshSynaptic( bool ) ;
	void autoUpdateResult( int ) ;
	void autoDownloadPackages( int ) ;
	void objectGone( QObject * ) ;
	void taskFinished( int taskAction,int taskStatus ) ;
	void synapticStatus( int ) ;
	void networResponse( QNetworkReply * ) ;
	void checkTwitter( void ) ;
private:
	void checkForPackageUpdates( void ) ;
	void autoDownloadPackages( void ) ;
	void autoUpdatePackages( void ) ;
	bool autoRefreshSYnaptic( void ) ;
	void setupTranslationText( void ) ;
	void printTime( const QString&,u_int64_t ) ;
	void saveAptGetLogOutPut( const QStringList& ) ;
	u_int64_t getCurrentTime( void ) ;
	QString getCurrentTime_1( void ) ;
	u_int64_t nextScheduledUpdateTime( void ) ;
	void writeUpdateTimeToConfigFile( u_int64_t ) ;
	void showToolTip( const QString&,const QString&,const QStringList& ) ;
	void showToolTip( const QString&,const QString&,const QString& ) ;
	void showToolTip( const QString&,const QString&,int ) ;
	void showToolTip( const QString&,const QString& ) ;
	QString nextUpdateTime( void ) ;
	QString nextUpdateTime( int ) ;
	QString logMsg( int ) ;
	QString logMsg( void ) ;
	bool m_canCloseApplication ;
	bool m_threadIsRunning ;
	bool m_autoStartEnabled ;
	QStringList m_updatesList ;
	QString m_url ;
	QByteArray m_token ;
	QTimer * m_timer ;
	u_int64_t m_sleepDuration ;
	u_int64_t m_currentTime ;
	QTranslator * m_translator ;
	QNetworkAccessManager * m_manager ;
	bool m_debug ;
};

#endif // MAINWINDOW_H
