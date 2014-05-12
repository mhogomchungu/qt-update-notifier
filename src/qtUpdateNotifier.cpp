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
#include "twitter.h"

#include <qjson/parser.h>

#include <QCoreApplication>

#define _debug 0

qtUpdateNotifier::qtUpdateNotifier() :statusicon()
{
	m_timer = new QTimer() ;
	connect( m_timer,SIGNAL( timeout() ),this,SLOT( automaticCheckForUpdates() ) ) ;

	m_threadIsRunning = false ;

	QCoreApplication::setApplicationName( QString( "qt-update-notfier" ) ) ;
	statusicon::setStatus( statusicon::Passive ) ;
	statusicon::setCategory( statusicon::ApplicationStatus ) ;

	this->changeIcon( QString( "qt-update-notifier" ) ) ;

	m_sleepDuration  = settings::updateCheckInterval() ;

	QString q = settings::delayTimeBeforeUpdateCheck( settings::delayTimeBeforeUpdateCheck() ) ;
	QString z = tr( "Waiting for %1 minutes before checking for updates" ).arg( q ) ;
	QString a = QString( "qt-update-notifier" ) ;
	QString b = tr( "Status" ) ;

	this->showToolTip( a,b,z ) ;

	this->setupTranslationText() ;

	QCoreApplication::setApplicationName( tr( "Qt-update-notifier" ) ) ;

	m_url   = settings::url() ;
	m_token = settings::token() ;
}

void qtUpdateNotifier::logWindowShow()
{
	logWindow * w = new logWindow( tr( "Update output log window" ) ) ;
	connect( this,SIGNAL( updateLogWindow() ),w,SLOT( updateLogWindow() ) ) ;
	connect( this,SIGNAL( configOptionsChanged_1() ),w,SLOT( updateLogWindow() ) ) ;
	w->showLogWindow() ;
}

void qtUpdateNotifier::aptGetLogWindow()
{
	logWindow * w = new logWindow( tr( "Apt-get upgrade output log window" ) )  ;
	connect( this,SIGNAL( updateLogWindow() ),w,SLOT( updateLogWindow_1() ) ) ;
	connect( this,SIGNAL( configOptionsChanged_1() ),w,SLOT( updateLogWindow_1() ) ) ;
	w->showAptGetWindow() ;
}

void qtUpdateNotifier::start()
{
	QMetaObject::invokeMethod( this,"run",Qt::QueuedConnection ) ;
}

void qtUpdateNotifier::changeIcon( QString icon )
{
	statusicon::setIconByName( icon ) ;
	statusicon::setAttentionIconByName( icon ) ;
}

void qtUpdateNotifier::startUpdater()
{
	Task * t = new Task() ;

	connect( t,SIGNAL( taskFinished( int ) ),this,SLOT( synapticStatus( int ) ) ) ;

	if( this->autoRefreshSYnaptic() ){
		t->start( Task::autoRefreshStartSYnaptic ) ;
	}else{
		t->start( Task::startSynaptic ) ;
	}
}

void qtUpdateNotifier::synapticStatus( int r )
{
	if( r != 0 ){
		this->logActivity( tr( "Synaptic exited with errors" ) ) ;
	}
	this->doneUpdating() ;
}

void qtUpdateNotifier::networResponse( QNetworkReply * r )
{
	QList<QByteArray> l = r->rawHeaderList() ;

	QString e ;

	if( m_debug ){
		for( const auto& it : l ){
			e += it + ":" + r->rawHeader( it ) + "\n" ;
		}
	}

	QByteArray data = r->readAll() ;

	if( data.isEmpty() ){
		return ;
	}

	this->checkAnnouncements( data ) ;

	QJson::Parser parser ;

	bool ok ;

	QVariant rr = parser.parse( data,&ok ) ;

	e += "\n" ;

	if( ok ){
		QVariantList l = rr.toList() ;

		for( const auto& it : l ){
			QVariantMap map = it.toMap() ;
			 e += map[ "created_at" ].toString() + ":\n" + map[ "text" ].toString() + "\n\n" ;
		}

		e += "https://twitter.com/iluvpclinuxos" ;

		emit msg( e ) ;
	}
}

