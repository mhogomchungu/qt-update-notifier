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
	emit outdatedPackages( m_package ) ;
}

void checkoldpackages::start()
{
	QThreadPool::globalInstance()->start( this ) ;
}

void checkoldpackages::run()
{
	this->checkKernelVersion() ;
	this->checkLibreOfficeVersion() ;
	this->checkVirtualBoxVersion() ;
	this->checkCallibeVersion() ;
}

void checkoldpackages::checkKernelVersion()
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

		/*
		 * start warning if a user uses a kernel less than 3.2.18
		 */
		if( update ){
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
		QString r = tr( "Updating VirtualBox from version \"%1\" to available version \"%2\" is recommended." ).arg( m_iv ).arg( m_nv ) ;
		m_package.append( r ) ;
	}else{
		m_package.append( QString( "" ) ) ;
	}
}

void checkoldpackages::checkCallibeVersion()
{
	if( this->updateAvailable( QString( "calibre-manager --vinfo" ) ) ){
		QString r = tr( "Updating Calibre from version \"%1\" to available version \"%2\" is recommended." ).arg( m_iv ).arg( m_nv ) ;
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
