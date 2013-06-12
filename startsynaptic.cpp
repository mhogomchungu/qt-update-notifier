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

#include "startsynaptic.h"
#include <QDebug>

startSynaptic::startSynaptic( bool b,QObject * parent ) : QObject( parent ),m_autoRefresh( b )
{
}

startSynaptic::~startSynaptic()
{
}

void startSynaptic::start()
{
	QThreadPool::globalInstance()->start( this );
}

void startSynaptic::run()
{
	QProcess exe ;
	qDebug() << QT_UPDATE_NOTIFIER_HELPER_PATH ;
	
	if( m_autoRefresh ){
		QString e = QString( QT_UPDATE_NOTIFIER_HELPER_PATH ) + QString( " --update-at-startup" ) ;
		exe.start( e ) ;
	}else{
		exe.start( QString( QT_UPDATE_NOTIFIER_HELPER_PATH ) ) ;
	}
	exe.waitForFinished( -1 ) ;
}
