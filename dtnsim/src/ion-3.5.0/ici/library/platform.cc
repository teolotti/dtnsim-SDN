/*
	platform.c:	platform-dependent implementation of common
			functions, to simplify porting.
									*/
/*	Copyright (c) 1997, California Institute of Technology.		*/
/*	ALL RIGHTS RESERVED. U.S. Government Sponsorship		*/
/*	acknowledged.							*/
/*									*/
/*	Author: Scott Burleigh, Jet Propulsion Laboratory		*/
/*									*/
/*	Scalar/SDNV conversion functions written by			*/
/*	Ioannis Alexiadis, Democritus University of Thrace, 2011.	*/
/*									*/
#include "platform.h"

void	_postErrmsg(const char *fileName, int lineNbr, const char *text,
		const char *arg)
{

}

void	_putErrmsg(const char *fileName, int lineNbr, const char *text,
		const char *arg)
{

}

void	_postSysErrmsg(const char *fileName, int lineNbr, const char *text,
		const char *arg)
{

}

void	_putSysErrmsg(const char *fileName, int lineNbr, const char *text,
		const char *arg)
{

}

int	_iEnd(const char *fileName, int lineNbr, const char *arg)
{

}

void	copyScalar(Scalar *to, Scalar *from)
{

}

void	loadScalar(Scalar *s, signed int i)
{

}

void	multiplyScalar(Scalar *s, signed int i)
{

}

void	divideScalar(Scalar *s, signed int i)
{

}

void	subtractFromScalar(Scalar *s, Scalar *decrement)
{

}

int	scalarIsValid(Scalar *s)
{

}

void	increaseScalar(Scalar *s, signed int i)
{

}

char	*uToa(unsigned int arg)
{

}

int	_isprintf(char *buffer, int bufSize, char *format, ...)
{

}

void	getCurrentTime(struct timeval *tvp)
{

}
