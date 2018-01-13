/*
 *
 *  Copyright (c) 2014
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

#include "twitter.h"
#include "ui_twitter.h"

twitter::twitter( QWidget * parent ) : QDialog( parent ),m_ui( new Ui::twitter )
{
	m_ui->setupUi( this ) ;

	this->setFixedSize( this->size() ) ;
	this->setWindowIcon( QIcon( ":/qt-update-notifier.png" ) ) ;

	connect( m_ui->pbClose,SIGNAL( clicked() ),this,SLOT( pbClose() ) ) ;
	connect( &m_timer,SIGNAL( timeout() ),this,SLOT( updateUI() ) ) ;

	this->installEventFilter( this ) ;
}

void twitter::ShowUI( const QString& text )
{
	m_string = text ;

	this->updateUI() ;

	m_timer.start( 1000 ) ;

	this->show() ;
}

bool twitter::eventFilter( QObject * watched,QEvent * event )
{
	if( watched == this ){

		if( event->type() == QEvent::KeyPress ){

			QKeyEvent * keyEvent = static_cast< QKeyEvent* >( event ) ;

			if( keyEvent->key() == Qt::Key_Escape ){

				this->pbClose() ;

				return true ;
			}
		}
	}

	return false ;
}

twitter::~twitter()
{
	delete m_ui ;
}

void twitter::msg( const QString& msg )
{
	m_timer.stop() ;
	m_ui->textEdit->setText( msg ) ;
}

void twitter::translate()
{
	m_ui->retranslateUi( this ) ;
}

void twitter::pbClose()
{
	this->hide() ;
}

void twitter::updateUI()
{
	m_ui->textEdit->setText( m_string ) ;

	m_string += " ..." ;
}

void twitter::closeEvent( QCloseEvent * e )
{
	e->ignore() ;

	this->pbClose() ;
}
