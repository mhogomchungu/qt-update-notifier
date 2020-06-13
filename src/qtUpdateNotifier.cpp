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

#include "qtUpdateNotifier.h"
#include "twitter.h"

#include <QCoreApplication>
#include <QJsonDocument>

#include <utility>

#include <iostream>

struct jsonResult
{
	bool ok ;
	QList<QVariant> value ;
} ;

jsonResult _parseJSON( const QByteArray& e )
{
	QJsonParseError error ;

	auto r = QJsonDocument::fromJson( e,&error ) ;

	if( error.error == QJsonParseError::NoError ){

		return { true,r.toVariant().toList() } ;
	}else{
		return { false,QList< QVariant >() } ;
	}
}

qtUpdateNotifier::qtUpdateNotifier( bool e ) : m_autoStart( e )
{
	this->setupTranslationText() ;
	m_twitter.translate() ;
}

void qtUpdateNotifier::logWindowShow()
{
	auto w = new logWindow( tr( "Update output log window" ) ) ;
	connect( this,SIGNAL( updateLogWindow() ),w,SLOT( updateLogWindow() ) ) ;
	connect( this,SIGNAL( configOptionsChanged_1() ),w,SLOT( updateLogWindow() ) ) ;
	w->showLogWindow() ;
}

void qtUpdateNotifier::ignorePackageList()
{
	auto w = new ignorepackagelist() ;

	w->show() ;
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
	m_statusicon.setIconByName( icon ) ;
	m_statusicon.setAttentionIconByName( icon ) ;
}

void qtUpdateNotifier::startUpdater()
{
	if( utility::startSynaptic().await() ){

		this->logActivity( tr( "Synaptic exited with errors" ) ) ;
	}

	this->doneUpdating() ;
}

QString qtUpdateNotifier::getLastTwitterUpdate()
{
	return m_lastTwitterUpdate ;
}

void qtUpdateNotifier::setLastTwitterUpdate( const QString& e )
{
	m_lastTwitterUpdate = e ;
	settings::setLastTwitterUpdate( e ) ;
}

QString qtUpdateNotifier::networResponse( QNetworkReply& s )
{
	auto l = s.rawHeaderList() ;

	QString e ;

	if( m_debug ){

		for( const auto& it : l ){

			e += it + ":" + s.rawHeader( it ) + "\n" ;
		}
	}

	if( s.rawHeader( "status" ) != "200 OK" ){

		return e ;
	}

	auto data = s.readAll() ;

	if( data.isEmpty() ){

		return e ;
	}

	auto j = _parseJSON( data ) ;

	if( j.ok ){

		const auto& l = j.value ;

		auto s = this->getLastTwitterUpdate().toULongLong() ;

		this->setLastTwitterUpdate( l.first().toMap()[ "id_str" ].toString() ) ;

		for( const auto& it : l ){

			auto map  = it.toMap() ;
			auto z    = map[ "id_str" ].toString().toULongLong() ;
			auto text = map[ "text" ].toString() ;

			if( z > s ){

				if( text.contains( "ANNOUNCEMENT" ) ){

					this->showToolTip( "qt-update-notifier-important-info",tr( "No updates found" ) ) ;
					this->logActivity_1( text ) ;
					this->showIconOnImportantInfo() ;
				}
			}

			e += "\n" + map[ "created_at" ].toString() + ":\n" + text + "\n" ;
		}

		e += "\nhttps://twitter.com/iluvpclinuxos" ;

		return e ;
	}else{
		return e ;
	}
}

void qtUpdateNotifier::checkTwitter()
{
	m_twitter.ShowUI( tr( "connecting ..." ) ) ;

	QUrl url( m_url ) ;

	QNetworkRequest s( url ) ;

	s.setRawHeader( "Host","api.twitter.com" ) ;
	s.setRawHeader( "User-Agent","qt-update-notifier" ) ;
	s.setRawHeader( "Authorization",m_token ) ;
	s.setRawHeader( "Accept-Encoding","text/plain" ) ;

	m_manager.get( 30,s,[ this ]( QNetworkReply& e ){

		m_twitter.msg( this->networResponse( e ) ) ;

	},[ this ](){

		m_twitter.msg( QString() ) ;
	} ) ;
}

