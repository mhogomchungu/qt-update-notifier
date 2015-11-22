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

#include "tray_application_type.h"

#include "version.h"

#if USE_KDE_STATUS_NOTIFIER

#include <QApplication>
#include "qtUpdateNotifier.h"

#include <kapplication.h>
#include <kcmdlineargs.h>
#include <kaboutdata.h>
#include <klocalizedstring.h>
#include <kuniqueapplication.h>

#include <unistd.h>

int autoStart( KUniqueApplication& a )
{
	if( qtUpdateNotifier::autoStartEnabled() ){

		qtUpdateNotifier w ;

		w.setDebug( KCmdLineArgs::allArguments().contains( "-d" ) ) ;

		w.start() ;

		return a.exec() ;
	}else{
		return 0 ;
	}
}

int start( KUniqueApplication& a )
{
	qtUpdateNotifier w ;

	w.setDebug( KCmdLineArgs::allArguments().contains( "-d" ) ) ;

	w.start() ;

	return a.exec() ;
}

int main( int argc,char * argv[] )
{
	KAboutData aboutData( 	"qt-update-notifier",
				0,
				ki18n( "qt-update-notifier" ),
				VERSION,
				ki18n( "a qt based apt-get package updates checker." ),
				KAboutData::License_GPL_V2,
				ki18n( "(c)2013-2015,Francis Banyikwa\nemail:mhogomchungu@gmail.com" ),
				ki18n( "mhogomchungu@gmail.com" ),
				"http://www.pclinuxos.com/forum/index.php/topic,112999.0/topicseen.html",
				"http://www.pclinuxos.com/forum/index.php/topic,112999.0/topicseen.html" ) ;

	KCmdLineArgs::init( argc,argv,&aboutData ) ;

	KCmdLineOptions options;

	options.add( "a",ki18n( "auto start application" ) ) ;
	options.add( "d",ki18n( "debug application" ) ) ;

	KCmdLineArgs::addCmdLineOptions( options ) ;

	KUniqueApplication::addCmdLineOptions() ;

	settings::init() ;

	if( KUniqueApplication::start() ){

		KUniqueApplication a ;

		if( KCmdLineArgs::allArguments().contains( "-a" ) ){

			return autoStart( a ) ;
		}else{
			return start( a ) ;
		}
	}else{
		return qtUpdateNotifier::instanceAlreadyRunning() ;
	}
}

#else
#include <QApplication>
#include "qtUpdateNotifier.h"
#include <iostream>

static const auto _msg = R"R(
copyright: 2013-2015 Francis Banyikwa,mhogomchungu@gmail.com
license  : GPLv2+

options:

-a       Usable with desktop environment's auto start feature at login.
	 When the application is started with this option,it will
	 continue to run only if "auto start at login" checkbox is set
	 in the application's configuration window.

	 The application's configuration window can be activated through
	 tray icon -> right click -> configuration window.
)R" ;

int main( int argc,char * argv[] )
{
	QApplication a( argc,argv ) ;

	auto v = QCoreApplication::arguments() ;

	auto _help = [ & ](){

		return  v.contains( "-h" )        ||
			v.contains( "-v" )        ||
			v.contains( "--help" )    ||
			v.contains( "--version" ) ||
			v.contains( "-version" )  ||
			v.contains( "-help" ) ;
	} ;

	auto _start = [ & ](){

		qtUpdateNotifier w ;

		w.setDebug( v.contains( "-d" ) ) ;

		w.start() ;

		return a.exec() ;
	} ;

	settings::init() ;

	if( v.contains( "-a" ) ){

		if( qtUpdateNotifier::autoStartEnabled() ){

			return _start() ;
		}else{
			return 0 ;
		}

	}else if( _help() ){

		std::cout << "\nversion  : " << VERSION << _msg << std::endl ;

		return 0 ;
	}else{
		return _start() ;
	}
}

#endif
