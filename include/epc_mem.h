/**
 * @file epc_mem.h
 * @author Thomas Keilbach | keiltronic GmbH
 * @date 27 Oct 2022
 * @brief This file contains functions headers to communicate with the pheripherals
 * @version 1.0.0
 */

#ifndef EPC_MEM_H
#define EPC_MEM_H

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include "flash.h"
#include "rfid.h"
#include "algorithms.h"
#include "cloud.h"
#include "system_mem.h"

/* RFID record database settings */
#define RFID_RECORD_REGION 0x20000UL        // Start address of memory region
#define RFID_RECORD_REGION_LENGTH 0x2FFFFUL // Up to 5000  -> Total: 32 byte * 5000 = 160.000 byte
#define RFID_RECORD_BYTE_LENGTH 32          // In byte, must be a value of power of 2  (2^n)

/* ROOM record database settings */
#define ROOM_RECORD_REGION 0x50000UL        // Start address of memory region
#define ROOM_RECORD_REGION_LENGTH 0x1FFFFUL // Up to 500  -> Total: 16 byte * 500 = 8.000 byte
#define ROOM_RECORD_BYTE_LENGTH 16          // In byte, must be a value of power of 2  (2^n)

/* MOP record database settings */
#define MOP_RECORD_REGION 0x70000UL        // Start address of memory region
#define MOP_RECORD_REGION_LENGTH 0xFFFFUL  // Up to 5000  -> Total: 8 byte * 5000 = 40.000 byte
#define MOP_RECORD_BYTE_LENGTH 8           // In byte, must be a value of power of 2  (2^n)
#define ADVANCED_MOP_RECORD_BYTE_LENGTH 16 // In byte, must be a value of power of 2  (2^n)

/* EPC string settings */
#define EPC_RING_BUFFER_SIZE 64
#define EPC_LENGTH_MIN 32
#define EPC_LENGTH_MAX 41
#define EPC_STRING_LENGTH 41
#define EPC_TOTAL_HEX_LENGTH 20
#define EPC_LAST_SEEN_COUNT 20
#define ROOM_TO_MOP_MAP_BYTE_LENGTH 42
#define LAST_SEEN_AUTO_RESET_TIME 60000 // millisec
#define LAST_SEEN_LOCATION_ARRAY_SIZE 64

/* RFID records types */
#define RESERVER_TAG 0x00
#define WALL_MOUNT_TAG 0x01 // type 1 -> room tags related to cleaning evidence & contamination control
#define ROOM_MOUNT_TAG 0x02 // type 2 -> location tags only for indoor positioning
#define WARNING_WALL_MOUNT_TAG 0x03
#define MOP_TAG 0x04 // type 4 -> tags are installed in a mop

#define MAX_MOP_PER_SHIFT 60

/* EPC raw data which comes from RFID reader module over UART*/
typedef union
{
  char string[EPC_STRING_LENGTH];
} EPC_TAG;

/* Return structure for binary search result */
typedef struct
{
  uint16_t epc_index;
  uint8_t iterations_made;
  uint8_t found;
} EPC_BINARY_SERACH_RESULT;

/* RFID objects for cloud */
typedef union
{
  uint8_t rfid_record_bytes[RFID_RECORD_BYTE_LENGTH]; // must be equal to power of 2 (2^n)

  struct __attribute__((packed))
  {
    uint8_t epc[EPC_TOTAL_HEX_LENGTH + 1];
    uint8_t type;
    uint16_t id;
  };
} RFID_RECORD;

typedef union
{
  uint8_t room_record_bytes[ROOM_RECORD_BYTE_LENGTH]; // must be equal to power of 2 (2^n)

  struct __attribute__((packed))
  {
    uint16_t room_id;
    uint32_t allowed_mop_colors;
    uint32_t allowed_mop_typegroups;
    uint16_t wall_rfid_count;
  };
} ROOM_RECORD;

typedef union
{
  uint8_t mop_record_bytes[MOP_RECORD_BYTE_LENGTH]; // must be equal to power of 2 (2^n)

  struct __attribute__((packed))
  {
    uint16_t mop_id;
    uint8_t mop_color;
    uint8_t mop_typegroup;
    uint8_t mop_size;
    uint8_t mop_sides;
  };
} MOP_RECORD;

/* The advanced mob definition included also a timestamp - timestamp is needed for mob change and mob last time seen algorithms */
typedef union
{
  uint8_t advanced_mop_record_bytes[ADVANCED_MOP_RECORD_BYTE_LENGTH]; // must be equal to power of 2 (2^n)

  struct __attribute__((packed))
  {
    uint16_t mop_id;
    uint8_t mop_color;
    uint8_t mop_typegroup;
    uint8_t mop_size;
    uint8_t mop_sides;
    time_t timestamp;
  };
} ADVANCED_MOP_RECORD;

/* In this structure the current mop rfid tag and the corresponding romm, in which the mop is used, are stored  */
typedef union
{
  uint8_t room_to_mop_map_record_bytes[ROOM_TO_MOP_MAP_BYTE_LENGTH]; // must be equal to power of 2 (2^n)

  struct __attribute__((packed))
  {
    uint16_t mop_linked_room_id;
    uint16_t current_mop_id;
    uint16_t previous_mop_id;
    uint8_t current_mop_epc[EPC_TOTAL_HEX_LENGTH];
    time_t timestamp;
  };
} ROOM_TO_MOP_MAP;