void qtUpdateNotifier::showIconOnImportantInfo()
{
	if( m_showIconOnImportantInfo ){

		m_statusicon.setStatus( statusicon::statusicon::ItemStatus::NeedsAttention ) ;
	}
}

void qtUpdateNotifier::doneUpdating()
{
	QDateTime d ;
	d.setMSecsSinceEpoch( this->nextScheduledUpdateTime() ) ;

	auto n = tr( "Next update check will be at %1" ).arg( d.toString( Qt::TextDate ) ) ;

	auto z = tr( "Status" ) ;

	this->showToolTip( m_defaulticon,z,n ) ;
	m_statusicon.setStatus( statusicon::ItemStatus::Passive ) ;
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
	auto e = settings::prefferedLanguage() ;

	if( e == "english_US" ){

		/*
		 *english_US language,its the default and hence dont load anything
		 */
	}else{
                auto q = new QTranslator( this ) ;
                q->load( e.toLatin1().constData(),QT_UPDATE_NOTIFIER_TRANSLATION_PATH ) ;
                QCoreApplication::installTranslator( q ) ;
	}
}

void qtUpdateNotifier::buildGUI()
{
        connect( &m_timer,SIGNAL( timeout() ),this,SLOT( automaticCheckForUpdates() ) ) ;

        m_threadIsRunning = false ;

	m_statusicon.setStatus( statusicon::ItemStatus::Passive ) ;
	m_statusicon.setCategory( statusicon::ItemCategory::ApplicationStatus ) ;

	m_defaulticon = settings::defaultIcon() + ".png" ;

        this->changeIcon( m_defaulticon ) ;

        auto q = settings::delayTimeBeforeUpdateCheck( settings::delayTimeBeforeUpdateCheck() ) ;
	auto z = tr( "Waiting for %1 minutes before checking for updates" ).arg( q ) ;
        auto a = m_defaulticon ;
        auto b = tr( "Status" ) ;

        this->showToolTip( a,b,z ) ;

        m_url   = settings::url() ;
        m_token = settings::token() ;

        m_showIconOnImportantInfo = settings::showIconOnImportantInfo() ;
        m_networkConnectivityChecker = settings::networkConnectivityChecker() ;

        m_lastTwitterUpdate = settings::getLastTwitterUpdate() ;
}

void qtUpdateNotifier::run()
{
        if( m_autoStart && !qtUpdateNotifier::autoStartEnabled() ){

                QCoreApplication::quit() ;
        }else{
		m_sleepDuration  = settings::updateCheckInterval() ;

                this->buildGUI() ;

                this->logActivity( tr( "Qt-update-notifier started" ) ) ;

                QAction * ac ;

		ac = m_statusicon.getAction( tr( "Open synaptic" ) ) ;
                connect( ac,SIGNAL( triggered() ),this,SLOT( startUpdater() ) ) ;

		ac = m_statusicon.getAction( tr( "Check twitter" ) ) ;
                connect( ac,SIGNAL( triggered() ),this,SLOT( checkTwitter() ) ) ;

		ac = m_statusicon.getAction( tr( "Check for updates" ) ) ;
                connect( ac,SIGNAL( triggered() ),this,SLOT( manualCheckForUpdates() ) ) ;

		ac = m_statusicon.getAction( tr( "Done updating" ) ) ;
                connect( ac,SIGNAL( triggered() ),this,SLOT( doneUpdating() ) ) ;

		ac = m_statusicon.getAction( tr( "Open update log window" ) ) ;
                connect( ac,SIGNAL( triggered() ),this,SLOT( logWindowShow() ) ) ;

		ac = m_statusicon.getAction( tr( "Open apt-get log window" ) ) ;
                connect( ac,SIGNAL( triggered() ),this,SLOT( aptGetLogWindow() ) ) ;

		ac = m_statusicon.getAction( tr( "Configuration window" ) ) ;
                connect( ac,SIGNAL( triggered() ),this,SLOT( openConfigureDialog() ) ) ;

		ac = m_statusicon.getAction( tr( "Ignore package list" ) ) ;
		connect( ac,SIGNAL( triggered() ),this,SLOT( ignorePackageList() ) ) ;

		m_statusicon.setStandardActionsEnabled( false ) ;

		m_statusicon.addQuitAction() ;

		auto s = settings::delayTimeBeforeUpdateCheck() ;

		if( s == 0 ){

			this->checkForUpdatesOnStartUp() ;
		}else{
			auto t = new QTimer() ;

			t->setSingleShot( true ) ;

			connect( t,SIGNAL( timeout() ),this,SLOT( checkForUpdatesOnStartUp() ) ) ;
			connect( t,SIGNAL( timeout() ),t,SLOT( deleteLater() ) ) ;

			t->start( s ) ;
		}
        }
}

