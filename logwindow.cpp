#include "logwindow.h"
#include "ui_logwindow.h"

logWindow::logWindow( QWidget * parent,QString logFile ) :QWidget( parent ),m_ui( new Ui::logWindow ),m_logFile( logFile )
{
	m_ui->setupUi( this );
	m_ui->textEditLogField->setAlignment( Qt::AlignHCenter );
	m_ui->pbQuit->setFocus();
	this->setFixedSize( this->size() ) ;
	this->setWindowIcon( QIcon( QString( ":/qt-update-notifier.png" ) ) );
	connect( m_ui->pbQuit,SIGNAL( clicked() ),this,SLOT( pbQuit() ) ) ;
	connect( m_ui->pbClear,SIGNAL( clicked() ),this,SLOT( pbClearLog() ) ) ;
}

logWindow::~logWindow()
{
	delete m_ui;
}

void logWindow::showLogWindow()
{
	QFile f( m_logFile ) ;
	if( f.exists() ){
		f.open( QIODevice::ReadOnly ) ;
		QString x = f.readAll() ;
		f.close();
		m_ui->textEditLogField->setText( x );
	}else{
		m_ui->textEditLogField->close() ;
	}
	this->show();
}

void logWindow::pbClearLog()
{
	m_ui->textEditLogField->clear();
	QFile f( m_logFile ) ;
	f.open( QIODevice::WriteOnly | QIODevice::Truncate ) ;
	f.close();
}

void logWindow::closeEvent( QCloseEvent * e )
{
	e->ignore();
	this->pbQuit();
}

void logWindow::pbQuit()
{
	this->hide();
	this->deleteLater();
}
