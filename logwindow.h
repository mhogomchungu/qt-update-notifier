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
	explicit logWindow( QWidget * parent = 0,QString logFile = QString() );
	~logWindow();
	void showLogWindow( void ) ;
	static void Show( QString ) ;
private slots:
	void pbClearLog( void ) ;
	void pbQuit( void ) ;
private:
	void closeEvent( QCloseEvent * ) ;
	Ui::logWindow * m_ui;
	QString m_logFile ;
};

#endif // LOGWINDOW_H
