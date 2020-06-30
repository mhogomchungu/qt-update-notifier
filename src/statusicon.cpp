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

#include <QProcess>
#include <iostream>
#include <QTimer>

static QPixmap _icon( const QString& name,int count )
{
	QIcon icon( ":/" + name ) ;
	QPixmap pixmap = icon.pixmap( QSize( 152,152 ),QIcon::Normal,QIcon::On ) ;
	int size = static_cast< int >( pixmap.height() * 0.01 * 60 ) ; //configurationoptionsdialog::fontSize() ;
	QPainter paint( &pixmap ) ;
	//QFont font( configurationoptionsdialog::fontFamily() ) ;
	QFont font( "Helvetica" ) ;

	QFontMetrics fm( font ) ;
	QString number = QString::number( count ) ;

	paint.setRenderHint( QPainter::SmoothPixmapTransform ) ;
	paint.setRenderHint( QPainter::Antialiasing ) ;

	int width = static_cast< int >( pixmap.width() * 0.8 ) ;

	if( fm.horizontalAdvance( number ) > width ){
		while( fm.horizontalAdvance( number ) > width && size > 0 ){
			size = size - 1 ;
			font.setPointSize( size ) ;
		}
	}

	font.setPixelSize( size ) ;
	font.setBold( true ) ;
	paint.setFont( font ) ;
	paint.setPen( QColor( "black" ) ) ;
	paint.drawText( pixmap.rect(),Qt::AlignVCenter | Qt::AlignHCenter,number ) ;
	paint.end() ;
	return pixmap ;
}

void statusicon::setIconClickedActions( const statusicon::clickActions& actions )
{
	m_clickActions = actions ;
}

#if KF5

statusicon::statusicon() : m_menu( new QMenu() )
{
	m_menu->clear() ;

	KStatusNotifierItem::setContextMenu( m_menu ) ;
	KStatusNotifierItem::setStandardActionsEnabled( false ) ;

	connect( this,SIGNAL( activateRequested( bool,const QPoint& ) ),
		 this,SLOT( activateRequested( bool,const QPoint& ) ) ) ;
}

statusicon::~statusicon()
{
}

void statusicon::setAttentionIcon( const QString& name )
{
	KStatusNotifierItem::setAttentionIconByPixmap( QIcon( ":/" + name ) ) ;
}

void statusicon::setCategory( const ItemCategory category )
{
	KStatusNotifierItem::setCategory( KStatusNotifierItem::ItemCategory( category ) ) ;
}

void statusicon::setIcon( const QString& name )
{
	KStatusNotifierItem::setIconByPixmap( QIcon( ":/" + name ) ) ;
	statusicon::setAttentionIcon( name ) ;
}

void statusicon::setIcon( const QString& name,int count )
{
	QPixmap pixmap = _icon( name,count ) ;
	KStatusNotifierItem::setIconByPixmap( pixmap ) ;
	KStatusNotifierItem::setAttentionIconByPixmap( pixmap ) ;
}

void statusicon::setOverlayIcon( const QString& name )
{
	KStatusNotifierItem::setOverlayIconByPixmap( QIcon( ":/" + name ) ) ;
}

void statusicon::setStatus( const ItemStatus status )
{
	KStatusNotifierItem::setStatus( KStatusNotifierItem::ItemStatus( status ) ) ;
}

void statusicon::setToolTip( const QString& iconName,const QString& title,const QString& subTitle )
{
	KStatusNotifierItem::setToolTip( QString(),title,subTitle ) ;
	KStatusNotifierItem::setToolTipIconByPixmap( QIcon( ":/" + iconName ) ) ;
}

void statusicon::setIconByName( const QString& name )
{
	this->setIcon( name ) ;
}

void statusicon::setStandardActionsEnabled( bool b )
{
	KStatusNotifierItem::setStandardActionsEnabled( b ) ;
}

void statusicon::addAction( QAction * ac )
{
	ac->setParent( m_menu ) ;
	m_menu->addAction( ac ) ;
}

QAction * statusicon::getAction( const QString& title )
{
	auto ac = new QAction( m_menu ) ;
	ac->setText( title ) ;
	m_menu->addAction( ac ) ;
	return ac ;
}

void statusicon::newEmailNotify()
{
	QByteArray r( "qCheckGMail" ) ;
	KNotification::event( "qCheckGMail-NewMail","",QPixmap(),0,0,r ) ;
}

bool statusicon::enableDebug()
{
	return QCoreApplication::arguments().contains( "-d" ) ;
}

void statusicon::quit()
{
	QCoreApplication::exit() ;
}

void statusicon::setAttentionIconByName( const QString& name )
{
	KStatusNotifierItem::setAttentionIconByName( name ) ;
}

void statusicon::activateRequested( bool x,const QPoint& y )
{
	Q_UNUSED( x ) ;
	Q_UNUSED( y ) ;
	m_clickActions.onLeftClick() ;
}

void statusicon::trayIconClicked( QSystemTrayIcon::ActivationReason reason )
{
	Q_UNUSED( reason ) ;
}