void qtUpdateNotifier::printTime( const QString& zz,qint64 time )
{
	QDateTime d ;
	d.setMSecsSinceEpoch( time ) ;
	qDebug() << zz << d.toString( Qt::TextDate ) ;
}

void qtUpdateNotifier::checkForUpdatesOnStartUp()
{
	if( settings::firstTimeRun() ){

		m_timer.stop() ;
		m_timer.start( static_cast< int >( m_sleepDuration ) ) ;

		m_nextScheduledUpdateTime = this->getCurrentTime() ;

		this->automaticCheckForUpdates() ;
	}else{
		auto currentTime = this->getCurrentTime() ;

		auto scheduledTime = settings::nextScheduledUpdateTime() ;

		qint64 interval = currentTime - scheduledTime ;

		if( interval < 0 ){

			interval = -interval ;

			/*
			 * the wait interval has not passed,wait for the remainder of the interval before
			 * checking for updates
			 */

			auto t = new QTimer() ;

			t->setSingleShot( true ) ;

			connect( t,SIGNAL( timeout() ),t,SLOT( deleteLater() ) ) ;
			connect( t,SIGNAL( timeout() ),this,SLOT( startTimer() ) ) ;

			auto s = static_cast< int >( interval ) ;

			t->start( s ) ;

			this->showToolTip( m_defaulticon,tr( "Status" ),s ) ;

			this->logActivity( this->logMsg() ) ;
		}else{
			/*
			 * the wait interval has passed.
			 */

			/*
			 * Calculate the time we are supposed to run next time.
			 */

			while( scheduledTime < currentTime ){

				scheduledTime += m_sleepDuration ;
			}

			/*
			 * Save the time we are supposed to run next.
			 */
			this->writeUpdateTimeToConfigFile( scheduledTime ) ;

			auto t = new QTimer() ;

			t->setSingleShot( true ) ;

			connect( t,SIGNAL( timeout() ),t,SLOT( deleteLater() ) ) ;
			connect( t,SIGNAL( timeout() ),this,SLOT( startTimer_1() ) ) ;

			/*
			 * Calculate time difference between now and next update check
			 */
			auto s = scheduledTime - currentTime ;

			/*
			 * Wait until the next time we run.
			 */
			t->start( static_cast< int >( s ) ) ;

			/*
			 * Check for updates.
			 */
			this->logActivity( tr( "Automatic check for updates initiated" ) ) ;

			this->checkForUpdates() ;
		}
	}
}

void qtUpdateNotifier::startTimer()
{
	m_timer.stop() ;
	m_timer.start( static_cast< int >( m_sleepDuration ) ) ;
	this->automaticCheckForUpdates() ;
}

void qtUpdateNotifier::startTimer_1()
{
	m_timer.stop() ;
	m_timer.start( static_cast< int >( m_sleepDuration ) ) ;
	this->automaticCheckForUpdates() ;
}

qint64 qtUpdateNotifier::getCurrentTime()
{
	return QDateTime::currentDateTime().toMSecsSinceEpoch() ;
}

QString qtUpdateNotifier::getCurrentTime_1()
{
	return QDateTime::currentDateTime().toString( Qt::TextDate ) ;
}

void qtUpdateNotifier::logActivity( const QString& msg )
{
	QString log = QString( "%1:   %2\n").arg( this->getCurrentTime_1(),msg ) ;
        utility::writeToFile( settings::activityLogFilePath(),log,false ) ;
	emit updateLogWindow() ;
}

