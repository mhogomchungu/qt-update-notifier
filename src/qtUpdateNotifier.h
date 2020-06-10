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
#include <QSettings>
#include <QCoreApplication>
#include <QTimer>
#include <QDateTime>
#include <cstdlib>
#include <cstdio>
#include <QTranslator>
#include <QLibraryInfo>

#include "networkAccessManager.hpp"
#include "logwindow.h"
#include "ignorepackagelist.h"
#include "instance.h"
#include "desktop_file.h"
#include "configuredialog.h"
#include "qt-update-install-path.h"
#include "utility.h"
#include "settings.h"
#include "statusicon.h"
#include "twitter.h"

#include <memory>

class qtUpdateNotifier : public QObject
{
	Q_OBJECT
public:
        explicit qtUpdateNotifier( bool ) ;
	~qtUpdateNotifier() ;
	void start( void ) ;
	void logActivity( const QString& ) ;
	void logActivity_1( const QString& ) ;
	void setDebug( bool ) ;
	static bool autoStartEnabled( void ) ;
signals:
	void updateLogWindow( void ) ;
	void configOptionsChanged_1( void ) ;
	void msg( QString ) ;
public slots:
	void startUpdater( void ) ;
	void setUpdateInterval( int ) ;
private slots:
        void buildGUI( void ) ;
	void run( void ) ;
	void closeApp( int ) ;
	void closeApp( void ) ;
	void changeIcon( QString ) ;
	void checkForUpdates( void ) ;
	void manualCheckForUpdates( void ) ;
	void automaticCheckForUpdates( void ) ;
	void checkForUpdatesOnStartUp( void ) ;
	void scheduleUpdates( int ) ;
	void logWindowShow( void ) ;
	void ignorePackageList( void ) ;
	void aptGetLogWindow( void) ;
	void doneUpdating( void ) ;
	void toggleAutoStart( bool ) ;
	void openConfigureDialog( void ) ;
	void startTimer( void ) ;
	void startTimer_1( void ) ;
	void configOptionsChanged( void ) ;
	void autoRefreshSynaptic( bool ) ;
	void objectGone( QObject * ) ;
	void checkTwitter( void ) ;
private:
	QString networResponse( QNetworkReply& ) ;
	QString getLastTwitterUpdate( void ) ;
	void setLastTwitterUpdate( const QString& ) ;
	void showIconOnImportantInfo( void ) ;
	void checkForPackageUpdates( void ) ;
	void autoDownloadPackages( void ) ;
	void autoUpdatePackages( void ) ;
	void setupTranslationText( void ) ;
	void printTime( const QString&,qint64 ) ;
	void saveAptGetLogOutPut( const result::array_t& ) ;
	qint64 getCurrentTime( void ) ;
	QString getCurrentTime_1( void ) ;
	qint64 nextScheduledUpdateTime( void ) ;
	void writeUpdateTimeToConfigFile( qint64 ) ;
	void showToolTip( const QString&,const QString&,const result::array_t& ) ;
	void showToolTip( const QString&,const QString&,const QString& ) ;
	void showToolTip( const QString&,const QString&,int ) ;
	void showToolTip( const QString&,const QString& ) ;
	QString nextUpdateTime( void ) ;
	QString nextAutoUpdateTime( void ) ;
	QString nextUpdateTime( qint64 ) ;
	QString logMsg( qint64 ) ;
	QString logMsg( void ) ;
	bool m_canCloseApplication ;
	bool m_threadIsRunning ;
	bool m_autoStartEnabled ;
	bool m_showIconOnImportantInfo ;
        bool m_autoStart ;
	QStringList m_updatesList ;
	QString m_url ;
	QString m_networkConnectivityChecker ;
	QString m_defaulticon ;
	QString m_lastTwitterUpdate ;
	QByteArray m_token ;
	QTimer m_timer ;
	qint64 m_sleepDuration ;
	qint64 m_currentTime ;
	qint64 m_nextScheduledUpdateTime ;
	NetworkAccessManager m_manager ;
	statusicon m_statusicon ;
	bool m_debug ;
	twitter m_twitter ;
};

#endif // MAINWINDOW_H
