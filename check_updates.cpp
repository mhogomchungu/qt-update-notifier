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

check_updates::check_updates( QString configPath,QString language,QObject * parent )
{
	Q_UNUSED( parent ) ;
	m_configPath = configPath ;
	m_language = language ;
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

void check_updates::processUpdates( QByteArray& output1,QByteArray& output2 )
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

	QString updates = tr( "<table><tr><td>%1 to be upgraded</td></tr><tr><td>%2 to be replaced</td></tr><tr><td>%3 to be installed</td></tr></table>" ).arg( x ).arg( y ).arg( z ) ;

	QStringList n ;
	n.append( updates ) ;
	n.append( output2 );
	emit updateStatus( UPDATES_FOUND,n ) ;
}

void check_updates::reportUpdates()
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

	exe.start( m_aptUpdate );
	exe.waitForFinished( -1 ) ;

	int st = exe.exitCode() ;
	exe.close();

	QByteArray bogusData = "xyz" ;

	if( st == 0 ){

		exe.start( m_aptUpgrade );
		exe.waitForFinished( -1 ) ;
		QByteArray output = exe.readAllStandardOutput() ;
		exe.close();

		if( !output.isEmpty() ){
			if( output.contains( error1 ) || output.contains( error2 ) || output.contains( error3 ) ){
				list.append( bogusData );
				if( m_language == QString( "english_US" ) ){
					list.append( output );
				}else{
					QByteArray output1 ;
					QProcess e ;
					e.start( m_aptUpgrade );
					e.waitForFinished( -1 ) ;
					output1 = e.readAllStandardOutput() ;
					e.close();
					list.append( output1 );
				}
				emit updateStatus( INCONSISTENT_STATE,list );
			}else if( output.contains( success ) ){
				if( m_language == QString( "english_US" ) ){
					this->processUpdates( output,output );
				}else{
					QByteArray output1 ;
					QProcess e ;
					e.start( m_aptUpgrade );
					e.waitForFinished( -1 ) ;
					output1 = e.readAllStandardOutput() ;
					e.close();
					this->processUpdates( output,output1 );
				}
			}else{
				list.append( bogusData );
				QString s = tr( "no updates found" ) ;
				list.append( s );
				emit updateStatus( NO_UPDATES_FOUND,list );
			}
		}else{
			list.append( bogusData );
			QString s = tr( "no updates found" ) ;
			list.append( s );
			emit updateStatus( NO_UPDATES_FOUND,list );
		}
	}else{
		/*
		 * I cant see how i will get here
		 */
		list.append( bogusData );
		QString s = tr( "warning: apt-get update finished with errors" ) ;
		list.append( s );
		emit updateStatus( NO_UPDATES_FOUND,list );
	}
}

check_updates::~check_updates()
{
}
