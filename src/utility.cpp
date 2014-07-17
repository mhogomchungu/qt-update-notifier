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

#include "settings.h"
#include "utility.h"
#include "qt-update-synaptic-helper.h"

#include <QDir>
#include <QProcess>
#include <QObject>
#include <QDebug>
#include <QFile>
#include <QIODevice>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

class bufferManager{
public:
	explicit bufferManager( size_t size ) ;
	wchar_t * getBuffer( void ) ;
	~bufferManager() ;
private:
	wchar_t * m_buffer ;
};

bufferManager::bufferManager( size_t size )
{
	m_buffer = new wchar_t[ size ] ;
}

bufferManager::~bufferManager()
{
	delete[] m_buffer ;
}

wchar_t * bufferManager::getBuffer()
{
	return m_buffer ;
}

class fileManager{
public:
	explicit fileManager( const QString& ) ;
	fileManager( const QString&,bool ) ;
	int getFd( void ) ;
	bool fileIsOpened( void ) ;
	size_t fileSize( void ) ;
	~fileManager() ;
private:
	int m_fd ;
};

fileManager::fileManager( const QString& filepath,bool truncate )
{
	QByteArray f = filepath.toLatin1() ;
	if( truncate ){
		m_fd = open( f.constData(),O_CREAT|O_TRUNC|O_WRONLY,S_IRUSR|S_IWUSR ) ;
	}else{
		m_fd = open( f.constData(),O_CREAT|O_APPEND|O_WRONLY,S_IRUSR|S_IWUSR ) ;
	}
}

fileManager::fileManager( const QString& filepath )
{
	QByteArray f = filepath.toLatin1() ;
	m_fd = open( f.constData(),O_RDONLY ) ;
}

fileManager::~fileManager()
{
	if( m_fd != -1 ){
		close( m_fd ) ;
	}
}

int fileManager::getFd()
{
	return m_fd ;
}

bool fileManager::fileIsOpened()
{
	return m_fd != -1 ;
}

size_t fileManager::fileSize()
{
	struct stat st ;
	fstat( m_fd,&st ) ;
	return st.st_size ;
}

void writeToFile_1( const QString& filepath,const QString& content,bool truncate )
{
	fileManager f( filepath,truncate ) ;
	if( f.fileIsOpened() ){
		int fd = f.getFd() ;
		bufferManager buffer( content.size() ) ;
		wchar_t * x = buffer.getBuffer() ;
		if( x ){
			size_t y = content.toWCharArray( x ) ;
			write( fd,x,y * sizeof( wchar_t ) ) ;
		}
	}
}

namespace utility
{

void writeToFile( const QString& filepath,const QString& content,bool truncate )
{
	if( filepath == settings::activityLogFilePath() ){
		if( settings::prefixLogEntries() ){
			/*
			 * add new entry at the front of the log
			 */
			QString data = content + utility::readFromFile( filepath ) ;
			writeToFile_1( filepath,data,true )  ;
		}else{
			/*
			 * add new entries at the back of the log
			 */
			writeToFile_1( filepath,content,truncate ) ;
		}
	}else{
		/*
		 * add new entries at the back of the log
		 */
		writeToFile_1( filepath,content,truncate ) ;
	}
}

QString readFromFile( const QString& filepath )
{
	fileManager f( filepath ) ;
	if( f.fileIsOpened() ){
		size_t n = f.fileSize() ;
		int fd = f.getFd() ;
		bufferManager buffer( n ) ;
		wchar_t * x = buffer.getBuffer() ;
		if( x ){
			size_t z = read( fd,x,n ) ;
			return QString::fromWCharArray( x,z / sizeof( wchar_t ) ) ;
		}else{
			return QObject::tr( "Log is empty" ) ;
		}
	}else{
		return QObject::tr( "Log is empty" ) ;
	}
}

static result _processUpdates( QByteArray& output1,QByteArray& output2 )
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

	QString q = QObject::tr( "<table><tr><td>%1 to be upgraded</td></tr><tr><td>%2 to be replaced</td></tr><tr><td>%3 to be installed</td></tr></table>" ) ;
	QString updates = q.arg( x ).arg( y ).arg( z ) ;

