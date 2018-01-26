#include "app_main.h"
#include "uib_app_manager.h"
#include <sensor.h>
#include "iotivity_config.h"
#include <pthread.h>
#include "ocstack.h"
#include "logger.h"
#include "resource/common.h"
#include "resource/userid.h"
#include "resource/clock.h"
#include "resource/heartrate.h"

/* app event callbacks */
static bool _on_create_cb(void *user_data);
static void _on_terminate_cb(void *user_data);
static void _on_app_control_cb(app_control_h app_control, void *user_data);
static void _on_resume_cb(void *user_data);
static void _on_pause_cb(void *user_data);
static void _on_low_memory_cb(app_event_info_h event_info, void *user_data);
static void _on_low_battery_cb(app_event_info_h event_info, void *user_data);
static void _on_device_orientation_cb(app_event_info_h event_info, void *user_data);
static void _on_language_changed_cb(app_event_info_h event_info, void *user_data);
static void _on_region_format_changed_cb(app_event_info_h event_info, void *user_data);

//bool hrm_supported = false;
//sensor_recorder_option_h option;
void
nf_hw_back_cb(void* param, Evas_Object * evas_obj, void* event_info) {
	//TODO : user define code
	evas_obj = uib_views_get_instance()->get_window_obj()->app_naviframe;
	elm_naviframe_item_pop(evas_obj);
}

void
win_del_request_cb(void *data, Evas_Object *obj, void *event_info)
{
	ui_app_exit();
}

Eina_Bool
nf_root_it_pop_cb(void* elm_win, Elm_Object_Item *it) {
	elm_win_lower(elm_win);
	return EINA_FALSE;
}

app_data *uib_app_create()
{
	return calloc(1, sizeof(app_data));
}

void uib_app_destroy(app_data *user_data)
{
	uib_app_manager_get_instance()->free_all_view_context();
	free(user_data);
}

int uib_app_run(app_data *user_data, int argc, char **argv)
{
	ui_app_lifecycle_callback_s cbs =
	{
		.create = _on_create_cb,
		.terminate = _on_terminate_cb,
		.pause = _on_pause_cb,
		.resume = _on_resume_cb,
		.app_control = _on_app_control_cb,
	};

	app_event_handler_h handlers[5] =
	{NULL, };

	ui_app_add_event_handler(&handlers[APP_EVENT_LOW_BATTERY], APP_EVENT_LOW_BATTERY, _on_low_battery_cb, user_data);
	ui_app_add_event_handler(&handlers[APP_EVENT_LOW_MEMORY], APP_EVENT_LOW_MEMORY, _on_low_memory_cb, user_data);
	ui_app_add_event_handler(&handlers[APP_EVENT_DEVICE_ORIENTATION_CHANGED], APP_EVENT_DEVICE_ORIENTATION_CHANGED, _on_device_orientation_cb, user_data);
	ui_app_add_event_handler(&handlers[APP_EVENT_LANGUAGE_CHANGED], APP_EVENT_LANGUAGE_CHANGED, _on_language_changed_cb, user_data);
	ui_app_add_event_handler(&handlers[APP_EVENT_REGION_FORMAT_CHANGED], APP_EVENT_REGION_FORMAT_CHANGED, _on_region_format_changed_cb, user_data);




	return ui_app_main(argc, argv, &cbs, user_data);
}

void
app_get_resource(const char *res_file_in, char *res_path_out, int res_path_max)
{
	char *res_path = app_get_resource_path();
	if (res_path) {
		snprintf(res_path_out, res_path_max, "%s%s", res_path, res_file_in);
		free(res_path);
	}
}

#define 	OC_SECURITY_DB_DAT_FILE_NAME   "oic_svr_db.dat"
static char CRED_FILE[] = "/opt/usr/media/server.dat";

FILE* server_fopen(const char *path, const char *mode)
{
    if (0 == strcmp(path, OC_SECURITY_DB_DAT_FILE_NAME))
    {
        return fopen(CRED_FILE, mode);
    }
    else
    {
        return fopen(path, mode);
    }
}

int gQuitFlag = 0;

/* SIGINT handler: set gQuitFlag to 1 for graceful termination */
void handleSigInt(int signum)
{
    if (signum == SIGINT)
    {
        gQuitFlag = 1;
    }
}

