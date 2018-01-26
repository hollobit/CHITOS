/*
 * common.h
 *
 *  Created on: Oct 29, 2017
 *      Author: hjshin
 */

#ifndef RESOURCE_COMMON_H_
#define RESOURCE_COMMON_H_


#include "ocstack.h"

/* Get the result in string format. */
const char *getResult(OCStackResult result);

/* Removes the new line character from a NULL terminated C string. */
void StripNewLineChar(char* str);

void getCurrentTime(char * buf);
void _getCurrentTime(char * buf);
void setUserTime(char * buf);

void setLastHrmValue (int v);
void setHrmValue(int v);
int getLastHrmValue();
int getHrmValue();

#endif /* RESOURCE_COMMON_H_ */
