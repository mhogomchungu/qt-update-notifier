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

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <string.h>
#include <grp.h>
#include <pwd.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>

#include "version.h"

#include "kdesu_path.h"
#include "process.h"

#define DEBUG 1

#define PRIORITY -15

static const char * groupName = "qtupdatenotifier" ;

#define stringsAreEqual( x,y ) strcmp( x,y ) == 0
#define stringContains( x,y ) strstr( x,y ) != NULL

static int userHasPermission( void )
{
	char ** entry ;
	char * name   ;

	uid_t uid = getuid() ;

	struct group * grp ;

	struct passwd * pass ;

	if( uid == 0 ){
		return 1 ;
	}

	pass = getpwuid( uid ) ;

	if( pass == NULL ){
		return 0 ;
	}

	grp = getgrnam( groupName ) ;

	if( grp == NULL ){
		return 0 ;
	}

	name  = pass->pw_name ;
	entry = grp->gr_mem ;

	while( *entry != NULL ){

		if( stringsAreEqual( *entry,name ) ){

			return 1 ;
		}else{
			entry++ ;
		}
	}

	return 0 ;
}

static int userHasNoPermission( void )
{
	return userHasPermission() == 0 ;
}

static inline void printOUtPut( const char * e,int debug )
{
	if( debug ){

		printf( "%s",e ) ;
	}
}

static inline void logStage( int fd,const char * msg )
{
	const char * e = "\n-------------------------------------------\n" ;

	size_t s = strlen( e ) ;

	printf( "%s\n",msg ) ;

	write( fd,e,s ) ;
	write( fd,msg,strlen( msg ) ) ;
	write( fd,e,s ) ;
}

static int itIsSafeToUpdate( const char * e,int debug )
{
	const char * error1 = "The following packages have unmet dependencies" ;
	const char * error2 = "E: Error, pkgProblemResolver::Resolve generated breaks, this may be caused by held packages." ;
	const char * error3 = "The following packages have been kept back" ;

	const char * success = "\nThe following packages will be" ;

	const char * noUpdates = "0 upgraded, 0 newly installed, 0 removed and 0 not upgraded." ;

	printOUtPut( e,debug ) ;

	if( e == NULL){
		return 1 ;
	}
	if( stringContains( e,error1 ) ){
		return 1 ;
	}
	if( stringContains( e,error2 ) ){
		return 1 ;
	}
	if( stringContains( e,error3 ) ){
		return 1 ;
	}
	if( stringContains( e,success ) ){
		return 0 ;
	}
	if( stringContains( e,noUpdates ) ){
		return 2 ;
	}

	return 1 ;
}

static void setDefaultLanguageToEnglish( void )
{
	setenv( "LANG","en_US.UTF-8",1 ) ;
	setenv( "LANGUAGE","en_US.UTF-8",1 ) ;
}

static void printProcessOUtPut( process_t p,int fd,int debug )
{
	char * e ;
	int z ;

	while( 1 ){

		e = NULL ;

		z = ProcessGetOutPut( p,&e,ProcessStdOut ) ;

		if( e ){

			write( fd,e,z ) ;

			printOUtPut( e,debug ) ;

			free( e ) ;
		}else{
			break ;
		}
	}
}

static int refreshPackageList( int fd,int debug )
{
	int r ;

	process_t p ;

	logStage( fd,"entering refreshPackageList" ) ;

	if( userHasPermission() ){

		p = Process( "/usr/bin/apt-get","update",NULL ) ;

		ProcessSetOptionUser( p,0 ) ;
		ProcessSetOptionPriority( p,PRIORITY ) ;

		ProcessStart( p ) ;

		printProcessOUtPut( p,fd,debug ) ;

		r = ProcessWaitUntilFinished( &p ) ;
	}else{
		printf( "error: insufficent privileges to perform this operation\n" ) ;
		r = 1 ;
	}

	logStage( fd,"leaving refreshPackageList" ) ;

	return r ;
}

static int aptAndSynapticAreRunning( void )
{
	int r ;

	uid_t uid = getuid() ;

	process_t p = Process( "/bin/pidof","/usr/sbin/synaptic",NULL ) ;

	ProcessSetOptionUser( p,uid ) ;
	ProcessStart( p ) ;

	r = ProcessWaitUntilFinished( &p ) ;

	if( r != 0 ){

		p = Process( "/bin/pidof","/usr/bin/apt-get",NULL ) ;
		ProcessSetOptionUser( p,uid ) ;

		ProcessStart( p ) ;

		r = ProcessWaitUntilFinished( &p ) ;
	}

	return r == 0 ;
}

