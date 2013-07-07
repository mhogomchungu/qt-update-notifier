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
			m_package.append( tr( "Recommending updating the kernel from version %1 to a more receant version." ).arg( version ) ) ;
		}else{
			m_package.append( QString( "" ) ) ;
		}
	}
}

void checkoldpackages::checkLibreOfficeVersion()
{
	//lomanager-4.0.4-1pclos2013.i586.rpm
	QString lo = QString( "lomanager" ) ;
	int index_1 = m_packageList.indexOf( lo ) ;
	if( index_1 == -1 ){
		m_package.append( QString( "" ) ) ;
	}else{
		int x = index_1 + lo.size() + 1 ;
		int index_2 = m_packageList.indexOf( "-",x ) ;
		int z = index_2 - x ;
		QByteArray lomanager = m_packageList.mid( x,z ) ;
		QProcess exe ;
		exe.start( QString( "libreoffice4.0 --version" ) ) ;
		exe.waitForFinished( -1 ) ;
		QByteArray data = exe.readAll() ;

		if( data.isEmpty() ){
			m_package.append( QString( "" ) ) ;
			return ;
		}

		data.remove( data.length() - 2,2 ) ; // remove the last new line character

		QString iv = QString( data.split( ' ' ).last() ) ;
		QString nv = QString( lomanager ) ;

		if( iv < nv ){
			QString r = tr( "Updating libreoffice from version \"%1\" to available version \"%2\" is recommended." ).arg( iv ).arg( nv ) ;
			m_package.append( r ) ;
		}else{
			m_package.append( QString( "" ) ) ;
		}
	}
}

void checkoldpackages::checkVirtualBoxVersion()
{
	//getvirtualbox-4.2.14-1pclos2013.i586.rpm

	QString vb = QString( "getvirtualbox" ) ;
	int index_1 = m_packageList.indexOf( vb ) ;
	if( index_1 == -1 ){
		m_package.append( tr( "" ) ) ;
	}else{
		int x = index_1 + vb.size() + 1 ;
		int index_2 = m_packageList.indexOf( "-",x ) ;
		int z = index_2 - x ;
		QByteArray vb = m_packageList.mid( x,z ) ;

		QProcess exe ;

		exe.start( QString( "VirtualBox --help" ) ) ;
		exe.waitForFinished( -1 ) ;

		QByteArray data = exe.readAll() ;

		if( data.isEmpty() ){
			m_package.append( QString( "" ) ) ;
			return ;
		}

		QString l = QString( "Oracle VM VirtualBox Manager " ) ;
		int ls = l.size() ;

		int v_1 = data.indexOf( l ) ;
		if( v_1 == -1 ){
			m_package.append( QString( "" ) ) ;
			return ;
		}

		int v_2 = data.indexOf( "\n",v_1 ) ;
		QByteArray r = data.mid( v_1 + ls,v_2 - ( v_1 + ls ) ) ;

		QString iv = QString( r ) ;
		QString nv = QString( vb ) ;

		if( iv < nv ){
			QString r = tr( "Updating Virtualbox from version \"%1\" to available version \"%2\" is recommended." ).arg( iv ).arg( nv ) ;
			m_package.append( r ) ;
		}else{
			m_package.append( QString( "" ) ) ;
		}
	}
}

void checkoldpackages::checkCallibeVersion()
{
	//calibre-manager-0.1-1pclos2013.noarch.rpm
	m_package.append( QString( "" ) ) ;
}
