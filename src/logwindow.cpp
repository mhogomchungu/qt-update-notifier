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

#include "settings.h"
#include "logwindow.h"
#include "ui_logwindow.h"
#include <QDebug>
logWindow::logWindow( QString title,QWidget * parent ) :QWidget( parent ),m_ui( new Ui::logWindow )
{
	m_ui->setupUi( this ) ;
	this->setWindowTitle( title ) ;
	m_ui->textEditLogField->setAlignment( Qt::AlignHCenter ) ;
	m_ui->pbQuit->setFocus() ;
	this->setWindowIcon( QIcon( ":/qt-update-notifier.png" ) ) ;
	connect( m_ui->pbQuit,SIGNAL( clicked() ),this,SLOT( pbQuit() ) ) ;
	connect( m_ui->pbQuit_2,SIGNAL( clicked() ),this,SLOT( pbQuit() ) ) ;
	connect( m_ui->pbClear,SIGNAL( clicked() ),this,SLOT( pbClearLog() ) ) ;

	this->installEventFilter( this ) ;
}

bool logWindow::eventFilter( QObject * watched,QEvent * event )
{
	if( watched == this ){

		if( event->type() == QEvent::KeyPress ){

			auto keyEvent = static_cast< QKeyEvent* >( event ) ;

			if( keyEvent->key() == Qt::Key_Escape ){

				this->pbQuit() ;

				return true ;
			}
		}
	}

	return false ;
}

logWindow::~logWindow()
{
	delete m_ui;
}

void logWindow::showLogWindow()
{
	m_logFile = settings::activityLogFilePath() ;
	m_logPath = m_logFile ;
        m_ui->textEditLogField->setText( utility::readFromFile( m_logFile ) ) ;
	m_ui->pbQuit_2->setVisible( false ) ;

	this->window()->setGeometry( settings::logWindowDimensions() ) ;

	m_windowType = windowType::logWindow ;

	this->show() ;
}

void logWindow::showAptGetWindow()
{
	m_logPath = settings::aptGetLogFilePath() ;
	this->updateLogWindow_1() ;
	m_ui->pbClear->setVisible( false ) ;
	m_ui->pbQuit->setVisible( false ) ;

	m_windowType = windowType::aptGetWindow ;

	this->window()->setGeometry( settings::aptGetWindowDimensions() ) ;

	this->show() ;
}

void logWindow::updateLogWindow()
{
        m_ui->textEditLogField->setText( utility::readFromFile( m_logPath ) ) ;
}

void logWindow::updateLogWindow_1()
{
        m_ui->textEditLogField->setText( utility::readFromFile( m_logPath ) ) ;
}

void logWindow::pbClearLog()
{
	m_ui->textEditLogField->clear() ;
	QFile f( m_logFile ) ;
	f.open( QIODevice::WriteOnly | QIODevice::Truncate ) ;
	f.close() ;
}

void logWindow::closeEvent( QCloseEvent * e )
{
	e->ignore() ;
	this->pbQuit() ;
}

void logWindow::pbQuit()
{	
	if( m_windowType == windowType::logWindow ){

		settings::logWindowDimensions( this->window()->geometry() ) ;
	}else{
		settings::aptGetWindowDimensions( this->window()->geometry() ) ;
	}

	this->hide() ;
	this->deleteLater() ;
}
