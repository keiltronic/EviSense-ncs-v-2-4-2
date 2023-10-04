// /**
//  * @file cloud.c
//  * @author Thomas Keilbach | keiltronic GmbH
//  * @date 27 Oct 2022
//  * @brief This file manges the connection to Bosch IoT cloud
//  * @version 1.0.0
//  */

// /*!
//  * @defgroup Cloud
//  * @brief This file manges the connection to Bosch IoT cloud
//  * @{*/

// /* https://github.com/protobuf-c/protobuf-c/wiki/Examples */

// #include "cloud.h"

// int64_t memerror_upd_tsp = 0LL;
// uint8_t Cloud_TestCredentials = false;
// time_t timestamp_last_cloud_transmission = 0;
// uint32_t coap_last_transmission_timer = 0;

// /* Create UsageUpdate object which is send in first sync interval */
// UsageUpdate myUsageUpdate = USAGE_UPDATE__INIT;
// DeviceStatus myDeviceStatus = DEVICE_STATUS__INIT;
// PackageDevice2Hub myPackageDevice2Hub = PACKAGE_DEVICE2_HUB__INIT;
// ProtobufCBinaryData dataupdate_uuid;
// ProtobufCBinaryData hubupdate_uuid;
// ProtobufCBinaryData imei;
// ProtobufCBinaryData current_location_epc;
// ProtobufCBinaryData fw;
// uint8_t fw_version[3];

// /**
//  * @brief Updates the DeviceStatusObject with the latest information
//  * @param DeviceStatusObject: Pointer to a DeviceStatusObject
//  */
// void protobuf_UpdateDeviceStatusData(DeviceStatus *DeviceStatusObject)
// {
//   /* Add time stamp */
//   DeviceStatusObject->status_timestamp = unixtime_ms;

//   /* Add IMEI number */
//   imei.len = 15;
//   imei.data = modem.IMEI;
//   DeviceStatusObject->device_imei = imei;

//   /* Add site id */
//   DeviceStatusObject->site_id = 0; // Unique ID of the building for which the Device is registered, this value should be stored on board of the Device and can be updated during sync session local storage data update. This parameter is first loaded to the Device at building implementation phase, again, using the same device's local storage update via sync session.

//   /* Add room id */
//   DeviceStatusObject->room_id = current_room_record.room_id;

//   /* Add location EPC */
//   current_location_epc.len = 20;
//   current_location_epc.data = current_location_epc_string;
//   DeviceStatusObject->location_epc = current_location_epc; // binary_data is written with location data in Event0x07

//   /* Add mob id */
//   DeviceStatusObject->mop_id = current_room_to_mop_mapping.current_mop_id;

//   /* Add frame side */
//   DeviceStatusObject->frame_side = Frame_side[0];

//   /* Add mopping pattern */
//   DeviceStatusObject->pattern_id = mopping_pattern[0];

//   /* Add battery charge status*/
//   DeviceStatusObject->battery_charge = (uint8_t)battery.StateOfCharge;

//   /* Add battery charging flag */
//   DeviceStatusObject->is_charging = (uint32_t)System.ChargeStatus;

//   /* Add battery life time (age) */
//   if (battery.age <= 100.0)
//   {
//     DeviceStatusObject->battery_lifetime = (uint8_t)battery.age;
//   }
//   else
//   {
//     DeviceStatusObject->battery_lifetime = 100;
//   }

//   /* Add on board temp */
//   DeviceStatusObject->env_temp = modem.temp;

//   /* Add cell signal */
//   DeviceStatusObject->cell_signal = get_signal_quality();

//   /* Add firmware version */
//   fw_version[0] = (uint8_t)Device.FirmwareMajorVersion;
//   fw_version[1] = (uint8_t)Device.FirmwareMinorVersion;
//   fw_version[2] = (uint8_t)Device.FirmwareInternVersion;

//   fw.len = 3;
//   fw.data = fw_version;

//   DeviceStatusObject->firmware = fw;

//   /* Print out debug messages */
//   if (Parameter.debug == true || Parameter.protobuf_verbose == true)
//   {
//     rtc_print_debug_timestamp();
//     uint16_t len = 0; // Length of serialized data

//     len = device_status__get_packed_size(DeviceStatusObject);
//     shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "DeviceStatus packed size: %d bytes\n", len);
//   }
// }

