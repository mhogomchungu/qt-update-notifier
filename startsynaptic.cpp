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

startSynaptic::startSynaptic( QObject * parent ) : QObject( parent )
{
}

startSynaptic::~startSynaptic()
{
}

void startSynaptic::start( QString option )
{
	m_option = option ;
	QThreadPool::globalInstance()->start( this );
}

void startSynaptic::run()
{
	QProcess exe ;

	if( m_option.isEmpty() ){
		exe.start( QString( QT_UPDATE_NOTIFIER_HELPER_PATH ) ) ;
	}else{
		QString e = QString( "%1 %2" ).arg( QString( QT_UPDATE_NOTIFIER_HELPER_PATH ) ).arg( m_option )  ;
		exe.start( e ) ;
	}

	exe.waitForFinished( -1 ) ;

	emit result( exe.exitCode() ) ;
}
