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

#ifndef STATUSICON_H
#define STATUSICON_H

#include <QString>
#include <QSystemTrayIcon>
#include <QAction>
#include <QCoreApplication>
#include <QDesktopServices>
#include <QUrl>
#include <QPixmap>
#include <QPainter>
#include <QMenu>

#include "tray_application_type.h"

enum class ItemCategory {
        ApplicationStatus = 1,
        Communications = 2,
        SystemServices = 3,
        Hardware = 4,
        Reserved = 129
};
enum class ItemStatus{
        Passive = 1,
        Active = 2,
        NeedsAttention = 3
};

class statusicon : public QObject
{
	Q_OBJECT
public:
        statusicon( QObject * parent ) ;
	virtual ~statusicon() ;
	void setAttentionIcon( const QString& name ) ;
	void setCategory( const ItemCategory category ) ;
	void setIconByName( const QString& name ) ;
	void setIcon( const QString& name ) ;
	void setAttentionIconByName( const QString& name ) ;
	void setStandardActionsEnabled( bool ) ;
	void setIcon( const QString& name,int count ) ;
	void setOverlayIcon( const QString& name ) ;
        void setStatus( const ItemStatus status ) ;
	void setToolTip( const QString& iconName,const QString& title,const QString& subTitle ) ;
	void addAction( QAction * ) ;
	QAction * getAction( const QString& title = QString() ) ;
	QString iconName( void ) ;
	QString toolTipTitle( void ) ;
	QWidget * widget( void ) ;
	QObject * statusQObject( void ) ;
	QList< QAction * > getMenuActions( void ) ;
	void addQuitAction( void ) ;
private slots:
	void quit( void ) ;
private:
        QMenu m_menu ;
	QSystemTrayIcon * m_trayIcon ;
};

#endif // STATUSICON_H