void *iotivityThread(void *data) {
    struct timespec timeout;

    timeout.tv_sec  = 0;
    timeout.tv_nsec = 100000000L;

    // Break from loop with Ctrl-C
    OIC_LOG(INFO, TAG, "Entering ocserver main loop...");
    signal(SIGINT, handleSigInt);
    while (!gQuitFlag)
    {
        if (OCProcess() != OC_STACK_OK)
        {
            OIC_LOG(ERROR, TAG, "OCStack process error");
            return 0;
        }
        nanosleep(&timeout, NULL);
    }

    OIC_LOG(INFO, TAG, "Exiting ocserver main loop...");

    if (OCStop() != OC_STACK_OK)
    {
        OIC_LOG(ERROR, TAG, "OCStack process error");
    }

    return NULL;
}

void *iotivityObserveThread(void *data) {
	struct timespec timeout;

	timeout.tv_sec  = 0;
	timeout.tv_nsec = 500000000L;

	// Break from loop with Ctrl-C
	OIC_LOG(DEBUG, "OBSERVE", "Entering observer thread..");
	signal(SIGINT, handleSigInt);
	while (!gQuitFlag)
	{
		if(getHrmValue() != getLastHrmValue()) {
			dlog_print(DLOG_DEBUG, LOG_TAG, "Changed HRM Value");
			setLastHrmValue(getHrmValue());
			changeHeartRateValue();
		} else {
			dlog_print(DLOG_DEBUG, LOG_TAG, "None changed HRM Value");
		}
        nanosleep(&timeout, NULL);
	}

	dlog_print(DLOG_DEBUG, LOG_TAG, "Exiting observer thread main loop...");

	return NULL;

}

// Platform Info
/**
 * This structure describes the platform properties. All non-Null properties will be
 * included in a platform discovery request.
 * @deprecated: Use OCSetPropertyValue  to set platform value.
 */
const char *gPlatformID = "12341234-1234-1234-1234-123412341234";
const char *gManufacturerName = "Electronics and Telecommunications Research Institute";
const char *gManufacturerLink = "https://www.etri.re.kr";
const char *gModelNumber = "myModelNumber";
const char *gDateOfManufacture = "2017-11-09";
const char *gPlatformVersion = "1.0";
const char *gOperatingSystemVersion = "TIZEN V2.3.2.4";
const char *gHardwareVersion = "SAMSUNG GEAR S3(SM-R760)";
const char *gFirmwareVersion = "1.0";
const char *gSupportLink = "https://www.etri.re.kr";
const char *gSystemTime = "2015-05-15T11.04";

// Device Info
/**
 * This structure is expected as input for device properties.
 * device name is mandatory and expected from the application.
 * device id of type UUID will be generated by the stack.
 * @deprecated: Use OCSetPropertyValue  to set device value.
 */
const char *gDeviceType = "oic.d.smartwatch";
const char *gDeviceName = "Smart Watch";
const char *gSpecVersion = "ocf.1.1.0";
const char *gDataModelVersions = "ocf.res.1.1.0,ocf.sh.1.1.0";
const char *gProtocolIndependentID = "12341234-1234-1234-1234-123412341234";

OCPlatformInfo platformInfo;

void DeletePlatformInfo()
{
    free(platformInfo.platformID);
    free(platformInfo.manufacturerName);
    free(platformInfo.manufacturerUrl);
    free(platformInfo.modelNumber);
    free(platformInfo.dateOfManufacture);
    free(platformInfo.platformVersion);
    free(platformInfo.operatingSystemVersion);
    free(platformInfo.hardwareVersion);
    free(platformInfo.firmwareVersion);
    free(platformInfo.supportUrl);
    free(platformInfo.systemTime);
}

bool DuplicateString(char** targetString, const char* sourceString)
{
    if(!sourceString)
    {
        return false;
    }
    else
    {
        *targetString = (char *) malloc(strlen(sourceString) + 1);

        if(*targetString)
        {
            strncpy(*targetString, sourceString, (strlen(sourceString) + 1));
            return true;
        }
    }
    return false;
}

