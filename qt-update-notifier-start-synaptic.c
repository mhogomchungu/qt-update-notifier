
#include<string.h>
#include <grp.h>
#include <pwd.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>

#include "kdesu_path.h"

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
		#if DEBUG
			puts( *entry  ) ;
		#endif	
		if( strcmp( *entry,name ) == 0 ){
			st = 1 ;
			break ;
		}else{
			entry++ ;
		}
	}
	
	return st ;
}

static int startKDESUSynaptic( int st )
{
	seteuid( 0 ) ;
	setuid( getuid() ) ;
#if DEBUG
	printf( "startKDESUSynaptic: getuid=%d,geteuid=%d\n",getuid(),geteuid() );
#endif
	if( st == 0 ){
		return execl( kdesu,kdesu,"/usr/sbin/synaptic",( void * )0 ) ;
	}else{
		return execl( kdesu,kdesu,"/usr/sbin/synaptic","--update-at-startup",( void * )0 ) ;	
	}
}

static int startElevatedSynatic( int st )
{
	seteuid( 0 ) ;
	setuid( 0 ) ;
#if DEBUG
	printf( "startElevatedSynatic: getuid=%d,geteuid=%d\n",getuid(),geteuid() );
#endif
	if( st == 0 ){
		return execl( "/usr/sbin/synaptic","/usr/sbin/synaptic",( void * )0 ) ;
	}else{
		return execl( "/usr/sbin/synaptic","/usr/sbin/synaptic","--update-at-startup",( void * )0 ) ;	
	}
}

static int autoRefreshSynaptic( const char * e )
{
	if( e == NULL ){
		return 0 ;
	}else{
		return strcmp( e,"--update-at-startup" ) == 0 ;
	}
}

int main( int argc,char * argv[] )
{
	const char * e ;
		
	if( argc < 2 ){
		e = NULL ;
	}else{
		e = argv[ 1 ] ;
	}
	
	if( userHasPermission() ){
		if( autoRefreshSynaptic( e ) ){
			return startElevatedSynatic( 1 ) ;
		}else{
			return startElevatedSynatic( 0 ) ;
		}
	}else{
		if( autoRefreshSynaptic( e ) ){
			return startKDESUSynaptic( 1 ) ;
		}else{
			return startKDESUSynaptic( 0 ) ;
		}
	}
}
