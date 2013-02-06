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

#include "qtUpdateNotifier.h"

qtUpdateNotifier::qtUpdateNotifier() :KStatusNotifierItem( 0 )
{
	m_timer = new QTimer() ;
	connect( m_timer,SIGNAL( timeout() ),this,SLOT( checkForUpdates() ) ) ;
	m_trayMenu = 0 ;
	m_threadIsRunning = false ;
	this->setStatus( KStatusNotifierItem::Passive );
	QCoreApplication::setApplicationName( QString( "qt-update-notifier" ) ) ;
	this->createEnvironment();
	this->logActivity( QString( "qt-update-notifier started" ) ) ;
}

void qtUpdateNotifier::createEnvironment()
{
	KStandardDirs k ;
	QString path = k.localxdgconfdir() + QString( "/qt-update-notifier" ) ;
	QDir d ;
	d.mkdir( path ) ;
	m_configTime = path + QString( "/qt-update-notifier.time" ) ;
	m_configLog = path  + QString( "/qt-update-notifier.log" ) ;

	QFile f( path + QString( "/qt-update-notifier.interval" ) ) ;
	if( !f.exists() ){
		f.open( QIODevice::WriteOnly ) ;
		f.write( "86400") ;
		f.close();
	}
	f.open( QIODevice::ReadOnly ) ;
	QByteArray x = f.readAll() ;
	f.close();
	const char * y = x.constData() ;
	m_sleepDuration = 1000 * atol( y ) ;
}

void qtUpdateNotifier::start()
{
	QMetaObject::invokeMethod( this,"run",Qt::QueuedConnection ) ;
}

void qtUpdateNotifier::changeIcon( QString icon )
{
	this->setIconByName( icon );
}

void qtUpdateNotifier::startSynaptic()
{
	QProcess exe ;
	exe.startDetached( QString( "kdesu -c /usr/sbin/synaptic" ) ) ;
}

void qtUpdateNotifier::run()
{
	this->setCategory( KStatusNotifierItem::ApplicationStatus );
	this->setStatus( KStatusNotifierItem::Passive );
	this->changeIcon( QString( "qt-update-notifier" ) );

	m_trayMenu = new KMenu() ;

	m_trayMenu->addAction( tr( "check for updates" ),this,SLOT( checkForUpdates() ) );
	m_trayMenu->addAction( tr( "open synaptic" ),this,SLOT( startSynaptic() ) );

	this->setContextMenu( m_trayMenu );
	this->setObjectName( "qtUpdateNotifier" );
	this->contextMenu()->setEnabled( true );
	QTimer * t = new QTimer() ;
	connect( t,SIGNAL( timeout() ),this,SLOT( checkForUpdatesOnStartUp() ) ) ;
	connect( t,SIGNAL( timeout() ),t,SLOT( deleteLater() ) ) ;
	t->start( 5 * 60 * 1000 ) ; //wait for 5 minutes before check for updates
	//t->start( 10 * 1000 ) ;
}

void qtUpdateNotifier::checkForUpdatesOnStartUp()
{
	QFile f( m_configTime ) ;
	if( !f.open( QIODevice::ReadOnly ) ){
		/*
		 * config file doesnt seem to be present
		 */
		this->checkForUpdates();
	}else{
		m_currentTime = this->getCurrentTime() ;
		u_int64_t configTime = this->getTimeFromConfigFile() ;
		u_int64_t interval = m_currentTime - configTime ;
		if( interval >= m_sleepDuration ){
			/*
			 * its been more than a day since last check,check now
			 */
			this->checkForUpdates();
		}else{
			/*
			 * its been less than a day,wait for the remaining time before checking
			 */
			this->scheduleUpdates( m_sleepDuration - interval );
		}
	}
}

u_int64_t qtUpdateNotifier::getCurrentTime()
{
	return static_cast<u_int64_t>( QDateTime::currentDateTime().toMSecsSinceEpoch() );
}

void qtUpdateNotifier::logActivity( QString msg )
{
	QFile f( m_configLog ) ;
	f.open( QIODevice::WriteOnly | QIODevice::Append ) ;
	QDateTime t = QDateTime::currentDateTime() ;
	QString time = QString( "%1:\t%2\n").arg( t.toString( Qt::TextDate ) ).arg( msg )  ;
	QByteArray r = time.toAscii() ;
	f.write( r ) ;
	f.close();
}

