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
#include "glucose5.h"
#include "../common.h"

//-----------------------------------------------------------------------------
// Defines
//-----------------------------------------------------------------------------

#define TAG "SERVER-GLUCOSE-5"

//-----------------------------------------------------------------------------
// Typedefs
//-----------------------------------------------------------------------------

/* Structure to represent a resource */
typedef struct GLUCOSE5RESOURCE{
    OCResourceHandle handle;
} Glucose5Resource;

//-----------------------------------------------------------------------------
// Variables
//-----------------------------------------------------------------------------

static Glucose5Resource Glucose5;

char *gGlucose5ResourceType= (char *)"oic.r.glucose.health";
char *gGlucose5ResourceUri= (char *)"/GlucoseHealthResURI";

//-----------------------------------------------------------------------------
// Function prototype
//-----------------------------------------------------------------------------

OCRepPayload* getGlucose5Payload(const char* uri);

/* This method converts the payload to JSON format */
OCRepPayload* constructGlucose5Response (OCEntityHandlerRequest *ehRequest);

/* Following methods process the GET
 * requests
 */
OCEntityHandlerResult ProcessGlucose5GetRequest (OCEntityHandlerRequest *ehRequest,
                                         OCRepPayload **payload);

int createGlucose5ResourceEx (char *uri, Glucose5Resource *glucose5Resource);       

//-----------------------------------------------------------------------------
// Callback functions
//-----------------------------------------------------------------------------

/* Entity Handler callback functions */
OCEntityHandlerResult
Glucose5OCEntityHandlerCb (OCEntityHandlerFlag flag,
        OCEntityHandlerRequest *entityHandlerRequest);

//-----------------------------------------------------------------------------
// Function Implementations
//-----------------------------------------------------------------------------
OCRepPayload* getGlucose5Payload(const char* uri)
{
    OCRepPayload* payload = OCRepPayloadCreate();
    if(!payload)
    {
        OIC_LOG(ERROR, TAG, PCF("Failed to allocate Payload"));
        return nullptr;
    }
    size_t dimensions[MAX_REP_ARRAY_DEPTH] = { 0 };

    dimensions[0] = 1;
    char * rtStr[] = {"oic.r.glucose.health"};
    OCRepPayloadSetStringArray(payload, "rt", (const char **)rtStr, dimensions);
    OCRepPayloadSetPropString(payload, "id", "user_example_id");
    OCRepPayloadSetPropString(payload, "health", "major");

    return payload;
}

OCRepPayload* constructGlucose5Response (OCEntityHandlerRequest *ehRequest)
{
    if(ehRequest->payload && ehRequest->payload->type != PAYLOAD_TYPE_REPRESENTATION)
    {
        OIC_LOG(ERROR, TAG, PCF("Incoming payload not a representation"));
        return nullptr;
    }

    return getGlucose5Payload(gGlucose5ResourceUri);
}

OCEntityHandlerResult ProcessGlucose5GetRequest (OCEntityHandlerRequest *ehRequest,
    OCRepPayload **payload)
{
    OCEntityHandlerResult ehResult;

    OCRepPayload *getResp = constructGlucose5Response(ehRequest);

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
Glucose5OCEntityHandlerCb (OCEntityHandlerFlag flag,
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
                ehResult = ProcessGlucose5GetRequest (entityHandlerRequest, &payload);
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

int createGlucose5Resource () {
    createGlucose5ResourceEx(gGlucose5ResourceUri, &Glucose5);
    return 0;
}

int createGlucose5ResourceEx (char *uri, Glucose5Resource *glucose5Resource)
{
    if (!uri)
    {
        OIC_LOG(ERROR, TAG, "Resource URI cannot be NULL");
        return -1;
    }

    OCStackResult res = OCCreateResource(&(glucose5Resource->handle),
            gGlucose5ResourceType,
            OC_RSRVD_INTERFACE_DEFAULT,
            gGlucose5ResourceUri,
            Glucose5OCEntityHandlerCb,
            NULL,
            OC_DISCOVERABLE | OC_OBSERVABLE 
#if IS_SECURE_MODE
            | OC_SECURE
#endif
        );
    OIC_LOG_V(INFO, TAG, "Created Glucose5 resource with result: %s", getResult(res));

    return 0;
}