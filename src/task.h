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

#ifndef TASK_H
#define TASK_H

#include <functional>
#include <QThread>

template< typename T >
class continuation
{
public:
	explicit continuation( std::function< void( void ) > function ) :
		m_function( []( const T& t ){ Q_UNUSED( t ) ; } ),m_start( function )
	{
	}
	void then( std::function< void( const T& ) > function )
	{
		m_function = function ;
		m_start() ;
	}
	void start()
	{
		m_start() ;
	}
	void run( const T& arg )
	{
		m_function( arg ) ;
	}
private:
	std::function< void( const T& ) > m_function ;
	std::function< void( void ) > m_start ;
};

template< typename T >
class thread : public QThread
{
public:
	thread( std::function< T ( void ) > function ) :
		m_function( function ),
		m_continuation( [&](){ this->start() ; } )
	{
		connect( this,SIGNAL( finished() ),this,SLOT( deleteLater() ) ) ;
	}
	continuation<T>& taskContinuation( void )
	{
		return m_continuation ;
	}
private:
	~thread()
	{
		m_continuation.run( m_cargo ) ;
	}
	void run( void )
	{
		m_cargo =  m_function() ;
	}
	std::function< T ( void ) > m_function ;
	continuation<T> m_continuation ;
	T m_cargo ;
};

namespace Task
{
	/*
	 * This API runs two tasks,the first one will be run in a different thread and
	 * the second one will be run on the original thread after the completion of the
	 * first one.
	 *
	 * See example at the end of this header file for a sample use case
	 */
	template< typename T >
	continuation<T>& run( std::function< T ( void ) > function )
	{
		auto t = new thread<T>( function ) ;
		return t->taskContinuation() ;
	}

	void exec( std::function< void( void ) > function ) ;
}

#if 0

auto _a = [](){
	/*
	 * task _a does what task _a does here.
	 *
	 * This function body will run on a different thread
	 */
	return 0 ;
}

auto _b = []( const int& r ){
	/*
	 * task _b does what task _b does here.
	 *
	 * r is a const reference to a value returned by _a
	 *
	 * This function body will run on the original thread
	 */
}

Task::run( _a ).then( _b ) ;

/*
 * if no continuation
 */
Task::exec( _a ) ;

#endif

#endif // TASK_H
