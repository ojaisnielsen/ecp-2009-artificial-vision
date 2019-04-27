/***************************************************************************
 *   Copyright (C) 2007 by Regis Behmo and Radhouene Neji   *
 *   regis.behmo@ecp.fr   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef ECPEXCEPTION_H
#define ECPEXCEPTION_H

#include <iostream>
#include <exception>
using namespace std;
		
class EcpException: public exception
{
	public:
		EcpException( const char* message )
		{
			strcpy( _message, message );
			cout << "ERROR: " << _message << endl;
			
		}
		void readMessage( char* preliminary = "ERROR: " )
		{
			cout << preliminary << _message << endl;
		}
	private:
		char _message[200];
};

#endif