OCStackResult SetPlatformInfo(const char* platformID, const char *manufacturerName,
    const char *manufacturerUrl, const char *modelNumber, const char *dateOfManufacture,
    const char *platformVersion, const char* operatingSystemVersion, const char* hardwareVersion,
    const char *firmwareVersion, const char* supportUrl, const char* systemTime)
{

    bool success = true;

    if(manufacturerName != NULL && (strlen(manufacturerName) > MAX_PLATFORM_NAME_LENGTH))
    {
        return OC_STACK_INVALID_PARAM;
    }

    if(manufacturerUrl != NULL && (strlen(manufacturerUrl) > MAX_PLATFORM_URL_LENGTH))
    {
        return OC_STACK_INVALID_PARAM;
    }

    if(!DuplicateString(&platformInfo.platformID, platformID))
    {
        success = false;
    }

    if(!DuplicateString(&platformInfo.manufacturerName, manufacturerName))
    {
        success = false;
    }

    if(!DuplicateString(&platformInfo.manufacturerUrl, manufacturerUrl))
    {
        success = false;
    }

    if(!DuplicateString(&platformInfo.modelNumber, modelNumber))
    {
        success = false;
    }

    if(!DuplicateString(&platformInfo.dateOfManufacture, dateOfManufacture))
    {
        success = false;
    }

    if(!DuplicateString(&platformInfo.platformVersion, platformVersion))
    {
        success = false;
    }

    if(!DuplicateString(&platformInfo.operatingSystemVersion, operatingSystemVersion))
    {
        success = false;
    }

    if(!DuplicateString(&platformInfo.hardwareVersion, hardwareVersion))
    {
        success = false;
    }

    if(!DuplicateString(&platformInfo.firmwareVersion, firmwareVersion))
    {
        success = false;
    }

    if(!DuplicateString(&platformInfo.supportUrl, supportUrl))
    {
        success = false;
    }

    if(!DuplicateString(&platformInfo.systemTime, systemTime))
    {
        success = false;
    }

    if(success)
    {
        return OC_STACK_OK;
    }

    DeletePlatformInfo();
    return OC_STACK_ERROR;
}

