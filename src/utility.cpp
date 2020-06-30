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

#include "settings.h"
#include "utility.h"
#include "qt-update-synaptic-helper.h"

#include <QDir>
#include <QProcess>
#include <QObject>
#include <QDebug>
#include <QFile>
#include <QIODevice>
#include <QVector>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

#include <iostream>

struct Result
{
	result m_result ;
	int upgrade ;
	int replace ;
	int New ;
};

static int _openFile( const QString& filePath,bool truncate )
{
	if( truncate ){

		return open( filePath.toLatin1().constData(),O_CREAT|O_TRUNC|O_WRONLY,S_IRUSR|S_IWUSR ) ;
	}else{
		return open( filePath.toLatin1().constData(),O_CREAT|O_APPEND|O_WRONLY,S_IRUSR|S_IWUSR ) ;
	}
}

static int _openFile( const QString& filePath )
{
	return open( filePath.toLatin1().constData(),O_RDONLY ) ;
}

static bool _writeToFile( const QString& filepath,const QString& content,bool truncate )
{
	int fd = _openFile( filepath,truncate ) ;

	if( fd != -1 ){

		QVector< wchar_t > buffer( content.size() ) ;
		auto x = buffer.data() ;

		auto y = content.toWCharArray( x ) ;
		auto r = write( fd,x,static_cast< size_t >( y ) * sizeof( wchar_t ) ) ;

		fchmod( fd,0600 ) ;

		close( fd ) ;

		return r > 0 ;
	}else{
		return false ;
	}
}

namespace utility
{

void waitForTwoSeconds()
{
	::sleep( 2 ) ;
}

bool writeToFile( const QString& filepath,const QString& content,bool truncate )
{
        if( filepath == settings::activityLogFilePath() ){

                if( settings::prefixLogEntries() ){
                        /*
                         * add new entry at the front of the log
                         */
                        auto data = content + utility::readFromFile( filepath ) ;
                        return _writeToFile( filepath,data,true )  ;
                }else{
                        /*
                         * add new entries at the back of the log
                         */
                        return _writeToFile( filepath,content,truncate ) ;
                }
        }else{
                /*
                 * add new entries at the back of the log
                 */
                return _writeToFile( filepath,content,truncate ) ;
        }
}

QString readFromFile( const QString& filepath )
{
        int fd = _openFile( filepath ) ;

        if( fd != -1 ){

                const int e = sizeof( wchar_t ) ;
                struct stat st ;

                fstat( fd,&st ) ;

		QVector< wchar_t > buffer( static_cast< int >( st.st_size ) ) ;
                auto x = buffer.data() ;

		auto z = read( fd,x,static_cast< size_t >( st.st_size ) ) ;

		fchmod( fd,0600 ) ;

                close( fd ) ;

		return QString::fromWCharArray( x,static_cast< int >( z / e ) ) ;
        }else{
                return QObject::tr( "Log is empty" ) ;
        }
}

static Result _processUpdates( QByteArray& output1,const QByteArray& output2 )
{
	auto l = QString( output1 ).split( "\n" ) ;

	int index = l.indexOf( "The following packages will be upgraded" ) ;

	const auto threeSpaceCharacters = "   " ;

	int upgrade = 0 ;

	auto ignorePackages = settings::ignorePackageList() ;

	auto _ignorePackage = [ & ]( const QString& e ){

		for( const auto& it : ignorePackages ){

			if( !it.isEmpty() && e.contains( it ) ){

				return true ;
			}
		}

		return false ;
	} ;

	if( index != -1 ){

		while( true ){

			index++ ;

			if( l.at( index ).startsWith( threeSpaceCharacters ) ){

				if( !_ignorePackage( l.at( index ) ) ){

					upgrade++ ;
				}
			}else{
				break ;
			}
		}
	}

	index = l.indexOf( "The following packages will be REPLACED:" ) ;

	int replace = 0 ;

	if( index != -1 ){

		while( true ){

			index++ ;

			if( l.at( index ).startsWith( threeSpaceCharacters ) ){

				if( !_ignorePackage( l.at( index ) ) ){

					replace++ ;
				}
			}else{
				break ;
			}
		}
	}

	index = l.indexOf( "The following NEW packages will be installed:" ) ;

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

	auto x = QString::number( upgrade ) ;
	auto y = QString::number( replace ) ;
	auto z = QString::number( New ) ;

	auto q = QObject::tr( "<table><tr><td>%1 to be upgraded</td></tr><tr><td><br>%2 to be replaced</td></tr><tr><td><br>%3 to be installed</td></tr></table>" ) ;
	auto updates = q.arg( x,y,z ) ;

	return Result{ { 0,result::repoState::updatesFound,{ updates,output2 } },upgrade,replace,New } ;
}

struct cmd_args_mutable{
	QString cmd ;
	QStringList args ;
} ;

struct cmd_args{

