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

#include <utility>

template< typename T >
class QObject_raii
{
public:
	explicit QObject_raii( T t ) : m_qObject( t )
	{
	}
	~QObject_raii()
	{
		m_qObject->deleteLater() ;
	}
private:
	T m_qObject ;
};

#define qObject_raii( x ) QObject_raii< decltype( x ) > QObject_raii_x( x ) ; Q_UNUSED( QObject_raii_x )

qtUpdateNotifier::qtUpdateNotifier() : statusicon()
{
	m_manager = nullptr ;

	m_timer = new QTimer() ;
	connect( m_timer,SIGNAL( timeout() ),this,SLOT( automaticCheckForUpdates() ) ) ;

	m_threadIsRunning = false ;

	QCoreApplication::setApplicationName( QString( "qt-update-notfier" ) ) ;
	statusicon::setStatus( statusicon::Passive ) ;
	statusicon::setCategory( statusicon::ApplicationStatus ) ;

	m_defaulticon = settings::defaultIcon() ;

	this->changeIcon( m_defaulticon ) ;

	m_sleepDuration  = settings::updateCheckInterval() ;
	m_nextScheduledUpdateTime = settings::nextScheduledUpdateTime() ;

	this->setupTranslationText() ;

	QString q = settings::delayTimeBeforeUpdateCheck( settings::delayTimeBeforeUpdateCheck() ) ;
	QString z = tr( "Waiting for %1 minutes before checking for updates" ).arg( q ) ;
	QString a = m_defaulticon ;
	QString b = tr( "Status" ) ;

	this->showToolTip( a,b,z ) ;

	QCoreApplication::setApplicationName( tr( "Qt-update-notifier" ) ) ;

	m_url   = settings::url() ;
	m_token = settings::token() ;

	m_showIconOnImportantInfo = settings::showIconOnImportantInfo() ;
	m_networkConnectivityChecker = settings::networkConnectivityChecker() ;
}

void qtUpdateNotifier::logWindowShow()
{
	auto w = new logWindow( tr( "Update output log window" ) ) ;
	connect( this,SIGNAL( updateLogWindow() ),w,SLOT( updateLogWindow() ) ) ;
	connect( this,SIGNAL( configOptionsChanged_1() ),w,SLOT( updateLogWindow() ) ) ;
	w->showLogWindow() ;
}

void qtUpdateNotifier::aptGetLogWindow()
{
	auto w = new logWindow( tr( "Apt-get upgrade output log window" ) )  ;
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
	if( Task::await< bool >( utility::startSynaptic() ) ){

		this->logActivity( tr( "Synaptic exited with errors" ) ) ;
	}

	this->doneUpdating() ;
}

void qtUpdateNotifier::networResponse( QNetworkReply * r )
{
	qObject_raii( r ) ;

	QList<QByteArray> l = r->rawHeaderList() ;

	QString e ;

	if( m_debug ){
		for( const auto& it : l ){
			e += it + ":" + r->rawHeader( it ) + "\n" ;
		}
	}

	QString s = r->rawHeader( "status" ) ;

	if( s != "200 OK" ){
		emit msg( e ) ;
		return ;
	}

	QByteArray data = r->readAll() ;

	if( data.isEmpty() ){
		emit msg( e ) ;
		return ;
	}

	QJson::Parser parser ;

	bool ok ;

	auto p = parser.parse( data,&ok ) ;

	if( ok ){

		QList<QVariant> l = p.toList() ;

		auto s = settings::getLastTwitterUpdate().toULongLong() ;

		auto u = l.first().toMap()[ "id_str" ].toString() ;

		settings::setLastTwitterUpdate( u ) ;

		for( const auto& it : l ){

			auto map  = it.toMap() ;
			auto z    = map[ "id_str" ].toString().toULongLong() ;
			auto text = map[ "text" ].toString() ;

			if( z > s ){
				if( text.contains( "ANNOUNCEMENT" ) ){
					this->showToolTip( QString( "qt-update-notifier-important-info" ),
							   tr( "No updates found" ) ) ;
					this->logActivity_1( text ) ;
					this->showIconOnImportantInfo() ;
				}
			}

			e += "\n" + map[ "created_at" ].toString() + ":\n" + text + "\n" ;
		}

		e += "\nhttps://twitter.com/iluvpclinuxos" ;

		emit msg( e ) ;
	}else{
		emit msg( e ) ;
	}
}

void qtUpdateNotifier::checkTwitter()
{
	auto t = new twitter() ;

	connect( this,SIGNAL( msg( QString ) ),t,SLOT( msg( QString ) ) ) ;

	t->ShowUI( tr( "connecting ..." ) ) ;

	this->accessTwitter() ;
}

