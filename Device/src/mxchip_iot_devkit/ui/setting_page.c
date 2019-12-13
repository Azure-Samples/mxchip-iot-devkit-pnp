#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "SystemWeb.h"
#include "EEPROMInterface.h"
#include "IoT_DevKit_HW.h"

//////////////////////////////////////////////////////////////////////////////////////////////
// Azure IoT hub device settings - sas token for IoT Central
#define DPS_DEFAULT_ENDPOINT "global.azure-devices-provisioning.net"

static WEB_PAGE_SETTING_ITEM az_iot_sas_token_items[] = 
{
    {
        "DeviceId",
        "Device ID",
        ITEM_INPUT_TEXT,
        NULL,
        NULL,
        128,
        NULL,
        0
    },
    {
        "IdScope",
        "ID Scope",
        ITEM_INPUT_TEXT,
        NULL,
        NULL,
        32,
        NULL,
        0
    },
    {
        "PrimaryKey",
        "SAS Primary Key",
        ITEM_INPUT_PASSWORD,
        NULL,
        NULL,
        128,
        "Primary Key",
        0
    }
};

static int process_az_sas_token_string(WEB_PAGE_SETTINGS *settings);

static WEB_PAGE_SETTINGS az_iot_sas_token_settings = 
{
    "Azure IoT Central Settings",
    sizeof(az_iot_sas_token_items) / sizeof(WEB_PAGE_SETTING_ITEM),
    az_iot_sas_token_items,
    process_az_sas_token_string
};

static int process_az_sas_token_string(WEB_PAGE_SETTINGS *settings)
{
    if (settings != &az_iot_sas_token_settings)
    {
        return -1;
    }
    
    if (settings->items[0].value_text == NULL || settings->items[0].value_text[0] == 0
        || settings->items[1].value_text == NULL || settings->items[1].value_text[1] == 0
        || settings->items[2].value_text == NULL || settings->items[2].value_text[2] == 0)
    {
        // One value is empty, the whole symmetric key setting is emtpy
        return 0;
    }

    char *device_connection_string = (char*)calloc(AZ_IOT_HUB_MAX_LEN, 1);
    if (device_connection_string == NULL)
    {
        return -1;
    }
    snprintf(device_connection_string, AZ_IOT_HUB_MAX_LEN, 
             "DPSEndpoint=%s;IdScope=%s;DeviceId=%s;SymmetricKey=%s",
             DPS_DEFAULT_ENDPOINT,
             settings->items[1].value_text,
             settings->items[0].value_text,
             settings->items[2].value_text);
    if (saveIoTHubConnectionString(device_connection_string))
    {
        settings->items[0].result = SETTING_RESULT_SAVE_FAILED;
        settings->items[1].result = SETTING_RESULT_SAVE_FAILED;
        settings->items[2].result = SETTING_RESULT_SAVE_FAILED;
    }
    else
    {
        settings->items[0].result = SETTING_RESULT_SAVED;
        settings->items[1].result = SETTING_RESULT_SAVED;
        settings->items[2].result = SETTING_RESULT_SAVED;
    }
    free(device_connection_string);
    return 0;
}

void enable_sas_token_setting(void)
{
    SystemWebAddSettings(&az_iot_sas_token_settings);
}