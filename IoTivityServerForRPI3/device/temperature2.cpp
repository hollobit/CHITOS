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
#include "temperature2.h"
#include "../common.h"

//-----------------------------------------------------------------------------
// Defines
//-----------------------------------------------------------------------------

#define TAG "SERVER-TEMPERATURE-2"

//-----------------------------------------------------------------------------
// Typedefs
//-----------------------------------------------------------------------------

/* Structure to represent a LED resource */
typedef struct TEMPERATURE2RESOURCE{
    OCResourceHandle handle;
} Temperature2Resource;

//-----------------------------------------------------------------------------
// LED Variables
//-----------------------------------------------------------------------------

static Temperature2Resource Temp2;

char *gTemp2ResourceType= (char *)"oic.r.body.temperature";
char *gTemp2ResourceUri= (char *)"/BodyTemperatureLLResURI";

//-----------------------------------------------------------------------------
// Function prototype
//-----------------------------------------------------------------------------

OCRepPayload* getTemp2Payload(const char* uri);

/* This method converts the payload to JSON format */
OCRepPayload* constructTemp2Response (OCEntityHandlerRequest *ehRequest);

/* Following methods process the GET
 * requests
 */
OCEntityHandlerResult ProcessTemp2GetRequest (OCEntityHandlerRequest *ehRequest,
                                         OCRepPayload **payload);

int createTemp2ResourceEx (char *uri, Temperature2Resource *temp2Resource);       

//-----------------------------------------------------------------------------
// Callback functions
//-----------------------------------------------------------------------------

/* Entity Handler callback functions */
OCEntityHandlerResult
Temp2OCEntityHandlerCb (OCEntityHandlerFlag flag,
        OCEntityHandlerRequest *entityHandlerRequest);

//-----------------------------------------------------------------------------
// Function Implementations
//-----------------------------------------------------------------------------
OCRepPayload* getTemp2Payload(const char* uri)
{
    OCRepPayload* payload = OCRepPayloadCreate();
    if(!payload)
    {
        OIC_LOG(ERROR, TAG, PCF("Failed to allocate Payload"));
        return nullptr;
    }
    size_t dimensions[MAX_REP_ARRAY_DEPTH] = { 0 };

    OCRepPayload* href1 = OCRepPayloadCreate();
    OCRepPayloadSetPropString(href1, "href", "/myTemperatureResURI");
    dimensions[0] = 1;
    char * href1RtStr[] = {"oic.r.temperature"};
    OCRepPayloadSetStringArray(href1, "rt", (const char **)href1RtStr, dimensions);
    dimensions[0] = 0;
    char * href1IfStr[] = {};
    OCRepPayloadSetStringArray(href1, "if", (const char **)href1IfStr, dimensions);

    OCRepPayload* href2 = OCRepPayloadCreate();
    OCRepPayloadSetPropString(href2, "href", "/myBodyLocationResURI");
    dimensions[0] = 1;
    char * href2RtStr[] = {"oic.r.body.location"};
    OCRepPayloadSetStringArray(href2, "rt", (const char **)href2RtStr, dimensions);
    dimensions[0] = 0;
    char * href2IfStr[] = {};
    OCRepPayloadSetStringArray(href2, "if", (const char **)href2IfStr, dimensions);

    OCRepPayload* href3 = OCRepPayloadCreate();
    OCRepPayloadSetPropString(href3, "href", "/myClockResURI");
    dimensions[0] = 1;
    char * href3RtStr[] = {"oic.r.clock"};
    OCRepPayloadSetStringArray(href3, "rt", (const char **)href3RtStr, dimensions);
    dimensions[0] = 0;
    char * href3IfStr[] = {};
    OCRepPayloadSetStringArray(href3, "if", (const char **)href3IfStr, dimensions);

    OCRepPayload* href4 = OCRepPayloadCreate();
    OCRepPayloadSetPropString(href4, "href", "/myUserIDResURI");
    dimensions[0] = 1;
    char * href4RtStr[] = {"oic.r.userid"};
    OCRepPayloadSetStringArray(href4, "rt", (const char **)href4RtStr, dimensions);
    dimensions[0] = 0;
    char * href4IfStr[] = {};
    OCRepPayloadSetStringArray(href4, "if", (const char **)href4IfStr, dimensions);

    OCRepPayload * hrefs[] = { href1, href2, href3, href4};
    dimensions[0] = 4;
    OCRepPayloadSetPropObjectArray(payload, "links", (const OCRepPayload **)hrefs, dimensions);

    return payload;
}

OCRepPayload* constructTemp2Response (OCEntityHandlerRequest *ehRequest)
{
    if(ehRequest->payload && ehRequest->payload->type != PAYLOAD_TYPE_REPRESENTATION)
    {
        OIC_LOG(ERROR, TAG, PCF("Incoming payload not a representation"));
        return nullptr;
    }

    return getTemp2Payload(gTemp2ResourceUri);
}

OCEntityHandlerResult ProcessTemp2GetRequest (OCEntityHandlerRequest *ehRequest,
    OCRepPayload **payload)
{
    OCEntityHandlerResult ehResult;

    OCRepPayload *getResp = constructTemp2Response(ehRequest);

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


OCEntityHandlerResult
Temp2OCEntityHandlerCb (OCEntityHandlerFlag flag,
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
                ehResult = ProcessTemp2GetRequest (entityHandlerRequest, &payload);
            }
            else
            {
                OIC_LOG_V (INFO, TAG, "Received unsupported method %d from client",
                        entityHandlerRequest->method);
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

int createTemp2Resource () {
    createTemp2ResourceEx(gTemp2ResourceUri, &Temp2);
    return 0;
}

int createTemp2ResourceEx (char *uri, Temperature2Resource *temp2Resource)
{
    if (!uri)
    {
        OIC_LOG(ERROR, TAG, "Resource URI cannot be NULL");
        return -1;
    }

    OCStackResult res = OCCreateResource(&(temp2Resource->handle),
            gTemp2ResourceType,
            OC_RSRVD_INTERFACE_DEFAULT,
            gTemp2ResourceUri,
            Temp2OCEntityHandlerCb,
            NULL,
            OC_DISCOVERABLE | OC_OBSERVABLE 
#if IS_SECURE_MODE
            | OC_SECURE
#endif
        );
    OIC_LOG_V(INFO, TAG, "Created Temp2 resource with result: %s", getResult(res));

    return 0;
}