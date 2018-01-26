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
#include "glucose0.h"
#include "../common.h"
#include "../MySignals/glucose.h"

//-----------------------------------------------------------------------------
// Defines
//-----------------------------------------------------------------------------

#define TAG "SERVER-GLUCOSE-0"

//-----------------------------------------------------------------------------
// Typedefs
//-----------------------------------------------------------------------------

/* Structure to represent a resource */
typedef struct GLUCOSE0RESOURCE{
    OCResourceHandle handle;
} Glucose0Resource;

//-----------------------------------------------------------------------------
// Variables
//-----------------------------------------------------------------------------

static Glucose0Resource Glucose0;

char *gGlucose0ResourceType= (char *)"oic.r.glucosemeter-am";
char *gGlucose0ResourceUri= (char *)"/GlucoseMeterAMResURI";

//-----------------------------------------------------------------------------
// Function prototype
//-----------------------------------------------------------------------------

OCRepPayload* getGlucose0Payload(const char* uri);

/* This method converts the payload to JSON format */
OCRepPayload* constructGlucose0Response (OCEntityHandlerRequest *ehRequest);

/* Following methods process the GET
 * requests
 */
OCEntityHandlerResult ProcessGlucose0GetRequest (OCEntityHandlerRequest *ehRequest,
                                         OCRepPayload **payload);

int createGlucose0ResourceEx (char *uri, Glucose0Resource *glucose0Resource);       

//-----------------------------------------------------------------------------
// Callback functions
//-----------------------------------------------------------------------------

/* Entity Handler callback functions */
OCEntityHandlerResult
Glucose0OCEntityHandlerCb (OCEntityHandlerFlag flag,
        OCEntityHandlerRequest *entityHandlerRequest);

//-----------------------------------------------------------------------------
// Function Implementations
//-----------------------------------------------------------------------------
OCRepPayload* getGlucose0Payload(const char* uri, const char * query)
{    
    OCRepPayload* payload = OCRepPayloadCreate();
    if(!payload)
    {
        OIC_LOG(ERROR, TAG, PCF("Failed to allocate Payload"));
        return nullptr;
    }
    size_t dimensions[MAX_REP_ARRAY_DEPTH] = { 0 };

    if(strlen(query) >= 10) {
        if(*query == 'i' && *(query+1) == 'f' && *(query+2) == '=') {
            if(*(query+3) == 'o' &&
            *(query+4) == 'i' &&
            *(query+5) == 'c' &&
            *(query+6) == '.' &&
            *(query+7) == 'i' &&
            *(query+8) == 'f' &&
            *(query+9) == '.' ) {
                if(*(query+10) == 'b' &&
                   strlen(query) == 11) {
                    OCRepPayload* child1 = OCRepPayloadCreate();
                    
                    OCRepPayload* child1Rep = OCRepPayloadCreate();
                    OCRepPayloadSetPropInt(child1Rep, "glucose", getGlucose());
                    OCRepPayloadSetPropString(child1Rep, "unit", "mm/dL");
                    OCRepPayloadSetPropObject(child1, "rep", child1Rep);
                    OCRepPayloadSetPropString(child1, "href", "/myGlucoseResURI");
                    
                    OCRepPayload* child2 = OCRepPayloadCreate();
                    
                    OCRepPayload* child2Rep = OCRepPayloadCreate();
                    OCRepPayloadSetPropInt(child2Rep, "hba1c", 5);
                    OCRepPayloadSetPropObject(child2, "rep", child2Rep);
                    OCRepPayloadSetPropString(child2, "href", "/myHbA1cResURI");

                    OCRepPayloadAppend(payload, child1);
                    OCRepPayloadAppend(payload, child2);
                } else if(*(query+10) == 'l' && 
                          *(query+11) == 'l' ) {
                    OCRepPayload* child1 = OCRepPayloadCreate();                       
                    OCRepPayloadSetPropString(child1, "href", "/myGlucoseResURI");                    
                    dimensions[0] = 1;
                    char * chile1rtStr[] = {"oic.r.glucose"};
                    OCRepPayloadSetStringArray(child1, "rt", (const char **)chile1rtStr, dimensions);
                    dimensions[0] = 2;
                    char * child1ifStr[] = {"oic.if.s", "oic.if.baseline"};
                    OCRepPayloadSetStringArray(child1, "if", (const char **)child1ifStr, dimensions);

                    OCRepPayload* child2 = OCRepPayloadCreate();                    
                    OCRepPayloadSetPropString(child2, "href", "/myHbA1cResURI");                    
                    dimensions[0] = 1;
                    char * chile2rtStr[] = {"oic.r.glucose.hba1c"};
                    OCRepPayloadSetStringArray(child2, "rt", (const char **)chile2rtStr, dimensions);
                    dimensions[0] = 2;
                    char * child2ifStr[] = {"oic.if.s", "oic.if.baseline"};
                    OCRepPayloadSetStringArray(child2, "if", (const char **)child2ifStr, dimensions);

                    OCRepPayloadAppend(payload, child1);
                    OCRepPayloadAppend(payload, child2);
                } else {
                    // baseline
                    dimensions[0] = 1;
                    char * rtStr[] = {"oic.wk.col.atomic"};
                    OCRepPayloadSetStringArray(payload, "rt", (const char **)rtStr, dimensions);

                    dimensions[0] = 3;
                    char * ifStr[] = {"oic.if.b", "oic.if.ll", "oic.if.baseline"};
                    OCRepPayloadSetStringArray(payload, "if", (const char **)ifStr, dimensions);

                    dimensions[0] = 2;
                    char * rtsStr[] = {"oic.r.glucose", "oic.r.glucose.hba1c"};
                    OCRepPayloadSetStringArray(payload, "rts", (const char **)rtsStr, dimensions);


                    OCRepPayload* href1 = OCRepPayloadCreate();
                    OCRepPayloadSetPropString(href1, "href", "/myGlucoseResURI");
                    dimensions[0] = 1;
                    char * href1RtStr[] = {"oic.r.glucose"};
                    OCRepPayloadSetStringArray(href1, "rt", (const char **)href1RtStr, dimensions);
                    dimensions[0] = 2;
                    char * href1IfStr[] = {"oic.if.s", "oic.if.baseline"};
                    OCRepPayloadSetStringArray(href1, "if", (const char **)href1IfStr, dimensions);

                    OCRepPayload* href2 = OCRepPayloadCreate();
                    OCRepPayloadSetPropString(href2, "href", "/myHbA1cResURI");
                    dimensions[0] = 1;
                    char * href2RtStr[] = {"oic.r.glucose.hba1c"};
                    OCRepPayloadSetStringArray(href2, "rt", (const char **)href2RtStr, dimensions);
                    dimensions[0] = 2;
                    char * href2IfStr[] = {"oic.if.s", "oic.if.baseline"};
                    OCRepPayloadSetStringArray(href2, "if", (const char **)href2IfStr, dimensions);

                    OCRepPayload * hrefs[] = { href1, href2};
                    dimensions[0] = 2;
                    OCRepPayloadSetPropObjectArray(payload, "links", (const OCRepPayload **)hrefs, dimensions);

                }
            }
        }
    }

    return payload;
}