void qtUpdateNotifier::checkAnnouncements( const QByteArray& data )
{
	QJson::Parser parser ;

	bool ok ;

	QString e ;

	QVariant rr = parser.parse( data,&ok ) ;

	if( ok ){
		QVariantList l = rr.toList() ;

		qulonglong s = settings::getLastTwitterUpdate().toULongLong() ;

		QString u = l.first().toMap()[ "id_str" ].toString() ;

		if( !u.isEmpty() ){
			settings::setLastTwitterUpdate( u ) ;
		}

		for( const auto& it : l ){
			QVariantMap map = it.toMap() ;
			qulonglong z = map[ "id_str" ].toString().toULongLong() ;
			if( z <= s ){
				break ;
			}else{
				QString a = map[ "text" ].toString() ;
				if( a.contains( "ANNOUNCEMENT" ) ){
					this->showToolTip( QString( "qt-update-notifier-important-info" ),
							   tr( "No updates found" ) ) ;
					this->logActivity_1( a ) ;
				}
			}
		}
	}
}

void qtUpdateNotifier::checkTwitter()
{
	this->accessTwitter() ;

	twitter * t = new twitter() ;

	connect( this,SIGNAL( msg( QString ) ),t,SLOT( msg( QString ) ) ) ;

	t->ShowUI( tr( "connecting ..." ) ) ;
}

void qtUpdateNotifier::accessTwitter()
{
	QUrl url( m_url ) ;

	QNetworkRequest rqt( url ) ;

	rqt.setRawHeader( "Host","api.twitter.com" ) ;
	rqt.setRawHeader( "User-Agent","qt-update-notifier" ) ;
	rqt.setRawHeader( "Authorization",m_token ) ;
	rqt.setRawHeader( "Accept-Encoding","text/plain" ) ;

	m_manager->get( rqt ) ;
}

void qtUpdateNotifier::doneUpdating()
{
	QDateTime d ;
	d.setMSecsSinceEpoch( this->nextScheduledUpdateTime() ) ;

	QString n = tr( "Next update check will be at %1" ).arg( d.toString( Qt::TextDate ) ) ;

	QString y = QString( "qt-update-notifier" ) ;
	QString z = tr( "Status" ) ;
	this->showToolTip( y,z,n ) ;
	statusicon::setStatus( statusicon::Passive ) ;
}

bool qtUpdateNotifier::autoStartEnabled()
{
	return settings::autoStartEnabled() ;
}

void qtUpdateNotifier::toggleAutoStart( bool autoStartEnable )
{
	settings::enableAutoStart( autoStartEnable ) ;
}

void qtUpdateNotifier::openConfigureDialog()
{
	configureDialog * cfg = new configureDialog() ;
	connect( cfg,SIGNAL( toggleAutoStart( bool ) ),this,SLOT( toggleAutoStart( bool ) ) ) ;
	connect( cfg,SIGNAL( setUpdateInterval( int ) ),this,SLOT( setUpdateInterval( int ) ) ) ;
	connect( cfg,SIGNAL( configOptionsChanged() ),this,SLOT( configOptionsChanged() ) ) ;
	connect( cfg,SIGNAL( autoReshreshSynaptic( bool ) ),this,SLOT( autoRefreshSynaptic( bool ) ) ) ;
	cfg->showUI() ;
}

void qtUpdateNotifier::autoRefreshSynaptic( bool b )
{
	settings::setAutoRefreshSynaptic( b ) ;
}

bool qtUpdateNotifier::autoRefreshSYnaptic()
{
	return settings::autoRefreshSynaptic() ;
}

void qtUpdateNotifier::configOptionsChanged()
{
	emit configOptionsChanged_1() ;
}

void qtUpdateNotifier::setupTranslationText()
{
	QString e = settings::prefferedLanguage() ;
	QString r( "english_US" ) ;
	if( e == r ){
		/*
		 *english_US language,its the default and hence dont load anything
		 */
	}else{
		m_translator = new QTranslator( this ) ;
		m_translator->load( e.toLatin1().constData(),QString( QT_UPDATE_NOTIFIER_TRANSLATION_PATH ) ) ;
		QCoreApplication::installTranslator( m_translator ) ;
	}
}

