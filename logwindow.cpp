#include "logwindow.h"
#include "ui_logwindow.h"
#include <QDebug>
logWindow::logWindow( QString title,QWidget * parent ) :QWidget( parent ),m_ui( new Ui::logWindow )
{
	m_ui->setupUi( this );
	this->setWindowTitle( title );
	m_ui->textEditLogField->setAlignment( Qt::AlignHCenter );
	m_ui->pbQuit->setFocus();
	this->setFixedSize( this->size() ) ;
	this->setWindowIcon( QIcon( QString( ":/qt-update-notifier.png" ) ) );
	connect( m_ui->pbQuit,SIGNAL( clicked() ),this,SLOT( pbQuit() ) ) ;
	connect( m_ui->pbQuit_2,SIGNAL( clicked() ),this,SLOT( pbQuit() ) ) ;
	connect( m_ui->pbClear,SIGNAL( clicked() ),this,SLOT( pbClearLog() ) ) ;
}

logWindow::~logWindow()
{
	delete m_ui;
}

void logWindow::showLogWindow( QString log )
{
	m_logFile = log ;
	m_logPath = log ;
	m_ui->textEditLogField->setText( utility::readFromFile( log ) );
	m_ui->pbQuit_2->setVisible( false ) ;
	this->show();
}

void logWindow::updateLogWindow()
{
	m_ui->textEditLogField->setText( utility::readFromFile( m_logPath ) );
}

void logWindow::updateLogWindow_1()
{
	m_ui->textEditLogField->setText( utility::readFromFile( m_logPath ) ) ;
}

void logWindow::showAptGetWindow( QString logpath )
{
	m_logPath = logpath ;
	this->updateLogWindow_1();
	m_ui->pbClear->setVisible( false );
	m_ui->pbQuit->setVisible( false ) ;
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
