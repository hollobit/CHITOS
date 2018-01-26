/*
 * heartrate.c
 *
 *  Created on: Oct 29, 2017
 *      Author: hjshin
 */

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
#include "heartrate.h"
#include "common.h"

//-----------------------------------------------------------------------------
// Defines
//-----------------------------------------------------------------------------

#define TAG "SERVER-HEARTRATE"
#define SAMPLE_MAX_NUM_OBSERVATIONS     8

//-----------------------------------------------------------------------------
// Typedefs
//-----------------------------------------------------------------------------

/* Structure to represent a LED resource */
typedef struct HEARTRATERESOURCE{
    OCResourceHandle handle;
} HeartrateResource;

/* Structure to represent the observers */
typedef struct
{
    OCObservationId observationId;
    bool valid;
    OCResourceHandle resourceHandle;
} Observers;

//-----------------------------------------------------------------------------
// Variables
//-----------------------------------------------------------------------------

static HeartrateResource Heartrate;

char *gHeartrateResourceType= (char *)"oic.r.heartrate";
char *gHeartrateResourceUri= (char *)"/HeartrateResURI";

Observers interestedObservers[SAMPLE_MAX_NUM_OBSERVATIONS];
int gHeartRateUnderObservation = 0;

//-----------------------------------------------------------------------------
// Function prototype
//-----------------------------------------------------------------------------

OCRepPayload* getHeartratePayload(const char* uri);

/* This method converts the payload to JSON format */
OCRepPayload* constructHeartrateResponse (OCEntityHandlerRequest *ehRequest);

/* Following methods process the GET
 * requests
 */
OCEntityHandlerResult ProcessHeartrateGetRequest (OCEntityHandlerRequest *ehRequest,
                                         OCRepPayload **payload);

int createHeartrateResourceEx (char *uri, HeartrateResource *heartrateResource);

//-----------------------------------------------------------------------------
// Callback functions
//-----------------------------------------------------------------------------

/* Entity Handler callback functions */
OCEntityHandlerResult
HeartrateOCEntityHandlerCb (OCEntityHandlerFlag flag,
        OCEntityHandlerRequest *entityHandlerRequest,
        void* callbackParam);




//-----------------------------------------------------------------------------
// Function Implementations
//-----------------------------------------------------------------------------
OCRepPayload* getHeartratePayload(const char* uri)
{
    OCRepPayload* payload = OCRepPayloadCreate();
    if(!payload)
    {
        OIC_LOG(ERROR, TAG, PCF("Failed to allocate Payload"));
        return NULL;
    }
    size_t dimensions[MAX_REP_ARRAY_DEPTH] = { 0 };

    dimensions[0] = 1;
	char * rtStr[] = {"oic.r.userid"};
	OCRepPayloadSetStringArray(payload, "rt", (const char **)rtStr, dimensions);
	OCRepPayloadSetPropString(payload, "id", "watch_example_id");
	OCRepPayloadSetPropInt(payload, "heartrate", getHrmValue());
    return payload;
}

OCRepPayload* constructHeartrateResponse (OCEntityHandlerRequest *ehRequest)
{
    if(ehRequest->payload && ehRequest->payload->type != PAYLOAD_TYPE_REPRESENTATION)
    {
        OIC_LOG(ERROR, TAG, PCF("Incoming payload not a representation"));
        return NULL;
    }

    return getHeartratePayload(gHeartrateResourceUri);
}