// /**
//  * @brief Triggers protobuf pack function and send the packed protobuf data to the cloud (with DTLS CoAP)
//  *
//  */
// uint32_t protobuf_EncodeUsageUpdateObject(uint8_t **buf)
// {
//   /* https://github.com/protobuf-c/protobuf-c/wiki/Examples */
//   uint16_t len = 0;
//   uint16_t i = 0;
//   uint8_t *memory = NULL;

//   /* Point myPackageDevice2Hub to myPackageDevice2Hub object*/
//   myPackageDevice2Hub.usage_update_message = &myUsageUpdate;

//   dataupdate_uuid.len = 5;
//   dataupdate_uuid.data = "12345";

//   hubupdate_uuid.len = 5;
//   hubupdate_uuid.data = "67890";

//   myPackageDevice2Hub.confirm_dataupdate_uuid = dataupdate_uuid;
//   myPackageDevice2Hub.confirm_hubupdate_uuid = hubupdate_uuid;
//   myPackageDevice2Hub.contains_usageupdate = 1;

//   /* Point DeviceStatus and EventArray objects to UsageUpdate object */
//   myUsageUpdate.device_status = &myDeviceStatus;
//   myUsageUpdate.device_events = &myEventArray;

//   /* Update data in DeviceStatus object */
//   protobuf_UpdateDeviceStatusData(&myDeviceStatus);

//   /* Print out debug messages */
//   if (Parameter.debug == true || Parameter.protobuf_verbose == true)
//   {
//     len = event_array__get_packed_size(&myEventArray);
//     rtc_print_debug_timestamp();
//     shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "EventArray packed size: %d bytes\n", len);
//   }

//   /* Serialize message */
//   len = package_device2_hub__get_packed_size(&myPackageDevice2Hub);

//   /* Print out debug messages */
//   if (Parameter.debug == true || Parameter.protobuf_verbose == true)
//   {
//     rtc_print_debug_timestamp();
//     shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "myPackageDevice2Hub packed size: %d bytes\n", len);
//   }

//   /* Serialize message */
//   memory = malloc(len);

//   if (memory != NULL)
//   {
//     /* Serialize message */
//     package_device2_hub__pack(&myPackageDevice2Hub, memory); // Pack (serialize) msg, including submessages

//     /* Print out debug messages */
//     if (Parameter.debug == true || Parameter.protobuf_verbose == true)
//     {
//       rtc_print_debug_timestamp();
//       shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "Total serialized length: %d bytes bytes\n", len);
//       rtc_print_debug_timestamp();
//       shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "Serialized message:\n");

//       for (i = 0; i < len; i++)
//       {
//         shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, " 0x%02X", memory[i]);
//       }
//       shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "\n");
//     }

//     *buf = memory;
//   }
//   else
//   {
//     //***************** TL new input 05.03.22 bug fix for memory allocation *******************************************
//     shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_RED, "Error: Can not allocate memory for UsageObject, need %d bytes, but only %d available on heap\n", len, memcheck_heap_freespace());

//     /* Clear EventArray (free all allocated memory to clean up heap memory) and reset last seen location strings */
//     Event_ClearArray();
//     // clear_last_seen_location_record_array();

//     if ((unixtime_ms - memerror_upd_tsp) > 2 * Parameter.cloud_sync_interval_idle)
//     {
//       rtc_print_debug_timestamp();
//       shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_RED, "System will reboot, it can not allocate memory for coap UsageObject.\n");

//       Device_PushRAMToFlash();
//       lte_lc_power_off();

//       k_msleep(1000); // Delay the reboot to give the system enough time to ouput the debug message on console

//       sys_reboot(0);
//     }
//     memerror_upd_tsp = unixtime_ms;
//   }

//   return len;
// }

// /**
//  * @brief Triggers protobuf pack function and send the packed protobuf data to the cloud (with DTLS CoAP)
//  *
//  */
// void cloud_SendUsageUpdateObject(void)
// {
//   uint8_t *payload = NULL;
//   uint16_t len = 0;
//   uint8_t rslt = 0;
//   uint32_t message_count = 0;

//   /* Pack UsageObject in RAM inlcuding all events in a protobuf object */
//   if (Parameter.protobuf_verbose == true)
//   {
//     rtc_print_debug_timestamp();
//     shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_RED, "Send protobuf message from RAM array in blocks\n", EVENT_MAX_ITEMS_IN_ARRAY, Event_NumberOfOutsourcedMessages);
//   }

