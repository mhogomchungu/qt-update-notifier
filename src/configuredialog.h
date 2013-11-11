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

#ifndef CONFIGUREDIALOG_H
#define CONFIGUREDIALOG_H

#include <QDialog>
#include <QString>
#include <QFile>
#include <QDir>
#include <QStringList>
#include <QCloseEvent>
#include <QDebug>
#include <QMessageBox>
#include <QFileDialog>
#include <QDir>
#include <QEvent>
#include <QKeyEvent>

#include "qt-update-install-path.h"

namespace Ui {
class configureDialog;
}

class configureDialog : public QDialog
{
	Q_OBJECT

public:
	configureDialog( QWidget * parent = 0 ) ;
	~configureDialog() ;
	void showUI() ;
signals:
	void toggleAutoStart( bool ) ;
	void setUpdateInterval( int ) ;
	void configOptionsChanged( void ) ;
	void localizationLanguage( QString ) ;
	void autoReshreshSynaptic( bool ) ;
private slots:
	void closeUI( void ) ;
	void autoStartToggled( bool ) ;
	void delayTimeChanged( int ) ;
	void setupLanguageList( void ) ;
	void labelDays( int ) ;
	void labelMinutes( int ) ;
	void labelHours( int ) ;
	void autoRefreshSynaptic_1( bool ) ;
private:
	void setDelayTimeAtLogIn( void ) ;
	void setIntervalBetweenUpdateChecks( void ) ;
	bool eventFilter( QObject * watched,QEvent * event ) ;
	void closeEvent( QCloseEvent * ) ;
	Ui::configureDialog * m_ui ;
};

#endif // CONFIGUREDIALOG_H