u_int64_t qtUpdateNotifier::getTimeFromConfigFile()
{
	QFile f( m_configTime ) ;
	if( f.open( QIODevice::ReadOnly ) ){
		QString x = f.readAll() ;
		f.close();
		QByteArray y = x.toAscii() ;
		const char * z = y.constData() ;
		return atoll( z ) ;
	}else{
		return 0 ;
	}
}

void qtUpdateNotifier::writeUpdateTimeToConfigFile()
{
	QFile f( m_configTime ) ;
	f.open( QIODevice::WriteOnly | QIODevice::Truncate ) ;
	char num[ 64 ] ;
	snprintf( num,64,"%llu",this->getCurrentTime() ) ;
	f.write( num ) ;
	f.close();
}

void qtUpdateNotifier::checkForUpdates()
{
	if( m_threadIsRunning ){
		this->logActivity( QString( "warning:\tattempt to start a check while another is already in progress" ) ) ;
		return ;
	}
	this->logActivity( QString( "checking for updates" ) ) ;
	this->changeIcon( QString( "qt-update-notifier-updating" ) );
	this->setStatus( KStatusNotifierItem::Passive );
	m_threadIsRunning = true ;
	m_updates = new check_updates( this ) ;
	connect( m_updates,SIGNAL( updateList( QStringList ) ),this,SLOT( updateList( QStringList ) ) ) ;
	connect( m_updates,SIGNAL( updatesFound( bool,QStringList ) ),this,SLOT( updatesFound( bool,QStringList ) ) ) ;
	connect( m_updates,SIGNAL( terminated() ),this,SLOT( threadTerminated() ) ) ;
	connect( m_updates,SIGNAL( finished() ),this,SLOT( threadisFinished() ) ) ;
	m_updatesFound = false ;
	this->contextMenu()->setEnabled( false );
	m_updates->start();
}

void qtUpdateNotifier::updatesFound( bool found,QStringList list )
{
	Q_UNUSED( list ) ;
	m_threadIsRunning = false ;
	this->contextMenu()->setEnabled( true );
	if( found ){
		this->setStatus( KStatusNotifierItem::NeedsAttention );
		this->changeIcon( QString( "qt-update-notifier-updates-are-available" ) ) ;
		m_updatesFound = true ;
		this->logActivity( QString( "update check complete,updates found" ) ) ;
	}else{
		this->changeIcon( QString( "qt-update-notifier" ) );
		this->logActivity( QString( "update check complete,no updates found" ) ) ;
	}
	this->writeUpdateTimeToConfigFile() ;
	this->scheduleUpdates( m_sleepDuration );
}

void qtUpdateNotifier::scheduleUpdates( int interval )
{
	int hours = interval / ( 1000 * 60 * 60 ) ;
	QString r = QString( "scheduled next check to be in %1 hours" ).arg( QString::number( hours ) ) ;
	this->logActivity( r ) ;
	m_timer->stop();
	m_timer->start( interval );
}

void qtUpdateNotifier::updateList( QStringList list )
{
	Q_UNUSED( list ) ;
}

void qtUpdateNotifier::threadTerminated( void )
{
	QCoreApplication::exit( 0 ) ;
}

void qtUpdateNotifier::threadisFinished()
{
	m_threadIsRunning = false ;
	this->contextMenu()->setEnabled( true );
}

void qtUpdateNotifier::_activate( QPoint &p )
{
	Q_UNUSED( p ) ;
	this->checkForUpdates();
}

void qtUpdateNotifier::_activateRequested( bool active,const QPoint &pos )
{
	Q_UNUSED( active ) ;
	Q_UNUSED( pos ) ;
	this->checkForUpdates();
}

void qtUpdateNotifier::closeApp()
{
	if( m_threadIsRunning ){
		m_updates->terminate();
	}else{
		QCoreApplication::exit( 0 ) ;
	}
}

void qtUpdateNotifier::closeApp( int st )
{
	if( m_threadIsRunning ){
		m_updates->terminate();
	}else{
		QCoreApplication::exit( st ) ;
	}
}

void qtUpdateNotifier::closeApplication()
{

}

qtUpdateNotifier::~qtUpdateNotifier()
{
	if( m_trayMenu ){
		m_trayMenu->deleteLater();
	}
	if( m_timer ){
		m_timer->stop();
		m_timer->deleteLater();
	}
}
