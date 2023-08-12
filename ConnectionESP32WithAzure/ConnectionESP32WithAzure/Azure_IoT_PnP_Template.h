#ifndef AZURE_IOT_PNP_TEMPLATE_H
#define AZURE_IOT_PNP_TEMPLATE_H

#include "AzureIoT.h"
void azure_pnp_init();
const az_span azure_pnp_get_model_id();

int azure_pnp_send_device_info(azure_iot_t* azure_iot, uint32_t request_id);                         
void azure_pnp_set_telemetry_frequency(size_t frequency_in_seconds);
int azure_pnp_send_telemetry(azure_iot_t* azure_iot);
int azure_pnp_handle_command_request(azure_iot_t* azure_iot, command_request_t command_request);
int azure_pnp_handle_properties_update(azure_iot_t* azure_iot, az_span properties, uint32_t request_id);

void initSensors();

#endif