//   /* ############# OUTSOURCED EVENT MESSAGES FROM FLASH MEMORY ##############*/

//   /* Send payload as CoAP message and delete then the allocated memories (call Event_ClearArray)
//      payload contains 64 events in maximum, more then 64 events gets send in another CoAP message */
//   while ((message_count < Event_NumberOfOutsourcedMessages) && (MoppingFlag[0] == 0))
//   {
//     if (Parameter.protobuf_verbose == true)
//     {
//       rtc_print_debug_timestamp();
//       shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_YELLOW, "Fetching (%d events per block, byte length %d, flash addr: 0x%X) protobuf messsage no. %d/%d from external flash and send them in blocks\n", EVENT_MAX_ITEMS_IN_ARRAY, Event_ListOfOutsourcedMessages[message_count].length, EVENT_MEM + Event_ListOfOutsourcedMessages[message_count].start_address, message_count + 1, Event_NumberOfOutsourcedMessages);
//     }

//     rslt = Event_ReadFromFlash(Event_ListOfOutsourcedMessages[message_count].start_address, Event_ListOfOutsourcedMessages[message_count].length, &payload);

//     if (rslt == true)
//     {
//       k_msleep(10);
//       send_coap_request(COAP_METHOD_POST, payload, Event_ListOfOutsourcedMessages[message_count].length);

//       if (Parameter.events_verbose)
//       {
//         rtc_print_debug_timestamp();
//         shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_YELLOW, "Sent outsourced message no. %d of %d\n", message_count + 1, Event_NumberOfOutsourcedMessages);
//       }

//       message_count++;

//       /* Free the allocated serialized buffer */
//       free(payload);
//     }
//     else
//     {
//       rtc_print_debug_timestamp();
//       shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_RED, "Could not allocate memory to import protbuf message from external flash\n");
//       break;
//     }

//     k_msleep(100);
//   }

//   /* Clear list of outsourced messages */
//   for (uint32_t j = 0; j < Event_NumberOfOutsourcedMessages; j++)
//   {
//     Event_ListOfOutsourcedMessages[j].start_address = 0UL;
//     Event_ListOfOutsourcedMessages[j].length = 0UL;
//   }

//   message_count = 0;
//   Event_NumberOfOutsourcedMessages = 0;

//   /* ########### LOCAL EVENT MESSAGES STORED IN RAM ############# */
//   len = protobuf_EncodeUsageUpdateObject(&payload); // payload holds the packed protobuf message (UsageUpdate object)
 

//   send_coap_request(COAP_METHOD_POST, payload, len);

//   /* Free the allocated serialized buffer */
//   free(payload);

//   /* Reset variables */
//   time_since_last_cloud_transmission = 0;
//   timestamp_last_cloud_transmission = unixtime_ms;
//   coap_last_transmission_timer = 0;

//   /* Clear EventArray (free all allocated memory to clean up heap memory) and reset last seen location strings */
//   Event_ClearArray();
//   clear_last_seen_location_record_array();

//   /* Check for messages from cloud*/
//   // process_large_coap_reply();
// }

// /**
//  * @brief Decodes a received UsageUpdate protobuf messages
//  *
//  * @param message: Pointer to a the message
//  * @param len: Length of the received message
//  */
// void cloud_DecodeUsageUpdateProtobuf(uint8_t *message, uint16_t len)
// {
//   UsageUpdate *ptrReceivedUsageUpdate;
//   DeviceStatus *ptrMyDeviceStatus;
//   EventArray *ptrMyEventArray;

//   shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "%d bytes received. Encoded message:", len);
//   for (uint32_t i = 0; i < len; i++)
//   {
//     shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, " 0x%02X", *(message + (i * sizeof(uint8_t))));
//   }
//   shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "\n");

//   // Unpack the message using protobuf-c.
//   ptrReceivedUsageUpdate = usage_update__unpack(NULL, len, message); // Deserialize the serialized input

//   if (ptrReceivedUsageUpdate == NULL)
//   {
//     shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_RED, "error unpacking incoming message\n");
//   }
//   else
//   {
//     // Point submessages to correct message
//     ptrMyDeviceStatus = ptrReceivedUsageUpdate->device_status;
//     ptrMyEventArray = ptrReceivedUsageUpdate->device_events;