void qtUpdateNotifier::showIconOnImportantInfo()
{
	if( m_showIconOnImportantInfo ){
		statusicon::setStatus( statusicon::NeedsAttention ) ;
	}
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

	QString z = tr( "Status" ) ;
	this->showToolTip( m_defaulticon,z,n ) ;
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
	auto cfg = new configureDialog() ;
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
		*( x + 0 ) = "qt-update-notifier" ;
		*( x + 1 ) = nullptr ;
		int z = 1 ;
		QCoreApplication app( z,const_cast< char ** >( x ) ) ;
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

	auto t = new QTimer() ;
	t->setSingleShot( true ) ;
	connect( t,SIGNAL( timeout() ),this,SLOT( checkForUpdatesOnStartUp() ) ) ;
	connect( t,SIGNAL( timeout() ),t,SLOT( deleteLater() ) ) ;
	t->start( settings::delayTimeBeforeUpdateCheck() ) ;

	m_manager = new QNetworkAccessManager( this ) ;
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
			auto t = new QTimer() ;
			t->setSingleShot( true ) ;
			connect( t,SIGNAL( timeout() ),t,SLOT( deleteLater() ) ) ;
			connect( t,SIGNAL( timeout() ),this,SLOT( startTimer() ) ) ;
			t->start( interval ) ;

			this->showToolTip( m_defaulticon,tr( "Status" ),interval ) ;

			this->logActivity( this->logMsg() ) ;
		}else{
			u_int64_t e = ( x - y ) / z ;
			e = e + 1 ;
			z = z * e ;

			this->writeUpdateTimeToConfigFile( y + z ) ;

			this->logActivity( tr( "Automatic check for updates initiated" ) ) ;

			this->checkForUpdates() ;

			auto t = new QTimer() ;
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
	return m_nextScheduledUpdateTime ;
}

void qtUpdateNotifier::writeUpdateTimeToConfigFile( u_int64_t time )
{
	m_nextScheduledUpdateTime = time ;
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

		auto r = Task::await< result >( utility::reportUpdates() ) ;

		m_threadIsRunning = false ;

		this->saveAptGetLogOutPut( r.taskOutput ) ;

		switch( r.repositoryState ){
		case result::updatesFound :

			icon = QString( "qt-update-notifier-updates-are-available" ) ;
			statusicon::setStatus( statusicon::NeedsAttention ) ;
			this->showToolTip( icon,tr( "There are updates in the repository" ),r.taskOutput ) ;
			this->autoDownloadPackages() ;

			break ;
		case result::inconsistentState :

			icon = QString( "qt-update-notifier-important-info" ) ;
			statusicon::setStatus( statusicon::Passive ) ;
			this->showToolTip( icon,tr( "Update check complete, repository appears to be in an inconsistent state" ) ) ;
			this->logActivity_1( r.taskOutput.first() ) ;
			this->showIconOnImportantInfo() ;

			break ;
		case result::noUpdatesFound :

			statusicon::setStatus( statusicon::Passive ) ;
			/*
			 * below function is called from checkForPackageUpdates() routine
			 * this->showToolTip( m_defaulticon,tr( "No updates found" ) ) ;
			 */
			this->checkForPackageUpdates() ;
			this->accessTwitter() ;

			break ;
		case result::noNetworkConnection :

			icon = m_defaulticon ;
			statusicon::setStatus( statusicon::Passive ) ;
			this->showToolTip( icon,tr( "Check skipped, user is not connected to the internet" ) ) ;

			break ;
		case result::undefinedState :

			icon = m_defaulticon ;
			statusicon::setStatus( statusicon::Passive ) ;
			this->showToolTip( icon,tr( "Update check complete, repository is in an unknown state" ) ) ;

			break ;
		default:
			/*
			 * currently,we dont get here,added for completeness' sake
			 */
			icon = m_defaulticon ;
			statusicon::setStatus( statusicon::Passive ) ;
			this->showToolTip( icon,tr( "Update check complete, repository is in an unknown state" ) ) ;
			this->checkForPackageUpdates() ;
		}
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

		auto r = Task::await< int >( utility::autoUpdatePackages() ) ;

		if( r == 0 || r == 2 ){
			this->showToolTip( m_defaulticon,tr( "Automatic package update completed" ) ) ;
		}else{
			QString icon = "qt-update-notifier-important-info" ;
			this->showToolTip( icon,tr( "Automatic package update failed" ) ) ;
			this->showIconOnImportantInfo() ;
		}

		statusicon::setStatus( statusicon::Passive ) ;
	}else{
		this->logActivity( this->logMsg() ) ;
	}
}

void qtUpdateNotifier::autoDownloadPackages()
{
	if( settings::autoDownloadPackages() ){

		QString icon = QString( "qt-update-notifier-updating" ) ;
		this->showToolTip( icon,tr( "Status" ),tr( "Downloading packages" ) ) ;
		statusicon::setStatus( statusicon::NeedsAttention ) ;
		this->logActivity( tr( "Packages downloading initiated" ) ) ;

		if( Task::await< bool >( utility::autoDownloadPackages() ) ){

			this->showToolTip( icon,tr( "Downloading of packages completed" ) ) ;
			statusicon::setStatus( statusicon::NeedsAttention ) ;
			this->autoUpdatePackages() ;
		}else{
			this->showToolTip( icon,tr( "Downloading of packages failed" ) ) ;
		}
	}else{
		this->autoUpdatePackages() ;
	}
}

void qtUpdateNotifier::checkForPackageUpdates()
{
	if( settings::skipOldPackageCheck() ){

		this->showToolTip( m_defaulticon,tr( "No updates found" ) ) ;
	}else{
		auto r = Task::await< QString >( utility::checkForPackageUpdates() ) ;

		if( r.isEmpty() ){

			this->showToolTip( m_defaulticon,tr( "No updates found" ) ) ;
		}else{
			QString	icon = QString( "qt-update-notifier-important-info" ) ;
			this->showToolTip( icon,tr( "Outdated packages found" ) ) ;
			this->logActivity_1( r ) ;
			this->showIconOnImportantInfo() ;
			emit updateLogWindow() ;
		}
	}
}

void qtUpdateNotifier::objectGone( QObject * obj )
{
	qDebug() << "destroyed object" << obj->objectName() ;
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
	QString x = m_defaulticon ;
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