int qtUpdateNotifier::instanceAlreadyRunning()
{
	QString r = settings::prefferedLanguage() ;
	QString e( "english_US" ) ;
	if( e == r ){
		/*
		 * english_US language,its the default and hence dont load anything
		 */
		qDebug() << tr( "Another instance is already running, closing this one" ) ;
	}else{
		const char * x[ 2 ] ;
		x[ 0 ] = "qt-update-notifier" ;
		x[ 1 ] = 0 ;
		int z = 1 ;
		QCoreApplication app( z,( char ** ) x ) ;
		QTranslator * translator = new QTranslator() ;
		translator->load( r.toLatin1().constData(),QString( QT_UPDATE_NOTIFIER_TRANSLATION_PATH ) ) ;
		app.installTranslator( translator ) ;
		qDebug() << tr( "Another instance is already running, closing this one" ) ;
		app.removeTranslator( translator ) ;
		translator->deleteLater() ;
	}
	return 1 ;
}

void qtUpdateNotifier::run()
{
	this->logActivity( tr( "Qt-update-notifier started" ) ) ;

	QAction * ac ;

	ac = statusicon::getAction( tr( "Open synaptic" ) ) ;
	connect( ac,SIGNAL( triggered() ),this,SLOT( startUpdater() ) ) ;

	ac = statusicon::getAction( tr( "Check twitter" ) ) ;
	connect( ac,SIGNAL( triggered() ),this,SLOT( checkTwitter() ) ) ;

	ac = statusicon::getAction( tr( "Check for updates" ) ) ;
	connect( ac,SIGNAL( triggered() ),this,SLOT( manualCheckForUpdates() ) ) ;

	ac = statusicon::getAction( tr( "Done updating" ) ) ;
	connect( ac,SIGNAL( triggered() ),this,SLOT( doneUpdating() ) ) ;

	ac = statusicon::getAction( tr( "Open update log window" ) ) ;
	connect( ac,SIGNAL( triggered() ),this,SLOT( logWindowShow() ) ) ;

	ac = statusicon::getAction( tr( "Open apt-get log window" ) ) ;
	connect( ac,SIGNAL( triggered() ),this,SLOT( aptGetLogWindow() ) ) ;

	ac = statusicon::getAction( tr( "Configuration window" ) ) ;
	connect( ac,SIGNAL( triggered() ),this,SLOT( openConfigureDialog() ) ) ;

	statusicon::setStandardActionsEnabled( false ) ;

	statusicon::addQuitAction() ;

	QTimer * t = new QTimer() ;
	t->setSingleShot( true ) ;
	connect( t,SIGNAL( timeout() ),this,SLOT( checkForUpdatesOnStartUp() ) ) ;
	connect( t,SIGNAL( timeout() ),t,SLOT( deleteLater() ) ) ;
	t->start( settings::delayTimeBeforeUpdateCheck() ) ;

	m_manager = new QNetworkAccessManager (this ) ;
	connect( m_manager,SIGNAL( finished( QNetworkReply * ) ),this,SLOT( networResponse( QNetworkReply * ) ) ) ;
}

void qtUpdateNotifier::printTime( const QString& zz,u_int64_t time )
{
	QDateTime d ;
	d.setMSecsSinceEpoch( time ) ;
	qDebug() << zz << d.toString( Qt::TextDate ) ;
}

