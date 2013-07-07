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

checkoldpackages::checkoldpackages( QByteArray data,QObject * parent ) : QObject( parent ),m_packageList( data )
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
	this->checkCallibeVersion();
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
		version.truncate( index ) ;
		/*
		 * start warning if a user uses a kernel less than 3.2.18
		 */
		if( version < QString( "3.2.18" ) ){
			m_package.append( tr( "Recommending updating the kernel from version %1 to a more recent version." ).arg( version ) ) ;
		}else{
			m_package.append( QString( "" ) ) ;
		}
	}
}

void checkoldpackages::checkLibreOfficeVersion()
{
	if( this->updateAvailable( QString( "lomanager --vinfo" ) ) ){
		QString r = tr( "Updating Libreoffice from version \"%1\" to available version \"%2\" is recommended." ).arg( m_iv ).arg( m_nv ) ;
		m_package.append( r ) ;
	}else{
		m_package.append( QString( "" ) ) ;
	}
}

void checkoldpackages::checkVirtualBoxVersion()
{
	if( this->updateAvailable( QString( "getvirtualbox --vinfo" ) ) ){
		QString r =tr( "Updating VirtualBox from version \"%1\" to available version \"%2\" is recommended." ).arg( m_iv ).arg( m_nv ) ;
		m_package.append( r ) ;
	}else{
		m_package.append( QString( "" ) ) ;
	}
}

void checkoldpackages::checkCallibeVersion()
{
	if( this->updateAvailable( QString( "calibre-manager --vinfo" ) ) ){
		QString r =tr( "Updating Calibre from version \"%1\" to available version \"%2\" is recommended." ).arg( m_iv ).arg( m_nv ) ;
		m_package.append( r ) ;
	}else{
		m_package.append( QString( "" ) ) ;
	}
}

bool checkoldpackages::updateAvailable( QString e )
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
				return m_nv > m_iv ;
			}
		}else{
			return false ;
		}
	}
}
