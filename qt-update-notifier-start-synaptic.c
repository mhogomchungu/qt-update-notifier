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

#include<string.h>
#include <grp.h>
#include <pwd.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "kdesu_path.h"
#include "process.h"

#define DEBUG 0

/*
 * KDESU_PATH constant is set at build time
 */
static const char * kdesu = KDESU_PATH ;

static const char * groupName = "qtupdatenotifier" ;

static int userHasPermission( void )
{
	int st = 0 ;

	const char ** entry ;
	const char * name   ;

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

	name  = ( const char * )pass->pw_name ;
	entry = ( const char ** )grp->gr_mem ;

	while( *entry != NULL ){
		if( strcmp( *entry,name ) == 0 ){
			st = 1 ;
			break ;
		}else{
			entry++ ;
		}
	}

	return st ;
}

static void printOUtPut( const char * e )
{
#if DEBUG
	printf( "%s",e ) ;
#else
	if( e ){;}
#endif
}

static void logStage( int fd,const char * msg )
{
	const char * e = "\n-------------------------------------------\n" ;
	size_t s = strlen( e ) ;
	printf( "%s\n",msg ) ;
	write( fd,e,s ) ;
	write( fd,msg,strlen( msg ) ) ;
	write( fd,e,s ) ;
}

static int itIsSafeToUpdate( const char * e )
{
	const char * error1 = "The following packages have unmet dependencies" ;
	const char * error2 = "E: Error, pkgProblemResolver::Resolve generated breaks, this may be caused by held packages." ;
	const char * error3 = "The following packages have been kept back" ;

	const char * success = "\nThe following packages will be" ;

	const char * noUpdates = "0 upgraded, 0 newly installed, 0 removed and 0 not upgraded." ;
	
	printOUtPut( e ) ;

	if( e == NULL){
		return 1 ;
	}
	if( strstr( e,error1 ) != NULL ){
		return 1 ;
	}
	if( strstr( e,error2 ) != NULL ){
		return 1 ;
	}
	if( strstr( e,error3 ) != NULL ){
		return 1 ;
	}
	if( strstr( e,success ) != NULL ){
		return 0 ;
	}
	if( strstr( e,noUpdates ) != NULL ){
		return 2 ;
	}
	
	return 1 ;
}

static void setDefaultLanguageToEnglish( void )
{
	setenv( "LANG","en_US.UTF-8",1 ) ;
	setenv( "LANGUAGE","en_US.UTF-8",1 ) ;
}

static void printProcessOUtPut( process_t p,int fd )
{
	char * e ;
	int z ;
	while( 1 ){
		e = NULL ;
		z = ProcessGetOutPut( p,&e,STDOUT ) ;
		if( e ){
			write( fd,e,z ) ;
			
			#if DEBUG
				printf( "%s",e ) ;
			#endif
				
			free( e ) ;
		}else{
			break ;
		}
	}
}

