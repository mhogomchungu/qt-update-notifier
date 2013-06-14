
#include<string.h>
#include <grp.h>
#include <pwd.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>

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

static void printProcessOUtPut( const char * e )
{
#if DEBUG
	printf( "%s",e ) ;
#else
	if( e ){;}
#endif
}

static int itIsSafeToUpdate( const char * e )
{
	const char * error1 = "The following packages have unmet dependencies" ;
	const char * error2 = "E: Error, pkgProblemResolver::Resolve generated breaks, this may be caused by held packages." ;
	const char * error3 = "The following packages have been kept back" ;

	const char * success = "\nThe following packages will be" ;

	const char * noUpdates = "0 upgraded, 0 newly installed, 0 removed and 0 not upgraded." ;
	
	printProcessOUtPut( e ) ;

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
	setenv( "LANG","en_US.UTF-8",1 ) ;
}

static int refreshPackageList( void )
{
	int r ;
	char * e = NULL ;
	
	process_t p = Process( "/usr/bin/apt-get" ) ;
	
	ProcessSetArgumentList( p,"update",ENDLIST ) ;
	ProcessSetOptionUser( p,0 ) ;
	ProcessStart( p ) ;
	
#if DEBUG
	while( 1 ){
		e = NULL ;
		ProcessGetOutPut( p,&e,STDOUT ) ;
		if( e ){
			printProcessOUtPut( e ) ;
			free( e ) ;
			e = NULL  ;
		}else{
			break ;
		}
	}
#endif
	if( e ){
		free( e ) ;
		e = NULL ;
	}
	
	r = ProcessExitStatus( p ) ;
	ProcessDelete( &p ) ;	
	
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

static int autoUpdate( void )
{
	process_t p ;
	char * e = NULL ;
	int r ;
	char * buffer = NULL ;
	size_t buffer_size = 0 ;
	size_t output_size = 0 ;

	if( !userHasPermission() ){
		/*
		 * user is not privileged,abort
		 */
		r = 1 ;
	}else{
		if( aptAndSynapticAreRunning() ){
			return 3 ;
		}
			
		/*
		 * make sure the output we are going to get is in english regardless of user locale
		 */
		setDefaultLanguageToEnglish() ;

		r = refreshPackageList() ;

		if( r == 0 ){
			/*
			 * check if its safe to update
			 */
			p = Process( "/usr/bin/apt-get" ) ;
			ProcessSetArgumentList( p,"dist-upgrade","--simulate",ENDLIST ) ;
			ProcessSetOptionUser( p,0 ) ;
			ProcessStart( p ) ;

			while( 1 ){
				output_size = ProcessGetOutPut( p,&e,STDOUT ) ;
				if( output_size > 0 ){
					buffer = realloc( buffer,buffer_size + output_size + 1 ) ;

					strcpy( buffer + buffer_size,e ) ;

					buffer_size += output_size ;
					printProcessOUtPut( e ) ;
					free( e ) ;
				}else{
					break ;
				}
			}

			ProcessExitStatus( p ) ;
			ProcessDelete( &p ) ;

			if( buffer ){
				r = itIsSafeToUpdate( buffer ) ;
				free( buffer ) ;
			}else{
				printProcessOUtPut( "IT IS NOT SAFE TO UPDATE,apt-get gave no output\n" ) ;
				r = 1 ;
			}

			if( r == 0 ){
				/*
				 * it seem to be safe to update,update
				 */
				printProcessOUtPut( "There are updates\n" ) ;
				
				p = Process( "/usr/bin/apt-get" ) ;
				ProcessSetArgumentList( p,"dist-upgrade","--assume-yes",ENDLIST ) ;
				ProcessSetOptionUser( p,0 ) ;
				ProcessStart( p ) ;
				r = ProcessExitStatus( p ) ;
				ProcessDelete( &p ) ;
				
				/*
				 * clear cache 
				 */
				p = Process( "/usr/bin/apt-get" ) ;
				ProcessSetArgumentList( p,"clean",ENDLIST ) ;
				ProcessSetOptionUser( p,0 ) ;
				ProcessStart( p ) ;
				ProcessExitStatus( p ) ;
				ProcessDelete( &p ) ;
			}else if( r == 2 ){
				printProcessOUtPut( "There are no updates\n" ) ;
			}else{
				printProcessOUtPut( "IT IS NOT SAFE TO UPDATE\n" ) ;
			}
		}
	}
	
	return r ;
}

static int downloadPackages( void )
{
	process_t p ;
	
	int r ;
	if( userHasPermission() ){
		r = refreshPackageList() ;
		if( r == 0 ){
			p = Process( "/usr/bin/apt-get" ) ;
			ProcessSetArgumentList( p,"dist-upgrade","--download-only","--assume-yes",ENDLIST ) ;
			ProcessSetOptionUser( p,0 ) ;
			ProcessStart( p ) ;
			r = ProcessExitStatus( p ) ;
			ProcessDelete( &p ) ;
		}
	}else{
		r = 1 ;
	}
	
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

int main( int argc,char * argv[] )
{
	const char * e ;

	if( argc < 2 ){
		return startSynaptic( NULL ) ;
	}else{
		e = argv[ 1 ] ;
		if( strcmp( e,"--auto-update" ) == 0 ){
			return autoUpdate() ;
		}else if( strcmp( e,"--update-at-startup" ) == 0 ){
			return startSynaptic( e ) ;
		}else if( strcmp( e,"--download-packages" ) == 0 ){
			return downloadPackages() ;
		}else{
			return 1 ;
		}
	}
}
