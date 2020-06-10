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
#ifndef IGNOREPACKAGELIST_H
#define IGNOREPACKAGELIST_H

#include <QDialog>
#include <QCloseEvent>

namespace Ui {
class ignorepackagelist;
}

class ignorepackagelist : public QDialog
{
	Q_OBJECT
public:
	explicit ignorepackagelist( QWidget * parent = nullptr ) ;
	~ignorepackagelist() ;
private:
	void Hide() ;
	Ui::ignorepackagelist * m_ui ;
	bool eventFilter( QObject * watched,QEvent * event ) ;
	void closeEvent( QCloseEvent * ) ;
};

#endif // IGNOREPACKAGELIST_H