OCRepPayload* constructGlucose0Response (OCEntityHandlerRequest *ehRequest)
{
    if(ehRequest->payload && ehRequest->payload->type != PAYLOAD_TYPE_REPRESENTATION)
    {
        OIC_LOG(ERROR, TAG, PCF("Incoming payload not a representation"));
        return nullptr;
    }

    return getGlucose0Payload(gGlucose0ResourceUri, ehRequest->query);
}

OCEntityHandlerResult ProcessGlucose0GetRequest (OCEntityHandlerRequest *ehRequest,
    OCRepPayload **payload)
{
    OCEntityHandlerResult ehResult;

    OCRepPayload *getResp = constructGlucose0Response(ehRequest);

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
Glucose0OCEntityHandlerCb (OCEntityHandlerFlag flag,
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
                ehResult = ProcessGlucose0GetRequest (entityHandlerRequest, &payload);
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

int createGlucose0Resource () {
    createGlucose0ResourceEx(gGlucose0ResourceUri, &Glucose0);
    return 0;
}

int createGlucose0ResourceEx (char *uri, Glucose0Resource *glucose0Resource)
{
    if (!uri)
    {
        OIC_LOG(ERROR, TAG, "Resource URI cannot be NULL");
        return -1;
    }

    OCStackResult res = OCCreateResource(&(glucose0Resource->handle),
            gGlucose0ResourceType,
            OC_RSRVD_INTERFACE_DEFAULT,
            gGlucose0ResourceUri,
            Glucose0OCEntityHandlerCb,
            NULL,
            OC_DISCOVERABLE | OC_OBSERVABLE 
#if IS_SECURE_MODE
            | OC_SECURE
#endif
        );
    OIC_LOG_V(INFO, TAG, "Created Glucose0 resource with result: %s", getResult(res));

    return 0;
}