	cmd_args( const cmd_args_mutable& s,const QProcessEnvironment& e ) :
		cmd( s.cmd ),args( s.args ),env( e )
	{
	}
	cmd_args( const QString& c,
		  const QStringList& a = QStringList(),
		  const QProcessEnvironment& e = QProcessEnvironment() ) :
		cmd( c ),args( a ),env( e )
	{
	}
	const QString& cmd ;
	const QStringList& args ;
	const QProcessEnvironment& env ;
} ;

static bool _debug = true ;

static Task::future< Task::process::result >& _run_cmd( const cmd_args& cmd_args )
{
	if( _debug ){

		std::cout << "\"" << cmd_args.cmd.toStdString() << "\"" ;

		for( const auto& it : cmd_args.args ){

			std::cout << " \"" + it.toStdString() << "\"" ;
		}

		std::cout << std::endl ;
	}

	return Task::process::run( cmd_args.cmd,cmd_args.args,-1,"",cmd_args.env ) ;
}

static cmd_args_mutable _setup_apt( const QString& cmd,
				    const QString& configPath,
				    const QString& opt )
{
	QStringList args = { "-s","-o","Debug::NoLocking=true","-o" } ;

	args.append( "dir::state=" + configPath + "/apt" ) ;

	args.append( opt ) ;

	return { cmd,args } ;
}

static QByteArray _upgrade_0( const QString& configPath,bool setEnglishLanguage )
{
	auto w = _setup_apt( "apt-get",configPath,"dist-upgrade" ) ;
	
	QProcessEnvironment env ;

	if( setEnglishLanguage ){

		env.insert( "LANG","en_US.UTF-8" ) ;
		env.insert( "LANGUAGE","en_US.UTF-8:en_US:en" ) ;

		return _run_cmd( { w,env } ).get().std_out() ;
	}else{
		return _run_cmd( { w,env } ).get().std_out() ;
	}
}

static QByteArray _upgrade( const QString& configPath )
{
	return _upgrade_0( configPath,true ) ;
}

static QByteArray _upgrade_1( const QString& configPath )
{
	return _upgrade_0( configPath,false ) ;
}

static bool _update( const QString& configPath )
{
	QProcessEnvironment env ;

	env.insert( "LANG","en_US.UTF-8" ) ;
	env.insert( "LANGUAGE","en_US.UTF-8:en_US:en" ) ;

	auto w = _setup_apt( "apt-get",configPath,"update" ) ;

	return _run_cmd( { w,env } ).get().success() ;
}

static result _reportUpdates()
{
	auto _not_online = [](){

		auto args = settings::networkConnectivityChecker().split( ' ',Qt::SkipEmptyParts ) ;

		auto exe = args.takeAt( 0 ) ;

		return !_run_cmd( { exe,args } ).get().success() ;
	}() ;

	if( _not_online ){

		return result{ 1,result::repoState::noNetworkConnection,{ "",QObject::tr( "Check skipped, user is not connected to the internet" ) } } ;
	}

	auto language   = settings::prefferedLanguage() ;
	auto configPath = settings::configPath() ;

	QDir dir ;

	dir.mkdir( configPath + "/apt"  ) ;
	dir.mkdir( configPath + "/apt/lists" ) ;
	dir.mkdir( configPath + "/apt/lists/partial" ) ;

	const auto error1 = "The following packages have unmet dependencies" ;
	const auto error2 = "E: Error, pkgProblemResolver::Resolve generated breaks, this may be caused by held packages." ;
	const auto error3 = "The following packages have been kept back" ;

	const auto success = "\nThe following packages will be" ;

	auto inconsistentState = QObject::tr( "\
Recommending trying again later as the Repository appear to be in an inconsistent state.\n\
If the problem persists, run Synaptic and see if it is still possible to update.\n\
If the problem persists and Synaptic is unable to solve it, then open a support post in the forum and ask for assistance." ) ;

	if( _update( configPath ) ){

		auto output = _upgrade( configPath ) ;

		if( output.isEmpty() ){

			return result{ 1,result::repoState::undefinedState,{ "",QObject::tr( "Warning: apt-get update finished with errors" ) } } ;
		}else{
			if( output.contains( error1 ) || output.contains( error2 ) || output.contains( error3 ) ){

				if( language == "english_US" ){

					return result{ 1,result::repoState::inconsistentState,{ inconsistentState,output } } ;
				}else{
					return result{ 1,result::repoState::inconsistentState,{ inconsistentState,_upgrade_1( configPath ) } } ;
				}

			}else if( output.contains( success ) ){

				auto r = [ & ](){

					if( language == "english_US" ){

						return _processUpdates( output,output ) ;
					}else{
						return _processUpdates( output,_upgrade_1( configPath ) ) ;
					}
				}() ;

				if( r.New > 0 || r.replace > 0 || r.upgrade > 0 ){

					return r.m_result ;
				}else{
					return result{ 0,result::repoState::noUpdatesFound,{ "",QObject::tr( "No updates found" ) } } ;
				}
			}else{
				return result{ 0,result::repoState::noUpdatesFound,{ "",QObject::tr( "No updates found" ) } } ;
			}
		}
	}else{
		return result{ 1,result::repoState::undefinedState,{ "",QObject::tr( "Warning: apt-get update finished with errors" ) } } ;
	}
}

Task::future< result >& reportUpdates()
{
	return Task::run( [](){ return _reportUpdates() ; } ) ;
}

static int _task( const QStringList& e )
{
	return _run_cmd( { QT_UPDATE_NOTIFIER_HELPER_PATH,e } ).get().exit_code() ;
}

Task::future< bool >& startSynaptic()
{
	return Task::run( [](){

		auto run = []()->QStringList{

			if( settings::autoRefreshSynaptic() ){

				return { "--start-synaptic","--update-at-startup" } ;
			}else{
				return { "--start-synaptic" } ;
			}
		}() ;

		return _task( run ) != 0 ;
	} ) ;
}

Task::future< bool >& autoDownloadPackages()
{
	return Task::run( [](){ return _task( { "--download-packages" } ) == 0 ; } ) ;
}

Task::future< int >& autoUpdatePackages()
{
	return Task::run( [](){ return _task( { "--auto-update" } ) ; } ) ;
}

static bool _check_version( const QString& e,const QString& f )
{
	auto nv = e.split( "." ) ;
	auto iv = f.split( "." ) ;

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

	if( installed_major_version_number < new_major_version_number ){

		/*
		 * installed major version number is less than new major version number
		 */

		return true ;

	}else if( installed_minor_version_number < new_minor_version_number &&
		  installed_major_version_number <= new_major_version_number ){

		/*
		 * installed minor version number is less than new minor version number
		 */

		return true ;

	}else if( installed_patch_version_number < new_patch_version_number &&
		  installed_major_version_number <= new_major_version_number &&
		  installed_minor_version_number <= new_minor_version_number ){

		/*
		 * installed path version number is less than new path version number
		 */

		return true ;
	}else{
		return false ;
	}
}

static QString _checkKernelVersion()
{
	QString version = _run_cmd( { "uname",{ "-r" } } ).get().std_out() ;

	int index = version.indexOf( "-" ) ;

	if( index != -1 ){

		version.truncate( index ) ;

		auto ver = version.split( "." ) ;

		if( ver.size() < 2 ){

			return QString() ;
		}

		int major = ver.at( 0 ).toInt() ;
		int minor = ver.at( 1 ).toInt() ;
		int patch ;

		if( ver.size() > 2 ){

			patch = ver.at( 2 ).toInt() ;
		}else{
			patch = 0 ;
		}

		auto _update = [ & ](){

			/*
			 * start warning if a user uses a kernel less than 4.4.79
			 */

			int base_kernel_major_version = 4 ;
			int base_kernel_minor_version = 19 ;
			int base_kernel_patch_version = 127 ;

			if( major < base_kernel_major_version ){

				return true ;

			}else if( minor < base_kernel_minor_version &&
				  major <= base_kernel_major_version ){

				return true ;

			}else if( patch < base_kernel_patch_version &&
				  major <= base_kernel_major_version &&
				  minor <= base_kernel_minor_version ){

				return true ;
			}

			return false ;
		}() ;

		if( _update ){

			return QObject::tr( "Recommending updating the kernel from version %1 to a more recent version." ).arg( version ) ;
		}else{
			return QString() ;
		}
	}else{
		return QString() ;
	}
}

static bool _updateAvailable( const QString& e,const QStringList& args,QString * newVersion,QString * installedVersion )
{
	QString r = _run_cmd( { e,args } ).get().std_out() ;

	if( r.isEmpty() ){

		return false ;
	}else{
		auto l = r.split( "\n" ) ;

		if( l.size() > 1 ){

			auto m_iv = l.at( 0 ).split( " " ).last() ;
			auto m_nv = l.at( 1 ).split( " " ).last() ;

			*newVersion       = m_nv ;
			*installedVersion = m_iv ;

			if( m_iv == "0" ){

				/*
				* program not installed
				*/

				return false ;
			}else{
				return _check_version( m_nv,m_iv ) ;
			}
		}else{
			return false ;
		}
	}
}

static QString _checkLibreOfficeVersion()
{
	QString iv ;
	QString nv ;

	if( _updateAvailable( "lomanager",{ "--vinfo" },&nv,&iv ) ){

		return QObject::tr( "Updating Libreoffice from version \"%1\" to available version \"%2\" is recommended." ).arg( iv ).arg( nv ) ;
	}else{
		return QString() ;
	}
}

static QString _checkVirtualBoxVersion()
{
	QString iv ;
	QString nv ;

	if( _updateAvailable( "getvirtualbox",{ "--vinfo" },&nv,&iv ) ){

		return QObject::tr( "Updating VirtualBox from version \"%1\" to available version \"%2\" is recommended." ).arg( iv ).arg( nv ) ;
	}else{
		return QString() ;
	}
}

static QString _checkCallibeVersion()
{
	QString iv ;
	QString nv ;

	if( _updateAvailable( "calibre-manager",{ "--vinfo" },&nv,&iv ) ){

		return QObject::tr( "Updating Calibre from version \"%1\" to available version \"%2\" is recommended." ).arg( iv ).arg( nv ) ;
	}else{
		return QString() ;
	}
}

Task::future< QString >& checkForPackageUpdates()
{
	return Task::run( [](){

		auto e = _checkKernelVersion() ;

		decltype( e ) r ;

		if( !e.isEmpty() ){

			r += "\n" + e ;
		}

		e = _checkLibreOfficeVersion() ;

		if( !e.isEmpty() ){

			r += "\n" + e ;
		}

		e = _checkVirtualBoxVersion() ;

		if( !e.isEmpty() ){

			r += "\n" + e ;
		}

		e = _checkCallibeVersion() ;

		if( !e.isEmpty() ){

			r += "\n" + e ;
		}

		return r ;
	} ) ;
}

static const auto e = R"R(
#!/bin/sh

a="$(apt-cache pkgnames | grep kernel-[0-9] | sort -Vr | head -n 1)"
b="$(rpm -qa --queryformat '%{name}\n' | grep kernel-[0-9] | sort -Vr | head -n 1)"

if [ "$a" != "$b" ]; then
	echo -n "$a"
else
	echo -n ""
fi

)R" ;

Task::future<QString>& checkKernelVersions()
{
	return Task::run( []()->QString{

		auto s = settings::configPath() + "/tmp" ;

		QDir d ;

		d.mkpath( s ) ;

		auto exe = s + "/checkKernelUpdate" ;

		QFile f( exe ) ;

		if( !f.exists() ){

			f.open( QIODevice::WriteOnly ) ;

			f.write( e ) ;

			f.close() ;
		}

		f.setPermissions( QFile::ReadOwner | QFile::ExeOwner ) ;

		return _run_cmd( exe ).get().std_out() ;
	} ) ;
}

}
