#include "task.h"

#include "qt-update-synaptic-helper.h"
#include <QThreadPool>
#include <QProcess>

#include <QString>
#include <QStringList>
#include <QDir>
#include <QDebug>
#include <QFile>
#include <QProcessEnvironment>

Task::Task()
{
}

Task::~Task()
{
}

void Task::start( Task::action action )
{
	m_action = action ;
	QThreadPool::globalInstance()->start( this ) ;
}

void Task::run()
{
	switch( m_action ){
	case Task::autoRefreshStartSYnaptic :
	case Task::startSynaptic :
	case Task::updateSystem :
	case Task::downloadPackages :
		return this->startSynapticTask() ;
	case Task::checkUpDates :
		return this->checkUpdatesTask() ;
	case Task::checkOutDatedPackages :
		return this->checkOutDatedPackagesTask() ;
	}
}

void Task::setLocalLanguage( const QString& language )
{
	m_language = language ;
}

void Task::setConfigPath( const QString& path )
{
	m_configPath = path ;
}

void Task::startSynapticTask()
{
	QProcess exe ;

	switch( m_action ){
	case Task::startSynaptic :
		exe.start( QString( "%1 --start-synaptic" ).arg( QT_UPDATE_NOTIFIER_HELPER_PATH ) ) ;
		break ;
	case Task::autoRefreshStartSYnaptic :
		exe.start( QString( "%1 --start-synaptic --update-at-startup" ).arg( QString( QT_UPDATE_NOTIFIER_HELPER_PATH ) ) ) ;
		break ;
	case Task::downloadPackages :
		exe.start( QString( "%1 --download-packages" ).arg( QString( QT_UPDATE_NOTIFIER_HELPER_PATH ) ) ) ;
		break ;
	case Task::updateSystem :
		exe.start( QString( "%1 --auto-update" ).arg( QString( QT_UPDATE_NOTIFIER_HELPER_PATH ) ) ) ;
		break ;
	case Task::checkUpDates :
	case Task::checkOutDatedPackages :
		break ;
	}

	exe.waitForFinished( -1 ) ;

	emit taskFinished( exe.exitCode() ) ;
}

void Task::checkOutDatedPackagesTask()
{
	this->checkKernelVersion() ;
	this->checkLibreOfficeVersion() ;
	this->checkVirtualBoxVersion() ;
	this->checkCallibeVersion() ;

	emit taskFinished( m_package ) ;
}

void Task::checkUpdatesTask()
{
	if( this->online() ){
		this->reportUpdates() ;
	}else{
		QStringList l ;
		l.append( "xyz" ) ;
		l.append( tr( "Check skipped, user is not connected to the internet" ) ) ;
		emit taskFinished( int( Task::noNetworkConnection ),l ) ;
	}
}

void Task::checkKernelVersion()
{
	QProcess exe ;
	exe.start( QString( "uname -r" ) ) ;
	exe.waitForFinished( -1 ) ;
	QString version = exe.readAll() ;
	exe.close() ;

	int index = version.indexOf( QString( "-" ) ) ;
	if( index != -1 ){

		version.truncate( index ) ;

		QStringList ver = version.split( "." ) ;

		int major = ver.at( 0 ).toInt() ;
		int minor = ver.at( 1 ).toInt() ;
		int patch = 0 ;

		if( ver.size() >= 3 ){
			patch = ver.at( 2 ).toInt() ;
		}else{
			;
		}

		/*
		 * start warning if a user uses a kernel less than 3.2.18
		 */
		int base_kernel_major_version = 3 ;
		int base_kernel_minor_version = 2 ;
		int base_kernel_patch_version = 18;
		bool update = false ;

		if( major < base_kernel_major_version ){
			update = true ;
		}else if( minor < base_kernel_minor_version && major <= base_kernel_major_version ){
			update = true ;
		}else if( patch < base_kernel_patch_version && major <= base_kernel_major_version && minor <= base_kernel_minor_version ){
			update = true ;
		}

		if( update ){
			m_package.append( tr( "Recommending updating the kernel from version %1 to a more recent version." ).arg( version ) ) ;
		}else{
			m_package.append( QString( "" ) ) ;
		}
	}
}

