#include "iotivity_config.h"
#include <functional>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#ifdef HAVE_WINDOWS_H
#include <windows.h>
#endif
#include <signal.h>
#ifdef HAVE_PTHREAD_H
#include <pthread.h>
#endif
#include "ocstack.h"
#include "logger.h"
#include "ocpayload.h"
#include "../common.h"

#define TAG "RESOURCE_CLASS"

class Resource {
private:
    OCResourceHandle mHandle;
    char * mResourceType;
    char * mResourceUri;
    OCRepPayload* (*mMakeResponse)(Resource*);

    OCRepPayload* constructResponse (OCEntityHandlerRequest *ehRequest);
    OCEntityHandlerResult processGetRequest (OCEntityHandlerRequest *ehRequest,
        OCRepPayload **payload);
    OCEntityHandlerResult
    static OCEntityHandlerResult entityHandlerCb (OCEntityHandlerFlag flag,
            OCEntityHandlerRequest *entityHandlerRequest,
            void* /*callbackParam*/);

public:
    Resource ();

    OCStackResult create(char * type, 
        char *uri, 
        OCRepPayload* (*makeResponse)(Resource*) );
};