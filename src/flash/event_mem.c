// /**
//  * @file epc_mem.c
//  * @author Thomas Keilbach | keiltronic GmbH
//  * @date 11 Oct 2022
//  * @brief This file contains function to write, read and erase events which happend during the last shift.
//  * @version 1.0.0
//  */

// /*!
//  * @defgroup Memory
//  * @brief This file contains function to write, read and erase event data to and from the external flash memory
//  * @{*/
// #include "event_mem.h"

// EVENT_LIST_OF_OUTSOURCED_MESSAGES Event_ListOfOutsourcedMessages[EVENT_MAX_OUTSOURCED_MESSAGES];
// uint32_t Event_NumberOfOutsourcedMessages = 0;
// uint32_t Event_flash_write_head = 0UL;
// uint8_t event_clearing_in_progress = false;

// /*!
//  * @brief This functions writes a complete data frame (structe) in the external flash.
//  * @note This function talks directly over the SPI bus with the external NOR flash memory. For this it uses the API calls defined in flash.c
//  * @see flash.c
//  */
// void Event_StorePackedUsageObjectToFlash(void)
// {
//     uint8_t *payload = NULL;
//     uint32_t len = 0;

//     /* Pack UsageObject inlcuding all events in a protobuf object */
//     len = protobuf_EncodeUsageUpdateObject(&payload); // payload holds the packed protobuf message

//     /* Write data to flash */
//     if (len > 0)
//     {
//         if (Event_NumberOfOutsourcedMessages < EVENT_MAX_OUTSOURCED_MESSAGES)
//         {
//             if (Parameter.events_verbose)
//             {
//                 rtc_print_debug_timestamp();             
//                 shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_YELLOW, "Packing (protobuf) and outsourcing local event array in RAM to external flash memory. Length: %d bytes\n", len);         
//             }

//             if (Event_flash_write_head < (EVENT_MEM + EVENT_MEM_LENGTH))
//             {
//                 /* Erase sector if the address points to the first element of the sector */
//                 if (!(EVENT_MEM + Event_flash_write_head) % FLASH_SUBSUBSECTOR_SIZE)
//                 {
//                     flash_EraseSector_64kB(GPIO_PIN_FLASH_CS1, EVENT_MEM + Event_flash_write_head); 

//                     if (Parameter.events_verbose)
//                     {
//                         rtc_print_debug_timestamp();
//                         shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "Flash sector erase. Adress: 0x%X\n", Event_flash_write_head);
//                     }
//                 }
//                 flash_write(GPIO_PIN_FLASH_CS1, EVENT_MEM + Event_flash_write_head, payload, len);
//             }
//             else
//             {
//                 rtc_print_debug_timestamp();
//                 shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_RED, "Event flash memory is full.");
//             }

//             /* Store outsourced message information in list array */
//             Event_NumberOfOutsourcedMessages++;
//             Event_ListOfOutsourcedMessages[Event_NumberOfOutsourcedMessages - 1].start_address = Event_flash_write_head;
//             Event_ListOfOutsourcedMessages[Event_NumberOfOutsourcedMessages - 1].length = len;

//             if (Parameter.events_verbose)
//             {
//                 rtc_print_debug_timestamp();
//                 shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_YELLOW, "Outsourced messages: %d, start addr: 0x%X, length: %d\n", Event_NumberOfOutsourcedMessages, EVENT_MEM + Event_ListOfOutsourcedMessages[Event_NumberOfOutsourcedMessages - 1].start_address, Event_ListOfOutsourcedMessages[Event_NumberOfOutsourcedMessages - 1].length);
//             }
//         }
//         else
//         {
//             if (Parameter.events_verbose)
//             {
//                 rtc_print_debug_timestamp();
//                 shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_RED, "Reached maximum number of %d outsourced messages", EVENT_MAX_OUTSOURCED_MESSAGES);
//             }
//         }

//         /* Update address in flash for next write cycle */
//         Event_flash_write_head += (len + 1);

//         /* Free the allocated serialized buffer */
//         free(payload);
//     }
// }

// /*!
//  * @brief This functions reads event objects from the external flash memory and prints it on console.
//  * @note This function talks directly over the SPI bus with the external NOR flash memory. For this it uses the API calls defined in flash.c
//  * @see flash.c
//  */
// uint8_t Event_ReadFromFlash(uint32_t address, uint32_t length, uint8_t **buf)
// {
//     uint8_t *memory = NULL;

//     memory = malloc(length);

//     if (memory != NULL)
//     {
//         flash_read(GPIO_PIN_FLASH_CS1, EVENT_MEM + address, memory, length);
//         *buf = memory;
//         return true;
//     }
//     else
//     {
//         return false;
//     }
// }

// /*!
//  * @brief This functions clears the event memory in the external flash and reset the write address.
//  * @note This function talks directly over the SPI bus with the external NOR flash memory. For this it uses the API calls defined in flash.c
//  * @see flash.c
//  */
// void Event_ClearCompleteFlash(void)
// {
//     event_clearing_in_progress = true;

//     flash_ClearMemAll(GPIO_PIN_FLASH_CS1, EVENT_MEM, EVENT_MEM_LENGTH);
//     Event_flash_write_head = 0UL;

//     event_clearing_in_progress = false;
// }