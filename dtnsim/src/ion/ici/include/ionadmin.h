/*

	ionadmin.c:	contact list adminstration interface.

									*/
/*	Copyright (c) 2007, California Institute of Technology.		*/
/*	All rights reserved.						*/
/*	Author: Scott Burleigh, Jet Propulsion Laboratory		*/

#ifndef _IONADMIN_H_
#define _IONADMIN_H_

#include "zco.h"
#include "rfx.h"

extern int	processLine(char *line, int lineLength, int *rc);
extern int	runIonadmin(char *cmdFileName);


#endif  /* _IONADMIN_H_ */