	result r ;
	r.taskStatus = 0 ;
	r.repositoryState = result::updatesFound ;
	r.taskOutput.append( updates ) ;
	r.taskOutput.append( output2 ) ;
	return r ;
}

result _reportUpdates()
{
	auto _not_online = [&](){
		QProcess exe ;
		exe.start( settings::networkConnectivityChecker() ) ;
		if( exe.waitForFinished() ){
			return exe.exitCode() != 0 ;
		}else{
			return true ;
		}
	} ;

	if( _not_online() ){
		result r ;
		r.taskStatus = 1 ;
		r.repositoryState = result::noNetworkConnection ;
		r.taskOutput.append( "xyz" ) ;
		r.taskOutput.append( QObject::tr( "Check skipped, user is not connected to the internet" ) ) ;
		return r ;
	}

	QString language = settings::prefferedLanguage() ;
	QString configPath = settings::configPath() ;

	QString aptUpdate = QString( "apt-get -s -o Debug::NoLocking=true -o dir::state=%1/apt update" ).arg( configPath ) ;
	QString aptUpgrade = QString( "apt-get -s -o Debug::NoLocking=true -o dir::state=%1/apt dist-upgrade" ).arg( configPath ) ;

	QDir dir ;

	dir.mkdir( configPath + QString( "/apt" ) ) ;
	dir.mkdir( configPath + QString( "/apt/lists" ) ) ;
	dir.mkdir( configPath + QString( "/apt/lists/partial" ) ) ;

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

	exe.start( aptUpdate ) ;
	exe.waitForFinished( -1 ) ;

	int st = exe.exitCode() ;
	exe.close() ;

	QByteArray bogusData = "xyz" ;

	QString inconsistentState = QObject::tr( "\
Recommending trying again later as the Repository appear to be in an inconsistent state.\n\
If the problem persists, run Synaptic and see if it is still possible to update.\n\
If the problem persists and Synaptic is unable to solve it, then open a support post in the forum and ask for assistance." ) ;

	if( st == 0 ){

		exe.start( aptUpgrade ) ;
		exe.waitForFinished( -1 ) ;
		QByteArray output = exe.readAllStandardOutput() ;
		exe.close() ;

		if( output.isEmpty() ){
			list.append( bogusData ) ;
			QString s = QObject::tr( "Warning: apt-get update finished with errors" ) ;
			list.append( s ) ;
			result r ;
			r.taskStatus = 1 ;
			r.repositoryState = result::undefinedState ;
			r.taskOutput = list ;
			return r ;
		}else{
			if( output.contains( error1 ) || output.contains( error2 ) || output.contains( error3 ) ){
				list.append( inconsistentState ) ;
				if( language == "english_US" ){
					list.append( output ) ;
				}else{
					QByteArray output1 ;
					QProcess e ;
					e.start( aptUpgrade ) ;
					e.waitForFinished( -1 ) ;
					output1 = e.readAllStandardOutput() ;
					e.close() ;
					list.append( output1 ) ;
				}
				result r ;
				r.taskStatus = 1 ;
				r.repositoryState = result::inconsistentState ;
				r.taskOutput = list ;
				return r ;
			}else if( output.contains( success ) ){
				if( language == "english_US" ){
					return _processUpdates( output,output ) ;
				}else{
					QByteArray output1 ;
					QProcess e ;
					e.start( aptUpgrade ) ;
					e.waitForFinished( -1 ) ;
					output1 = e.readAllStandardOutput() ;
					e.close() ;
					return _processUpdates( output,output1 ) ;
				}
			}else{
				list.append( bogusData ) ;
				QString s = QObject::tr( "No updates found" ) ;
				list.append( s ) ;
				result r ;
				r.taskStatus = 0 ;
				r.repositoryState = result::noUpdatesFound ;
				r.taskOutput = list ;
				return r ;
			}
		}
	}else{
		list.append( bogusData ) ;
		QString s = QObject::tr( "Warning: apt-get update finished with errors" ) ;
		list.append( s ) ;
		result r ;
		r.taskStatus = 1 ;
		r.repositoryState = result::undefinedState ;
		r.taskOutput = list ;
		return r ;
	}
}

