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
#include "scale8.h"
#include "../common.h"
#include "../MySignals/scale.h"

//-----------------------------------------------------------------------------
// Defines
//-----------------------------------------------------------------------------

#define TAG "SERVER-SCALE-8"

//-----------------------------------------------------------------------------
// Typedefs
//-----------------------------------------------------------------------------

/* Structure to represent a resource */
typedef struct SCALE8RESOURCE{
    OCResourceHandle handle;
} Scale8Resource;

//-----------------------------------------------------------------------------
// Variables
//-----------------------------------------------------------------------------

static Scale8Resource Scale8;

char *gScale8ResourceType= (char *)"kr.re.etri.r.userinfo";
char *gScale8ResourceUri= (char *)"/UserInfoResURI";

//-----------------------------------------------------------------------------
// Function prototype
//-----------------------------------------------------------------------------

OCRepPayload* getScale8Payload(const char* uri);

/* This method converts the payload to JSON format */
OCRepPayload* constructScale8Response (OCEntityHandlerRequest *ehRequest);

/* Following methods process the GET
 * requests
 */
OCEntityHandlerResult ProcessScale8GetRequest (OCEntityHandlerRequest *ehRequest,
                                         OCRepPayload **payload);

int createScale8ResourceEx (char *uri, Scale8Resource *scale8Resource);       

//-----------------------------------------------------------------------------
// Callback functions
//-----------------------------------------------------------------------------

/* Entity Handler callback functions */
OCEntityHandlerResult
Scale8OCEntityHandlerCb (OCEntityHandlerFlag flag,
        OCEntityHandlerRequest *entityHandlerRequest);

//-----------------------------------------------------------------------------
// Function Implementations
//-----------------------------------------------------------------------------
OCRepPayload* getScale8Payload(const char* uri)
{
    OCRepPayload* payload = OCRepPayloadCreate();
    if(!payload)
    {
        OIC_LOG(ERROR, TAG, PCF("Failed to allocate Payload"));
        return nullptr;
    }
    size_t dimensions[MAX_REP_ARRAY_DEPTH] = { 0 };

    OCRepPayloadSetPropInt(payload, "gender", (int)getGender());
    OCRepPayloadSetPropInt(payload, "age", (int)getAge());

    return payload;
}

OCRepPayload* constructScale8Response (OCEntityHandlerRequest *ehRequest)
{
    if(ehRequest->payload && ehRequest->payload->type != PAYLOAD_TYPE_REPRESENTATION)
    {
        OIC_LOG(ERROR, TAG, PCF("Incoming payload not a representation"));
        return nullptr;
    }

    return getScale8Payload(gScale8ResourceUri);
}

OCEntityHandlerResult ProcessScale8PostRequest (OCEntityHandlerRequest *ehRequest,
    OCRepPayload **payload)
{
    OCEntityHandlerResult ehResult = OC_EH_ERROR;

    OCRepPayload* _payload = (OCRepPayload*)(ehRequest->payload);
    if (!_payload)
    {
        ehResult = OC_EH_ERROR;
    } else {

        int64_t _gender = 0;
        int64_t _age = 0;
        
        printf("USERINFO POST DATA CHECK START!\n");
        if(OCRepPayloadGetPropInt(_payload, "gender", &_gender))
        {
            printf("POST GENDER[%d]\n", _gender);
            setGender((uint8_t)_gender);
        }

        if(OCRepPayloadGetPropInt(_payload, "age", &_age))
        {
            printf("POST AGE[%d]\n", _age);
            setAge((uint8_t)_age);
        }

        printf("USERINFO POST DATA CHECK END!\n");
        
        
        OCRepPayload *postResp = constructScale8Response(ehRequest);

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

OCEntityHandlerResult ProcessScale8GetRequest (OCEntityHandlerRequest *ehRequest,
    OCRepPayload **payload)
{
    OCEntityHandlerResult ehResult;

    OCRepPayload *getResp = constructScale8Response(ehRequest);

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
Scale8OCEntityHandlerCb (OCEntityHandlerFlag flag,
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
                ehResult = ProcessScale8GetRequest (entityHandlerRequest, &payload);
            }
            else if (OC_REST_POST == entityHandlerRequest->method)
            {
                OIC_LOG (INFO, TAG, "Received OC_REST_POST from client");
                ehResult = ProcessScale8PostRequest (entityHandlerRequest, &payload);
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

int createScale8Resource () {
    createScale8ResourceEx(gScale8ResourceUri, &Scale8);
    return 0;
}

int createScale8ResourceEx (char *uri, Scale8Resource *scale8Resource)
{
    if (!uri)
    {
        OIC_LOG(ERROR, TAG, "Resource URI cannot be NULL");
        return -1;
    }

    OCStackResult res = OCCreateResource(&(scale8Resource->handle),
            gScale8ResourceType,
            OC_RSRVD_INTERFACE_DEFAULT,
            gScale8ResourceUri,
            Scale8OCEntityHandlerCb,
            NULL,
            OC_DISCOVERABLE | OC_OBSERVABLE 
#if IS_SECURE_MODE
            | OC_SECURE
#endif
        );
    OIC_LOG_V(INFO, TAG, "Created Scale8 resource with result: %s", getResult(res));

    return 0;
}