char * getProcessOutPut( process_t p,int fd,int debug )
{
	char * e      = NULL ;
	char * f      = NULL ;
	char * buffer = NULL ;

	size_t buffer_size = 0 ;
	size_t output_size = 0 ;

	while( 1 ){

		output_size = ProcessGetOutPut( p,&e,ProcessStdOut ) ;

		if( output_size > 0 ){

			write( fd,e,output_size ) ;

			f = realloc( buffer,buffer_size + output_size + 1 ) ;

			if( f != NULL ){

				buffer = f ;

				strcpy( buffer + buffer_size,e ) ;

				buffer_size += output_size ;

				printOUtPut( e,debug ) ;
				free( e ) ;
			}else{
				free( e ) ;
				break ;
			}
		}else{
			break ;
		}
	}

	return buffer ;
}

static int autoUpdate( int fd,int debug )
{
	process_t p ;

	int r ;

	char * buffer ;

	logStage( fd,"entering autoUpdate" ) ;

	if( userHasNoPermission() ){

		printf( "error: insufficent privileges to perform this operation\n" ) ;

		r = 1 ;
	}else{
		if( aptAndSynapticAreRunning() ){

			printf( "error: apt and/or synaptic are running\n" ) ;

			return 3 ;
		}

		/*
		 * make sure the output we are going to get is in english regardless of user locale
		 */
		setDefaultLanguageToEnglish() ;

		r = refreshPackageList( fd,debug ) ;

		if( r == 0 ){

			/*
			 * check if its safe to update
			 */

			p = Process( "/usr/bin/apt-get","dist-upgrade","--simulate",NULL ) ;

			ProcessSetOptionUser( p,0 ) ;
			ProcessSetOptionPriority( p,PRIORITY ) ;

			ProcessStart( p ) ;

			buffer = getProcessOutPut( p,fd,debug ) ;

			ProcessWaitUntilFinished( &p ) ;

			if( buffer ){

				r = itIsSafeToUpdate( buffer,debug ) ;

				free( buffer ) ;
			}else{
				printf( "IT IS NOT SAFE TO UPDATE,apt-get gave no output\n" ) ;
				r = 1 ;
			}

			if( r == 0 ){

				logStage( fd,"running apt-get dist-upgrade --assume-yes" ) ;
				/*
				 * it seem to be safe to update,update
				 */
				printf( "updates found\n" ) ;

				p = Process( "/usr/bin/apt-get","dist-upgrade","--assume-yes",NULL ) ;

				ProcessSetOptionUser( p,0 ) ;
				ProcessSetOptionPriority( p,PRIORITY ) ;

				ProcessStart( p ) ;

				printProcessOUtPut( p,fd,debug ) ;

				r = ProcessWaitUntilFinished( &p ) ;

				logStage( fd,"done running apt-get dist-upgrade --assume-yes" ) ;

				if( r != 0 ){

					printf( "error: failed to run dist-upgrade --assume-yes\n" ) ;
					logStage( fd,"error: failed to run dist-upgrade --assume-yes" ) ;
				}else{
					logStage( fd,"running apt-get clean" ) ;

					/*
					 * clear cache
					 */
					p = Process( "/usr/bin/apt-get","clean",NULL ) ;

					ProcessSetOptionUser( p,0 ) ;
					ProcessSetOptionPriority( p,PRIORITY ) ;

					ProcessStart( p ) ;

					ProcessWaitUntilFinished( &p ) ;

					logStage( fd,"done running apt-get clean" ) ;
				}

			}else if( r == 2 ){

				printf( "There are no updates\n" ) ;
			}else{
				printf( "IT IS NOT SAFE TO UPDATE\n" ) ;
			}
		}else{
			printf( "failed to refresh package list\n" ) ;
		}
	}

	logStage( fd,"leaving autoUpdate" ) ;
	return r ;
}

