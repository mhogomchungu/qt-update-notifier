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

#ifndef SETTINGS_H
#define SETTINGS_H

#include<QString>

class settings
{
public:
	static void init( void) ;
	static QString configPath( void ) ;
	static QString aptGetLogFilePath( void ) ;
	static QString activityLogFilePath( void ) ;
	static QString prefferedLanguage( void ) ;
	static QString nextTimeToCheckForUpdatesLogFile( void ) ;
	static QString checkDelayOnStartUplogFile( void ) ;
	static QString updateCheckIntervalLogFile( void ) ;
	static QString delayTimeBeforeUpdateCheck( int ) ;
	static int delayTimeBeforeUpdateCheck( void ) ;
	static bool autoRefreshSynaptic( void ) ;
	static bool configTimeOptionDoesNotExist( void ) ;
	static u_int32_t updateCheckInterval( void ) ;
	static u_int64_t nextScheduledUpdateTime() ;
	static void writeUpdateTimeToConfigFile( u_int64_t time ) ;
	static bool autoUpdatePackages( void ) ;
	static bool autoDownloadPackages( void ) ;
	static bool skipOldPackageCheck( void ) ;
	static bool autoStartEnabled( void ) ;
	static void enableAutoStart( bool ) ;
	static void setAutoRefreshSynaptic( bool ) ;
	static void setPrefferedLanguage( const QString& ) ;
	static void setCheckDelayOnStartUp( const QString& ) ;
	static void setNextUpdateInterval( const QString& ) ;
};

#endif // SETTINGS_H
