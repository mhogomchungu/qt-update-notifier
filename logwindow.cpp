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
