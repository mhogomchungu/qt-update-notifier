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

#include "check_updates.h"

check_updates::check_updates( QString configPath,QObject * parent )
{
	Q_UNUSED( parent ) ;
	m_configPath = configPath ;
}

bool check_updates::online()
{
	QProcess exe ;
	exe.start( QString( "ping -c 1 8.8.8.8" ) ) ;
	exe.waitForFinished() ;
	int st = exe.exitCode() ;
	exe.close();
	return st == 0 ;
}

void check_updates::run()
{
	if( this->online() ){
		this->reportUpdates();
	}else{
		QStringList l ;
		emit updateStatus( NO_NET_CONNECTION,l );
	}
}

void check_updates::processUpdates( QByteArray data )
{
	QString updates ;

	int count ;

	QStringList l = QString( data ).split( "\n" ) ;

	int index = l.indexOf( QString( "The following packages will be upgraded" ) ) ;

	const char * threeSpaceCharacters = "   " ;

	count = 0 ;
	if( index != -1 ){
		while( true ){
			index++ ;
			if( l.at( index ).startsWith( threeSpaceCharacters ) ){
				count++ ;
			}else{
				break ;
			}
		}
	}

	updates = tr( "pkgs to be upgraded: %1\n" ).arg( QString::number( count ) ) ;

	index = l.indexOf( QString( "The following packages will be REPLACED:" ) ) ;

	count = 0 ;
	if( index != -1 ){
		while( true ){
			index++ ;
			if( l.at( index ).startsWith( threeSpaceCharacters ) ){
				count++ ;
			}else{
				break ;
			}
		}
	}

	updates += tr( "pkgs to be replaced: %1\n" ).arg( QString::number( count ) ) ;

	index = l.indexOf( QString( "The following NEW packages will be installed:" ) ) ;

	count = 0 ;
	if( index != -1 ){
		while( true ){
			index++ ;
			if( l.at( index ).startsWith( threeSpaceCharacters ) ){
				count++ ;
			}else{
				break ;
			}
		}
	}

	updates += tr( "pkgs to be installed: %1" ).arg( QString::number( count ) ) ;

	int size = l.size() ;
	int i = 0 ;
	while( i < size ){
		if( l.at( i ).startsWith( QString( "Inst " ) ) ){
			l.removeAt( i );
			i = 0 ;
			size = l.size() ;
		}else{
			i++ ;
		}
	}

	size = l.size() ;
	i = 0 ;
	while( i < size ){
		if( l.at( i ).startsWith( QString( "Conf " ) ) ){
			l.removeAt( i );
			i = 0 ;
			size = l.size() ;
		}else{
			i++ ;
		}
	}

	l.prepend( updates );
	emit updateStatus( UPDATES_FOUND,l );
}

void check_updates::reportUpdates()
{
	QString aptUpdate = QString( "apt-get -s -o Debug::NoLocking=true -o dir::state=%1/apt update" ).arg( m_configPath ) ;
	QString aptUpgrade = QString( "apt-get -s -o Debug::NoLocking=true -o dir::state=%1/apt dist-upgrade" ).arg( m_configPath ) ;

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

	exe.start( aptUpdate );
	exe.waitForFinished( -1 ) ;

	int st = exe.exitCode() ;
	exe.close();

	QByteArray bogusData = "xyz" ;

	if( st == 0 ){

		exe.start( aptUpgrade );
		exe.waitForFinished( -1 ) ;
		QByteArray output = exe.readAllStandardOutput() ;
		exe.close();

		if( !output.isEmpty() ){
			if( output.contains( error1 ) || output.contains( error2 ) || output.contains( error3 ) ){
				list.append( bogusData );
				list.append( output );
				emit updateStatus( INCONSISTENT_STATE,list );
			}else if( output.contains( success ) ){
				this->processUpdates( output );
			}else{
				list.append( bogusData );
				QString s = tr( "no updates found" ) ;
				list.append( s.toAscii() );
				emit updateStatus( NO_UPDATES_FOUND,list );
			}
		}else{
			list.append( bogusData );
			QString s = tr( "no updates found" ) ;
			list.append( s.toAscii() );
			emit updateStatus( NO_UPDATES_FOUND,list );
		}
	}else{
		/*
		 * I cant see how i will get here
		 */
		list.append( bogusData );
		QString s = tr( "warning: apt-get update finished with errors" ) ;
		list.append( s.toAscii() );
		emit updateStatus( NO_UPDATES_FOUND,list );
	}
}

check_updates::~check_updates()
{
}
