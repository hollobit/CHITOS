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
#include "scale6.h"
#include "../common.h"
#include "../MySignals/scale.h"

//-----------------------------------------------------------------------------
// Defines
//-----------------------------------------------------------------------------

#define TAG "SERVER-SCALE-6"

//-----------------------------------------------------------------------------
// Typedefs
//-----------------------------------------------------------------------------

/* Structure to represent a resource */
typedef struct SCALE6RESOURCE{
    OCResourceHandle handle;
} Scale6Resource;

//-----------------------------------------------------------------------------
// Variables
//-----------------------------------------------------------------------------

static Scale6Resource Scale6;

char *gScale6ResourceType= (char *)"oic.r.height";
char *gScale6ResourceUri= (char *)"/HeightResURI";

//-----------------------------------------------------------------------------
// Function prototype
//-----------------------------------------------------------------------------

OCRepPayload* getScale6Payload(const char* uri);

/* This method converts the payload to JSON format */
OCRepPayload* constructScale6Response (OCEntityHandlerRequest *ehRequest);

/* Following methods process the GET
 * requests
 */
OCEntityHandlerResult ProcessScale6GetRequest (OCEntityHandlerRequest *ehRequest,
                                         OCRepPayload **payload);

int createScale6ResourceEx (char *uri, Scale6Resource *scale6Resource);       

//-----------------------------------------------------------------------------
// Callback functions
//-----------------------------------------------------------------------------

/* Entity Handler callback functions */
OCEntityHandlerResult
Scale6OCEntityHandlerCb (OCEntityHandlerFlag flag,
        OCEntityHandlerRequest *entityHandlerRequest);

//-----------------------------------------------------------------------------
// Function Implementations
//-----------------------------------------------------------------------------
OCRepPayload* getScale6Payload(const char* uri)
{
    OCRepPayload* payload = OCRepPayloadCreate();
    if(!payload)
    {
        OIC_LOG(ERROR, TAG, PCF("Failed to allocate Payload"));
        return nullptr;
    }
    size_t dimensions[MAX_REP_ARRAY_DEPTH] = { 0 };

    dimensions[0] = 1;
    char * rtStr[] = {"oic.r.height"};
    OCRepPayloadSetStringArray(payload, "rt", (const char **)rtStr, dimensions);
    OCRepPayloadSetPropString(payload, "id", "user_example_id");
    OCRepPayloadSetPropDouble(payload, "height", getHeight());
    OCRepPayloadSetPropString(payload, "unit", "m");

    return payload;
}

OCRepPayload* constructScale6Response (OCEntityHandlerRequest *ehRequest)
{
    if(ehRequest->payload && ehRequest->payload->type != PAYLOAD_TYPE_REPRESENTATION)
    {
        OIC_LOG(ERROR, TAG, PCF("Incoming payload not a representation"));
        return nullptr;
    }

    return getScale6Payload(gScale6ResourceUri);
}

OCRepPayload* getScale6PostPayload(const char* uri, const char * _id, const char * _unit)
{
    OCRepPayload* payload = OCRepPayloadCreate();
    if(!payload)
    {
        OIC_LOG(ERROR, TAG, PCF("Failed to allocate Payload"));
        return nullptr;
    }
    size_t dimensions[MAX_REP_ARRAY_DEPTH] = { 0 };

    OCRepPayloadSetPropString(payload, "id", _id);
    OCRepPayloadSetPropDouble(payload, "height", getHeight());
    OCRepPayloadSetPropString(payload, "unit", _unit);

    return payload;
}

OCRepPayload* constructScale6PostResponse (OCEntityHandlerRequest *ehRequest, const char * _id, const char * _unit)
{
    if(ehRequest->payload && ehRequest->payload->type != PAYLOAD_TYPE_REPRESENTATION)
    {
        OIC_LOG(ERROR, TAG, PCF("Incoming payload not a representation"));
        return nullptr;
    }

    return getScale6PostPayload(gScale6ResourceUri, _id, _unit);
}

OCEntityHandlerResult ProcessScale6PostRequest (OCEntityHandlerRequest *ehRequest,
    OCRepPayload **payload)
{
    OCEntityHandlerResult ehResult = OC_EH_ERROR;

    OCRepPayload* _payload = (OCRepPayload*)(ehRequest->payload);
    if (!_payload)
    {
        ehResult = OC_EH_ERROR;
    } else {

        char * _id = (char*)malloc(255);
        double _height = 0.0f;
        char * _unit = (char*)malloc(255);

        memset(_id, 0x00, 255);
        memset(_unit, 0x00, 255);

        printf("POST DATA CHECK START!\n");
        if(OCRepPayloadGetPropString(_payload, "id", &_id))
        {
            printf("POST ID[%s]\n", _id);
        }

        if(OCRepPayloadGetPropDouble(_payload, "height", &_height))
        {
            printf("POST HEIGHT[%f]\n", _height);
            setHeight(_height);
        }

        if(OCRepPayloadGetPropString(_payload, "unit", &_unit))
        {
            printf("POST UNIT[%s]\n", _unit);
        }

        printf("POST DATA CHECK END!\n");
        
        
        OCRepPayload *postResp = constructScale6PostResponse(ehRequest, _id, _unit);

        free(_id);
        free(_unit);

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

OCEntityHandlerResult ProcessScale6GetRequest (OCEntityHandlerRequest *ehRequest,
    OCRepPayload **payload)
{
    OCEntityHandlerResult ehResult;

    OCRepPayload *getResp = constructScale6Response(ehRequest);

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
Scale6OCEntityHandlerCb (OCEntityHandlerFlag flag,
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
                ehResult = ProcessScale6GetRequest (entityHandlerRequest, &payload);
            }
            else if (OC_REST_POST == entityHandlerRequest->method)
            {
                OIC_LOG (INFO, TAG, "Received OC_REST_POST from client");
                ehResult = ProcessScale6PostRequest (entityHandlerRequest, &payload);
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

int createScale6Resource () {
    createScale6ResourceEx(gScale6ResourceUri, &Scale6);
    return 0;
}

int createScale6ResourceEx (char *uri, Scale6Resource *scale6Resource)
{
    if (!uri)
    {
        OIC_LOG(ERROR, TAG, "Resource URI cannot be NULL");
        return -1;
    }

    OCStackResult res = OCCreateResource(&(scale6Resource->handle),
            gScale6ResourceType,
            OC_RSRVD_INTERFACE_DEFAULT,
            gScale6ResourceUri,
            Scale6OCEntityHandlerCb,
            NULL,
            OC_DISCOVERABLE | OC_OBSERVABLE 
#if IS_SECURE_MODE
            | OC_SECURE
#endif
        );
    OIC_LOG_V(INFO, TAG, "Created Scale6 resource with result: %s", getResult(res));

    return 0;
}