void qtUpdateNotifier::checkForUpdatesOnStartUp()
{
	if( settings::firstTimeRun() ){
		m_timer->stop() ;
		m_timer->start( m_sleepDuration ) ;
		this->automaticCheckForUpdates() ;
	}else{
		m_currentTime = this->getCurrentTime() ;
		u_int64_t x = m_currentTime ;
		u_int64_t y = this->nextScheduledUpdateTime() ;
		u_int64_t z = m_sleepDuration ;

		int64_t interval = y - x ;

		if( interval >= 0 ){
			/*
			 * the wait interval has not passed,wait for the remainder of the interval before
			 * checking for updates
			 */
			QTimer * t = new QTimer() ;
			t->setSingleShot( true ) ;
			connect( t,SIGNAL( timeout() ),t,SLOT( deleteLater() ) ) ;
			connect( t,SIGNAL( timeout() ),this,SLOT( startTimer() ) ) ;
			t->start( interval ) ;

			this->showToolTip( QString( "qt-update-notifier" ),tr( "Status" ),interval ) ;

			this->logActivity( this->logMsg() ) ;
		}else{
			u_int64_t e = ( x - y ) / z ;
			e = e + 1 ;
			z = z * e ;

			this->writeUpdateTimeToConfigFile( y + z ) ;

			this->logActivity( tr( "Automatic check for updates initiated" ) ) ;

			this->checkForUpdates() ;

			QTimer * t = new QTimer() ;
			t->setSingleShot( true ) ;
			connect( t,SIGNAL( timeout() ),t,SLOT( deleteLater() ) ) ;
			connect( t,SIGNAL( timeout() ),this,SLOT( startTimer_1() ) ) ;
			t->start( y + z - x ) ;
		}
	}
}

void qtUpdateNotifier::startTimer()
{
	m_timer->stop() ;
	m_timer->start( m_sleepDuration ) ;
	this->automaticCheckForUpdates() ;
}

void qtUpdateNotifier::startTimer_1()
{
	m_timer->stop() ;
	m_timer->start( m_sleepDuration ) ;
	this->automaticCheckForUpdates() ;
}

u_int64_t qtUpdateNotifier::getCurrentTime()
{
	return static_cast<u_int64_t>( QDateTime::currentDateTime().toMSecsSinceEpoch() ) ;
}

QString qtUpdateNotifier::getCurrentTime_1()
{
	return QDateTime::currentDateTime().toString( Qt::TextDate ) ;
}

void qtUpdateNotifier::logActivity( const QString& msg )
{
	QString t = this->getCurrentTime_1() ;
	QString log = QString( "%1:   %2\n").arg( t ).arg( msg ) ;
	utility::writeToFile( settings::activityLogFilePath(),log,false ) ;
	emit updateLogWindow() ;
}

void qtUpdateNotifier::logActivity_1( const QString& msg )
{
	QString line = QString( "------------------------------------------------------" ) ;
	line += QString( "----------------------------------------------------------------" ) ;
	QString t = this->getCurrentTime_1() ;
	QString log = QString( "%1\n%2:   %3\n%4\n" ).arg( line ).arg( t ).arg( msg ).arg( line )  ;
	utility::writeToFile( settings::activityLogFilePath(),log,false ) ;
	emit updateLogWindow() ;
}

void qtUpdateNotifier::setDebug( bool debug )
{
	m_debug = debug ;
}

u_int64_t qtUpdateNotifier::nextScheduledUpdateTime()
{
	return settings::nextScheduledUpdateTime() ;
}

void qtUpdateNotifier::writeUpdateTimeToConfigFile( u_int64_t time )
{
	settings::writeUpdateTimeToConfigFile( time ) ;
}

void qtUpdateNotifier::manualCheckForUpdates()
{
	this->logActivity( tr( "Manual check for updates initiated" ) ) ;
	this->checkForUpdates() ;
}

void qtUpdateNotifier::automaticCheckForUpdates()
{
	this->logActivity( tr( "Automatic check for updates initiated" ) ) ;
	this->checkForUpdates() ;
	this->writeUpdateTimeToConfigFile( this->getCurrentTime() + m_sleepDuration ) ;
}

void qtUpdateNotifier::checkForUpdates()
{
	if( m_threadIsRunning ){
		this->logActivity( tr( "Warning:\tattempt to start update check while another one is still in progress" ) ) ;
	}else{
		QString icon = QString( "qt-update-notifier-updating" ) ;

		this->showToolTip( icon,tr( "Status" ),tr( "Checking for updates" ) ) ;

		m_threadIsRunning = true ;

		Task * t = new Task() ;

		t->setLocalLanguage( settings::prefferedLanguage() ) ;
		t->setConfigPath( settings::configPath() ) ;

		connect( t,SIGNAL( taskFinished( int,QStringList ) ),this,SLOT( updateStatus( int,QStringList ) ) ) ;
		t->start( Task::checkUpDates ) ;
	}
}

