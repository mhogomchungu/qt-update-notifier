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

#include <QFile>
#include <QDir>
#include <QDebug>
#include <cmath>
#include <QSettings>

#include "tray_application_type.h"
#include "settings.h"

#define APP_NAME "qt-update-notifier"

static const char * _secret = "AAAAAAAAAAAAAAAAAAAAADibXQAAAAAADEVZcGLIBzf8rhjIdxff9P08qIU%3Dxexvyewewzu7i1LH8049xGJWI4kv7rBEnkis2t6HHlkDCSsUgB" ;

static const char * _url = "https://api.twitter.com/1.1/statuses/user_timeline.json?screen_name=iluvpclinuxos&count=50&exclude_replies=true&include_rts=false" ;

static QString _configPath ;

static QSettings _settings( QString( APP_NAME ),QString( APP_NAME ) ) ;

#if USE_KDE_STATUS_NOTIFIER
#include <kstandarddirs.h>
static QString _localConfigDir( void )
{
	KStandardDirs k ;
	return k.localxdgconfdir() ;
}
#else
static QString _localConfigDir( void )
{
	return QString( "%1/%2" ).arg( QDir::homePath() ).arg( "/.config/" ) ;
}
#endif

void convertOldConfigSystemToNewSystem()
{
	QString path ;
	QFile e ;

	path = QString( "%1/%2" ).arg( _configPath ).arg( QString( "qt-update-notifier.conf" ) ) ;
	e.setFileName( path ) ;
	if( e.exists() ){
		return ;
	}

	QString opt ;
	QString value ;

	path = QString( "%1/%2" ).arg( _configPath ).arg( QString( "language.option" ) ) ;
	e.setFileName( path ) ;
	if( e.exists() ){
		e.open( QIODevice::ReadOnly ) ;
		value = e.readAll() ;
		settings::setPrefferedLanguage( value ) ;
		e.close() ;
		e.remove() ;
	}else{
		settings::setPrefferedLanguage( QString( "english_US" ) ) ;
	}

	path = QString( "%1/%2" ).arg( _configPath ).arg( QString( "qt-update-notifier-startup_check_delay.time" ) ) ;
	e.setFileName( path ) ;
	if( e.exists() ){
		e.open( QIODevice::ReadOnly ) ;
		value = e.readAll() ;
		settings::setCheckDelayOnStartUp( value ) ;
		e.close() ;
		e.remove() ;
	}else{
		settings::setCheckDelayOnStartUp( QString( "300" ) ) ;
	}

	path = QString( "%1/%2" ).arg( _configPath ).arg( QString( "qt-update-notifier-interval.time" ) ) ;
	e.setFileName( path ) ;
	if( e.exists() ){
		e.open( QIODevice::ReadOnly ) ;
		value = e.readAll() ;
		settings::setNextUpdateInterval( value ) ;
		e.close() ;
		e.remove() ;
	}else{
		settings::setNextUpdateInterval( QString( "86400" ) ) ;
	}

	path = QString( "%1/%2" ).arg( _configPath ).arg( QString( "qt-update-notifier-synaptic_autorefresh.option" ) ) ;
	e.setFileName( path ) ;
	if( e.exists() ){
		settings::setAutoRefreshSynaptic( true ) ;
		e.remove() ;
	}else{
		settings::setAutoRefreshSynaptic( false ) ;
	}

	path = QString( "%1/%2" ).arg( _configPath ).arg( QString( "doNotAutoStart" ) ) ;
	e.setFileName( path ) ;
	if( e.exists() ){
		settings::enableAutoStart( false ) ;
		e.remove() ;
	}else{
		settings::enableAutoStart( true ) ;
	}

	path = QString( "%1/%2" ).arg( _configPath ).arg( QString( "qt-update-notifier-next_auto_update.time" ) ) ;
	e.setFileName( path ) ;
	if( e.exists() ){
		e.open( QIODevice::ReadOnly ) ;
		value = e.readAll() ;
		opt = QString( "nextUpdateTime" ) ;
		_settings.setValue( opt,value ) ;
		e.close() ;
		e.remove() ;
	}else{
		;
	}

	path = QString( "%1/%2" ).arg( _configPath ).arg( QString( "autoUpdatePackages.option" ) ) ;
	e.setFileName( path ) ;
	if( e.exists() ){
		opt = QString( "autoUpdatePackages" ) ;
		_settings.setValue( opt,true ) ;
		e.remove() ;
	}else{
		opt = QString( "autoUpdatePackages" ) ;
		_settings.setValue( opt,false ) ;
	}

	path = QString( "%1/%2" ).arg( _configPath ).arg( QString( "autoDownloadPackages.option" ) ) ;
	e.setFileName( path ) ;
	if( e.exists() ){
		opt = QString( "autoDownloadPackages" ) ;
		_settings.setValue( opt,true ) ;
		e.remove() ;
	}else{
		opt = QString( "autoDownloadPackages" ) ;
		_settings.setValue( opt,false ) ;
	}

	path = QString( "%1/%2" ).arg( _configPath ).arg( QString( "skipOldPackageCheck.option" ) ) ;
	e.setFileName( path ) ;
	if( e.exists() ){
		opt = QString( "skipOldPackageCheck" ) ;
		_settings.setValue( opt,true ) ;
		e.remove() ;
	}else{
		opt = QString( "skipOldPackageCheck" ) ;
		_settings.setValue( opt,false ) ;
	}

	_settings.sync() ;
}

QString settings::aptGetLogFilePath()
{
	return QString( "%1/%2" ).arg( _configPath ).arg( QString( "qt-update-notifier-apt_output.log" ) ) ;
}

