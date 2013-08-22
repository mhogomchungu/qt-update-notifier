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



#ifndef INSTANCE_H
#define INSTANCE_H

#include <QObject>
#include <QtNetwork/QLocalServer>
#include <QtNetwork/QLocalSocket>
#include <kstandarddirs.h>
#include <QString>
#include <QFile>
#include <QDir>
#include <QDebug>

class instance : public QObject
{
	Q_OBJECT
public:
	explicit instance( QObject * parent = 0 ) ;
	~instance() ;
	bool firstInstance( void ) ;
signals:

private slots:
	void newInstanceObserved( void ) ;
private:
	bool startServer( void ) ;
	QLocalServer * m_localServer ;
	QString m_socketPath ;
	bool m_removeSocketPath ;
};

#endif // INSTANCE_H
