/*
 * FE_error.h
 *
 *  Created on: Jul 8, 2024
 *      Author: j.Girard
 */

#ifndef FE_ERROR_H_
#define FE_ERROR_H_

#define FE_SUCCESS 0
#define FE_ERROR -1

#include <iostream>

class feErrorException_t : public std::exception
{
	char serror[512];
public:
	virtual const char* what() const throw()
		{
			return serror;
		}
public:
	void setMsg (const char *fmt,...);
};

void feError (const char *fmt,...);
void feWarning (const char *fmt,...);
void feInfo (const char *fmt,...);


#endif /* FE_ERROR_H_ */
