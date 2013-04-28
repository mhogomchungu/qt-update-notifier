#include "startsynaptic.h"

startSynaptic::startSynaptic( QObject * parent ) : QObject( parent )
{
}

startSynaptic::~startSynaptic()
{
}

void startSynaptic::start()
{
	QThreadPool::globalInstance()->start( this );
}

void startSynaptic::run()
{
	QProcess exe ;
	exe.start( QString( "kdesu -c /usr/sbin/synaptic" ) ) ;
	exe.waitForFinished( -1 ) ;
}
