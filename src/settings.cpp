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

#include <QFile>
#include <QDir>
#include <QDebug>
#include <cmath>
#include <QSettings>
#include <QDateTime>

#include <functional>

#include "tray_application_type.h"
#include "settings.h"

#include <iostream>

static const auto _secret = "AAAAAAAAAAAAAAAAAAAAADibXQAAAAAADEVZcGLIBzf8rhjIdxff9P08qIU%3Dxexvyewewzu7i1LH8049xGJWI4kv7rBEnkis2t6HHlkDCSsUgB" ;

static const auto _url = "https://api.twitter.com/1.1/statuses/user_timeline.json?screen_name=iluvpclinuxos&count=50&exclude_replies=true&include_rts=false" ;

static QString _configPath ;

static QSettings * _settings ;

static QString _localConfigDir( void )
{
	return QString( "%1/%2" ).arg( QDir::homePath(),"/.config/" ) ;
}

static bool _option_bool( const QString& opt,bool e )
{
	if( _settings->contains( opt ) ){

		return _settings->value( opt,e ).toBool() ;
	}else{
		_settings->setValue( opt,e ) ;
		return e ;
	}
}

static QString _option_qstring( const QString& opt,const QString& e )
{
	if( _settings->contains( opt ) ){

		return _settings->value( opt ).toString() ;
	}else{
		_settings->setValue( opt,e ) ;
		return e ;
	}
}

static int _option_int( const QString& opt,int e,int multiply = 1000 )
{
	if( _settings->contains( opt ) ){

		return multiply * _settings->value( opt ).toInt() ;
	}else{
		_settings->setValue( opt,QString::number( e ) ) ;
		return multiply * e ;
	}
}

static qint64 _option_longlong( const QString& opt,int e,int multiply = 1000 )
{
	if( _settings->contains( opt ) ){

		return multiply * _settings->value( opt ).toString().toLong() ;
	}else{
		_settings->setValue( opt,QString::number( e ) ) ;
		return multiply * e ;
	}
}

QString settings::aptGetLogFilePath()
{
	return QString( "%1/%2" ).arg( _configPath,"qt-update-notifier-apt_output.log" ) ;
}

QString settings::activityLogFilePath()
{
	return QString( "%1/%2" ).arg( _configPath,"qt-update-notifier-activity.log" ) ;
}

void settings::init( QSettings * settings )
{
        _settings = settings ;

	QString e = _localConfigDir() ;
	/*
	 * remove all old configuration files and use new one
	 */
	QFile::remove( QString( "%1/autostart/qt-update-notifier.desktop" ).arg( e ) ) ;

	_configPath = QString( "%1/qt-update-notifier/" ).arg( e ) ;

        _settings->setPath( QSettings::IniFormat,QSettings::UserScope,e ) ;

	QDir d ;
	d.mkpath( _configPath ) ;
}

QString settings::configPath()
{
	return _configPath ;
}

QString settings::prefferedLanguage()
{
	return _option_qstring( "language","english_US" ) ;
}

void settings::setPrefferedLanguage( const QString& language )
{
        _settings->setValue( "language",language ) ;
}

void settings::setCheckDelayOnStartUp( const QString& interval )
{
        _settings->setValue( "startUpDelay",interval ) ;
}

void settings::setNextUpdateInterval( const QString& interval )
{
        _settings->setValue( "updateCheckInterval",interval ) ;
}

QString settings::networkConnectivityChecker()
{
	return _option_qstring( "networkConnectivityChecker","ping -c 1 8.8.8.8" ) ;
}

QString settings::delayTimeBeforeUpdateCheck( int time )
{
	int rr = 60 * 1000 ;

	char buffer[64] ;

	if( fmod( time,rr ) == 0 ){

		int ff = time / rr ;
		snprintf( buffer,64,"%d",ff ) ;
	}else{
		auto ff =  static_cast<double>( time ) / rr ;
		snprintf( buffer,64,"%.2f",ff ) ;
	}

	return QString( buffer ) ;
}

QString settings::defaultIcon()
{
	return _option_qstring( "defaultIcon","qt-update-notifier" ) ;
}

