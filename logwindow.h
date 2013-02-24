#ifndef LOGWINDOW_H
#define LOGWINDOW_H

#include <QWidget>
#include <QFile>
#include <QString>
#include <QCloseEvent>

namespace Ui {
class logWindow;
}

class logWindow : public QWidget
{
	Q_OBJECT
public:
	explicit logWindow( QString logFile = QString(),QWidget * parent = 0 );
	~logWindow();
	void showLogWindow( void ) ;
private slots:
	void pbClearLog( void ) ;
	void pbQuit( void ) ;
	void updateLogWindow( void ) ;
private:
	QString getLogContents( void ) ;
	void closeEvent( QCloseEvent * ) ;
	Ui::logWindow * m_ui;
	QString m_logFile ;
};

#endif // LOGWINDOW_H