Task::future< result >& reportUpdates()
{
	return Task::run< result >( [](){
		return _reportUpdates() ;
	} ) ;
}

static result _task( const char * arg )
{
	QProcess exe ;

	exe.start( QString( "%1 %2" ).arg( QT_UPDATE_NOTIFIER_HELPER_PATH,arg ) ) ;
	exe.waitForFinished( -1 ) ;

	result r ;
	r.taskStatus = exe.exitCode() ;
	return r ;
}

Task::future< bool >& startSynaptic()
{
	return Task::run< bool >( [](){
		return _task( "--start-synaptic" ).taskStatus != 0 ;
	} ) ;
}

Task::future< bool >& autoRefreshStartSYnaptic()
{
	return Task::run< bool >( [](){
		return _task( "---start-synaptic --update-at-startup" ).taskStatus != 0 ;
	} ) ;
}

Task::future< bool >& autoDownloadPackages()
{
	return Task::run< bool >( [](){
		return _task( "--download-packages" ).passed() ;
	} ) ;
}

Task::future< result >& autoUpdatePackages()
{
	return Task::run< result >( [](){
		return _task( "--auto-update" ) ;
	} ) ;
}

QString checkKernelVersion()
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
		int patch ;

		if( ver.size() >= 3 ){
			patch = ver.at( 2 ).toInt() ;
		}else{
			patch = 0 ;
		}

		/*
		 * start warning if a user uses a kernel less than 3.12.16
		 */
		int base_kernel_major_version = 3 ;
		int base_kernel_minor_version = 12 ;
		int base_kernel_patch_version = 16 ;
		bool update = false ;

		if( major < base_kernel_major_version ){
			update = true ;
		}else if( minor < base_kernel_minor_version && major <= base_kernel_major_version ){
			update = true ;
		}else if( patch < base_kernel_patch_version && major <= base_kernel_major_version && minor <= base_kernel_minor_version ){
			update = true ;
		}

		if( update ){
			return QObject::tr( "Recommending updating the kernel from version %1 to a more recent version." ).arg( version ) ;
		}else{
			return QString() ;
		}
	}else{
		return QString() ;
	}
}

bool updateAvailable( const QString& e,QString * newVersion,QString * installedVersion )
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

			QString m_iv = l.at( 0 ).split( " " ).last() ;
			QString m_nv = l.at( 1 ).split( " " ).last() ;
			*newVersion       = m_nv ;
			*installedVersion = m_iv ;

			if( m_iv == "0" ){
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

QString checkLibreOfficeVersion()
{
	QString iv ;
	QString nv ;

	if( utility::updateAvailable( "lomanager --vinfo",&nv,&iv ) ){
		return QObject::tr( "Updating Libreoffice from version \"%1\" to available version \"%2\" is recommended." ).arg( iv ).arg( nv ) ;
	}else{
		return QString() ;
	}
}

QString checkVirtualBoxVersion()
{
	QString iv ;
	QString nv ;

	if( utility::updateAvailable( "getvirtualbox --vinfo",&nv,&iv ) ){
		return QObject::tr( "Updating VirtualBox from version \"%1\" to available version \"%2\" is recommended." ).arg( iv ).arg( nv ) ;
	}else{
		return QString() ;
	}
}

QString checkCallibeVersion()
{
	QString iv ;
	QString nv ;

	if( utility::updateAvailable( "calibre-manager --vinfo",&nv,&iv ) ){
		return QObject::tr( "Updating Calibre from version \"%1\" to available version \"%2\" is recommended." ).arg( iv ).arg( nv ) ;
	}else{
		return QString() ;
	}
}

Task::future< result >& checkForPackageUpdates()
{
	return Task::run< result >( [](){
		result r ;

		r.taskOutput.append( checkKernelVersion() ) ;
		r.taskOutput.append( checkLibreOfficeVersion() ) ;
		r.taskOutput.append( checkVirtualBoxVersion() ) ;
		r.taskOutput.append( checkCallibeVersion() ) ;

		return r ;
	} ) ;
}

}