QByteArray settings::token()
{
	return QByteArray( "Bearer " ) + _secret ;
}

QString settings::url()
{
	return _option_qstring( "url",_url ) ;
}

qint64 settings::nextScheduledUpdateTime()
{
	if( _settings->contains( "nextScheduledUpdateTime0" ) ){

		return _settings->value( "nextScheduledUpdateTime0" ).toLongLong() ;
	}else{
		qDebug() << "invalid code path in \"settings::nextScheduledUpdateTime()\"" ;

		return QDateTime::currentDateTime().toMSecsSinceEpoch() ;
	}
}

QString settings::getLastTwitterUpdate()
{
	return _option_qstring( "lastTwitterUpdate",QString() ) ;
}

qint64 settings::updateCheckInterval()
{
	return _option_longlong( "updateCheckInterval",86400 ) ;
}

int settings::delayTimeBeforeUpdateCheck()
{
	return _option_int( "startUpDelay",300 ) ;
}

void settings::setLastTwitterUpdate( const QString& t )
{
	QString opt( "lastTwitterUpdate" ) ;
        _settings->setValue( opt,t ) ;
}

void settings::setAutoRefreshSynaptic( bool autoRefresh )
{
	QString opt( "autoRefreshSynaptic" ) ;

	if( autoRefresh ){

                _settings->setValue( opt,true ) ;
	}else{
                _settings->setValue( opt,false ) ;
	}
}

bool settings::firstTimeRun()
{
	return !_settings->contains( "nextScheduledUpdateTime0" ) ;
}

void settings::updateNextScheduledUpdateTime( qint64 time )
{
	_settings->setValue( "nextScheduledUpdateTime0",QString::number( time ) ) ;
}

bool settings::autoUpdatePackages()
{
	return _option_bool( "autoUpdatePackages",false ) ;
}

bool settings::autoRefreshSynaptic()
{
	return _option_bool( "autoRefreshSynaptic",false ) ;
}

bool settings::autoDownloadPackages()
{
	return _option_bool( "autoDownloadPackages",false ) ;
}

bool settings::skipOldPackageCheck()
{
	return _option_bool( "skipOldPackageCheck",true ) ;
}

void settings::skipOldPackageCheck( bool e )
{
	_settings->setValue( "skipOldPackageCheck",e ) ;
}

bool settings::autoStartEnabled()
{
	return _option_bool( "autoStartAtLogin",true ) ;
}

void settings::enableAutoStart( bool autoStart )
{
	QString opt( "autoStartAtLogin" ) ;
	_settings->setValue( opt,autoStart ) ;
}

bool settings::warnOnInconsistentState()
{
	return _option_bool( "warnOnInconsistentState",true ) ;
}

bool settings::prefixLogEntries()
{
	return _option_bool( "prefixLogEntries",true ) ;
}

bool settings::showIconOnImportantInfo()
{
	return _option_bool( "showIconOnImportantInfo",true ) ;
}

bool settings::checkNewerKernels()
{
	return _option_bool( "checkNewerKernels",false ) ;
}

QStringList settings::ignorePackageList()
{
	if( _settings->contains( "ignoredPackageList" ) ){

		return _settings->value( "ignoredPackageList" ).toStringList() ;
	}else{
		QStringList e ;
		_settings->setValue( "ignoredPackageList",e ) ;
		return e ;
	}
}

void settings::ignorePackageList( const QStringList& e )
{
	_settings->setValue( "ignoredPackageList",e ) ;
}

QRect settings::logWindowDimensions()
{
	if( !_settings->contains( "logWindowDimensions" ) ){

		QRect r ;

		r.setX( 332 ) ;
		r.setY( 188 ) ;
		r.setWidth( 701 ) ;
		r.setHeight( 380 ) ;

		_settings->setValue( "logWindowDimensions",r ) ;
	}

	return _settings->value( "logWindowDimensions" ).toRect() ;
}

void settings::logWindowDimensions( const QRect& e )
{
	_settings->setValue( "logWindowDimensions",e ) ;
}
