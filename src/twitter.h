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

#ifndef TWITTER_H
#define TWITTER_H

#include <QDialog>
#include <QTimer>
#include <QCloseEvent>
#include <QString>
#include <QKeyEvent>

namespace Ui {
class twitter;
}

class twitter : public QDialog
{
	Q_OBJECT
public:
	explicit twitter( QWidget * parent = 0 ) ;
	void ShowUI( const QString& ) ;
	~twitter();
public slots:
	void msg( const QString& ) ;
private slots:
	void pbClose( void ) ;
	void updateUI( void ) ;
private:
	bool eventFilter( QObject * watched,QEvent * event ) ;
	void closeEvent( QCloseEvent * e ) ;
	QString m_string ;
	QTimer m_timer ;
	Ui::twitter * m_ui ;
};

#endif // TWITTER_H