static int downloadPackages( int fd,int debug )
{
	process_t p ;

	int r ;

	logStage( fd,"entering downloadPackages" ) ;

	if( userHasPermission() ){

		r = refreshPackageList( fd,debug ) ;

		if( r == 0 ){

			p = Process( "/usr/bin/apt-get","dist-upgrade","--download-only","--assume-yes",NULL ) ;

			ProcessSetOptionUser( p,0 ) ;
			ProcessSetOptionPriority( p,PRIORITY ) ;

			ProcessStart( p ) ;

			printProcessOUtPut( p,fd,debug ) ;

			r = ProcessWaitUntilFinished( &p ) ;
		}
	}else{
		logStage( fd,"error: insufficent privileges to perform this operation\n" ) ;
		r = 1 ;
	}

	logStage( fd,"leaving downloadPackages" ) ;

	return r ;
}

static int startSynaptic( const char * e )
{
	process_t p = Process( "/usr/bin/synaptic",e,NULL ) ;

	if( userHasPermission() ){

		ProcessSetOptionUser( p,0 ) ;
	}else{
		ProcessSetOptionUser( p,getuid() ) ;
	}

	ProcessStart( p ) ;

	return ProcessWaitUntilFinished( &p ) ;
}

static int printOptions( void )
{
	const char * options = "\
version  : "VERSION"\n\
copyright: 2013 Ink Francis,mhogomchungu@gmail.com\n\
license  : GPLv2+\n\
\n\
argument list:\n\
	--auto-update		calls \"apt-get update\" followed by \"apt-get dist-upgrade\"\n\
	--download-packages	calls \"apt-get update\" followed by \"apt-get --dist-upgrade --download-only --assume-yes\"\n\
	--start-synaptic	calls \"kdesu /usr/sbin/synaptic\"\n\
	--start-synaptic --update-at-startup	calls \"kdesu /usr/sbin/synaptic --update-at-startup\"\n\
	--debug      	this option can be added as the last option to print program output on the terminal.\n\
			The same printed information will also be  in ~/.config/qt-update-notifier/backEnd.log\n\
NOTE:\n\
	\"kdesu\" will be used in kde session and gksu will be used in any other session\n" ;

	printf( "%s",options ) ;
	return 0 ;
}

static int _help( const char * e )
{
	#define x( z ) strcmp( e,z ) == 0
	return x( "--help" ) || x( "-help" ) || x( "-h" ) || x( "-version" ) || x( "--version" ) || x( "-v" ) ;
}

int main( int argc,char * argv[] )
{
	const char * e ;
	const char * f ;

	int fd ;
	int st ;

	int debug ;

	char logPath[ 1024 ] ;

	struct passwd * pass = getpwuid( getuid() ) ;

	if( pass == NULL ){
		return 10 ;
	}

	snprintf( logPath,1024,"/home/%s/.config/qt-update-notifier/backEnd.log",pass->pw_name ) ;

	if( argc < 2 ){
		return printOptions() ;
	}

	if( seteuid( getuid() ) == -1 ){
		return 12 ;
	}

	fd = open( logPath,O_CREAT|O_TRUNC|O_WRONLY,S_IRUSR|S_IWUSR ) ;

	if( seteuid( 0 ) == -1 ){
		close( fd ) ;
		return 12 ;
	}

	if( fd == -1 ){
		return 11 ;
	}

	fchmod( fd,S_IRUSR|S_IWUSR|S_IRWXG|S_IRGRP|S_IROTH|S_IWOTH ) ;

	e = *( argv + 1 ) ;

	if( argc > 1 ){

		if( stringsAreEqual( *( argv + argc - 1 ),"--debug" ) ){

			debug = 1 ;
		}else{
			debug = 0 ;
		}
	}

	if( _help( e ) ){

		st = printOptions() ;
	}else{
		if( stringsAreEqual( e,"--start-synaptic" ) ){

			if( argc == 3 ){

				f = *( argv + 2 ) ;

				if( stringsAreEqual( f,"--update-at-startup" ) ){

					st = startSynaptic( f ) ;
				}else{
					printf( "error: unrecognized or invalid synaptic option\n" ) ;
					st = 1 ;
				}
			}else{
				st = startSynaptic( NULL ) ;
			}
		}else{
			if( stringsAreEqual( e,"--auto-update" ) ){

				st = autoUpdate( fd,debug ) ;

			}else if( stringsAreEqual( e,"--download-packages" ) ){

				st = downloadPackages( fd,debug ) ;

			}else{
				printf( "error: unrecognized or invalid option\n" ) ;
				st = 1 ;
			}
		}
	}

	close( fd ) ;
	return st ;
}
