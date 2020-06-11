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

#ifndef SETTINGS_H
#define SETTINGS_H

#include <QString>
#include <QStringList>
#include <QSettings>

namespace settings
{
        void init( QSettings * ) ;
	QString configPath( void ) ;
	QString aptGetLogFilePath( void ) ;
	QString activityLogFilePath( void ) ;
	QString prefferedLanguage( void ) ;
	QString nextTimeToCheckForUpdatesLogFile( void ) ;
	QString checkDelayOnStartUplogFile( void ) ;
	QString updateCheckIntervalLogFile( void ) ;
	QString delayTimeBeforeUpdateCheck( int ) ;
	QString defaultIcon( void ) ;
	QByteArray token( void ) ;
	QString url( void ) ;
	QString getLastTwitterUpdate( void ) ;
	void setLastTwitterUpdate( const QString& ) ;
	int delayTimeBeforeUpdateCheck( void ) ;
	bool autoRefreshSynaptic( void ) ;
	bool firstTimeRun( void ) ;
	qint64 updateCheckInterval( void ) ;
	qint64 nextScheduledUpdateTime() ;
	void updateNextScheduledUpdateTime( qint64 time ) ;
	bool autoUpdatePackages( void ) ;
	bool autoDownloadPackages( void ) ;
	bool skipOldPackageCheck( void ) ;
	void skipOldPackageCheck( bool ) ;
	bool autoStartEnabled( void ) ;
	bool warnOnInconsistentState( void ) ;
	bool prefixLogEntries( void ) ;
	bool showIconOnImportantInfo( void ) ;
	void enableAutoStart( bool ) ;
	void setAutoRefreshSynaptic( bool ) ;
	void setPrefferedLanguage( const QString& ) ;
	void setCheckDelayOnStartUp( const QString& ) ;
	void setNextUpdateInterval( const QString& ) ;
	QString networkConnectivityChecker( void ) ;
	bool checkNewerKernels( void ) ;
	QStringList ignorePackageList( void ) ;
	void ignorePackageList( const QStringList& ) ;
}

#endif // SETTINGS_H
