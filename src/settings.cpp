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
#include <QSettings>

#include "settings.h"

static KStandardDirs _k ;
static QString _configPath ;

#define APP_NAME "qt-update-notifier"

static void _setUpSettingsDefaultOptions( QSettings& settings )
{
	settings.setPath( QSettings::IniFormat,QSettings::UserScope,_k.localxdgconfdir() ) ;
}

void convertOldConfigSystemToNewSystem()
{
	QSettings settings( QString( APP_NAME ),QString( APP_NAME ) ) ;
	_setUpSettingsDefaultOptions( settings ) ;

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
		opt = QString( "language" ) ;
		value = QString( "english_US" ) ;
		settings.setValue( opt,value ) ;
		e.close() ;
		e.remove() ;
	}else{
		opt = QString( "language" ) ;
		value = QString( "english_US" ) ;
		settings.setValue( opt,value ) ;
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
		opt = QString( "autoRefreshSynaptic" ) ;
		settings.setValue( opt,true ) ;
		e.remove() ;
	}else{
		opt = QString( "autoRefreshSynaptic" ) ;
		settings.setValue( opt,false ) ;
	}

	path = QString( "%1/%2" ).arg( _configPath ).arg( QString( "qt-update-notifier-next_auto_update.time" ) ) ;
	e.setFileName( path ) ;
	if( e.exists() ){
		e.open( QIODevice::ReadOnly ) ;
		value = e.readAll() ;
		opt = QString( "nextUpdateTime" ) ;
		settings.setValue( opt,value ) ;
		e.close() ;
		e.remove() ;
	}else{
		;
	}

	path = QString( "%1/%2" ).arg( _configPath ).arg( QString( "autoUpdatePackages.option" ) ) ;
	e.setFileName( path ) ;
	if( e.exists() ){
		opt = QString( "autoUpdatePackages" ) ;
		settings.setValue( opt,true ) ;
		e.remove() ;
	}else{
		opt = QString( "autoUpdatePackages" ) ;
		settings.setValue( opt,false ) ;
	}

	path = QString( "%1/%2" ).arg( _configPath ).arg( QString( "autoDownloadPackages.option" ) ) ;
	e.setFileName( path ) ;
	if( e.exists() ){
		opt = QString( "autoDownloadPackages" ) ;
		settings.setValue( opt,true ) ;
		e.remove() ;
	}else{
		opt = QString( "autoDownloadPackages" ) ;
		settings.setValue( opt,false ) ;
	}

	path = QString( "%1/%2" ).arg( _configPath ).arg( QString( "skipOldPackageCheck.option" ) ) ;
	e.setFileName( path ) ;
	if( e.exists() ){
		opt = QString( "skipOldPackageCheck" ) ;
		settings.setValue( opt,true ) ;
		e.remove() ;
	}else{
		opt = QString( "skipOldPackageCheck" ) ;
		settings.setValue( opt,false ) ;
	}

	path = QString( "%1/%2" ).arg( _configPath ).arg( QString( "doNotAutoStart" ) ) ;
	e.setFileName( path ) ;
	if( e.exists() ){
		opt = QString( "autoStartAtLogin" ) ;
		settings.setValue( opt,false ) ;
		e.remove() ;
	}else{
		opt = QString( "autoStartAtLogin" ) ;
		settings.setValue( opt,true ) ;
	}
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
	/*
	 * remove all old configuration files and use new one
	 */
	QFile::remove( QDir::homePath() + QString( "/.config/autostart/qt-update-notifier.desktop" ) ) ;

	_configPath = _k.localxdgconfdir() + QString( "/qt-update-notifier/" ) ;

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
	QSettings settings( QString( APP_NAME ),QString( APP_NAME ) ) ;
	_setUpSettingsDefaultOptions( settings ) ;
	return settings.value( QString( "language" ) ).toString() ;
}

void settings::setPrefferedLanguage( const QString& language )
{
	QSettings settings( QString( APP_NAME ),QString( APP_NAME ) ) ;
	_setUpSettingsDefaultOptions( settings ) ;
	settings.setValue( QString( "language" ),language ) ;
}