void qtUpdateNotifier::saveAptGetLogOutPut( const QStringList& log )
{
	QString x = log.at( 1 ) ;
	if( x == tr( "No updates found" ) ){
		/*
		 * update log file only when there are new updates
		 */
	}else{
		QString line = QString( "-------------------------------------------------------------------------------\n" ) ;
		QString msg = tr( "Log entry was created at: " ) ;
		QString header = line + msg + QDateTime::currentDateTime().toString( Qt::TextDate ) + QString( "\n" ) + line ;
		utility::writeToFile( settings::aptGetLogFilePath(),header + x,true ) ;
	}
}

void qtUpdateNotifier::autoUpdatePackages()
{
	if( settings::autoUpdatePackages() ){
		QString icon = QString( "qt-update-notifier-updating" ) ;
		this->showToolTip( icon,tr( "Status" ),tr( "Update in progress, do not power down computer" ) ) ;
		statusicon::setStatus( statusicon::NeedsAttention ) ;
		this->logActivity( tr( "Automatic package update initiated" ) ) ;

		Task * t = new Task() ;
		connect( t,SIGNAL( taskFinished( int ) ),this,SLOT( autoUpdateResult( int ) ) ) ;
		t->start( Task::updateSystem ) ;
	}else{
		this->logActivity( this->logMsg() ) ;
	}
}

void qtUpdateNotifier::autoUpdateResult( int r )
{
	if( r == 0 || r == 2 ){
		QString icon = QString( "qt-update-notifier" ) ;
		this->showToolTip( icon,tr( "Automatic package update completed" ) ) ;
	}else{
		QString icon = QString( "qt-update-notifier-important-info" ) ;
		this->showToolTip( icon,tr( "Automatic package update failed" ) ) ;
	}

	statusicon::setStatus( statusicon::Passive ) ;
}

void qtUpdateNotifier::autoDownloadPackages( int r )
{
	QString icon = QString( "qt-update-notifier-updates-are-available" ) ;

	if( r == 0 ){
		this->showToolTip( icon,tr( "Downloading of packages completed" ) ) ;
		statusicon::setStatus( statusicon::NeedsAttention ) ;
		this->autoUpdatePackages() ;
	}else{
		this->showToolTip( icon,tr( "Downloading of packages failed" ) ) ;
	}
}

void qtUpdateNotifier::autoDownloadPackages()
{
	if( settings::autoDownloadPackages() ){
		QString icon = QString( "qt-update-notifier-updating" ) ;
		this->showToolTip( icon,tr( "Status" ),tr( "Downloading packages" ) ) ;
		statusicon::setStatus( statusicon::NeedsAttention ) ;
		this->logActivity( tr( "Packages downloading initiated" ) ) ;

		Task * t = new Task() ;
		connect( t,SIGNAL( taskFinished( int ) ),this,SLOT( autoDownloadPackages( int ) ) ) ;
		t->start( Task::downloadPackages ) ;
	}else{
		this->autoUpdatePackages() ;
	}
}

