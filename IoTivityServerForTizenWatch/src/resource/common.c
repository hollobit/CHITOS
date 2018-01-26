/*
 * common.c
 *
 *  Created on: Oct 29, 2017
 *      Author: hjshin
 */


#include "common.h"

#include "ocstack.h"
#include "logger.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>

#define TAG "sample-common"

const char *getResult(OCStackResult result) {
    switch (result) {
    case OC_STACK_OK:
        return "OC_STACK_OK";
    case OC_STACK_RESOURCE_CREATED:
        return "OC_STACK_RESOURCE_CREATED";
    case OC_STACK_RESOURCE_DELETED:
        return "OC_STACK_RESOURCE_DELETED";
    case OC_STACK_INVALID_URI:
        return "OC_STACK_INVALID_URI";
    case OC_STACK_INVALID_QUERY:
        return "OC_STACK_INVALID_QUERY";
    case OC_STACK_INVALID_IP:
        return "OC_STACK_INVALID_IP";
    case OC_STACK_INVALID_PORT:
        return "OC_STACK_INVALID_PORT";
    case OC_STACK_INVALID_CALLBACK:
        return "OC_STACK_INVALID_CALLBACK";
    case OC_STACK_INVALID_METHOD:
        return "OC_STACK_INVALID_METHOD";
    case OC_STACK_NO_MEMORY:
        return "OC_STACK_NO_MEMORY";
    case OC_STACK_COMM_ERROR:
        return "OC_STACK_COMM_ERROR";
    case OC_STACK_INVALID_PARAM:
        return "OC_STACK_INVALID_PARAM";
    case OC_STACK_NOTIMPL:
        return "OC_STACK_NOTIMPL";
    case OC_STACK_NO_RESOURCE:
        return "OC_STACK_NO_RESOURCE";
    case OC_STACK_RESOURCE_ERROR:
        return "OC_STACK_RESOURCE_ERROR";
    case OC_STACK_SLOW_RESOURCE:
        return "OC_STACK_SLOW_RESOURCE";
    case OC_STACK_NO_OBSERVERS:
        return "OC_STACK_NO_OBSERVERS";
    case OC_STACK_UNAUTHORIZED_REQ:
        return "OC_STACK_UNAUTHORIZED_REQ";
    #ifdef WITH_PRESENCE
    case OC_STACK_PRESENCE_STOPPED:
        return "OC_STACK_PRESENCE_STOPPED";
    #endif
    case OC_STACK_ERROR:
        return "OC_STACK_ERROR";
    default:
        return "UNKNOWN";
    }
}

void StripNewLineChar(char* str) {
    int i = 0;
    if (str)
    {
        while( str[i])
        {
            if (str[i] == '\n')
                str[i] = '\0';
            i++;
        }
    }
}

time_t _user_set_time = 0;

void getCurrentTime(char * buf) {
    time_t now;
    time(&now);
    now -= _user_set_time;
    struct tm* tm_info;
    tm_info = localtime(&now);
    strftime(buf, 25, "%Y-%m-%dT%H:%M:%S%z", tm_info);
}

void _getCurrentTime(char * buf) {
    time_t now;
    time(&now);
    struct tm* tm_info;
    tm_info = localtime(&now);
    strftime(buf, 9, "%H:%M:%S", tm_info);
}


void setUserTime(char * buf) {
    time_t now;
    time(&now);

    struct tm _user_time = {0};
    int year, month, day, hour, min, sec, timezone = 0;
    sscanf(buf, "%04d-%02d-%02dT%02d:%02d:%02d+%04d",
        &year, &month, &day, &hour, &min, &sec, &timezone);
    _user_time.tm_year = year - 1900;
    _user_time.tm_mon = month -1;
    _user_time.tm_mday = day;
    _user_time.tm_hour = hour;
    _user_time.tm_min = min;
    _user_time.tm_sec = sec;

    printf("%04d-%02d-%02dT%02d:%02d:%02d+%04d\n",
        year, month, day, hour, min, sec, timezone);

    time_t user_time = mktime(&_user_time);

    _user_set_time = now - user_time;
}

int _hrm_value = 0;
int _last_hrm_value = 0;

void setLastHrmValue (int v) {
	_last_hrm_value = v;
}

void setHrmValue(int v) {
	_hrm_value = v;
}

int getLastHrmValue() {
	return _last_hrm_value;
}

int getHrmValue() {
	return _hrm_value;
}
