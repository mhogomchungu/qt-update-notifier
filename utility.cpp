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


#include "utility.h"

utility::utility()
{
}

void utility::writeToFile( QString filepath,QString content,bool truncate )
{
	QByteArray f = filepath.toAscii() ;
	int fd ;
	if( truncate ){
		fd = open( f.constData(),O_CREAT | O_TRUNC | O_WRONLY ) ;
	}else{
		fd = open( f.constData(),O_CREAT | O_APPEND | O_WRONLY ) ;
	}
	if( fd != -1 ){

		fchmod( fd,S_IRUSR|S_IWUSR ) ;

		size_t n = content.size() ;

		size_t t = sizeof( wchar_t ) ;

		wchar_t * x = new wchar_t[ n ] ;

		size_t y = content.toWCharArray( x ) ;

		write( fd,x,y * t ) ;

		close( fd ) ;

		delete[] x ;
	}
}

QString utility::readFromFile( QString filepath )
{
	QByteArray f = filepath.toAscii() ;
	const char * path = f.constData() ;
	int fd = open( path,O_RDONLY ) ;
	if( fd != -1 ){
		struct stat st ;
		fstat( fd,&st ) ;

		size_t n = st.st_size ;

		size_t t = sizeof( wchar_t ) ;

		wchar_t * x = new wchar_t[ n ] ;

		size_t z = read( fd,x,n ) ;

		close( fd ) ;

		QString s = QString::fromWCharArray( x,z/t ) ;

		delete[] x ;

		return s ;
	}else{
		return QObject::tr( "Log is empty" ) ;
	}
}