void qtUpdateNotifier::updateStatus( int r,QStringList list )
{
	m_threadIsRunning = false ;
	QString icon ;

	this->saveAptGetLogOutPut( list ) ;

	switch( Task::updateState( r ) ){
	case Task::updatesFound :

		icon = QString( "qt-update-notifier-updates-are-available" ) ;
		statusicon::setStatus( statusicon::NeedsAttention ) ;
		this->showToolTip( icon,tr( "There are updates in the repository" ),list ) ;
		this->autoDownloadPackages() ;

		break ;
	case Task::inconsistentState :

		icon = QString( "qt-update-notifier-important-info" ) ;
		statusicon::setStatus( statusicon::Passive ) ;
		this->showToolTip( icon,tr( "Update check complete, repository appears to be in an inconsistent state" ) ) ;
		this->logActivity_1( list.at( 0 ) ) ;

		break ;
	case Task::noUpdatesFound :

		statusicon::setStatus( statusicon::Passive ) ;
		/*
		 * below function is called from checkForPackageUpdates() routine
		 * icon = QString( "qt-update-notifier" ) ;
		 * this->showToolTip( icon,tr( "No updates found" ) ) ;
		 */
		this->checkForPackageUpdates() ;

		break ;
	case Task::noNetworkConnection :

		icon = QString( "qt-update-notifier" ) ;
		statusicon::setStatus( statusicon::Passive ) ;
		this->showToolTip( icon,tr( "Check skipped, user is not connected to the internet" ) ) ;

		break ;
	case Task::undefinedState :

		icon = QString( "qt-update-notifier" ) ;
		statusicon::setStatus( statusicon::Passive ) ;
		this->showToolTip( icon,tr( "Update check complete, repository is in an unknown state" ) ) ;

		break ;
	default:
		/*
		 * currently,we dont get here,added for completeness' sake
		 */
		icon = QString( "qt-update-notifier" ) ;
		statusicon::setStatus( statusicon::Passive ) ;
		this->showToolTip( icon,tr( "Update check complete, repository is in an unknown state" ) ) ;
		this->checkForPackageUpdates() ;
	}
}

void qtUpdateNotifier::checkForPackageUpdates()
{
	if( settings::skipOldPackageCheck() ){
		this->showToolTip( QString( "qt-update-notifier" ),tr( "No updates found" ) ) ;
	}else{
		Task * t = new Task() ;
		connect( t,SIGNAL( taskFinished( QStringList ) ),this,SLOT( checkOutDatedPackages( QStringList ) ) ) ;
		t->start( Task::checkOutDatedPackages ) ;
	}
}

void qtUpdateNotifier::objectGone( QObject * obj )
{
	qDebug() << "destroyed object" << obj->objectName() ;
}

void qtUpdateNotifier::taskFinished( int taskAction,int taskStatus )
{
	Q_UNUSED( taskAction ) ;
	Q_UNUSED( taskStatus ) ;
}

void qtUpdateNotifier::checkOutDatedPackages( QStringList list )
{
	QString	icon = QString( "qt-update-notifier-important-info" ) ;
	bool updatesFound = false ;

	if( list.size() < 4 ){
		this->showToolTip( QString( "qt-update-notifier" ),tr( "No updates found" ) ) ;
		return ;
	}

	QString kernelVersion = list.at( 0 ) ;

	if( !kernelVersion.isEmpty() ){
		updatesFound = true ;
		this->logActivity_1( kernelVersion ) ;
		this->showToolTip( icon,tr( "Outdated packages found" ) ) ;
	}

	QString libreofficeVersion = list.at( 1 ) ;
	if( !libreofficeVersion.isEmpty() ){
		updatesFound = true ;
		this->logActivity_1( libreofficeVersion ) ;
		this->showToolTip( icon,tr( "Outdated packages found" ) ) ;
	}

	QString virtualBoxVersion = list.at( 2 ) ;
	if( !virtualBoxVersion.isEmpty() ){
		updatesFound = true ;
		this->logActivity_1( virtualBoxVersion ) ;
		this->showToolTip( icon,tr( "Outdated packages found" ) ) ;
	}

	QString callibre = list.at( 3 ) ;
	if( !callibre.isEmpty() ){
		updatesFound = true ;
		this->logActivity_1( callibre ) ;
		this->showToolTip( icon,tr( "Outdated packages found" ) ) ;
	}

	if( updatesFound ){
		emit updateLogWindow() ;
	}else{
		this->showToolTip( QString( "qt-update-notifier" ),tr( "No updates found" ) ) ;
	}

	this->accessTwitter() ;
}

void qtUpdateNotifier::showToolTip( const QString& x,const QString& y,const QStringList& list )
{
	this->logActivity( y ) ;
	statusicon::setToolTip( x,tr( "Updates found" ),list.at( 0 ) ) ;
	this->changeIcon( x ) ;
}

void qtUpdateNotifier::showToolTip( const QString& x,const QString& y,const QString& z )
{
	statusicon::setToolTip( x,y,z ) ;
	this->changeIcon( x ) ;
}