void Task::checkLibreOfficeVersion()
{
	if( this->updateAvailable( QString( "lomanager --vinfo" ) ) ){
		QString r = tr( "Updating Libreoffice from version \"%1\" to available version \"%2\" is recommended." ).arg( m_iv ).arg( m_nv ) ;
		m_package.append( r ) ;
	}else{
		m_package.append( QString( "" ) ) ;
	}
}

void Task::checkVirtualBoxVersion()
{
	if( this->updateAvailable( QString( "getvirtualbox --vinfo" ) ) ){
		QString r = tr( "Updating VirtualBox from version \"%1\" to available version \"%2\" is recommended." ).arg( m_iv ).arg( m_nv ) ;
		m_package.append( r ) ;
	}else{
		m_package.append( QString( "" ) ) ;
	}
}

void Task::checkCallibeVersion()
{
	if( this->updateAvailable( QString( "calibre-manager --vinfo" ) ) ){
		QString r = tr( "Updating Calibre from version \"%1\" to available version \"%2\" is recommended." ).arg( m_iv ).arg( m_nv ) ;
		m_package.append( r ) ;
	}else{
		m_package.append( QString( "" ) ) ;
	}
}

bool Task::updateAvailable( const QString& e )
{
	QProcess exe ;
	exe.start( e ) ;
	exe.waitForFinished( -1 ) ;

	QString r = exe.readAll() ;

	if( r.isEmpty() ){
		return false ;
	}else{
		QStringList l = r.split( "\n" ) ;
		if( l.size() > 1 ){
			m_iv = l.at( 0 ).split( " " ).last() ;
			m_nv = l.at( 1 ).split( " " ).last() ;

			if( m_iv == QString( "0" ) ){
				/*
				* program not installed
				*/
				return false ;
			}else{
				QStringList nv = m_nv.split( "." ) ;
				QStringList iv = m_iv.split( "." ) ;

				int installed_major_version_number = iv.at( 0 ).toInt() ;
				int installed_minor_version_number ;
				int installed_patch_version_number ;

				if( iv.size() >= 2 ){
					installed_minor_version_number = iv.at( 1 ).toInt() ;
				}else{
					installed_minor_version_number = 0 ;
				}

				if( iv.size() >= 3 ){
					installed_patch_version_number = iv.at( 2 ).toInt() ;
				}else{
					installed_patch_version_number = 0 ;
				}

				int new_major_version_number = nv.at( 0 ).toInt() ;
				int new_minor_version_number ;
				int new_patch_version_number  ;

				if( nv.size() >= 2 ){
					new_minor_version_number = nv.at( 1 ).toInt() ;
				}else{
					new_minor_version_number = 0 ;
				}

				if( nv.size() >= 3 ){
					new_patch_version_number = nv.at( 2 ).toInt() ;
				}else{
					new_patch_version_number = 0 ;
				}

				bool update = false ;

				if( installed_major_version_number < new_major_version_number ){
					/*
					 * installed major version number is less than new major version number
					 */
					update = true ;
				}else if( installed_minor_version_number < new_minor_version_number &&
					  installed_major_version_number <= new_major_version_number ){
					/*
					 * installed minor version number is less than new minor version number
					 */
					update = true ;
				}else if( installed_patch_version_number < new_patch_version_number &&
					  installed_major_version_number <= new_major_version_number &&
					  installed_minor_version_number <= new_minor_version_number ){
					/*
					 * installed path version number is less than new path version number
					 */
					update = true ;
				}

				return update ;
			}
		}else{
			return false ;
		}
	}
}

bool Task::online()
{
	QProcess exe ;
	exe.start( QString( "ping -c 1 8.8.8.8" ) ) ;
	exe.waitForFinished() ;
	int st = exe.exitCode() ;
	exe.close() ;
	return st == 0 ;
}