void qtUpdateNotifier::logActivity_1( const QString& msg )
{
	QString line( "------------------------------------------------------" ) ;
	line += "--------------------------------------------------" ;

	auto t = this->getCurrentTime_1() ;
	auto log = QString( "%1\n%2:   %3\n%4\n" ).arg( line,t,msg,line )  ;

        utility::writeToFile( settings::activityLogFilePath(),log,false ) ;

	emit updateLogWindow() ;
}

void qtUpdateNotifier::setDebug( bool debug )
{
	m_debug = debug ;
}

qint64 qtUpdateNotifier::nextScheduledUpdateTime()
{
	if( m_nextScheduledUpdateTime == 0 ){

		m_nextScheduledUpdateTime = settings::nextScheduledUpdateTime() ;
	}

	return m_nextScheduledUpdateTime ;
}

void qtUpdateNotifier::writeUpdateTimeToConfigFile( qint64 time )
{
	m_nextScheduledUpdateTime = time ;
	settings::updateNextScheduledUpdateTime( time ) ;
}

void qtUpdateNotifier::manualCheckForUpdates()
{
	this->logActivity( tr( "Manual check for updates initiated" ) ) ;

	if( settings::firstTimeRun() ){

		settings::updateNextScheduledUpdateTime( this->getCurrentTime() + m_sleepDuration ) ;
	}

	this->checkForUpdates() ;
}

void qtUpdateNotifier::automaticCheckForUpdates()
{
	/*
	 * since we are doing an automatic update,m_nextScheduledUpdateTime will have the same value as this->getCurrentTime()
	 */

	this->writeUpdateTimeToConfigFile( m_nextScheduledUpdateTime + m_sleepDuration ) ;
	this->logActivity( tr( "Automatic check for updates initiated" ) ) ;
	this->checkForUpdates() ;
}

void qtUpdateNotifier::checkForUpdates()
{
	if( m_threadIsRunning ){

		this->logActivity( tr( "Warning:\tattempt to start update check while another one is still in progress" ) ) ;
	}else{
		QString icon( "qt-update-notifier-updating" ) ;

		this->showToolTip( icon,tr( "Status" ),tr( "Checking for updates" ) ) ;

		m_threadIsRunning = true ;

		auto r = utility::reportUpdates().await() ;

		m_threadIsRunning = false ;

		switch( r.repositoryState ){
		case result::repoState::updatesFound :

			this->saveAptGetLogOutPut( r.taskOutput ) ;
			icon = "qt-update-notifier-updates-are-available" ;
			m_statusicon.setStatus( statusicon::ItemStatus::NeedsAttention ) ;
			this->showToolTip( icon,tr( "There are updates in the repository" ),r.taskOutput ) ;
			this->autoDownloadPackages() ;

			break ;
		case result::repoState::inconsistentState :

			this->saveAptGetLogOutPut( r.taskOutput ) ;
			icon = "qt-update-notifier-important-info" ;
			m_statusicon.setStatus( statusicon::ItemStatus::Passive ) ;
			this->showToolTip( icon,tr( "Update check complete, repository appears to be in an inconsistent state" ) ) ;
			this->logActivity_1( r.taskOutput.at( 0 ) ) ;
			this->showIconOnImportantInfo() ;

			break ;
		case result::repoState::noUpdatesFound :

			m_statusicon.setStatus( statusicon::ItemStatus::Passive ) ;

			/*
			 * below function is called from checkForPackageUpdates() routine
			 * this->showToolTip( m_defaulticon,tr( "No updates found" ) ) ;
			 */

			this->checkForPackageUpdates() ;

			/*
			 * disabling this functionality since it doesnt appear to be in use
			 */
			//this->accessTwitter() ;

			break ;
		case result::repoState::noNetworkConnection :

			icon = m_defaulticon ;
			m_statusicon.setStatus( statusicon::ItemStatus::Passive ) ;
			this->showToolTip( icon,tr( "Check skipped, user is not connected to the internet" ) ) ;

			break ;
		case result::repoState::undefinedState :

			icon = m_defaulticon ;
			m_statusicon.setStatus( statusicon::ItemStatus::Passive ) ;
			this->showToolTip( icon,tr( "Update check complete, repository is in an unknown state" ) ) ;

			break ;
		default:
			/*
			 * currently,we dont get here,added for completeness' sake
			 */

			icon = m_defaulticon ;
			m_statusicon.setStatus( statusicon::ItemStatus::Passive ) ;
			this->showToolTip( icon,tr( "Update check complete, repository is in an unknown state" ) ) ;
			this->checkForPackageUpdates() ;
		}
	}
}