void qtUpdateNotifier::showToolTip( const QString& x,const QString& y,int z )
{
	QString n = tr( "Next update check will be at %1" ).arg( this->nextUpdateTime( z ) ) ;
	statusicon::setToolTip( x,y,n ) ;
	this->changeIcon( x ) ;
}

void qtUpdateNotifier::showToolTip( const QString& x,const QString& y )
{
	QDateTime d ;
	d.setMSecsSinceEpoch( this->nextScheduledUpdateTime() ) ;

	QString n = tr( "Next update check will be at %1" ).arg( d.toString( Qt::TextDate ) ) ;

	if( y == tr( "No updates found" ) ){
		this->logActivity( y ) ;
		this->logActivity( this->logMsg() ) ;
		statusicon::setToolTip( x,y,n ) ;
	}else{
		QString msg = QString( "<table><tr><td><b>%1</b></tr></td><tr><td>%2</tr></td></table>" ).arg( y ).arg( n ) ;
		this->logActivity( y ) ;
		this->logActivity( this->logMsg() ) ;
		statusicon::setToolTip( x,tr( "Status" ),msg ) ;
	}

	this->changeIcon( x ) ;
}

QString qtUpdateNotifier::nextUpdateTime( void )
{
	return this->nextUpdateTime( m_sleepDuration ) ;
}

QString qtUpdateNotifier::nextUpdateTime( int interval )
{
	QDateTime d ;
	d.setMSecsSinceEpoch( QDateTime::currentMSecsSinceEpoch() + interval ) ;
	return d.toString( Qt::TextDate ) ;
}

QString qtUpdateNotifier::logMsg( int interval )
{
	char num[ 64 ] ;
	float f = static_cast<float>( interval ) ;
	snprintf( num,64,"%.2f",f / ( 1000 * 60 * 60 ) ) ;
	QString n = this->nextUpdateTime( interval ) ;
	return tr( "Scheduled next check to be in %1 hours at %2" ).arg( QString( num ) ).arg( n ) ;
}

QString qtUpdateNotifier::logMsg( void )
{
	u_int64_t x = this->getCurrentTime() ;
	u_int64_t y = this->nextScheduledUpdateTime() ;
	u_int64_t e = y - x ;
	char num[ 64 ] ;
	float f = static_cast<float>( e ) ;
	snprintf( num,64,"%.2f",f / ( 1000 * 60 * 60 ) ) ;
	QString n = this->nextUpdateTime( e ) ;
	return tr( "Scheduled next check to be in %1 hours at %2" ).arg( QString( num ) ).arg( n ) ;
}

void qtUpdateNotifier::scheduleUpdates( int interval )
{
	this->logActivity( this->logMsg( interval ) ) ;
	m_timer->stop() ;
	m_timer->start( interval ) ;
}

void qtUpdateNotifier::setUpdateInterval( int interval )
{
	this->logActivity( tr( "Rescheduling update check" ) ) ;

	m_sleepDuration = interval ;

	this->logActivity( this->logMsg( m_sleepDuration ) ) ;

	this->writeUpdateTimeToConfigFile( this->getCurrentTime() + m_sleepDuration ) ;

	m_timer->stop() ;
	m_timer->start( m_sleepDuration ) ;

	//QString x = statusicon::iconName() ;
	QString x = QString( "qt-update-notifier" ) ;
	QString y = statusicon::toolTipTitle() ;

	int d = static_cast<int>( m_sleepDuration ) ;
	this->showToolTip( x,y,d ) ;
}

void qtUpdateNotifier::closeApp()
{
	if( m_threadIsRunning ){
		;
	}else{
		QCoreApplication::exit( 0 ) ;
	}
}

void qtUpdateNotifier::closeApp( int st )
{
	if( m_threadIsRunning ){
		;
	}else{
		QCoreApplication::exit( st ) ;
	}
}

qtUpdateNotifier::~qtUpdateNotifier()
{
	if( m_timer ){
		m_timer->stop() ;
		m_timer->deleteLater() ;
	}
	this->logActivity( tr( "Qt-update-notifier quitting" ) ) ;
}
