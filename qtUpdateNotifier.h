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
#include <kstandarddirs.h>
#include <cstdlib>
#include <cstdio>
#include <QTranslator>
#include <QLibraryInfo>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>

#include "check_updates.h"
#include "logwindow.h"
#include "instance.h"
#include "desktop_file.h"
#include "startsynaptic.h"
#include "configuredialog.h"
#include "checkoldpackages.h"
#include "qt-update-install-path.h"
#include "utility.h"

class qtUpdateNotifier : public KStatusNotifierItem
{
	Q_OBJECT
public:
	explicit qtUpdateNotifier() ;
	~qtUpdateNotifier() ;
	void start( void ) ;
	void logActivity( QString ) ;
	void logActivity_1( QString ) ;
	static bool autoStartEnabled( void ) ;
	static int instanceAlreadyRunning( void ) ;
signals:
	void updateLogWindow( void ) ;
	void configOptionsChanged_1( void ) ;
public slots:
	void startUpdater( void ) ;
	void setUpdateInterval( int ) ;
private slots:
	void run( void ) ;
	void closeApp( int ) ;
	void closeApp( void ) ;
	void threadTerminated( void ) ;
	void threadisFinished( void ) ;
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
	void enableAutoStart( void ) ;
	void disableAutoStart( void ) ;
	void toggleAutoStart( bool ) ;
	void updaterClosed( void ) ;
	void openConfigureDialog( void ) ;
	void checkOldPackages( QStringList ) ;
	void startTimer( void ) ;
	void startTimer_1( void ) ;
	void configOptionsChanged( void ) ;
	void localizationLanguage( QString ) ;
	void autoRefreshSynaptic( bool ) ;
	void autoUpdateResult( int ) ;
	void autoDownloadPackages( int ) ;
	void checkForPackageUpdates( QNetworkReply * ) ;
	void objectGone( QObject * ) ;
private:
	void checkForPackageUpdates( void ) ;
	void autoDownloadPackages( void ) ;
	void autoUpdatePackages( void ) ;
	bool autoRefreshSYnaptic( void ) ;
	void setupTranslationText( void ) ;
	void printTime( QString,u_int64_t ) ;
	void saveAptGetLogOutPut( QStringList& ) ;
	u_int64_t getCurrentTime( void ) ;
	QString getCurrentTime_1( void ) ;
	u_int64_t nextScheduledUpdateTime( void ) ;
	void createEnvironment( void ) ;
	void writeUpdateTimeToConfigFile( u_int64_t ) ;
	void showToolTip( QString,QString,QStringList& ) ;
	void showToolTip( QString,QString,QString ) ;
	void showToolTip( QString,QString,int ) ;
	void showToolTip( QString,QString ) ;
	QString nextUpdateTime( void ) ;
	QString nextUpdateTime( int ) ;
	QString logMsg( int ) ;
	QString logMsg( void ) ;
	bool m_canCloseApplication ;
	bool m_threadIsRunning ;
	bool m_autoStartEnabled ;
	QStringList m_updatesList ;
	check_updates * m_updates ;
	KMenu * m_trayMenu ;
	QTimer * m_timer ;
	QString m_configTime ;
	QString m_configLog ;
	u_int64_t m_sleepDuration ;
	u_int64_t m_currentTime ;
	int m_waitForFirstCheck ;
	QString m_configPath ;
	QString m_aptGetConfigLog ;
	QString m_CheckDelayOnStartUp ;
	QString m_updateCheckInterval ;
	QTranslator * m_translator ;
	QString m_prefferedLanguage ;
};

#endif // MAINWINDOW_H