void settings::setCheckDelayOnStartUp( const QString& interval )
{
	QSettings settings( QString( APP_NAME ),QString( APP_NAME ) ) ;
	_setUpSettingsDefaultOptions( settings ) ;
	settings.setValue( QString( "startUpDelay" ),interval ) ;
}

void settings::setNextUpdateInterval( const QString& interval )
{
	QSettings settings( QString( APP_NAME ),QString( APP_NAME ) ) ;
	_setUpSettingsDefaultOptions( settings ) ;
	settings.setValue( QString( "updateCheckInterval" ),interval ) ;
}

int settings::delayTimeBeforeUpdateCheck()
{
	QSettings settings( QString( APP_NAME ),QString( APP_NAME ) ) ;
	_setUpSettingsDefaultOptions( settings ) ;
	return 1000 * settings.value( QString( "startUpDelay" ) ).toString().toInt() ;
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

void settings::setAutoRefreshSynaptic( bool autoRefresh )
{
	QSettings settings( QString( APP_NAME ),QString( APP_NAME ) ) ;
	_setUpSettingsDefaultOptions( settings ) ;
	QString opt = QString( "autoRefreshSynaptic" ) ;
	if( autoRefresh ){
		settings.setValue( opt,true ) ;
	}else{
		settings.setValue( opt,false ) ;
	}
}

bool settings::autoRefreshSynaptic()
{
	QSettings settings( QString( APP_NAME ),QString( APP_NAME ) ) ;
	_setUpSettingsDefaultOptions( settings ) ;
	return settings.value( QString( "autoRefreshSynaptic" ) ).toBool() ;
}

bool settings::firstTimeRun()
{
	QSettings settings( QString( APP_NAME ),QString( APP_NAME ) ) ;
	_setUpSettingsDefaultOptions( settings ) ;
	return !settings.contains( QString( "nextUpdateTime" ) ) ;
}

u_int32_t settings::updateCheckInterval()
{
	QSettings settings( QString( APP_NAME ),QString( APP_NAME ) ) ;
	_setUpSettingsDefaultOptions( settings ) ;
	return 1000 * settings.value( QString( "updateCheckInterval" ) ).toString().toULong() ;
}

u_int64_t settings::nextScheduledUpdateTime()
{
	QSettings settings( QString( APP_NAME ),QString( APP_NAME ) ) ;
	_setUpSettingsDefaultOptions( settings ) ;
	return settings.value( QString( "nextUpdateTime" ) ).toString().toULongLong() ;
}

void settings::writeUpdateTimeToConfigFile( u_int64_t time )
{
	QSettings settings( QString( APP_NAME ),QString( APP_NAME ) ) ;
	_setUpSettingsDefaultOptions( settings ) ;
	settings.setValue( QString( "nextUpdateTime" ),QString::number( time ) ) ;
}

bool settings::autoUpdatePackages()
{
	QSettings settings( QString( APP_NAME ),QString( APP_NAME ) ) ;
	_setUpSettingsDefaultOptions( settings ) ;
	return settings.value( QString( "autoUpdatePackages" ) ).toBool() ;
}

bool settings::autoDownloadPackages()
{
	QSettings settings( QString( APP_NAME ),QString( APP_NAME ) ) ;
	_setUpSettingsDefaultOptions( settings ) ;
	return settings.value( QString( "autoDownloadPackages" ) ).toBool() ;
}

bool settings::skipOldPackageCheck()
{
	QSettings settings( QString( APP_NAME ),QString( APP_NAME ) ) ;
	_setUpSettingsDefaultOptions( settings ) ;
	return settings.value( QString( "skipOldPackageCheck" ) ).toBool() ;
}

bool settings::autoStartEnabled()
{
	QSettings settings( QString( APP_NAME ),QString( APP_NAME ) ) ;
	_setUpSettingsDefaultOptions( settings ) ;
	return settings.value( QString( "autoStartAtLogin" ) ).toBool() ;
}

void settings::enableAutoStart( bool autoStart )
{
	QSettings settings( QString( APP_NAME ),QString( APP_NAME ) ) ;
	_setUpSettingsDefaultOptions( settings ) ;
	QString opt = QString( "autoStartAtLogin" ) ;
	settings.setValue( opt,autoStart ) ;
}