static int refreshPackageList( int fd )
{
	int r ;
	process_t p ;
	
	logStage( fd,"entering refreshPackageList" ) ;
	
	if( userHasPermission() ){
		p = Process( "/usr/bin/apt-get" ) ;
	
		ProcessSetArgumentList( p,"update",ENDLIST ) ;
		ProcessSetOptionUser( p,0 ) ;
		ProcessStart( p ) ;
	
		printProcessOUtPut( p,fd ) ;
	
		r = ProcessExitStatus( p ) ;
		ProcessDelete( &p ) ;	
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
	process_t p = Process( "/bin/pidof" ) ;	
	ProcessSetArgumentList( p,"/usr/sbin/synaptic",ENDLIST ) ;
	ProcessSetOptionUser( p,uid ) ;
	ProcessStart( p ) ;
	r = ProcessExitStatus( p ) ;
	ProcessDelete( &p ) ;
	
	if( r == 1 ){
		p = Process( "/bin/pidof" ) ;	
		ProcessSetArgumentList( p,"/usr/bin/apt-get",ENDLIST ) ;
		ProcessSetOptionUser( p,uid ) ;
		ProcessStart( p ) ;
		r = ProcessExitStatus( p ) ;
		ProcessDelete( &p ) ;
	}
	
	return r == 0 ;
}

char * getProcessOutPut( process_t p,int fd )
{
	char * e = NULL ;
	char * buffer = NULL ;
	size_t buffer_size = 0 ;
	size_t output_size = 0 ;
	
	while( 1 ){
		output_size = ProcessGetOutPut( p,&e,STDOUT ) ;
		if( output_size > 0 ){
			
			write( fd,e,output_size ) ;
			
			buffer = realloc( buffer,buffer_size + output_size + 1 ) ;
			
			strcpy( buffer + buffer_size,e ) ;
			
			buffer_size += output_size ;
			#if DEBUG
				printf( "%s",e ) ;
			#endif
			free( e ) ;
		}else{
			break ;
		}
	}
	
	return buffer ;
}

static int autoUpdate( int fd )
{
	process_t p ;
	int r ;
	char * buffer ;
	
	logStage( fd,"entering autoUpdate" ) ;
	
	if( !userHasPermission() ){
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

		r = refreshPackageList( fd ) ;

		if( r == 0 ){
			/*
			 * check if its safe to update
			 */
			p = Process( "/usr/bin/apt-get" ) ;
			ProcessSetArgumentList( p,"dist-upgrade","--simulate",ENDLIST ) ;
			ProcessSetOptionUser( p,0 ) ;
			ProcessStart( p ) ;
			
			buffer = getProcessOutPut( p,fd ) ;

			ProcessExitStatus( p ) ;
			ProcessDelete( &p ) ;

			if( buffer ){
				r = itIsSafeToUpdate( buffer ) ;
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
				
				p = Process( "/usr/bin/apt-get" ) ;
				ProcessSetArgumentList( p,"dist-upgrade","--assume-yes",ENDLIST ) ;
				ProcessSetOptionUser( p,0 ) ;
				ProcessStart( p ) ;
				
				printProcessOUtPut( p,fd ) ;
				
				r = ProcessExitStatus( p ) ;
				
				logStage( fd,"done running apt-get dist-upgrade --assume-yes" ) ;
				
				if( r != 0 ){
					printf( "error: failed to run dist-upgrade --assume-yes\n" ) ;
				}
				
				ProcessDelete( &p ) ;
				
				logStage( fd,"running apt-get clean" ) ;
				
				/*
				 * clear cache 
				 */
				p = Process( "/usr/bin/apt-get" ) ;
				ProcessSetArgumentList( p,"clean",ENDLIST ) ;
				ProcessSetOptionUser( p,0 ) ;
				ProcessStart( p ) ;
				ProcessExitStatus( p ) ;
				ProcessDelete( &p ) ;
				
				logStage( fd,"done running apt-get clean" ) ;
				
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

static int downloadPackages( int fd )
{
	process_t p ;
	
	int r ;
	
	logStage( fd,"entering downloadPackages" ) ;
	
	if( userHasPermission() ){
		r = refreshPackageList( fd ) ;
		if( r == 0 ){
			p = Process( "/usr/bin/apt-get" ) ;
			ProcessSetArgumentList( p,"dist-upgrade","--download-only","--assume-yes",ENDLIST ) ;
			ProcessSetOptionUser( p,0 ) ;
			ProcessStart( p ) ;
			printProcessOUtPut( p,fd ) ;
			r = ProcessExitStatus( p ) ;
			ProcessDelete( &p ) ;
		}
	}else{
		printf( "error: insufficent privileges to perform this operation\n" ) ;
		r = 1 ;
	}
	
	logStage( fd,"leaving downloadPackages" ) ;
	
	return r ;
}

static int startSynaptic( const char * e )
{
	int r ;
	process_t p ;
	
	if( userHasPermission() ){
		p = Process( "/usr/sbin/synaptic" ) ;
		ProcessSetOptionUser( p,0 ) ;
		
		if( e != NULL ){
			ProcessSetArgumentList( p,e,ENDLIST ) ;
		}
	}else{
		p = Process( kdesu ) ;
		ProcessSetOptionUser( p,getuid() ) ;
		
		if( e != NULL ){
			ProcessSetArgumentList( p,"/usr/sbin/synaptic",e,ENDLIST ) ;
		}else{
			ProcessSetArgumentList( p,"/usr/sbin/synaptic",ENDLIST ) ;
		}
	}
		
	ProcessStart( p ) ;
	r = ProcessExitStatus( p ) ;
	ProcessDelete( &p ) ;
	
	return r ;
}

static int printOptions( void )
{
	const char * options = "\
version  : 1.0.0\n\
copyright: 2013 Ink Francis,mhogomchungu@gmail.com\n\
license  : GPLv2+\n\
\n\
argument list:\n\
	--auto-update		calls \"apt-get update\" followed by \"apt-get dist-upgrade\"\n\
	--download-packages	calls \"apt-get update\" followed by \"apt-get --dist-upgrade --download-only --assume-yes\"\n\
	--start-synaptic	calls \"kdesu /usr/sbin/synaptic\"\n\
	--start-synaptic --update-at-startup	calls \"kdesu /usr/sbin/synaptic --update-at-startup\"\n" ;
	
	printf( "%s",options ) ;
	return 0 ;
}

int main( int argc,char * argv[] )
{
	const char * e ;
	const char * f ;
	
	int fd ;
	int st ;
	
	char logPath[ 1024 ] ;
	
	struct passwd * pass = getpwuid( getuid() ) ;
	
	if( pass == NULL ){
		return 10 ;
	}
	
	snprintf( logPath,1024,"/home/%s/.config/qt-update-notifier/backEnd.log",pass->pw_name ) ;
		
	if( argc < 2 ){
		
		return printOptions() ;
	}
		
	e = argv[ 1 ] ;

	#define x( z ) strcmp( e,z ) == 0
	if( x( "--help" ) || x( "-help" ) || x( "-h" ) || x( "-version" ) || x( "--version" ) || x( "-v" ) ){
		return printOptions() ;
	}else{
		if( strcmp( e,"--start-synaptic" ) == 0 ){
			if( argc == 3 ){
				f = argv[ 2 ] ;
				if( strcmp( f,"--update-at-startup" ) == 0 ){
					return startSynaptic( f ) ;
				}else{
					printf( "error: unrecognized or invalid synaptic option\n" ) ;
					return 1 ;
				}
			}else{
				return startSynaptic( NULL ) ;
			}
		}
		
		if( seteuid( getuid() ) == -1 ){
			return 12 ;
		}
		
		fd = open( logPath,O_CREAT|O_TRUNC|O_WRONLY,S_IRUSR|S_IWUSR ) ;
		
		if( seteuid( 0 ) == -1 ){
			return 12 ;
		}
		
		if( fd == -1 ){
			return 11 ;
		}
		
		fchmod( fd,S_IRUSR|S_IWUSR|S_IRWXG|S_IRGRP|S_IROTH|S_IWOTH ) ;
		
		if( strcmp( e,"--auto-update" ) == 0 ){
			st = autoUpdate( fd ) ;
		}else if( strcmp( e,"--download-packages" ) == 0 ){
			st = downloadPackages( fd ) ;
		}else{
			printf( "error: unrecognized or invalid option\n" ) ;
			st = 1 ;
		}
		
		close( fd ) ;
		
		return st ;
	}
}
