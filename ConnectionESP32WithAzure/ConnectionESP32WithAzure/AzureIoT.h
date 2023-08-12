#ifndef AZURE_IOT_H
#define AZURE_IOT_H

#include <stdlib.h>
#include <stdint.h>
#include <time.h>

#include <az_core.h>
#include <az_iot.h>

#define lengthof(s) (sizeof(s) - 1)
#define sizeofarray(a) (sizeof(a) / sizeof(a[0]))
#define STR_HELPER(x) #x
#define STR(x) STR_HELPER(x)
static const uint8_t null_terminator = '\0';


#define INDEFINITE_TIME ((time_t)-1)

#ifndef DISABLE_LOGGING
typedef enum log_level_t_enum
{
  log_level_info,
  log_level_error
}
log_level_t;

typedef void (*log_function_t)(log_level_t log_level, char const* const format, ...);

extern log_function_t default_logging_function;

#define set_logging_function(custom_logging_function) \
  default_logging_function = custom_logging_function;

#define Log(level, message, ...) default_logging_function(level, message, ##__VA_ARGS__ )
#define LogInfo(message, ...) Log(log_level_info, message, ##__VA_ARGS__ )
#define LogError(message, ...) Log(log_level_error, message, ##__VA_ARGS__ )
#else
#define set_logging_function(custom_logging_function) 
#define Log(level, message, ...) 
#define LogInfo(message, ...)
#define LogError(message, ...)
#endif // DISABLE_LOGGING



#define DPS_GLOBAL_ENDPOINT_FQDN          "global.azure-devices-provisioning.net"
#define DPS_GLOBAL_ENDPOINT_PORT          AZ_IOT_DEFAULT_MQTT_CONNECT_PORT
#define IOT_HUB_ENDPOINT_PORT             AZ_IOT_DEFAULT_MQTT_CONNECT_PORT

#define DEFAULT_SAS_TOKEN_LIFETIME_IN_MINUTES 60
#define SAS_TOKEN_REFRESH_THRESHOLD_IN_SECS 30 


#define MQTT_QOS_AT_MOST_ONCE  0
#define MQTT_QOS_AT_LEAST_ONCE 1
#define MQTT_QOS_EXACTLY_ONCE  2

typedef enum mqtt_qos_t_enum
{
  mqtt_qos_at_most_once = MQTT_QOS_AT_MOST_ONCE,
  mqtt_qos_at_least_once = MQTT_QOS_AT_LEAST_ONCE,
  mqtt_qos_exactly_once = MQTT_QOS_EXACTLY_ONCE
}
mqtt_qos_t;
typedef struct mqtt_message_t_struct
{
  az_span topic;
  az_span payload;
  mqtt_qos_t qos;
}
mqtt_message_t;


typedef struct mqtt_client_config_t_struct
{
 
  az_span address;
  int port;
  az_span client_id;
  az_span username;
  az_span password;
}
mqtt_client_config_t;

typedef void* mqtt_client_handle_t;
typedef int (*mqtt_client_init_function_t)(mqtt_client_config_t* mqtt_client_config, mqtt_client_handle_t* mqtt_client_handle);
typedef int (*mqtt_client_deinit_function_t)(mqtt_client_handle_t mqtt_client_handle);
typedef int (*mqtt_client_publish_function_t)(mqtt_client_handle_t mqtt_client_handle, mqtt_message_t* mqtt_message);
typedef int (*mqtt_client_subscribe_function_t)(mqtt_client_handle_t mqtt_client_handle, az_span topic, mqtt_qos_t qos);
typedef struct mqtt_client_interface_t_struct
{
  mqtt_client_init_function_t mqtt_client_init;
  mqtt_client_deinit_function_t mqtt_client_deinit;
  mqtt_client_publish_function_t mqtt_client_publish;
  mqtt_client_subscribe_function_t mqtt_client_subscribe;
}
mqtt_client_interface_t;
typedef int (*base64_decode_function_t)(uint8_t* data, size_t data_length, uint8_t* decoded, size_t decoded_size, size_t* decoded_length);
typedef int (*base64_encode_function_t)(uint8_t* data, size_t data_length, uint8_t* encoded, size_t encoded_size, size_t* encoded_length);
typedef int (*hmac_sha256_encryption_function_t)(const uint8_t* key, size_t key_length, const uint8_t* payload, size_t payload_length, uint8_t* encrypted_payload, size_t encrypted_payload_size);
typedef struct data_manipulation_functions_t_struct
{
  base64_decode_function_t base64_decode;
  base64_encode_function_t base64_encode;
  hmac_sha256_encryption_function_t hmac_sha256_encrypt;
}
data_manipulation_functions_t;
typedef void (*properties_update_completed_t)(uint32_t request_id, az_iot_status status_code);