void qtUpdateNotifier::saveAptGetLogOutPut( const result::array_t& l )
{
	auto x = l.at( 1 ) ;

	if( x == tr( "No updates found" ) ){

		/*
		 * update log file only when there are new updates
		 */
	}else{
		QString line( "-------------------------------------------------------------------------------\n" ) ;

		auto msg = tr( "Log entry was created at: " ) ;
		auto header = line + msg + QDateTime::currentDateTime().toString( Qt::TextDate ) + "\n" + line ;

                utility::writeToFile( settings::aptGetLogFilePath(),header + x,true ) ;
	}
}

void qtUpdateNotifier::autoUpdatePackages()
{
	if( settings::autoUpdatePackages() ){

		QString icon( "qt-update-notifier-updating" ) ;

		this->showToolTip( icon,tr( "Status" ),tr( "Update in progress, do not power down computer" ) ) ;

		m_statusicon.setStatus( statusicon::ItemStatus::NeedsAttention ) ;

		this->logActivity( tr( "Automatic package update initiated" ) ) ;

		auto r = utility::autoUpdatePackages().await() ;

		if( r == 0 || r == 2 ){

			this->showToolTip( m_defaulticon,tr( "Automatic package update completed" ) ) ;
		}else{
			QString icon( "qt-update-notifier-important-info" ) ;
			this->showToolTip( icon,tr( "Automatic package update failed" ) ) ;
			this->showIconOnImportantInfo() ;
		}

		m_statusicon.setStatus( statusicon::ItemStatus::Passive ) ;
	}else{
		this->logActivity( this->logMsg() ) ;
	}
}