/* EPC raw data which comes from RFID reader module over UART*/
typedef union
{
  uint8_t epc_frame[EPC_STRING_LENGTH + sizeof(int64_t) + sizeof(uint32_t)]; // Total size is EPC frame length + 8 byte for timestamp + 4 byte for counts

  struct __attribute__((packed))
  {
    EPC_TAG record;
    int64_t timestamp;
    uint32_t counts;
  };
} LAST_SEEN_TAG;

extern LAST_SEEN_TAG EPC_last_seen_records[EPC_LAST_SEEN_COUNT];
extern EPC_TAG epc_ring_buffer[EPC_RING_BUFFER_SIZE];
extern ROOM_RECORD current_room_record;
extern MOP_RECORD current_mop_record;
extern ADVANCED_MOP_RECORD new_advanced_mop_record;
extern ROOM_TO_MOP_MAP current_room_to_mop_mapping;
extern RFID_RECORD room_wall_tag_last_seen[EPC_LAST_SEEN_COUNT];
extern MOP_RECORD new_mop_record;
extern uint8_t room_detected;
extern ROOM_RECORD current_room_record;
extern MOP_RECORD current_mop_record;
extern ADVANCED_MOP_RECORD last_seen_mop_records_array[MAX_MOP_PER_SHIFT];
extern char Currentmop_RFID[EPC_TOTAL_HEX_LENGTH];
extern char Newmop_RFID[EPC_TOTAL_HEX_LENGTH];

extern char current_mop_epc[EPC_TOTAL_HEX_LENGTH];
extern uint8_t epc_next_tag;
extern uint32_t epc_head_position;
extern uint32_t epc_tail_position;
extern uint32_t epc_session_tag_counter;
extern uint32_t epc_total_tag_counter;
extern uint32_t EPC_last_processed_tag;
extern uint16_t EPC_last_rfid_record_index;
extern uint16_t EPC_last_room_record_index;
extern uint16_t EPC_last_mop_record_index;
extern uint32_t last_seen_array_auto_clear_timer;
extern uint8_t mop_installed;
extern char current_mob_string_reading[EPC_TOTAL_HEX_LENGTH + 1];
extern uint8_t last_seen_mop_count;
extern uint32_t last_seen_mop_auto_clear_timer;
extern uint32_t last_seen_mop_id;

extern void epc_process_tags(void);
extern void epc_mem_init(void);
extern void EPC_Sort_last_seen(void);
extern void EPC_Update_last_seen(char *epc_new_rfid_record);
extern void EPC_Clear_last_seen(void);
extern void EPC_AddItemToFlash(uint8_t cs_pin, EPC_TAG *tag);
extern void EPC_DeleteListAtFlash(uint8_t cs_pin);
extern EPC_TAG EPC_ReadItemAtFlash(uint8_t cs_pin, uint16_t epc_index);
extern uint16_t EPC_GetLastEPCIndexAtFlash(uint8_t cs_pin);
extern EPC_BINARY_SERACH_RESULT EPC_BinarySearch(uint8_t cs_pin, char *epc_value_to_find, uint16_t epc_list_length);
extern void SerializeCharToHex(char *source, uint8_t *dest, uint16_t len);
extern void PrintDeserializeHexToChar(uint8_t *source, uint16_t len);
extern const char *DeserializeHexToChar(uint8_t *source, uint16_t len);
extern void EPC_Memory_Delete_All_Records(const uint8_t cs_pin, const uint32_t memory, const uint32_t len);
extern void EPC_Memory_Write_RFID_Record(uint8_t cs_pin, RFID_RECORD *record, uint32_t index);
extern void EPC_Memory_Write_Room_Record(uint8_t cs_pin, ROOM_RECORD *record, uint32_t index);
extern void EPC_Memory_Write_Mop_Record(uint8_t cs_pin, MOP_RECORD *record, uint32_t index);
extern void EPC_Memory_Read_RFID_Record(uint8_t cs_pin, RFID_RECORD *record, uint32_t index);
extern void EPC_Memory_Read_RFID_Record_fast(uint8_t cs_pin, RFID_RECORD *record, uint32_t index);
extern void EPC_Memory_Read_Room_Record(uint8_t cs_pin, ROOM_RECORD *record, uint32_t index);
extern void EPC_Memory_Read_Mop_Record(uint8_t cs_pin, MOP_RECORD *record, uint32_t index);
extern uint16_t EPC_Memory_GetLastIndex(uint8_t cs_pin, uint32_t memory, uint32_t len, uint16_t frame_len);
extern uint8_t update_last_seen_room_id_array(uint8_t *room_wall_epc, uint8_t type, uint32_t id, uint16_t length);
extern void clear_last_seen_room_id_array(uint16_t length);
extern void PrintLastSeenLocationRecords(void);
extern void reset_room_to_mop_mapping(void);
extern void check_max_sqm_coveraged_per_mop(void);
extern uint8_t check_for_room_change(void);
extern uint8_t check_allowed_mop_type(void);
extern uint8_t check_allowed_mop_color(void);
extern uint8_t mop_check_already_used(void);
extern void Mop_ClearLastSeenArray(void);
extern void Mop_AddItemInLastSeenArray(ADVANCED_MOP_RECORD current_mop);
extern uint8_t Mop_CheckIDInLastSeenArray(ADVANCED_MOP_RECORD current_mop);
extern uint8_t room_to_mop_linkage(void);
extern void list_last_seen_location_record_array(void);
extern void clear_last_seen_location_record_array(void);
extern uint16_t check_or_add_location_record_in_array(uint8_t *location_epc, uint8_t epc_len);
extern void epc_extract_tags_from_buffer(void);
extern void bubblesort(LAST_SEEN_TAG array[], uint16_t length);
#endif
