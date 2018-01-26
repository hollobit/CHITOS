#include "iotivity_config.h"
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
#include "clock.h"
#include "../common.h"

//-----------------------------------------------------------------------------
// Defines
//-----------------------------------------------------------------------------

#define TAG "SERVER-CLOCK"

//-----------------------------------------------------------------------------
// Typedefs
//-----------------------------------------------------------------------------

/* Structure to represent a resource */
typedef struct CLOCKRESOURCE{
    OCResourceHandle handle;
} ClockResource;

//-----------------------------------------------------------------------------
// Variables
//-----------------------------------------------------------------------------

static ClockResource Clock;

char *gClockResourceType= (char *)"oic.r.time.observed";
char *gClockResourceUri= (char *)"/ClockResURI";
char _clockid[255] = {0x00, };
double _countdown = 0.0f;

//-----------------------------------------------------------------------------
// Function prototype
//-----------------------------------------------------------------------------

OCRepPayload* getClockPayload(const char* uri);

/* This method converts the payload to JSON format */
OCRepPayload* constructClockResponse (OCEntityHandlerRequest *ehRequest);

/* Following methods process the GET
 * requests
 */
OCEntityHandlerResult ProcessClockGetRequest (OCEntityHandlerRequest *ehRequest,
                                         OCRepPayload **payload);

int createClockResourceEx (char *uri, ClockResource *clockResource);       

//-----------------------------------------------------------------------------
// Callback functions
//-----------------------------------------------------------------------------

/* Entity Handler callback functions */
OCEntityHandlerResult
ClockOCEntityHandlerCb (OCEntityHandlerFlag flag,
        OCEntityHandlerRequest *entityHandlerRequest);

//-----------------------------------------------------------------------------
// Function Implementations
//-----------------------------------------------------------------------------
OCRepPayload* getClockPayload(const char* uri)
{    
    OCRepPayload* payload = OCRepPayloadCreate();
    if(!payload)
    {
        OIC_LOG(ERROR, TAG, PCF("Failed to allocate Payload"));
        return nullptr;
    }
    size_t dimensions[MAX_REP_ARRAY_DEPTH] = { 0 };

    dimensions[0] = 1;
    char * rtStr[] = {"oic.r.time.observed"};
    OCRepPayloadSetStringArray(payload, "rt", (const char **)rtStr, dimensions);
    OCRepPayloadSetPropString(payload, "id", _clockid);
    char buf[25] = "";
    getCurrentTime(buf);
    OCRepPayloadSetPropString(payload, "datetime", buf);
    OCRepPayloadSetPropDouble(payload, "countdown", _countdown);

    return payload;
}

OCRepPayload* constructClockResponse (OCEntityHandlerRequest *ehRequest)
{
    if(ehRequest->payload && ehRequest->payload->type != PAYLOAD_TYPE_REPRESENTATION)
    {
        OIC_LOG(ERROR, TAG, PCF("Incoming payload not a representation"));
        return nullptr;
    }

    return getClockPayload(gClockResourceUri);
}

OCEntityHandlerResult ProcessClockGetRequest (OCEntityHandlerRequest *ehRequest,
    OCRepPayload **payload)
{
    OCEntityHandlerResult ehResult;

    OCRepPayload *getResp = constructClockResponse(ehRequest);

    if(getResp)
    {
        *payload = getResp;
        ehResult = OC_EH_OK;
    }
    else
    {
        ehResult = OC_EH_ERROR;
    }

    return ehResult;
}

OCEntityHandlerResult ProcessClockPostRequest (OCEntityHandlerRequest *ehRequest,
    OCEntityHandlerResponse *response, OCRepPayload **payload)
{
    OCEntityHandlerResult ehResult = OC_EH_ERROR;

    OCRepPayload* _payload = (OCRepPayload*)(ehRequest->payload);
    if (!_payload)
    {
        ehResult = OC_EH_ERROR;
    } else {

        printf("POST DATA CHECK START!\n");
        char * dateTimeStr = (char*)malloc(25);
        memset(dateTimeStr, 0x00, 25);
        if(OCRepPayloadGetPropString(_payload, "datetime", &dateTimeStr))
        {
            printf("POST DATETIME[%s]\n", dateTimeStr);
            setUserTime(dateTimeStr);
        }
        free(dateTimeStr);

        if(OCRepPayloadGetPropDouble(_payload, "countdown", &_countdown))
        {
            printf("POST COUNTDOWN[%f]\n", _countdown);
        }
        printf("POST DATA CHECK END!\n");
        
        OCRepPayload *postResp = constructClockResponse(ehRequest);

        if(postResp)
        {
            *payload = postResp;
            ehResult = OC_EH_OK;
        }
        else
        {
            ehResult = OC_EH_ERROR;
        }
    }

    return ehResult;
}


