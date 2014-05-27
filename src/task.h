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

#ifndef TASK_H
#define TASK_H

#include <QRunnable>
#include <QObject>
#include <QStringList>

class Task : public QObject,public QRunnable
{
	Q_OBJECT
public:
	typedef enum{
		autoRefreshStartSYnaptic,
		startSynaptic,
		downloadPackages,
		updateSystem,
		checkOutDatedPackages,
		checkUpDates
	}action;

	typedef enum{
		updatesFound = 0,
		inconsistentState,
		noUpdatesFound,
		noNetworkConnection,
		undefinedState,
		aptGetFailed
	}updateState;

	Task( QString n = QString()) ;
	~Task() ;

	void start( Task::action ) ;
	void setConfigPath( const QString& path ) ;
	void setLocalLanguage( const QString& languale ) ;
signals:
	void taskFinished( int,QStringList ) ;
	void taskFinished( int taskStatus ) ;
	void taskFinished( QStringList ) ;
private:
	void run( void ) ;

	void startSynapticTask( void ) ;
	void checkUpdatesTask( void ) ;
	void checkOutDatedPackagesTask( void ) ;

	void checkKernelVersion( void ) ;
	void checkLibreOfficeVersion( void ) ;
	void checkVirtualBoxVersion( void ) ;
	void checkCallibeVersion( void ) ;
	bool updateAvailable( const QString& ) ;

	QString m_iv ;
	QString m_nv ;
	QStringList m_package ;
	Task::action m_action ;

	void processUpdates( QByteArray& output1,QByteArray& output2 ) ;
	void reportUpdates( void ) ;

	QString m_configPath ;
	QString m_language ;
	QString m_aptUpdate ;
	QString m_aptUpgrade ;
	QString m_networkConnectivityChecker ;
};

#endif // TASK_H