void qtUpdateNotifier::autoDownloadPackages()
{
	if( settings::autoDownloadPackages() ){

		QString icon( "qt-update-notifier-updating" ) ;

		this->showToolTip( icon,tr( "Status" ),tr( "Downloading packages" ) ) ;

		m_statusicon.setStatus( statusicon::ItemStatus::NeedsAttention ) ;

		this->logActivity( tr( "Packages downloading initiated" ) ) ;

		if( utility::autoDownloadPackages().await() ){

			this->showToolTip( icon,tr( "Downloading of packages completed" ) ) ;
			m_statusicon.setStatus( statusicon::ItemStatus::NeedsAttention ) ;
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
		auto r = utility::checkForPackageUpdates().await() ;

		if( r.isEmpty() ){

			this->showToolTip( m_defaulticon,tr( "No updates found" ) ) ;
		}else{
			QString	icon( "qt-update-notifier-important-info" ) ;
			this->showToolTip( icon,tr( "Outdated packages found" ) ) ;
			this->logActivity_1( r ) ;
			this->showIconOnImportantInfo() ;
			emit updateLogWindow() ;
		}
	}

	if( settings::checkNewerKernels() ){

		const auto& e = utility::checkKernelVersions().await() ;

		if( !e.isEmpty() ){

			this->logActivity_1( tr( "Kernel version \"%1\" is available" ).arg( e ) ) ;
		}
	}
}

void qtUpdateNotifier::objectGone( QObject * obj )
{
	qDebug() << "destroyed object" << obj->objectName() ;
}

void qtUpdateNotifier::showToolTip( const QString& x,const QString& y,const result::array_t& l )
{
	this->logActivity( y ) ;
	m_statusicon.setToolTip( x,tr( "Updates found" ),l.at( 0 ) ) ;
	this->changeIcon( x ) ;
}

void qtUpdateNotifier::showToolTip( const QString& x,const QString& y,const QString& z )
{
	m_statusicon.setToolTip( x,y,z ) ;
	this->changeIcon( x ) ;
}

void qtUpdateNotifier::showToolTip( const QString& x,const QString& y,int z )
{
	QString n = tr( "Next update check will be at %1" ).arg( this->nextUpdateTime( z ) ) ;
	m_statusicon.setToolTip( x,y,n ) ;
	this->changeIcon( x ) ;
}

void qtUpdateNotifier::showToolTip( const QString& x,const QString& y )
{
	auto n = tr( "Next update check will be at %1" ).arg( this->nextAutoUpdateTime() ) ;

	if( y == tr( "No updates found" ) ){

		this->logActivity( y ) ;

		this->logActivity( this->logMsg() ) ;

		m_statusicon.setToolTip( x,y,n ) ;
	}else{
		auto msg = QString( "<table><tr><td><b>%1</b><br></tr></td><tr><td>%2</tr></td></table>" ).arg( y,n ) ;

		this->logActivity( y ) ;

		this->logActivity( this->logMsg() ) ;

		m_statusicon.setToolTip( x,tr( "Status" ),msg ) ;
	}

	this->changeIcon( x ) ;
}

QString qtUpdateNotifier::nextUpdateTime( void )
{
	return this->nextUpdateTime( m_sleepDuration ) ;
}

QString qtUpdateNotifier::nextUpdateTime( qint64 interval )
{
	QDateTime d ;
	d.setMSecsSinceEpoch( QDateTime::currentMSecsSinceEpoch() + interval ) ;
	return d.toString( Qt::TextDate ) ;
}

QString qtUpdateNotifier::nextAutoUpdateTime()
{
	auto a = settings::nextScheduledUpdateTime() ;
	auto b = this->getCurrentTime() ;

	QDateTime d ;

	if( a >= b ){

		d.setMSecsSinceEpoch( a ) ;
	}else{
		while( a < b ){

			a += m_sleepDuration ;
		}

		d.setMSecsSinceEpoch( a ) ;
	}

	return d.toString( Qt::TextDate ) ;
}

QString qtUpdateNotifier::logMsg( qint64 interval )
{
	if( int64_t( interval ) > 0 ){

		auto n = this->nextUpdateTime( interval ) ;

		std::array< char,64 > num ;

		auto f = static_cast<double>( interval ) ;

		f = f / ( 1000 * 60 * 60 ) ;

		snprintf( num.data(),num.size(),"%.2f",f ) ;

		return tr( "Scheduled next check to be in %1 hours at %2" ).arg( num.data(),n ) ;
	}else{
		return tr( "Next update check will be at %1" ).arg( this->nextAutoUpdateTime() ) ;
	}
}

QString qtUpdateNotifier::logMsg( void )
{
	return this->logMsg( this->nextScheduledUpdateTime() - this->getCurrentTime() ) ;
}

void qtUpdateNotifier::scheduleUpdates( int interval )
{
	this->logActivity( this->logMsg( interval ) ) ;
	m_timer.stop() ;
	m_timer.start( interval ) ;
}

void qtUpdateNotifier::setUpdateInterval( int interval )
{
	this->logActivity( tr( "Rescheduling update check" ) ) ;

	m_sleepDuration = interval ;

	this->logActivity( this->logMsg( m_sleepDuration ) ) ;

	this->writeUpdateTimeToConfigFile( this->getCurrentTime() + m_sleepDuration ) ;

	auto d = static_cast<int>( m_sleepDuration ) ;

	m_timer.stop() ;
	m_timer.start( d ) ;

	auto x = m_defaulticon ;
	auto y = m_statusicon.toolTipTitle() ;

	this->showToolTip( x,y,d ) ;
}

void qtUpdateNotifier::closeApp()
{
	if( !m_threadIsRunning ){

		QCoreApplication::exit( 0 ) ;
	}
}

void qtUpdateNotifier::closeApp( int st )
{
	if( !m_threadIsRunning ){

		QCoreApplication::exit( st ) ;
	}
}

qtUpdateNotifier::~qtUpdateNotifier()
{
	this->logActivity( tr( "Qt-update-notifier quitting" ) ) ;
}
