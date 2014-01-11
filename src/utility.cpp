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

#include <QObject>
#include <QDebug>
#include <QFile>
#include <QIODevice>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "settings.h"

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
	QByteArray f = filepath.toAscii() ;
	if( truncate ){
		m_fd = open( f.constData(),O_CREAT|O_TRUNC|O_WRONLY,S_IRUSR|S_IWUSR ) ;
	}else{
		m_fd = open( f.constData(),O_CREAT|O_APPEND|O_WRONLY,S_IRUSR|S_IWUSR ) ;
	}
}

fileManager::fileManager( const QString& filepath )
{
	QByteArray f = filepath.toAscii() ;
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

utility::utility()
{
}

static void writeToFile( const QString& filepath,const QString& content,bool truncate )
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

void utility::writeToFile( const QString& filepath,const QString& content,bool truncate )
{
	if( filepath == settings::activityLogFilePath() ){
		if( settings::prefixLogEntries() ){
			/*
			 * add new entry at the front of the log
			 */
			QString data = content + utility::readFromFile( filepath ) ;
			::writeToFile( filepath,data,true )  ;
		}else{
			/*
			 * add new entries at the back of the log
			 */
			::writeToFile( filepath,content,truncate ) ;
		}
	}else{
		/*
		 * add new entries at the back of the log
		 */
		::writeToFile( filepath,content,truncate ) ;
	}
}

QString utility::readFromFile( const QString& filepath )
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
