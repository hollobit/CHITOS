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
#include "temperature3.h"
#include "../common.h"
#include "../MySignals/temperature.h"

//-----------------------------------------------------------------------------
// Defines
//-----------------------------------------------------------------------------

#define TAG "SERVER-TEMPERATURE-3"

//-----------------------------------------------------------------------------
// Typedefs
//-----------------------------------------------------------------------------

/* Structure to represent a LED resource */
typedef struct TEMPERATURE3RESOURCE{
    OCResourceHandle handle;
} Temperature3Resource;

//-----------------------------------------------------------------------------
// LED Variables
//-----------------------------------------------------------------------------

static Temperature3Resource Temp3;

char *gTemp3ResourceType= (char *)"oic.r.body.temperature";
char *gTemp3ResourceUri= (char *)"/BodyTemperatureBatchResURI";

//-----------------------------------------------------------------------------
// Function prototype
//-----------------------------------------------------------------------------

OCRepPayload* getTemp3Payload(const char* uri);

/* This method converts the payload to JSON format */
OCRepPayload* constructTemp3Response (OCEntityHandlerRequest *ehRequest);

/* Following methods process the GET
 * requests
 */
OCEntityHandlerResult ProcessTemp3GetRequest (OCEntityHandlerRequest *ehRequest,
                                         OCRepPayload **payload);

int createTemp3ResourceEx (char *uri, Temperature3Resource *temp3Resource);       

//-----------------------------------------------------------------------------
// Callback functions
//-----------------------------------------------------------------------------

/* Entity Handler callback functions */
OCEntityHandlerResult
Temp3OCEntityHandlerCb (OCEntityHandlerFlag flag,
        OCEntityHandlerRequest *entityHandlerRequest);

//-----------------------------------------------------------------------------
// Function Implementations
//-----------------------------------------------------------------------------
OCRepPayload* getTemp3Payload(const char* uri)
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
    OCRepPayload* rep1 = OCRepPayloadCreate();    
    OCRepPayloadSetPropDouble(rep1, "temperature", readTemperature());    
    OCRepPayloadSetPropObject(href1, "rep", rep1);

    OCRepPayload* href2 = OCRepPayloadCreate();
    OCRepPayloadSetPropString(href2, "href", "/myBodyLocationResURI");
    OCRepPayload* rep2 = OCRepPayloadCreate();    
    OCRepPayloadSetPropString(rep2, "bloc", "finger");    
    OCRepPayloadSetPropObject(href2, "rep", rep2);

    OCRepPayload* href3 = OCRepPayloadCreate();
    OCRepPayloadSetPropString(href3, "href", "/myClockResURI");
    OCRepPayload* rep3 = OCRepPayloadCreate();  
    char buf[25] = "";
    getCurrentTime(buf);      
    OCRepPayloadSetPropString(rep3, "datetime", buf);    
    OCRepPayloadSetPropObject(href3, "rep", rep3);

    OCRepPayload* href4 = OCRepPayloadCreate();
    OCRepPayloadSetPropString(href4, "href", "/myUserIDResURI");
    OCRepPayload* rep4 = OCRepPayloadCreate();    
    OCRepPayloadSetPropString(rep4, "userid", "myOCF");    
    OCRepPayloadSetPropObject(href4, "rep", rep4);
    
    OCRepPayload * hrefs[] = { href1, href2, href3, href4};
    dimensions[0] = 4;
    OCRepPayloadSetPropObjectArray(payload, "data", (const OCRepPayload **)hrefs, dimensions);

    return payload;
}

OCRepPayload* constructTemp3Response (OCEntityHandlerRequest *ehRequest)
{
    if(ehRequest->payload && ehRequest->payload->type != PAYLOAD_TYPE_REPRESENTATION)
    {
        OIC_LOG(ERROR, TAG, PCF("Incoming payload not a representation"));
        return nullptr;
    }

    return getTemp3Payload(gTemp3ResourceUri);
}

OCEntityHandlerResult ProcessTemp3GetRequest (OCEntityHandlerRequest *ehRequest,
    OCRepPayload **payload)
{
    OCEntityHandlerResult ehResult;

    OCRepPayload *getResp = constructTemp3Response(ehRequest);

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
Temp3OCEntityHandlerCb (OCEntityHandlerFlag flag,
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
                ehResult = ProcessTemp3GetRequest (entityHandlerRequest, &payload);
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

int createTemp3Resource () {
    createTemp3ResourceEx(gTemp3ResourceUri, &Temp3);
    return 0;
}

int createTemp3ResourceEx (char *uri, Temperature3Resource *temp3Resource)
{
    if (!uri)
    {
        OIC_LOG(ERROR, TAG, "Resource URI cannot be NULL");
        return -1;
    }

    OCStackResult res = OCCreateResource(&(temp3Resource->handle),
            gTemp3ResourceType,
            OC_RSRVD_INTERFACE_DEFAULT,
            gTemp3ResourceUri,
            Temp3OCEntityHandlerCb,
            NULL,
            OC_DISCOVERABLE | OC_OBSERVABLE 
#if IS_SECURE_MODE
            | OC_SECURE
#endif
        );
    OIC_LOG_V(INFO, TAG, "Created Temp3 resource with result: %s", getResult(res));

    return 0;
}