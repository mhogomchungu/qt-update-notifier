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

#include "instance.h"

instance::instance( QObject * parent ) :QObject( parent )
{
	KStandardDirs k ;
	m_socketPath = k.localxdgconfdir() + QString( "/qt-update-notifier/oneInstance.socket" ) ;
	m_removeSocketPath = false ;
}

bool instance::firstInstance()
{
	QFile f( m_socketPath ) ;

	bool r ;
	if( f.exists() ){
		/*
		 * sockect file exists,that means either another instance is running or the previous one
		 * crashed before it removed the socket file
		 */
		QLocalSocket * s = new QLocalSocket( this ) ;
		s->connectToServer( m_socketPath ) ;
		if( s->waitForConnected() ){
			/*
			 * we got connected,that means another instance is running
			 */
			s->close() ;
			r =  false ;
		}else{
			/*
			 * could not connect,assume the previous instance crashed,remove the socket file and continue
			 */
			f.remove() ;
			r = this->startServer() ;
		}
		s->deleteLater() ;
		return r ;
	}else{
		/*
		 * sockect file does not exist,assume we are the first instance,set the socket file and start to listen on it
		 */
		return this->startServer() ;
	}
}

bool instance::startServer()
{
	m_localServer = new QLocalServer( this ) ;
	connect( m_localServer,SIGNAL( newConnection() ),this,SLOT( newInstanceObserved() ) ) ;
	m_localServer->listen( m_socketPath ) ;
	m_removeSocketPath = true ;
	return true ;
}

void instance::newInstanceObserved()
{
	QLocalSocket * s = m_localServer->nextPendingConnection() ;
	s->close() ;
	s->deleteLater() ;
}

instance::~instance()
{
	if( m_removeSocketPath ){
		QFile f( m_socketPath ) ;
		f.remove() ;
	}
}