typedef void (*properties_received_t)(az_span properties);
typedef struct command_request_t_struct
{

  az_span request_id;
  az_span component_name;
  az_span command_name;
  az_span payload;
}
command_request_t;
typedef void (*command_request_received_t)(command_request_t command);
typedef enum azure_iot_status_t_struct
{
  azure_iot_disconnected,
  azure_iot_connecting,
  azure_iot_connected,
  azure_iot_error
}
azure_iot_status_t;
typedef enum azure_iot_client_state_t_struct
{
  azure_iot_state_not_initialized = 0,
  azure_iot_state_initialized,
  azure_iot_state_started,
  azure_iot_state_connecting_to_dps,
  azure_iot_state_connected_to_dps,
  azure_iot_state_subscribing_to_dps,
  azure_iot_state_subscribed_to_dps,
  azure_iot_state_provisioning_querying,
  azure_iot_state_provisioning_waiting,
  azure_iot_state_provisioned,
  azure_iot_state_connecting_to_hub,
  azure_iot_state_connected_to_hub,
  azure_iot_state_subscribing_to_pnp_cmds,
  azure_iot_state_subscribed_to_pnp_cmds,
  azure_iot_state_subscribing_to_pnp_props,
  azure_iot_state_subscribed_to_pnp_props,
  azure_iot_state_subscribing_to_pnp_writable_props,
  azure_iot_state_ready,
  azure_iot_state_refreshing_sas,
  azure_iot_state_error
}
azure_iot_client_state_t;
typedef struct azure_iot_config_t_struct
{
  az_span user_agent;
  bool use_device_provisioning;
  az_span iot_hub_fqdn;
  az_span device_id;
  az_span device_key;
  az_span device_certificate;
  az_span device_certificate_private_key;
  az_span dps_registration_id;
  this member MUST be set with AZ_SPAN_EMPTY.
  az_span dps_id_scope;
  az_span model_id;
  az_span data_buffer;
  mqtt_client_interface_t mqtt_client_interface;
  data_manipulation_functions_t data_manipulation_functions;
  uint32_t sas_token_lifetime_in_minutes;
  properties_update_completed_t on_properties_update_completed;
  properties_received_t on_properties_received;
  command_request_received_t on_command_request_received;
}
azure_iot_config_t;
typedef struct azure_iot_t_struct
{
  azure_iot_config_t* config;
  az_span data_buffer;
  mqtt_client_handle_t mqtt_client_handle;
  az_iot_hub_client iot_hub_client;
  az_iot_hub_client_options iot_hub_client_options;
  az_iot_provisioning_client dps_client;
  azure_iot_client_state_t state;
  uint32_t sas_token_expiration_time;
  uint32_t dps_retry_after_seconds;
  uint32_t dps_last_query_time;
  az_span dps_operation_id;
}
azure_iot_t;
void azure_iot_init(azure_iot_t* azure_iot, azure_iot_config_t* azure_iot_config);
int azure_iot_start(azure_iot_t* azure_iot);
int azure_iot_stop(azure_iot_t* azure_iot);
azure_iot_status_t azure_iot_get_status(azure_iot_t* azure_iot);
void azure_iot_do_work(azure_iot_t* azure_iot);
int azure_iot_send_telemetry(azure_iot_t* azure_iot, az_span message);
int azure_iot_send_properties_update(azure_iot_t* azure_iot, uint32_t request_id, az_span message);
int azure_iot_send_command_response(azure_iot_t* azure_iot, az_span request_id, uint16_t response_status, az_span payload);
int azure_iot_mqtt_client_connected(azure_iot_t* azure_iot);
int azure_iot_mqtt_client_disconnected(azure_iot_t* azure_iot);
int azure_iot_mqtt_client_subscribe_completed(azure_iot_t* azure_iot, int packet_id);
int azure_iot_mqtt_client_publish_completed(azure_iot_t* azure_iot, int packet_id);
int azure_iot_mqtt_client_message_received(azure_iot_t* azure_iot, mqtt_message_t* mqtt_message);
az_span split_az_span(az_span span, int32_t size, az_span* remainder);
static az_span slice_and_copy_az_span(az_span destination, az_span source, az_span* remainder);

#endif 