//     shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "Received: timestamp=%lld\n", ptrMyDeviceStatus->status_timestamp);
//     shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "Received: frame side=%d\n", ptrMyDeviceStatus->frame_side);
//     shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "Received: cell signal=%d\n", ptrMyDeviceStatus->cell_signal);
//     shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "Received: env_temp=%d\n", ptrMyDeviceStatus->env_temp);
//     shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "Received: battery_charge=%d\n", ptrMyDeviceStatus->battery_charge);
//     //    shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "Received: event=%d\n", ptrMyEventArray->event_message[0]->one_of_generic_event_case);
//   }

//   usage_update__free_unpacked(ptrReceivedUsageUpdate, NULL);
// }

// /*!
//  * @brief Decodes a received HubUpdate protobuf messages
//  * @param message: Pointer to a the message
//  * @param len: Length of the received message
//  */
// void cloud_DecodeHubUpdateProtobuf(uint8_t *message, uint16_t len)
// {
//   // Create pointer to message types
//   HubUpdate *ptrHubUpdate;
//   HubCommands *ptrHubCommands;
//   HubNotifications *ptrHubNotifications;

//   shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "%d bytes received. Encoded message:", len);

//   for (uint32_t i = 0; i < len; i++)
//   {
//     shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, " 0x%02X", *(message + (i * sizeof(uint8_t))));
//   }
//   shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "\n");

//   // Unpack the message using protobuf-c.
//   ptrHubUpdate = hub_update__unpack(NULL, len, message); // Deserialize the serialized input

//   if (ptrHubUpdate == NULL)
//   {
//     shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "error unpacking incoming message\n");
//   }
//   else
//   {
//     // Point submessages to correct message
//     ptrHubCommands = ptrHubUpdate->hub_commands;
//     ptrHubNotifications = ptrHubUpdate->hub_notifications;

//     //   shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "Received: timestamp=%lld\n", ptrMyDeviceStatus->status_timestamp);
//     //   shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "Received: frame side=%d\n", ptrMyDeviceStatus->frame_side);
//     //   shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "Received: cell signal=%d\n", ptrMyDeviceStatus->cell_signal);
//     //   shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "Received: env_temp=%d\n", ptrMyDeviceStatus->env_temp);
//     //   shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "Received: battery_charge=%d\n", ptrMyDeviceStatus->battery_charge);
//     //  //    shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "Received: event=%d\n", ptrMyEventArray->event_message[0]->one_of_generic_event_case);
//   }

//   hub_update__free_unpacked(ptrHubUpdate, NULL);
// }

// /*!
//  * @brief Decodes a received DataUpdate protobuf messages
//  * @param message: Pointer to a the message
//  * @param len: Length of the received message
//  */
// void cloud_DecodeDataUpdateProtobuf(uint8_t *message, uint16_t len)
// {
//   // Create pointer to message types
//   DataUpdate *ptrDataUpdate;
//   LocalData *ptrLocalData;

//   shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "%d bytes received. Encoded message:", len);

//   for (uint32_t i = 0; i < len; i++)
//   {
//     shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, " 0x%02X", *(message + (i * sizeof(uint8_t))));
//   }
//   shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "\n");

//   // Unpack the message using protobuf-c.
//   ptrDataUpdate = data_update__unpack(NULL, len, message); // Deserialize the serialized input

//   if (ptrDataUpdate == NULL)
//   {
//     shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "error unpacking incoming message\n");
//   }
//   else
//   {

//     //// Point submessages to correct message
//     // ptrHubCommands = ptrHubUpdate->hub_commands;
//     // ptrHubNotifications = ptrHubUpdate->hub_notifications;

//     //   shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "Received: timestamp=%lld\n", ptrMyDeviceStatus->status_timestamp);
//     //   shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "Received: frame side=%d\n", ptrMyDeviceStatus->frame_side);
//     //  shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "Received: cell signal=%d\n", ptrMyDeviceStatus->cell_signal);
//     //   shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "Received: env_temp=%d\n", ptrMyDeviceStatus->env_temp);
//     //   shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "Received: battery_charge=%d\n", ptrMyDeviceStatus->battery_charge);
//     //  //   printk("Received: event=%d\n", ptrMyEventArray->event_message[0]->one_of_generic_event_case);
//   }

//   data_update__free_unpacked(ptrDataUpdate, NULL);
// }
