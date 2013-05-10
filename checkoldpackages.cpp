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

#include "checkoldpackages.h"

checkoldpackages::checkoldpackages( QObject * parent ) : QObject( parent )
{
}

checkoldpackages::~checkoldpackages()
{
	emit outdatedPackages( m_package );
}

void checkoldpackages::start()
{
	QThreadPool::globalInstance()->start( this ) ;
}

void checkoldpackages::run()
{
	this->checkKernelVersion();
	this->checkLibreOfficeVersion();
	this->checkVirtualBoxVersion();
}

void checkoldpackages::checkKernelVersion()
{
	QProcess exe ;
	exe.start( QString( "uname -r" ) ) ;
	exe.waitForFinished( -1 ) ;
	QString version = exe.readAll() ;
	exe.close();

	int index = version.indexOf( QString( "-" ) ) ;
	if( index != -1 ){
		version = version.remove( index,100 ) ;
		if( version.startsWith( QString( "3.2.18" ) ) ){
			//m_package.append( tr( "kernel version %1 is too old,recommending upgrading it" ).arg( version ) ) ;
			m_package.append( tr( "" ) ) ;
		}else{
			m_package.append( tr( "" ) ) ;
		}
	}
}

void checkoldpackages::checkLibreOfficeVersion()
{
	m_package.append( tr( "" ) ) ;
}

void checkoldpackages::checkVirtualBoxVersion()
{
	m_package.append( tr( "" ) ) ;
}
