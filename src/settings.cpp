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

#include <kstandarddirs.h>
#include <QFile>
#include <QDir>
#include <QDebug>
#include <cmath>

#include "settings.h"

static KStandardDirs _k ;
static QString _configPath ;

void settings::init()
{
	/*
	 * remove all old configuration files and use new one
	 */
	QFile::remove( QDir::homePath() + QString( "/.config/autostart/qt-update-notifier.desktop" ) ) ;

	_configPath = _k.localxdgconfdir() + QString( "/qt-update-notifier/" ) ;

	QDir d ;
	d.mkpath( _configPath ) ;
}

QString settings::configPath()
{
	return _configPath ;
}

QString settings::aptGetLogFilePath()
{
	return QString( "%1/%2" ).arg( _configPath ).arg( QString( "qt-update-notifier-apt_output.log" ) ) ;
}

QString settings::activityLogFilePath()
{
	return QString( "%1/%2" ).arg( _configPath ).arg( QString( "qt-update-notifier-activity.log" ) ) ;
}

QString settings::prefferedLanguage()
{
	QFile e( QString( "%1/%2" ).arg( _configPath ).arg( QString( "language.option" ) ) ) ;

	if( !e.exists() ){
		e.open( QIODevice::WriteOnly ) ;
		e.write( "english_US" ) ;
		e.close() ;
	}

	e.open( QIODevice::ReadOnly ) ;
	return e.readAll() ;
}

void settings::setPrefferedLanguage( const QString& language )
{
	QFile f( QString( "%1/%2" ).arg( _configPath ).arg( QString( "language.option" ) ) ) ;
	f.open( QIODevice::WriteOnly | QIODevice::Truncate ) ;
	f.write( language.toLatin1() ) ;
	f.close() ;
}

void settings::setCheckDelayOnStartUp(const QString& interval )
{
	QFile f( settings::checkDelayOnStartUplogFile() ) ;
	f.open( QIODevice::WriteOnly | QIODevice::Truncate ) ;
	f.write( interval.toLatin1() ) ;
}

void settings::setNextUpdateInterval( const QString& e )
{
	QFile f( settings::updateCheckIntervalLogFile() ) ;
	f.open( QIODevice::WriteOnly | QIODevice::Truncate ) ;
	f.write( e.toLatin1() ) ;
	f.close() ;
}

QString settings::nextTimeToCheckForUpdatesLogFile()
{
	return QString( "%1/%2" ).arg( _configPath ).arg( QString( "qt-update-notifier-next_auto_update.time" ) ) ;
}

QString settings::checkDelayOnStartUplogFile()
{
	return QString( "%1/%2" ).arg( _configPath ).arg( QString( "qt-update-notifier-startup_check_delay.time" ) ) ;
}

QString settings::updateCheckIntervalLogFile()
{
	return QString( "%1/%2" ).arg( _configPath ).arg( QString( "qt-update-notifier-interval.time" ) ) ;
}

QString settings::delayTimeBeforeUpdateCheck( int time )
{
	int rr = 60 * 1000 ;

	char buffer[64] ;

	if( fmod( time,rr ) == 0 ){
		int ff = time / rr ;
		snprintf( buffer,64,"%d",ff ) ;
	}else{
		float ff =  static_cast<float>( time ) / rr ;
		snprintf( buffer,64,"%.2f",ff ) ;
	}

	return QString( buffer ) ;
}

int settings::delayTimeBeforeUpdateCheck()
{
	QFile w( settings::checkDelayOnStartUplogFile() ) ;

	if( !w.exists() ){
		w.open( QIODevice::WriteOnly ) ;
		w.write( "300" ) ; // wait for 5 minutes before check for updates on startup
		w.close() ;
	}

	w.open( QIODevice::ReadOnly ) ;
	QString wd = w.readAll() ;

	w.close() ;
	return 1000 * wd.toInt() ;
}

void settings::setAutoRefreshSynaptic( bool b )
{
	QString x = QString( "%1/%2" ).arg( _configPath ).arg( QString( "qt-update-notifier-synaptic_autorefresh.option" ) ) ;
	if( b ){
		QFile f( x ) ;
		f.open( QIODevice::WriteOnly ) ;
		f.close() ;
	}else{
		QFile::remove( x ) ;
	}
}

bool settings::autoRefreshSynaptic()
{
	QString x = QString( "%1/%2" ).arg( _configPath ).arg( QString( "qt-update-notifier-synaptic_autorefresh.option" ) ) ;
	QFile f( x ) ;
	return f.exists() ;
}

bool settings::configTimeOptionDoesNotExist()
{
	QFile f( settings::nextTimeToCheckForUpdatesLogFile() ) ;
	return !f.exists() ;
}

u_int32_t settings::updateCheckInterval()
{
	QFile f( settings::updateCheckIntervalLogFile() ) ;
	if( !f.exists() ){
		f.open( QIODevice::WriteOnly ) ;
		f.write( "86400" ) ; // wait for 24 hours before checking for updates
		f.close() ;
	}

	f.open( QIODevice::ReadOnly ) ;
	QString x = f.readAll() ;
	f.close() ;
	return 1000 * x.toULong() ;
}

u_int64_t settings::nextScheduledUpdateTime()
{
	QFile f( settings::nextTimeToCheckForUpdatesLogFile() ) ;
	if( f.open( QIODevice::ReadOnly ) ){
		QString x = f.readAll() ;
		f.close() ;
		return x.toULongLong() ;
	}else{
		return 0 ;
	}
}

void settings::writeUpdateTimeToConfigFile( u_int64_t time )
{
	QFile f( settings::nextTimeToCheckForUpdatesLogFile() ) ;
	f.open( QIODevice::WriteOnly | QIODevice::Truncate ) ;
	QString z = QString::number( time ) ;
	f.write( z.toAscii() ) ;
	f.close() ;
}

bool settings::autoUpdatePackages()
{
	QFile e( QString( "%1/%2" ).arg( _configPath ).arg( QString( "qt-update-notifier/autoUpdatePackages.option" ) ) ) ;
	return e.exists() ;
}

bool settings::autoDownloadPackages()
{
	QFile e( QString( "%1/%2" ).arg( _configPath ).arg( QString( "qt-update-notifier/autoDownloadPackages.option" ) ) ) ;
	return e.exists() ;
}

bool settings::skipOldPackageCheck()
{
	QFile e( QString( "%1/%2" ).arg( _configPath ).arg( QString( "qt-update-notifier/skipOldPackageCheck.option" ) ) ) ;
	return e.exists() ;
}

bool settings::autoStartEnabled()
{
	QFile e( QString( "%1/%2" ).arg( _configPath ).arg( QString( "qt-update-notifier/doNotAutoStart" ) ) ) ;
	return !e.exists() ;
}

void settings::enableAutoStart( bool autoStart )
{
	QFile e( QString( "%1/%2" ).arg( _configPath ).arg( QString( "qt-update-notifier/doNotAutoStart" ) ) ) ;
	if( autoStart ){
		e.remove() ;
	}else{
		e.open( QIODevice::WriteOnly ) ;
		e.close() ;
	}
}
