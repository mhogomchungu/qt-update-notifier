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


#ifndef CHECK_UPDATES_H
#define CHECK_UPDATES_H

#include <QThread>

#include <QString>
#include <QStringList>
#include <QDir>
#include <QDebug>
#include <QProcess>
#include <QFile>

#define UPDATES_FOUND      0 
#define INCONSISTENT_STATE 1
#define NO_UPDATES_FOUND   2
#define NO_NET_CONNECTION  3
#define UNDEFINED_STATE    4

class check_updates : public QThread
{
	Q_OBJECT
public:
	explicit check_updates( QString configPath,QObject * parent = 0 );
	~check_updates() ;
signals:
	void updateList( QStringList ) ;
	void updateStatus( int,QStringList ) ;
public slots:
private:
	QString m_configPath ;
	void reportUpdates( void ) ;
	bool online( void ) ;
	void run( void ) ;
};

#endif // CHECK_UPDATES_H
