/*
 *
 *  Copyright (c) 2020
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

#include "ignorepackagelist.h"
#include "ui_ignorepackagelist.h"

#include <QMenu>

#include "tablewidget.h"
#include "settings.h"

ignorepackagelist::ignorepackagelist( QWidget * parent ) :
        QDialog( parent ),m_ui( new Ui::ignorepackagelist )
{
	m_ui->setupUi( this ) ;

	m_ui->tableWidget->verticalHeader()->setSectionResizeMode( QHeaderView::ResizeToContents ) ;

	m_ui->tableWidget->horizontalHeader()->setStretchLastSection( true ) ;

	m_ui->tableWidget->setMouseTracking( true ) ;

	m_ui->tableWidget->setContextMenuPolicy( Qt::CustomContextMenu ) ;

	connect( m_ui->tableWidget,&QTableWidget::currentItemChanged,[]( QTableWidgetItem * c,QTableWidgetItem * p ){

		tablewidget::selectRow( c,p ) ;
	} ) ;

	connect( m_ui->tableWidget,&QTableWidget::customContextMenuRequested,[ this ]( QPoint s ){

		Q_UNUSED( s )

		auto item = m_ui->tableWidget->currentItem() ;

		if( item ){

			QMenu m ;

			m.setFont( this->font() ) ;

			connect( m.addAction( tr( "Delete" ) ),&QAction::triggered,[ this,item ](){

				tablewidget::deleteRow( m_ui->tableWidget,item->row() ) ;
			} ) ;

			m.exec( QCursor::pos() ) ;
		}
	} ) ;

	connect( m_ui->pbAdd,&QPushButton::clicked,[ this ](){

		auto a = m_ui->lineEdit->text() ;

		if( !a.isEmpty() ){

			tablewidget::addRow( m_ui->tableWidget,{ a } ) ;

			m_ui->lineEdit->clear() ;
		}
	} ) ;

	connect( m_ui->pbClose,&QPushButton::clicked,[ this ](){

		this->Hide() ;
	} ) ;

	for( const auto& it : settings::ignorePackageList() ){

		tablewidget::addRow( m_ui->tableWidget,{ it } ) ;
	}
}

ignorepackagelist::~ignorepackagelist()
{
	delete m_ui ;
}

void ignorepackagelist::Hide()
{
	this->hide() ;

	QStringList s ;

	auto table = m_ui->tableWidget ;

	for( int i = 0 ; i < table->rowCount() ; i++ ){

		s.append( table->item( i,0 )->text() ) ;
	}

	settings::ignorePackageList( s ) ;

	this->deleteLater() ;
}

void ignorepackagelist::closeEvent( QCloseEvent * e )
{
	e->ignore() ;
	this->Hide();
}

bool ignorepackagelist::eventFilter( QObject * watched,QEvent * event )
{
	if( watched == this ){

		if( event->type() == QEvent::KeyPress ){

			auto keyEvent = static_cast< QKeyEvent* >( event ) ;

			if( keyEvent->key() == Qt::Key_Escape ){

				this->Hide() ;
				this->deleteLater() ;

				return true ;
			}
		}
	}

	return false ;
}