QString settings::activityLogFilePath()
{
	return QString( "%1/%2" ).arg( _configPath ).arg( QString( "qt-update-notifier-activity.log" ) ) ;
}

void settings::init()
{
	QString e = _localConfigDir() ;
	/*
	 * remove all old configuration files and use new one
	 */
	QFile::remove( QString( "%1/autostart/qt-update-notifier.desktop" ).arg( e ) ) ;

	_configPath = QString( "%1/qt-update-notifier/" ).arg( e ) ;

	_settings.setPath( QSettings::IniFormat,QSettings::UserScope,e ) ;

	QDir d ;
	d.mkpath( _configPath ) ;

	convertOldConfigSystemToNewSystem() ;
}

QString settings::configPath()
{
	return _configPath ;
}

QString settings::prefferedLanguage()
{
	return _settings.value( QString( "language" ) ).toString() ;
}

void settings::setPrefferedLanguage( const QString& language )
{
	_settings.setValue( QString( "language" ),language ) ;
}

void settings::setCheckDelayOnStartUp( const QString& interval )
{
	_settings.setValue( QString( "startUpDelay" ),interval ) ;
}

void settings::setNextUpdateInterval( const QString& interval )
{
	_settings.setValue( QString( "updateCheckInterval" ),interval ) ;
}

QString settings::networkConnectivityChecker()
{
	QString opt = QString( "networkConnectivityChecker" ) ;

	if( _settings.contains( opt ) ){
		return _settings.value( opt ).toString() ;
	}else{
		QString s = "ping -c 1 8.8.8.8" ;
		_settings.setValue( opt,s ) ;
		return s ;
	}
}

int settings::delayTimeBeforeUpdateCheck()
{
	return 1000 * _settings.value( QString( "startUpDelay" ) ).toString().toInt() ;
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

QString settings::defaultIcon()
{
	QString opt = "defaultIcon" ;

	if( _settings.contains( opt ) ){
		return _settings.value( opt ).toString() + ".png" ;
	}else{
		QString u = "qt-update-notifier" ;
		_settings.setValue( opt,u ) ;
		return u + ".png" ;
	}
}

QByteArray settings::token()
{
	return QByteArray( "Bearer " ) + _secret ;
}

QString settings::url()
{
	QString opt = QString( "url" ) ;

	if( _settings.contains( opt ) ){
		return _settings.value( opt ).toString() ;
	}else{
		QString u( _url ) ;
		_settings.setValue( opt,u ) ;
		return u ;
	}
}

QString settings::getLastTwitterUpdate()
{
	QString opt = QString( "lastTwitterUpdate" ) ;

	_settings.sync() ;

	if( _settings.contains( opt ) ){
		return _settings.value( opt ).toString() ;
	}else{
		QString u( "" ) ;
		_settings.setValue( opt,u ) ;
		return u ;
	}
}

void settings::setLastTwitterUpdate( const QString& t )
{
	QString opt = QString( "lastTwitterUpdate" ) ;
	_settings.setValue( opt,t ) ;
	_settings.sync() ;
}

void settings::setAutoRefreshSynaptic( bool autoRefresh )
{
	QString opt = QString( "autoRefreshSynaptic" ) ;
	if( autoRefresh ){
		_settings.setValue( opt,true ) ;
	}else{
		_settings.setValue( opt,false ) ;
	}
}

bool settings::autoRefreshSynaptic()
{
	return _settings.value( QString( "autoRefreshSynaptic" ) ).toBool() ;
}

bool settings::firstTimeRun()
{
	return !_settings.contains( QString( "nextUpdateTime" ) ) ;
}

u_int32_t settings::updateCheckInterval()
{
	return 1000 * _settings.value( QString( "updateCheckInterval" ) ).toString().toULong() ;
}

u_int64_t settings::nextScheduledUpdateTime()
{
	return _settings.value( QString( "nextUpdateTime" ) ).toString().toULongLong() ;
}

void settings::writeUpdateTimeToConfigFile( u_int64_t time )
{
	_settings.setValue( QString( "nextUpdateTime" ),QString::number( time ) ) ;
}

bool settings::autoUpdatePackages()
{
	return _settings.value( QString( "autoUpdatePackages" ) ).toBool() ;
}

bool settings::autoDownloadPackages()
{
	return _settings.value( QString( "autoDownloadPackages" ) ).toBool() ;
}

bool settings::skipOldPackageCheck()
{
	return _settings.value( QString( "skipOldPackageCheck" ) ).toBool() ;
}

bool settings::autoStartEnabled()
{
	return _settings.value( QString( "autoStartAtLogin" ) ).toBool() ;
}

bool settings::warnOnInconsistentState()
{
	return _settings.value( QString( "warnOnInconsistentState" ) ).toBool() ;
}

bool settings::prefixLogEntries()
{
	QString opt = QString( "prefixLogEntries" ) ;
	if( _settings.contains( opt ) ){
		return _settings.value( opt ).toBool() ;
	}else{
		_settings.setValue( opt,true ) ;
		return true ;
	}
}

bool settings::showIconOnImportantInfo()
{
	QString opt = QString( "showIconOnImportantInfo" ) ;
	if( _settings.contains( opt ) ){
		return _settings.value( opt ).toBool() ;
	}else{
		_settings.setValue( opt,true ) ;
		return true ;
	}
}

void settings::enableAutoStart( bool autoStart )
{
	QString opt = QString( "autoStartAtLogin" ) ;
	_settings.setValue( opt,autoStart ) ;
}