void Task::processUpdates( QByteArray& output1,QByteArray& output2 )
{
	QStringList l = QString( output1 ).split( "\n" ) ;

	int index = l.indexOf( QString( "The following packages will be upgraded" ) ) ;

	const char * threeSpaceCharacters = "   " ;

	int upgrade = 0 ;
	if( index != -1 ){
		while( true ){
			index++ ;
			if( l.at( index ).startsWith( threeSpaceCharacters ) ){
				upgrade++ ;
			}else{
				break ;
			}
		}
	}

	index = l.indexOf( QString( "The following packages will be REPLACED:" ) ) ;

	int replace = 0 ;
	if( index != -1 ){
		while( true ){
			index++ ;
			if( l.at( index ).startsWith( threeSpaceCharacters ) ){
				replace++ ;
			}else{
				break ;
			}
		}
	}

	index = l.indexOf( QString( "The following NEW packages will be installed:" ) ) ;

	int New = 0 ;
	if( index != -1 ){
		while( true ){
			index++ ;
			if( l.at( index ).startsWith( threeSpaceCharacters ) ){
				New++ ;
			}else{
				break ;
			}
		}
	}

	QString x = QString::number( upgrade ) ;
	QString y = QString::number( replace ) ;
	QString z = QString::number( New ) ;

	QString q = tr( "<table><tr><td>%1 to be upgraded</td></tr><tr><td>%2 to be replaced</td></tr><tr><td>%3 to be installed</td></tr></table>" ) ;
	QString updates = q.arg( x ).arg( y ).arg( z ) ;

	QStringList n ;
	n.append( updates ) ;
	n.append( output2 ) ;
	emit taskFinished( int( Task::updatesFound ),n ) ;
}

void Task::reportUpdates()
{
	m_aptUpdate = QString( "apt-get -s -o Debug::NoLocking=true -o dir::state=%1/apt update" ).arg( m_configPath ) ;
	m_aptUpgrade = QString( "apt-get -s -o Debug::NoLocking=true -o dir::state=%1/apt dist-upgrade" ).arg( m_configPath ) ;

	QDir dir ;
	dir.mkdir( m_configPath + QString( "/apt" ) ) ;
	dir.mkdir( m_configPath + QString( "/apt/lists" ) ) ;
	dir.mkdir( m_configPath + QString( "/apt/lists/partial" ) ) ;

	const char * error1 = "The following packages have unmet dependencies" ;
	const char * error2 = "E: Error, pkgProblemResolver::Resolve generated breaks, this may be caused by held packages." ;
	const char * error3 = "The following packages have been kept back" ;

	const char * success = "\nThe following packages will be" ;

	QStringList list ;
	QProcess exe ;

	QProcessEnvironment env ;
	env.insert( QString( "LANG" ),QString( "en_US.UTF-8" ) ) ;
	env.insert( QString( "LANGUAGE" ),QString( "en_US.UTF-8:en_US:en" ) ) ;

	exe.setProcessEnvironment( env ) ;

	exe.start( m_aptUpdate ) ;
	exe.waitForFinished( -1 ) ;

	int st = exe.exitCode() ;
	exe.close() ;

	QByteArray bogusData = "xyz" ;

	if( st == 0 ){

		exe.start( m_aptUpgrade ) ;
		exe.waitForFinished( -1 ) ;
		QByteArray output = exe.readAllStandardOutput() ;
		exe.close() ;

		if( output.isEmpty() ){
			list.append( bogusData ) ;
			QString s = tr( "Warning: apt-get update finished with errors" ) ;
			list.append( s ) ;
			emit taskFinished( int( Task::undefinedState ),list ) ;
		}else{
			if( output.contains( error1 ) || output.contains( error2 ) || output.contains( error3 ) ){
				list.append( bogusData ) ;
				if( m_language == QString( "english_US" ) ){
					list.append( output ) ;
				}else{
					QByteArray output1 ;
					QProcess e ;
					e.start( m_aptUpgrade ) ;
					e.waitForFinished( -1 ) ;
					output1 = e.readAllStandardOutput() ;
					e.close() ;
					list.append( output1 ) ;
				}
				emit taskFinished( int( Task::inconsistentState ),list ) ;
			}else if( output.contains( success ) ){
				if( m_language == QString( "english_US" ) ){
					this->processUpdates( output,output ) ;
				}else{
					QByteArray output1 ;
					QProcess e ;
					e.start( m_aptUpgrade ) ;
					e.waitForFinished( -1 ) ;
					output1 = e.readAllStandardOutput() ;
					e.close() ;
					this->processUpdates( output,output1 ) ;
				}
			}else{
				list.append( bogusData ) ;
				QString s = tr( "No updates found" ) ;
				list.append( s ) ;
				emit taskFinished( int( Task::noUpdatesFound ),list ) ;
			}
		}
	}else{
		list.append( bogusData ) ;
		QString s = tr( "Warning: apt-get update finished with errors" ) ;
		list.append( s ) ;
		emit taskFinished( int( Task::undefinedState ),list ) ;
	}
}