#define VERIFY_SUCCESS(op)                          \
{                                                   \
    if (op !=  OC_STACK_OK)                         \
    {                                               \
        OIC_LOG_V(FATAL, TAG, "%s failed!!", #op);  \
        goto exit;                                  \
    }                                               \
}

OCStackResult SetDeviceInfo()
{
    OCResourceHandle resourceHandle = OCGetResourceHandleAtUri(OC_RSRVD_DEVICE_URI);
    if (resourceHandle == NULL)
    {
        OIC_LOG(ERROR, TAG, "Device Resource does not exist.");
        goto exit;
    }

	VERIFY_SUCCESS(OCBindResourceTypeToResource(resourceHandle, gDeviceType));
	VERIFY_SUCCESS(OCSetPropertyValue(PAYLOAD_TYPE_DEVICE, OC_RSRVD_DEVICE_NAME, gDeviceName));
    VERIFY_SUCCESS(OCSetPropertyValue(PAYLOAD_TYPE_DEVICE, OC_RSRVD_SPEC_VERSION, gSpecVersion));

//    VERIFY_SUCCESS(OCSetPropertyValue(PAYLOAD_TYPE_DEVICE, OC_RSRVD_DATA_MODEL_VERSION,
//                                      gDataModelVersions));
//    VERIFY_SUCCESS(OCSetPropertyValue(PAYLOAD_TYPE_DEVICE, OC_RSRVD_PROTOCOL_INDEPENDENT_ID,
//                                      gProtocolIndependentID));

    OIC_LOG(INFO, TAG, "Device information(Smart Watch) initialized successfully.");

    return OC_STACK_OK;

exit:
    return OC_STACK_ERROR;
}


static bool _on_create_cb(void *user_data)
{
	/*
	 * This area will be auto-generated when you add or delete user_view
	 * Please do not hand edit this area. The code may be lost.
	 */
	uib_app_manager_st* app_manager = uib_app_manager_get_instance();

	app_manager->initialize();
	/*
	 * End of area
	 */

//	sensor_recorder_is_supported(SENSOR_HRM, &hrm_supported);
//	if (!hrm_supported) {
//		/* Pedometer is not supported or cannot be recorded on the current device */
//		dlog_print(DLOG_INFO, LOG_TAG, "HRM not supported.");
//	} else {
//		sensor_recorder_create_option(&option);
//		/* 24 hours (1 day) */
//		sensor_recorder_option_set_int(option, SENSOR_RECORDER_OPTION_RETENTION_PERIOD, 24);
//
//		sensor_recorder_start(SENSOR_HRM, option);
//		dlog_print(DLOG_INFO, LOG_TAG, "HRM STARTED");
//	}

	dlog_print(DLOG_INFO, LOG_TAG, "OCStack Init START!");
	// Initialize Persistent Storage for SVR database
	//OCPersistentStorage ps = { server_fopen, fread, fwrite, fclose, unlink };
	//OCRegisterPersistentStorageHandler(&ps);

	if (OCInit(NULL, 0, OC_SERVER) != OC_STACK_OK)
	{
		dlog_print(DLOG_INFO, LOG_TAG, "OCStack Init Error!");
	}
	dlog_print(DLOG_INFO, LOG_TAG, "OCStack Init END!");

	OCStackResult registrationResult =
	    SetPlatformInfo(gPlatformID, gManufacturerName, gManufacturerLink, gModelNumber,
	                    gDateOfManufacture, gPlatformVersion, gOperatingSystemVersion,
	                    gHardwareVersion, gFirmwareVersion, gSupportLink, gSystemTime);
	if (registrationResult != OC_STACK_OK)
	{
		OIC_LOG(INFO, TAG, "Platform info setting failed locally!");
		exit (EXIT_FAILURE);
	}

	registrationResult = OCSetPlatformInfo(platformInfo);
	if (registrationResult != OC_STACK_OK)
	{
		OIC_LOG(INFO, TAG, "Platform Registration failed!");
		exit (EXIT_FAILURE);
	} else {
		OIC_LOG(INFO, TAG, "Platform information initialized successfully.");
	}

	registrationResult = SetDeviceInfo();
	if (registrationResult != OC_STACK_OK)
	{
		OIC_LOG(INFO, TAG, "Device Registration failed!");
		exit (EXIT_FAILURE);
	} else {
		OIC_LOG(INFO, TAG, "Device Registration OK!");
	}

	createUserIdResource("watch_unique_example_id", "watch_OCF2017");
	createClockResource("watch_unique_example_id");
	createHeartrateResource();
	dlog_print(DLOG_INFO, LOG_TAG, "OCStack Create Resource END!");

	pthread_t p_thread;
	int thread_id;

	char pn[] = "iotivity_thread";
	thread_id = pthread_create(&p_thread, NULL, iotivityThread, (void *)pn);
	if (thread_id < 0)
	{
		dlog_print(DLOG_INFO, LOG_TAG, "iotivity_thread create error");
	}

	pthread_t p_thread1;
	int thread_id1;
	char pn1[] = "iotivity_thread";
	thread_id1 = pthread_create(&p_thread1, NULL, iotivityObserveThread, (void *)pn1);
	if (thread_id1 < 0)
	{
		dlog_print(DLOG_INFO, LOG_TAG, "iotivityObserveThread create error");
	}

	dlog_print(DLOG_INFO, LOG_TAG, "IoTivity Started");


	return true;
}

static void _on_terminate_cb(void *user_data)
{
//	sensor_recorder_stop(SENSOR_HRM);
//	dlog_print(DLOG_INFO, LOG_TAG, "HRM STOPPED");
	uib_views_get_instance()->destroy_window_obj();
}

static void _on_resume_cb(void *user_data)
{
	/* Take necessary actions when application becomes visible. */
}

static void _on_pause_cb(void *user_data)
{
	/* Take necessary actions when application becomes invisible. */
}

static void _on_app_control_cb(app_control_h app_control, void *user_data)
{
	/* Handle the launch request. */
}

static void _on_low_memory_cb(app_event_info_h event_info, void *user_data)
{
	/* Take necessary actions when the system runs low on memory. */
}

static void _on_low_battery_cb(app_event_info_h event_info, void *user_data)
{
	/* Take necessary actions when the battery is low. */
}

static void _on_device_orientation_cb(app_event_info_h event_info, void *user_data)
{
	/* deprecated APIs */
}

static void _on_language_changed_cb(app_event_info_h event_info, void *user_data)
{
	/* Take necessary actions is called when language setting changes. */
	uib_views_get_instance()->uib_views_current_view_redraw();
}

static void _on_region_format_changed_cb(app_event_info_h event_info, void *user_data)
{
	/* Take necessary actions when region format setting changes. */
}