void statusicon::addQuitAction()
{
	m_menu->addAction( [ this ](){

		auto ac = new QAction( m_menu ) ;

		ac->setText( tr( "Quit" ) ) ;
                connect( ac,SIGNAL( triggered() ),this,SLOT( quit() ) ) ;

                return ac ;
        }() ) ;
}

QString statusicon::toolTipTitle()
{
	return KStatusNotifierItem::toolTipTitle() ;
}

QList< QAction * > statusicon::getMenuActions()
{
	return m_menu->actions() ;
}

#else

statusicon::statusicon()
{
	connect( &m_trayIcon,SIGNAL( activated( QSystemTrayIcon::ActivationReason ) ),
		this,SLOT( trayIconClicked(QSystemTrayIcon::ActivationReason ) ) ) ;

	m_trayIcon.setContextMenu( &m_menu ) ;
}

statusicon::~statusicon()
{
}

QList< QAction * > statusicon::getMenuActions()
{
	return m_menu.actions() ;
}

QString statusicon::toolTipTitle()
{
	return QString() ;
}

void statusicon::setAttentionIcon( const QString& name )
{
	m_trayIcon.setIcon( QIcon( ":/" + name ) ) ;
}

void statusicon::setCategory( const ItemCategory category )
{
	Q_UNUSED( category )
}

void statusicon::quit()
{
	QCoreApplication::quit() ;
}

void statusicon::setAttentionIconByName( const QString& name )
{
	Q_UNUSED( name )
}

void statusicon::setStandardActionsEnabled( bool b )
{
	Q_UNUSED( b )
}

void statusicon::setIcon( const QString& name )
{
	m_trayIcon.setIcon( QIcon( ":/" + name ) ) ;
}

void statusicon::setIcon( const QString& name,int count )
{
        auto pixmap = _icon( name,count ) ;
	m_trayIcon.setIcon( pixmap ) ;
}

void statusicon::setIconByName( const QString& name )
{
	m_trayIcon.setIcon( QIcon( ":/" + name ) ) ;
}

void statusicon::setOverlayIcon( const QString& name )
{
	Q_UNUSED( name )
}

void statusicon::setStatus( const ItemStatus status )
{
	Q_UNUSED( status )
}

void statusicon::setToolTip( const QString& iconName,const QString& title,const QString& subTitle )
{
	Q_UNUSED( iconName )

	//std::cout << subTitle.toStdString() << std::endl ;

	if( subTitle.startsWith( "<table>" ) ){

		auto a = subTitle ;

		a.replace( "<table><tr><td>","" ) ;
		a.replace( "</td></tr><tr><td><br>","\n" ) ;
		a.replace( "</td></tr></table>","" ) ;
		a.replace( "<table><tr><td><br>","" ) ;
		a.replace( "<table><tr><td><b>","" ) ;
		a.replace( "</b><br></tr></td><tr><td>","\n" ) ;
		a.replace( "</tr></td></table>","" ) ;
		a.replace( "<b>","" ) ;

		m_trayIcon.setToolTip( title + "\n" + a ) ;
	}else{
		m_trayIcon.setToolTip( title + "\n" + subTitle ) ;
	}
}

static void _suspend( int s )
{
	QTimer t ;

	QEventLoop l ;

	QObject::connect( &t,SIGNAL( timeout() ),&l,SLOT( quit() ) ) ;

	t.start( 1000 * s ) ;

	l.exec() ;
}

void statusicon::addQuitAction()
{
	m_menu.addAction( [ this ](){

		auto ac = new QAction( &m_menu ) ;
		ac->setText( tr( "Quit" ) ) ;
                connect( ac,SIGNAL( triggered() ),this,SLOT( quit() ) ) ;

                return ac ;
        }() ) ;

	for( int i = 0 ; i < 10 ; i++ ){

		if( QSystemTrayIcon::isSystemTrayAvailable() ){

			break ;
		}else{
			_suspend( 1 ) ;
		}
	}

	m_trayIcon.show() ;
}

void statusicon::newEmailNotify()
{
}

bool statusicon::enableDebug()
{
	return QCoreApplication::arguments().contains( "-d" ) ;
}

void statusicon::addAction( QAction * ac )
{
	ac->setParent( &m_menu ) ;
	m_menu.addAction( ac ) ;
}

QAction * statusicon::getAction( const QString& title )
{
	auto ac = new QAction( &m_menu ) ;
	ac->setText( title ) ;
	m_menu.addAction( ac ) ;
	return ac ;
}

void statusicon::activateRequested( bool x,const QPoint& y )
{
	Q_UNUSED( x )
	Q_UNUSED( y )
}

void statusicon::trayIconClicked( QSystemTrayIcon::ActivationReason reason )
{
	if( reason == QSystemTrayIcon::Context ){

		m_clickActions.onRightClick() ;

	}else if( reason == QSystemTrayIcon::Trigger ){

		m_clickActions.onLeftClick() ;

	}else if( reason == QSystemTrayIcon::MiddleClick ){

		m_clickActions.onMiddleClick() ;
	}else{
		m_clickActions.onRightClick() ;
	}
}

#endif
