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

#include "tray_application_type.h"
#include "settings.h"

static const auto _secret = "AAAAAAAAAAAAAAAAAAAAADibXQAAAAAADEVZcGLIBzf8rhjIdxff9P08qIU%3Dxexvyewewzu7i1LH8049xGJWI4kv7rBEnkis2t6HHlkDCSsUgB" ;

static const auto _url = "https://api.twitter.com/1.1/statuses/user_timeline.json?screen_name=iluvpclinuxos&count=50&exclude_replies=true&include_rts=false" ;

static QString _configPath ;

static QSettings * _settings ;

static QString _localConfigDir( void )
{
	return QString( "%1/%2" ).arg( QDir::homePath(),"/.config/" ) ;
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
        return _settings->value( "language" ).toString() ;
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
	QString opt( "networkConnectivityChecker" ) ;

        if( _settings->contains( opt ) ){

                return _settings->value( opt ).toString() ;
	}else{
		QString s( "ping -c 1 8.8.8.8" ) ;
                _settings->setValue( opt,s ) ;
		return s ;
	}
}

int settings::delayTimeBeforeUpdateCheck()
{
	QString opt( "startUpDelay" ) ;

	if( _settings->contains( opt ) ){

		return 1000 * _settings->value( opt ).toString().toInt() ;
	}else{
		_settings->setValue( opt,QString::number( 10 * 60 ) ) ;
		return 1000 * 10 * 60 ;
	}
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
	QString opt( "defaultIcon" ) ;

        if( _settings->contains( opt ) ){

                return _settings->value( opt ).toString() + ".png" ;
	}else{
		QString u( "qt-update-notifier" ) ;
                _settings->setValue( opt,u ) ;
		return u + ".png" ;
	}
}

QByteArray settings::token()
{
	return QByteArray( "Bearer " ) + _secret ;
}

QString settings::url()
{
	QString opt( "url" ) ;

        if( _settings->contains( opt ) ){

                return _settings->value( opt ).toString() ;
	}else{
		QString u( _url ) ;
                _settings->setValue( opt,u ) ;
		return u ;
	}
}

QString settings::getLastTwitterUpdate()
{
	QString opt( "lastTwitterUpdate" ) ;

        if( _settings->contains( opt ) ){

                return _settings->value( opt ).toString() ;
	}else{
		QString u ;
                _settings->setValue( opt,u ) ;
		return u ;
	}
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

bool settings::autoRefreshSynaptic()
{
        return _settings->value( "autoRefreshSynaptic" ).toBool() ;
}

bool settings::firstTimeRun()
{
        return !_settings->contains( "nextScheduledUpdateTime" ) ;
}

u_int32_t settings::updateCheckInterval()
{
	QString opt( "updateCheckInterval" ) ;

	if( _settings->contains( opt ) ){

		return 1000 * _settings->value( "updateCheckInterval" ).toString().toULong() ;
	}else{
		return 1000 * 86400 ;
	}
}

u_int64_t settings::nextScheduledUpdateTime()
{
	if( _settings->contains( "nextScheduledUpdateTime" ) ){

		return _settings->value( "nextScheduledUpdateTime" ).toString().toULongLong() ;
	}else{
		auto s = QDateTime::currentDateTime().toMSecsSinceEpoch() + settings::updateCheckInterval() ;
		settings::writeUpdateTimeToConfigFile( s ) ;
		return s ;
	}
}

void settings::writeUpdateTimeToConfigFile( u_int64_t time )
{
        _settings->setValue( "nextScheduledUpdateTime",QString::number( time ) ) ;
}

bool settings::autoUpdatePackages()
{
        return _settings->value( "autoUpdatePackages" ).toBool() ;
}

bool settings::autoDownloadPackages()
{
        return _settings->value( "autoDownloadPackages" ).toBool() ;
}

bool settings::skipOldPackageCheck()
{
        return _settings->value( "skipOldPackageCheck" ).toBool() ;
}

bool settings::autoStartEnabled()
{
        return _settings->value( "autoStartAtLogin" ).toBool() ;
}

bool settings::warnOnInconsistentState()
{
        return _settings->value( "warnOnInconsistentState" ).toBool() ;
}

bool settings::prefixLogEntries()
{
	QString opt( "prefixLogEntries" ) ;

        if( _settings->contains( opt ) ){

                return _settings->value( opt ).toBool() ;
	}else{
                _settings->setValue( opt,true ) ;

		return true ;
	}
}

bool settings::showIconOnImportantInfo()
{
	QString opt( "showIconOnImportantInfo" ) ;

        if( _settings->contains( opt ) ){

                return _settings->value( opt ).toBool() ;
	}else{
                _settings->setValue( opt,true ) ;
		return true ;
	}
}

void settings::enableAutoStart( bool autoStart )
{
	QString opt( "autoStartAtLogin" ) ;
        _settings->setValue( opt,autoStart ) ;
}

bool settings::checkNewerKernels()
{
	QString opt( "checkNewerKernels" ) ;

        if( _settings->contains( opt ) ){

                return _settings->value( opt ).toBool() ;
	}else{
                _settings->setValue( opt,false ) ;
		return false ;
	}
}
