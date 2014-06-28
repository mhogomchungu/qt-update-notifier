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

#ifndef UTILITY_H
#define UTILITY_H

#include <QString>
#include <QStringList>

struct result
{
	typedef enum{
		undefinedState,
		inconsistentState,
		noUpdatesFound,
		updatesFound,
		noNetworkConnection
	}repoState ;
	bool passed()
	{
		return taskStatus == 0 ;
	}
	int taskStatus ;
	result::repoState repositoryState ;
	QStringList taskOutput ;
};

namespace utility
{
	void writeToFile( const QString& filepath,const QString& content,bool truncate ) ;
	QString readFromFile( const QString& filepath ) ;
	result reportUpdates( void ) ;
	result autoDownloadPackages( void ) ;
	result autoUpdatePackages( void ) ;
	result checkForPackageUpdates( void ) ;
	result autoRefreshStartSYnaptic( void ) ;
	result startSynaptic( void ) ;
}

#endif // UTILITY_H
