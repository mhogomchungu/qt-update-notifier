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

        if( _help() ){

                std::cout << "\nversion  : " << VERSION << _msg << std::endl ;

                return 0 ;
	}else{
                qtUpdateNotifier w( v.contains( "-a" ) ) ;

                w.setDebug( v.contains( "-d" ) ) ;

                w.start() ;

                return a.exec() ;
        }
}