OCEntityHandlerResult
ClockOCEntityHandlerCb (OCEntityHandlerFlag flag,
        OCEntityHandlerRequest *entityHandlerRequest,
        void* /*callbackParam*/)
{
    OIC_LOG_V (INFO, TAG, "Inside entity handler - flags: 0x%x", flag);

    OCEntityHandlerResult ehResult = OC_EH_ERROR;
    OCEntityHandlerResponse response = { 0, 0, OC_EH_ERROR, 0, 0, { },{ 0 }, false };
    // Validate pointer
    if (!entityHandlerRequest)
    {
        OIC_LOG (ERROR, TAG, "Invalid request pointer");
        return OC_EH_ERROR;
    }

    OCRepPayload* payload = nullptr;

    if (flag & OC_REQUEST_FLAG)
    {
        OIC_LOG (INFO, TAG, "Flag includes OC_REQUEST_FLAG");
        if (entityHandlerRequest)
        {
            if (OC_REST_GET == entityHandlerRequest->method)
            {
                OIC_LOG (INFO, TAG, "Received OC_REST_GET from client");
                ehResult = ProcessClockGetRequest (entityHandlerRequest, &payload);
            }
            else if (OC_REST_POST == entityHandlerRequest->method)
            {
                OIC_LOG (INFO, TAG, "Received OC_REST_POST from client");
                ehResult = ProcessClockPostRequest (entityHandlerRequest, &response, &payload);
            }
            else
            {
                OIC_LOG_V (INFO, TAG, "Received unsupported method %d from client, GET[%d] POST[%d]",
                        entityHandlerRequest->method, OC_REST_GET, OC_REST_POST);
                ehResult = OC_EH_ERROR;
            }

            if (ehResult == OC_EH_OK && ehResult != OC_EH_FORBIDDEN)
            {
                // Format the response.  Note this requires some info about the request
                response.requestHandle = entityHandlerRequest->requestHandle;
                response.ehResult = ehResult;
                response.payload = reinterpret_cast<OCPayload*>(payload);
                response.numSendVendorSpecificHeaderOptions = 0;
                memset(response.sendVendorSpecificHeaderOptions, 0, sizeof response.sendVendorSpecificHeaderOptions);
                memset(response.resourceUri, 0, sizeof(response.resourceUri));
                // Indicate that response is NOT in a persistent buffer
                response.persistentBufferFlag = 0;

                // Send the response
                if (OCDoResponse(&response) != OC_STACK_OK)
                {
                    OIC_LOG(ERROR, TAG, "Error sending response");
                    ehResult = OC_EH_ERROR;
                }
            }
        }
    }

    OCRepPayloadDestroy(payload);
    return ehResult;
}

int createClockResource (char * id) {
    memset (_clockid, 0x00, 255);
    sprintf(_clockid, "%s", id);
    createClockResourceEx(gClockResourceUri, &Clock);
    return 0;
}

int createClockResourceEx (char *uri, ClockResource *clockResource)
{
    if (!uri)
    {
        OIC_LOG(ERROR, TAG, "Resource URI cannot be NULL");
        return -1;
    }

    OCStackResult res = OCCreateResource(&(clockResource->handle),
            gClockResourceType,
            OC_RSRVD_INTERFACE_DEFAULT,
            gClockResourceUri,
            ClockOCEntityHandlerCb,
            NULL,
            OC_DISCOVERABLE | OC_OBSERVABLE 
#if IS_SECURE_MODE
            | OC_SECURE
#endif
        );
    OIC_LOG_V(INFO, TAG, "Created Clock resource with result: %s", getResult(res));

    return 0;
}