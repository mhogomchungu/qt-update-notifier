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

#include "statusicon.h"

#include <QDebug>

statusicon::statusicon( QObject * parent ) : QObject( parent )
{
        m_trayIcon = new QSystemTrayIcon( parent ) ;
}

QWidget * statusicon::widget()
{
        return nullptr ;
}

statusicon::~statusicon()
{
}

void statusicon::setAttentionIcon( const QString& name )
{
	m_trayIcon->setIcon( QIcon( ":/" + name ) ) ;
}

void statusicon::setCategory( const ItemCategory category )
{
	Q_UNUSED( category ) ;
}

void statusicon::setIconByName( const QString& name )
{
	m_trayIcon->setIcon( QIcon( ":/" + name ) ) ;
}

void statusicon::setIcon( const QString& name )
{
	this->setIcon( name ) ;
}

void statusicon::quit()
{
	QCoreApplication::quit() ;
}

void statusicon::setAttentionIconByName( const QString& name )
{
	Q_UNUSED( name ) ;
}

void statusicon::setStandardActionsEnabled( bool b )
{
	Q_UNUSED( b ) ;
}

void statusicon::setIcon( const QString& name,int count )
{
        Q_UNUSED( count ) ;

        m_trayIcon->setIcon( QIcon( ":/" + name ) );
}

void statusicon::setOverlayIcon( const QString& name )
{
	Q_UNUSED( name ) ;
}

void statusicon::setStatus( const ItemStatus status )
{
	Q_UNUSED( status ) ;
}

void statusicon::setToolTip( const QString& iconName,const QString& title,const QString& subTitle )
{
	Q_UNUSED( iconName ) ;
	auto e = "<table><tr><td><b>%1</b></td><br></tr><tr><td>%2</td></tr></table>" ;
	m_trayIcon->setToolTip( QString( e ).arg( title,subTitle ) ) ;
}

QList<QAction *> statusicon::getMenuActions()
{
        return m_menu.actions() ;
}

void statusicon::addQuitAction()
{
        auto ac = new QAction( this ) ;
	ac->setText( tr( "Quit" ) ) ;
	connect( ac,SIGNAL( triggered() ),this,SLOT( quit() ) ) ;
        m_menu.addAction( ac ) ;
        m_trayIcon->setContextMenu( &m_menu ) ;
        m_trayIcon->show() ;
}

void statusicon::addAction( QAction * ac )
{
        ac->setParent( this ) ;
        m_menu.addAction( ac ) ;
}

QAction * statusicon::getAction( const QString& title )
{
        auto ac = new QAction( this ) ;
	ac->setText( title ) ;
        m_menu.addAction( ac ) ;
	return ac ;
}

QString statusicon::iconName()
{
	return QString() ;
}

QString statusicon::toolTipTitle()
{
	return QString() ;
}

QObject * statusicon::statusQObject()
{
	return this ;
}
