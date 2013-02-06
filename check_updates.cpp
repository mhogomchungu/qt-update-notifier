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

check_updates::check_updates( QObject * parent )
{
	Q_UNUSED( parent ) ;
	connect( this,SIGNAL( finished() ),this,SLOT( deleteLater() ) ) ;
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
	}
}

void check_updates::reportUpdates()
{
	QString userDir = QDir::homePath() + QString( "/.qt-update-notifier" ) ;

	QString aptUpdate = QString( "apt-get -s -o Debug::NoLocking=true -o dir::state=%1/apt update" ).arg( userDir ) ;
	QString aptUpgrade = QString( "apt-get -s -o Debug::NoLocking=true -o dir::state=%1/apt dist-upgrade" ).arg( userDir ) ;

	QDir dir ;
	dir.mkdir( userDir ) ;
	dir.mkdir( userDir + QString( "/apt" ) ) ;
	dir.mkdir( userDir + QString( "/apt/lists" ) ) ;
	dir.mkdir( userDir + QString( "/apt/lists/partial" ) ) ;

	QStringList list ;
	QProcess exe ;
	exe.start( aptUpdate );
	exe.waitForFinished( -1 ) ;
	int st = exe.exitCode() ;
	exe.close();
	if( st == 0 ){
		exe.start( aptUpgrade );
		exe.waitForFinished( -1 ) ;
		QByteArray output = exe.readAllStandardOutput() ;
		int st = exe.exitCode() ;
		exe.close();
		if( st == 0 ){
			if( output.contains( "\nThe following packages will be" ) ){
				emit updatesFound( true,list );
				return ;
			}
		}
	}
	emit updatesFound( false,list );
}

check_updates::~check_updates()
{
	;//qDebug() << "~check_updates()";
}
