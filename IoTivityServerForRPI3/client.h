#ifndef CLIENT_H_
#define CLIENT_H_

#include "ocstack.h"

//-----------------------------------------------------------------------------
// Defines
//-----------------------------------------------------------------------------
#define DEFAULT_CONTEXT_VALUE 0x99

//-----------------------------------------------------------------------------
// Typedefs
//-----------------------------------------------------------------------------

/**
 * List of methods that can be inititated from the client
 */
typedef enum {
    TEST_DISCOVER_REQ = 1,
    TEST_NON_CON_OP,
    TEST_CON_OP,
    MAX_TESTS
} CLIENT_TEST;

/**
 * List of connectivity types that can be initiated from the client
 * Required for user input validation
 */
typedef enum {
    CT_ADAPTER_DEFAULT = 0,
    CT_IP,
    MAX_CT
} CLIENT_CONNECTIVITY_TYPE;

//-----------------------------------------------------------------------------
// Function prototype
//-----------------------------------------------------------------------------

/* Get the IP address of the server */
std::string getIPAddrTBServer(OCClientResponse * clientResponse);

/* Get the port number the server is listening on */
std::string getPortTBServer(OCClientResponse * clientResponse);

/* Returns the query string for GET and PUT operations */
std::string getQueryStrForGetPut(OCClientResponse * clientResponse);

/* Following are initialization functions for GET, PUT
 * POST & Discovery operations
 */
int InitPutRequest(OCDevAddr *endpoint, OCQualityOfService qos);
int InitGetRequest(OCDevAddr *endpoint, OCQualityOfService qos);
int InitPostRequest(OCDevAddr *endpoint, OCQualityOfService qos);
int InitDiscovery();

/* Function to retrieve ip address, port no. of the server
 *  and query for the operations to be performed.
 */
int parseClientResponse(OCClientResponse * clientResponse);

/* This method calls OCDoResource() which in turn makes calls
 * to the lower layers
 */
OCStackResult InvokeOCDoResource(std::ostringstream &query,
        OCMethod method, OCQualityOfService qos,
        OCClientResponseHandler cb, OCHeaderOption * options, uint8_t numOptions);

//-----------------------------------------------------------------------------
// Callback functions
//-----------------------------------------------------------------------------

/* Following are callback functions for the  GET, PUT
 * POST & Discovery operations
 */

OCStackApplicationResult putReqCB(void* ctx, OCDoHandle handle, OCClientResponse * clientResponse);

OCStackApplicationResult postReqCB(void *ctx, OCDoHandle handle, OCClientResponse *clientResponse);

OCStackApplicationResult getReqCB(void* ctx, OCDoHandle handle, OCClientResponse * clientResponse);

OCStackApplicationResult discoveryReqCB(void* ctx, OCDoHandle handle,
        OCClientResponse * clientResponse);

#endif