OCEntityHandlerResult ProcessHeartrateGetRequest (OCEntityHandlerRequest *ehRequest,
    OCRepPayload **payload)
{
    OCEntityHandlerResult ehResult;

    OCRepPayload *getResp = constructHeartrateResponse(ehRequest);

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

void changeHeartRateValue() {
	OCStackResult result = OC_STACK_ERROR;
	uint8_t j = 0;
	uint8_t numNotifies = (SAMPLE_MAX_NUM_OBSERVATIONS)/2;
	OCObservationId obsNotify[numNotifies];

	j = 0;
	for (uint8_t i = 0; i < SAMPLE_MAX_NUM_OBSERVATIONS; (i=i+2))
	{
		if (interestedObservers[i].valid == true)
		{
			obsNotify[j] = interestedObservers[i].observationId;
			j++;
		}
	}

    if(j>0) {
    	//dlog_print(DLOG_DEBUG, LOG_TAG, "send notify to observer");
		OCRepPayload* payload = getHeartratePayload(gHeartrateResourceUri);
		OCNotifyAllObservers(Heartrate.handle, OC_HIGH_QOS);
		//result = OCNotifyListOfObservers (Heartrate.handle, obsNotify, j,
		//		payload, OC_HIGH_QOS);
		OCRepPayloadDestroy(payload);
    } else {
    	//dlog_print(DLOG_DEBUG, LOG_TAG, "no send...");
    }
}

void ProcessObserveRegister (OCEntityHandlerRequest *ehRequest)
{
    //cout << "\nReceived observation registration request with observation Id "
    //     << ehRequest->obsInfo.obsId;
    for (uint8_t i = 0; i < SAMPLE_MAX_NUM_OBSERVATIONS; i++)
    {
        if (interestedObservers[i].valid == false)
        {
            interestedObservers[i].observationId = ehRequest->obsInfo.obsId;
            interestedObservers[i].valid = true;
            gHeartRateUnderObservation = 1;
            break;
        }
    }
}

void ProcessObserveDeregister (OCEntityHandlerRequest *ehRequest)
{
    bool clientStillObserving = false;

    //cout << "\nReceived observation deregistration request for observation Id "
    //     << ehRequest->obsInfo.obsId;
    for (uint8_t i = 0; i < SAMPLE_MAX_NUM_OBSERVATIONS; i++)
    {
        if (interestedObservers[i].observationId == ehRequest->obsInfo.obsId)
        {
            interestedObservers[i].valid = false;
        }
        if (interestedObservers[i].valid == true)
        {
            // Even if there is one single client observing we continue notifying entity handler
            clientStillObserving = true;
        }
    }
    if (clientStillObserving == false)
    	gHeartRateUnderObservation = 0;
}

OCEntityHandlerResult
HeartrateOCEntityHandlerCb (OCEntityHandlerFlag flag,
        OCEntityHandlerRequest *entityHandlerRequest,
        void* callbackParam)
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

    OCRepPayload* payload = NULL;

    if (flag & OC_REQUEST_FLAG)
    {
        OIC_LOG (INFO, TAG, "Flag includes OC_REQUEST_FLAG");
        if (entityHandlerRequest)
        {
            if (OC_REST_GET == entityHandlerRequest->method)
            {
                OIC_LOG (INFO, TAG, "Received OC_REST_GET from client");
                ehResult = ProcessHeartrateGetRequest (entityHandlerRequest, &payload);
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
                response.payload = (OCPayload*)/*reinterpret_cast<OCPayload*>*/(payload);
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

    if (flag & OC_OBSERVE_FLAG)
	{
		//cout << "\nFlag includes OC_OBSERVE_FLAG";

		if (OC_OBSERVE_REGISTER == entityHandlerRequest->obsInfo.action)
		{
			OIC_LOG(INFO, "OBSERVE", "regist observe");
			//cout << "\nReceived OC_OBSERVE_REGISTER from client";

			ProcessObserveRegister (entityHandlerRequest);
		}
		else if (OC_OBSERVE_DEREGISTER == entityHandlerRequest->obsInfo.action)
		{
			OIC_LOG(INFO, "OBSERVE", "DEregist observe");
			//cout << "\nReceived OC_OBSERVE_DEREGISTER from client";
			ProcessObserveDeregister (entityHandlerRequest);
		}
	}

    OCRepPayloadDestroy(payload);
    return ehResult;
}

int createHeartrateResource () {
    createHeartrateResourceEx(gHeartrateResourceUri, &Heartrate);
    return 0;
}

int createHeartrateResourceEx (char *uri, HeartrateResource *heartrateResource)
{
    if (!uri)
    {
        OIC_LOG(ERROR, TAG, "Resource URI cannot be NULL");
        return -1;
    }

    /*OCStackResult res =*/ OCCreateResource(&(heartrateResource->handle),
            gHeartrateResourceType,
            OC_RSRVD_INTERFACE_DEFAULT,
            gHeartrateResourceUri,
			HeartrateOCEntityHandlerCb,
            NULL,
            OC_DISCOVERABLE | OC_OBSERVABLE | OC_SECURE);
    OIC_LOG_V(INFO, TAG, "Created Heartrate resource with result: %s", getResult(res));

    return 0;
}


