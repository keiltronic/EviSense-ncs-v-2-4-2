/**
 * @file epc_mem.c
 * @author Thomas Keilbach | keiltronic GmbH
 * @date 08 Oct 2023
 * @brief This file contains function to write, read and search epc data to and from the external flash memory
 * @version 2.0.0
 */

/*!
 * @defgroup Memory
 * @brief This file contains function to write, read and search epc data to and from the external flash memory
 * @{*/
#include "epc_mem.h"

EPC_TAG epc_ring_buffer[EPC_RING_BUFFER_SIZE];
LAST_SEEN_TAG EPC_last_seen_records[EPC_LAST_SEEN_COUNT];
RFID_RECORD room_wall_tag_last_seen[EPC_LAST_SEEN_COUNT];
ADVANCED_MOP_RECORD last_seen_mop_records_array[MAX_MOP_PER_SHIFT];

RFID_RECORD last_seen_location_records_array[LAST_SEEN_LOCATION_ARRAY_SIZE]; // Includes all last seen room-, wall- and warning rfid tags. They are listed only once!
uint16_t last_seen_location_records_array_position = 0;

RFID_RECORD new_rfid_record;
ROOM_RECORD new_room_record;
MOP_RECORD new_mop_record;
ADVANCED_MOP_RECORD new_advanced_mop_record;
ROOM_RECORD current_room_record;
MOP_RECORD current_mop_record;
ROOM_TO_MOP_MAP current_room_to_mop_mapping;

char current_mob_string_reading[EPC_TOTAL_HEX_LENGTH + 1];
char epc_extracted_last[EPC_STRING_LENGTH];

uint8_t last_seen_mop_count = 0;
uint32_t last_seen_mop_id = 0;
uint32_t epc_total_tag_counter = 0;
uint32_t epc_session_tag_counter = 0;
uint32_t epc_head_position = 0;
uint32_t epc_tail_position = 0;
uint8_t last_seen_array_entries = 0;
uint16_t EPC_last_rfid_record_index = 0;
uint16_t EPC_last_room_record_index = 0;
uint16_t EPC_last_mop_record_index = 0;
uint32_t last_seen_array_auto_clear_timer = 0;
uint32_t last_seen_mop_auto_clear_timer = 0;
uint8_t mop_installed = false;
uint16_t last_another_room_id = 0;

bool epc_string_start_flag;
bool epc_string_end_flag;

uint32_t j = 0;
uint16_t epc_length = 0;

/*!
 * @brief This functions initialize the epc buffers, flags and counters in RAM.
 */
void epc_mem_init(void)
{
  uint8_t i = 0;

  for (i = 0; i < EPC_LAST_SEEN_COUNT; i++)
  {
    memcpy(EPC_last_seen_records[i].record.string, "0", EPC_TOTAL_HEX_LENGTH);
    EPC_last_seen_records[i].timestamp = 0LL;
    memset(room_wall_tag_last_seen[i].epc, 0, EPC_TOTAL_HEX_LENGTH);
  }

  current_room_record.allowed_mop_colors = 0;
  current_room_record.allowed_mop_colors = 0;
  current_room_record.allowed_mop_typegroups = 0;
  current_room_record.wall_rfid_count = 0;

  current_mop_record.mop_id = 0;
  current_mop_record.mop_color = 0;
  current_mop_record.mop_typegroup = 0;
  current_mop_record.mop_size = 0;
  current_mop_record.mop_sides = 0;

  current_room_to_mop_mapping.mop_linked_room_id = 0;
  current_room_to_mop_mapping.current_mop_id = 0;
  current_room_to_mop_mapping.previous_mop_id = 0;
  current_room_to_mop_mapping.timestamp = 0;
  memset(current_room_to_mop_mapping.current_mop_epc, '0', EPC_TOTAL_HEX_LENGTH);
  memset(last_seen_mop_records_array, 0, MAX_MOP_PER_SHIFT);
  memset(current_mob_string_reading, 0, EPC_TOTAL_HEX_LENGTH);

  clear_last_seen_location_record_array();
}

/*!
 * @brief This functions clears the "last seen location record array".
 */
void clear_last_seen_location_record_array(void)
{
  uint16_t i = 0;

  for (i = 0; i < LAST_SEEN_LOCATION_ARRAY_SIZE; i++)
  {
    memset(last_seen_location_records_array[i].epc, '0', EPC_TOTAL_HEX_LENGTH);
    last_seen_location_records_array[i].id = 0;
    last_seen_location_records_array[i].type = 0;
  }

  last_seen_location_records_array_position = 0;
}

/*!
 * @brief This functions prints out all entries in the "last seen location record array" on console.
 */
void list_last_seen_location_record_array(void)
{
  uint16_t i = 0;

 // shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "Last seen location epc:\n");

  for (i = 0; i < last_seen_location_records_array_position; i++)
  {
   // shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "%d: %s\n", i, DeserializeHexToChar(last_seen_location_records_array[i].epc, EPC_TOTAL_HEX_LENGTH));
  }
}

/**
 * @brief  This functions takes a new location record and adds it to the "last seen location array list" if it its not already listed.
 *
 * @param location_epc: Pointer to location_epc string
 * @param epc_len: string length
 * @return uint16_t: Current position in "last seen location array"
 */
uint16_t check_or_add_location_record_in_array(uint8_t *location_epc, uint8_t epc_len) // returns the postion of the epc in the last seen location array
{
  uint16_t i = 0;
  uint8_t rslt = 0;

  /* Search for location epc in array - if already exisiting do not add it again*/
  for (i = 0; i < LAST_SEEN_LOCATION_ARRAY_SIZE; i++)
  {
    rslt = memcmp(last_seen_location_records_array[i].epc, location_epc, EPC_TOTAL_HEX_LENGTH);

    if (rslt == 0)
    {
      return i; // already listed
    }
  }

  /* If location epc not found then add it into array*/
  if (last_seen_location_records_array_position <= LAST_SEEN_LOCATION_ARRAY_SIZE)
  {
    memcpy(last_seen_location_records_array[last_seen_location_records_array_position++].epc, location_epc, EPC_TOTAL_HEX_LENGTH);
  }

  return last_seen_location_records_array_position - 1;
}

/**
 * @brief This functions takes a new mop record and adds it to the "last seen mop array list"
 *
 * @param current_mop: Structure of current mop
 */
void Mop_AddItemInLastSeenArray(ADVANCED_MOP_RECORD current_mop)
{
  if (last_seen_mop_count < MAX_MOP_PER_SHIFT)
  {
    /* Reset auto reset timer if the first mop gets added */
    if (last_seen_mop_count == 0)
    {
      last_seen_mop_auto_clear_timer = 0;
    }

    /* Add mop parameter to last seen arry*/
    last_seen_mop_records_array[last_seen_mop_count].mop_color = current_mop.mop_color;
    last_seen_mop_records_array[last_seen_mop_count].mop_id = current_mop.mop_id;
    last_seen_mop_records_array[last_seen_mop_count].mop_sides = current_mop.mop_sides;
    last_seen_mop_records_array[last_seen_mop_count].mop_size = current_mop.mop_size;
    last_seen_mop_records_array[last_seen_mop_count].mop_typegroup = current_mop.mop_typegroup;
    last_seen_mop_records_array[last_seen_mop_count].timestamp = current_mop.timestamp;
    last_seen_mop_count++;
  }
}

/**
 * @brief This functions search the current mop id in the "last seen mop records array" and updates its timestamp
 *
 * @param current_mop: Structure of current mop
 * @return uint8_t: true, if found (and updated), otherwise false
 */

uint8_t Mop_CheckIDInLastSeenArray(ADVANCED_MOP_RECORD current_mop)
{
  uint8_t i = 0;

  if (current_mop.mop_id != 0)
  {
    for (i = 0; i < MAX_MOP_PER_SHIFT; i++)
    {
      if (last_seen_mop_records_array[i].mop_id == current_mop.mop_id)
      {
        /* Update last time seen time stamp */
        last_seen_mop_records_array[i].timestamp = unixtime_ms;
        return 1;
      }
    }
  }
  return 0;
}

/*!
 * @brief This functions clears the last seen mop array
 */
void Mop_ClearLastSeenArray(void)
{
  uint8_t i = 0;

  for (i = 0; i < last_seen_mop_count; i++)
  {
    last_seen_mop_records_array[i].mop_color = 0;
    last_seen_mop_records_array[i].mop_id = 0;
    last_seen_mop_records_array[i].mop_sides = 0;
    last_seen_mop_records_array[i].mop_size = 0;
    last_seen_mop_records_array[i].mop_typegroup = 0;
    last_seen_mop_records_array[i].timestamp = 0;
  }

  /* Print rfid record and room record details if listed in databases */
  if ((Parameter.epc_verbose == true) && (datalog_ReadOutisActive == false))
  {
    rtc_print_debug_timestamp();
    shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "Cleared last seen mop arrays \n");
  }

  last_seen_mop_count = 0;
}

/**
 * @brief This functions deletes all stored EPC databases in the external flash memory
 * This function talks directly over the SPI bus with the external NOR flash memory. For this it uses the API calls defined in flash.c
 *
 * @param cs_pin: GPIO number of the cs pin
 * @param memory: address of memory
 * @param len: total length to clear
 */
void EPC_Memory_Delete_All_Records(const uint8_t cs_pin, const uint32_t memory, const uint32_t len)
{
 // flash_ClearBlock_4kB(cs_pin, memory, len);
}

/**
 * @brief This functions writes a new rfid record (structure) to the external flash memory
 * This function talks directly over the SPI bus with the external NOR flash memory. For this it uses the API calls defined in flash.c
 *
 * @param cs_pin: GPIO number of the cs pin
 * @param record: Pointer to data structure where the data read out gets stored
 * @param index: Index number of the record which should read out
 */
void EPC_Memory_Write_RFID_Record(uint8_t cs_pin, RFID_RECORD *record, uint32_t index)
{
  uint32_t wall_record_address = 0UL;
  datalog_EnableFlag = false;

  /* Calculate address in flash */
  wall_record_address = RFID_RECORD_REGION + (index * RFID_RECORD_BYTE_LENGTH);

  /* Write epc tag data to flash*/
  if (wall_record_address < (RFID_RECORD_REGION + RFID_RECORD_REGION_LENGTH))
  {
    flash_write(cs_pin, wall_record_address, record->rfid_record_bytes, RFID_RECORD_BYTE_LENGTH);
  }

  if (Parameter.datalogEnable == true)
  {
    datalog_EnableFlag = true;
  }
  else
  {
    datalog_EnableFlag = false;
  }
}

/**
 * @brief This functions writes a new room record (structure)  to the external flash memory
 * This function talks directly over the SPI bus with the external NOR flash memory. For this it uses the API calls defined in flash.c
 *
 * @param cs_pin: GPIO number of the cs pin
 * @param record: Pointer to data structure where the data read out gets stored
 * @param index: Index number of the record which should read out
 */
void EPC_Memory_Write_Room_Record(uint8_t cs_pin, ROOM_RECORD *record, uint32_t index)
{
  uint32_t room_record_address = 0UL;
  datalog_EnableFlag = false;

  /* Calculate address in flash */
  room_record_address = ROOM_RECORD_REGION + (index * ROOM_RECORD_BYTE_LENGTH);

  /* Write epc tag data to flash*/
  if (room_record_address < (ROOM_RECORD_REGION + ROOM_RECORD_REGION_LENGTH))
  {
    flash_write(cs_pin, room_record_address, record->room_record_bytes, ROOM_RECORD_BYTE_LENGTH);
  }

  if (Parameter.datalogEnable == true)
  {
    datalog_EnableFlag = true;
  }
  else
  {
    datalog_EnableFlag = false;
  }
}

/**
 * @brief This functions writes a new mop record (structure) to the external flash memory
 * This function talks directly over the SPI bus with the external NOR flash memory. For this it uses the API calls defined in flash.c
 *
 * @param cs_pin: GPIO number of the cs pin
 * @param record: Pointer to data structure where the data read out gets stored
 * @param index: Index number of the record which should read out
 */
void EPC_Memory_Write_Mop_Record(uint8_t cs_pin, MOP_RECORD *record, uint32_t index)
{
  uint32_t mop_record_address = 0UL;
  datalog_EnableFlag = false;

  /* Calculate address in flash */
  mop_record_address = MOP_RECORD_REGION + (index * MOP_RECORD_BYTE_LENGTH);

  /* Write epc tag data to flash*/
  if (mop_record_address < (MOP_RECORD_REGION + MOP_RECORD_REGION_LENGTH))
  {
    flash_write(cs_pin, mop_record_address, record->mop_record_bytes, MOP_RECORD_BYTE_LENGTH);
  }

  if (Parameter.datalogEnable == true)
  {
    datalog_EnableFlag = true;
  }
  else
  {
    datalog_EnableFlag = false;
  }
}

/**
 * @brief This functions reads a rfid record (structure) from the external flash memory
 * This function talks directly over the SPI bus with the external NOR flash memory. For this it uses the API calls defined in flash.c
 *
 * @param cs_pin: GPIO number of the cs pin
 * @param record: Pointer to data structure where the data read out gets stored
 * @param index: Index number of the record which should read out
 */
void EPC_Memory_Read_RFID_Record(uint8_t cs_pin, RFID_RECORD *record, uint32_t index)
{
  uint32_t flash_epc_address = 0UL;
  datalog_EnableFlag = false;

  /* Calculate address in flash */
  flash_epc_address = RFID_RECORD_REGION + (index * RFID_RECORD_BYTE_LENGTH);

  /* Read epc tag from flash*/
  if (flash_epc_address < (RFID_RECORD_REGION + RFID_RECORD_REGION_LENGTH))
  {
    flash_read(cs_pin, flash_epc_address, record->rfid_record_bytes, RFID_RECORD_BYTE_LENGTH);
  }

  if (Parameter.datalogEnable == true)
  {
    datalog_EnableFlag = true;
  }
  else
  {
    datalog_EnableFlag = false;
  }
}

/**
 * @brief This functions reads a rfid record (structure) from the external flash memory
 * This function talks directly over the SPI bus with the external NOR flash memory. For this it uses the API calls defined in flash.c
 *
 * @param cs_pin: GPIO number of the cs pin
 * @param record: Pointer to data structure where the data read out gets stored
 * @param index: Index number of the record which should read out
 */
void EPC_Memory_Read_RFID_Record_fast(uint8_t cs_pin, RFID_RECORD *record, uint32_t index)
{
  // uint32_t flash_epc_address = 0UL;
  // datalog_EnableFlag = false;

  // /* Calculate address in flash */
  // flash_epc_address = RFID_RECORD_REGION + (index * RFID_RECORD_BYTE_LENGTH);

  // /* Read epc tag from flash*/
  // if (flash_epc_address < (RFID_RECORD_REGION + RFID_RECORD_REGION_LENGTH))
  // {
  //   flash_read_fast(cs_pin, flash_epc_address, record->rfid_record_bytes, RFID_RECORD_BYTE_LENGTH);
  // }

  // if (Parameter.datalogEnable == true)
  // {
  //   datalog_EnableFlag = true;
  // }
  // else
  // {
  //   datalog_EnableFlag = false;
  // }
}

/**
 * @brief This functions reads a room record (structure) from the external flash memory
 * This function talks directly over the SPI bus with the external NOR flash memory. For this it uses the API calls defined in flash.c
 *
 * @param cs_pin: GPIO number of the cs pin
 * @param record: Pointer to data structure where the data read out gets stored
 * @param index: Index number of the record which should read out
 */
void EPC_Memory_Read_Room_Record(uint8_t cs_pin, ROOM_RECORD *record, uint32_t index)
{
  uint32_t flash_epc_address = 0UL;
  datalog_EnableFlag = false;

  /* Calculate address in flash */
  flash_epc_address = ROOM_RECORD_REGION + (index * ROOM_RECORD_BYTE_LENGTH);

  /* Read epc tag from flash*/
  if (flash_epc_address < (ROOM_RECORD_REGION + ROOM_RECORD_REGION_LENGTH))
  {
    flash_read(cs_pin, flash_epc_address, record->room_record_bytes, ROOM_RECORD_BYTE_LENGTH);
  }

  if (Parameter.datalogEnable == true)
  {
    datalog_EnableFlag = true;
  }
  else
  {
    datalog_EnableFlag = false;
  }
}

/**
 * @brief This functions reads a mop record (structure) from the external flash memory
 *
 * @param cs_pin: GPIO number of the cs pin
 * @param record: Structure to out the read out data
 * @param index: Index in memory to read
 */
void EPC_Memory_Read_Mop_Record(uint8_t cs_pin, MOP_RECORD *record, uint32_t index)
{
  uint32_t flash_epc_address = 0UL;
  datalog_EnableFlag = false;

  /* Calculate address in flash */
  flash_epc_address = MOP_RECORD_REGION + (index * MOP_RECORD_BYTE_LENGTH);

  /* Read epc tag from flash*/
  if (flash_epc_address < (MOP_RECORD_REGION + MOP_RECORD_REGION_LENGTH))
  {
    flash_read(cs_pin, flash_epc_address, record->mop_record_bytes, MOP_RECORD_BYTE_LENGTH);
  }

  if (Parameter.datalogEnable == true)
  {
    datalog_EnableFlag = true;
  }
  else
  {
    datalog_EnableFlag = false;
  }
}

/**
 * @brief  This functions reads out the last index number (from any rfid, room or mop database) from the external flash memory
 * This function talks directly over the SPI bus with the external NOR flash memory. For this it uses the API calls defined in flash.c
 *
 * @param cs_pin: GPIO number of the cs pin
 * @param memory: memory starting address
 * @param len: Size of memory in bytes
 * @param frame_len: Length of one frame in bytes
 * @return uint16_t: Index (total count of stored elements)
 */
uint16_t EPC_Memory_GetLastIndex(uint8_t cs_pin, uint32_t memory, uint32_t len, uint16_t frame_len)
{
  uint32_t current_sector = 0UL;
  uint32_t addr = 0UL;
  uint16_t index = 0;
  uint16_t data = 0;
  uint32_t i = 0UL;

  /* Identify the sector in which the last valid frame number is stored (if it is not valid, it has the NOR flash default id 0) */
  // for (i = 0UL; i < len; i += FLASH_SUBSUBSECTOR_SIZE)
  // {
  //   flash_read(cs_pin, memory + i, &data, sizeof(data));

  //   /* Read and validate current frame number from flash. The frame number must match to the current sector number (dataframe number must match to current sector number) */
  //   if (data != 0xFFFF)
  //   {
  //     current_sector++;
  //   }
  //   else
  //   {
  //     if (current_sector > 0UL)
  //     {
  //       current_sector--;
  //     }
  //     addr = memory + (current_sector * FLASH_SUBSUBSECTOR_SIZE);
  //     index = current_sector * (FLASH_SUBSUBSECTOR_SIZE / frame_len);
  //     break;
  //   }
  // }

  // /* Within sector search for the latest frame number which has a valid number (and not the default flash erase state -> 0xFFFFFFFF)*/
  // for (i = 0UL; i < FLASH_SUBSUBSECTOR_SIZE; i += frame_len)
  // {
  //   flash_read(cs_pin, addr + i, &data, sizeof(data));

  //   if (data != 0xFFFF)
  //   {
  //     index++;
  //   }
  //   else
  //   {
  //     break;
  //   }
  // }
  return index;
}

/**
 * @brief  This function serialize an ASCII string to hex values
 * @details Example Ascii EPC tag string: 3000E280689400004003A1F5B51AA872
 *  Two Ascii digits to one 8-bit nummer
 *  "30"  -> 48
 *  "00"  -> 0
 *  "E2"  -> 226
 *  "80"  -> 128
 *  "68"  -> 104
 *  "94"  -> 148
 *  "00"  -> 0
 *  "00"  -> 0
 *  "40"  -> 64
 *  "03"  -> 3
 *  "A1"  -> 161
 *  "F5"  -> 245
 *  "B5"  -> 181
 *  "1A"  -> 26
 *  "A8"  -> 168
 *  "72"  -> 114
 * @param source: Pointer to source char string array
 * @param dest: Pointer to desition array
 * @param len: Length of source char string array
 */
void SerializeCharToHex(char *source, uint8_t *dest, uint16_t len)
{
  uint8_t i = 0;
  char snippet[2];

  for (i = 0; i < len; i++)
  {
    strncpy(snippet, (source + (i * 2)), 2);          // Combine two ascii digits to one new ascii string (hex number)
    *(dest + i) = (uint8_t)strtol(snippet, NULL, 16); // convert the generated hex number string (base 16) to a decimal number (255 max, 8-bit)
  }
}

/**
 * @brief This function serialize hex values to an ASCII string and prints it on console
 * @details Example Ascii EPC tag string: 3000E280689400004003A1F5B51AA872
 *  Two Ascii digits to one 8-bit nummer
 *  "30"  <- 48
 *  "00"  <- 0
 *  "E2"  <- 226
 *  "80"  <- 128
 *  "68"  <- 104
 *  "94"  <- 148
 *  "00"  <- 0
 *  "00"  <- 0
 *  "40"  <- 64
 *  "03"  <- 3
 *  "A1"  <- 161
 *  "F5"  <- 245
 *  "B5"  <- 181
 *  "1A"  <- 26
 *  "A8"  <- 168
 *  "72"  <- 114
 * @param source: Pointer to source char string array
 * @param len: Length of source char string array
 */
void PrintDeserializeHexToChar(uint8_t *source, uint16_t len)
{
  uint8_t i = 0;
  uint8_t cross_sum = 0;

  // check if stored EPC tag is zero
  for (i = 0; i < len; i++)
  {
    cross_sum += source[i];
  }

  // Print EPC code to console
  if (cross_sum > 0)
  {
    char _hex[] = "0123456789ABCDEF";

    // shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "%c%c", _hex[(int)(source[0]) / 16], _hex[(int)(source[0]) % 16]);
    printf("%c%c", _hex[(int)(source[0]) / 16], _hex[(int)(source[0]) % 16]);
    for (i = 1; i < len; ++i)
    {
      // shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "%c%c", _hex[(int)(source[i]) / 16], _hex[(int)(source[i]) % 16]);
      printf("%c%c", _hex[(int)(source[i]) / 16], _hex[(int)(source[i]) % 16]);
    }
  }
  else
  {
    // shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "0");
    printf("0");
  }
  // shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, ";");
  printf(";");
}

/**
 * @brief This function serialize hex values to an ASCII string
 * @details Example Ascii EPC tag string: 3000E280689400004003A1F5B51AA872
 *  Two Ascii digits to one 8-bit nummer
 *  "30"  <- 48
 *  "00"  <- 0
 *  "E2"  <- 226
 *  "80"  <- 128
 *  "68"  <- 104
 *  "94"  <- 148
 *  "00"  <- 0
 *  "00"  <- 0
 *  "40"  <- 64
 *  "03"  <- 3
 *  "A1"  <- 161
 *  "F5"  <- 245
 *  "B5"  <- 181
 *  "1A"  <- 26
 *  "A8"  <- 168
 *  "72"  <- 114
 * @param source: Pointer to source array
 * @param len: Length of source array
 * @return const char*: Pointer to resulting char array
 */
const char *DeserializeHexToChar(uint8_t *source, uint16_t len)
{
  uint8_t i = 0;

  static char string[45];
  memset(string, 0, 45);

  char _hex[] = "0123456789ABCDEF";

  for (i = 0; i < len; i++)
  {
    string[(2 * i)] = _hex[((uint16_t)source[i]) / 16];
    string[(2 * i) + 1] = _hex[((uint16_t)source[i]) % 16];
  }

  return string;
}

/**
 * @brief This function returns the epc in ASCII format from external flash for a given index number
 * This function talks directly over the SPI bus with the external NOR flash memory. For this it uses the API calls defined in flash.c
 *
 * @param cs_pin: GPIO number of the cs pin
 * @param epc_index: Index number of EPC string to read
 * @return const char*: Returns the pointer epc string array
 */
const char *EPC_ReadEPCAtFlash_fast(uint8_t cs_pin, uint16_t epc_index)
{
  RFID_RECORD record;

  EPC_Memory_Read_RFID_Record_fast(cs_pin, &record, epc_index);
  return DeserializeHexToChar(record.epc, 20);
}

/**
 * @brief This function returns the epc in ASCII format from external flash for a given index number
 * This function talks directly over the SPI bus with the external NOR flash memory. For this it uses the API calls defined in flash.c
 *
 * @param cs_pin: GPIO number of the cs pin
 * @param epc_index: Index number of EPC string to read
 * @return const char*: Returns the pointer epc string array
 */
const char *EPC_ReadEPCAtFlash(uint8_t cs_pin, uint16_t epc_index)
{
  RFID_RECORD record;

  EPC_Memory_Read_RFID_Record(cs_pin, &record, epc_index);
  return DeserializeHexToChar(record.epc, 20);
}

/**
 * @brief This function takes a epc (in ASCII format) and search for it in the external flash memory
 * This function talks directly over the SPI bus with the external NOR flash memory. For this it uses the API calls defined in flash.c
 *
 * @param cs_pin:  GPIO number of the cs pin
 * @param epc_value_to_find: Pointer to EPC string which should be searched for
 * @param epc_list_length: Length of the list
 * @return EPC_BINARY_SERACH_RESULT: Result structure which containes if the string was found and how many iterations it took
 */
EPC_BINARY_SERACH_RESULT EPC_BinarySearch(uint8_t cs_pin, char *epc_value_to_find, uint16_t epc_list_length)
{
  // Setup binary search result structure
  EPC_BINARY_SERACH_RESULT ret;
  ret.epc_index = 0;
  ret.iterations_made = 0;
  ret.found = 0;

  if (EPC_last_rfid_record_index != 0) // Do not execute the binary search if the rfid record database is empty
  {
    // Preload binary search boundaries
    int16_t lidx = 0;                   // current low index of search interval
    int16_t hidx = epc_list_length - 1; // top index of search interval
    int16_t midx = (hidx - lidx) / 2;   // middle index of search interval
    int16_t m_cmp = 0;
    int16_t l_cmp = 0;
    int16_t h_cmp = 0;

    char read_epc[41];
    memset(read_epc, 0, 40);

    // Test if low or high boundary already matches to wanted EPC tag
    memcpy(read_epc, EPC_ReadEPCAtFlash_fast(cs_pin, lidx), 40);
    l_cmp = memcmp(epc_value_to_find, read_epc, 40);
    memcpy(read_epc, EPC_ReadEPCAtFlash_fast(cs_pin, hidx), 40);
    h_cmp = memcmp(epc_value_to_find, read_epc, 40);

    while (1)
    {
      /* Limit iterations to 30 avoid never ending loops */
      if (ret.iterations_made > 30)
      {
        ret.epc_index = midx;
        ret.found = 0;
        break;
      }
      else
      {
        ret.iterations_made++;
      }

      if (l_cmp == 0)
      { // EPC tag found
        ret.epc_index = lidx;
        ret.found = 1;
        break;
      }

      if (h_cmp == 0)
      { // EPC tag found
        ret.epc_index = hidx;
        ret.found = 1;
        break;
      }

      // Test if current middle index matches to wanted EPC tag
      midx = lidx + (hidx - lidx) / 2; // subdivision index
      memcpy(read_epc, EPC_ReadEPCAtFlash_fast(cs_pin, midx), 40);
      m_cmp = memcmp(epc_value_to_find, read_epc, 40);

      if (m_cmp == 0)
      { // EPC tag found
        ret.epc_index = midx;
        ret.found = 1;
        break;
      }
      else if (m_cmp < 0)
      { // EPC_value_to_find is lower than current probe in flash memory
        hidx = midx;
        memcpy(read_epc, EPC_ReadEPCAtFlash_fast(cs_pin, hidx), 40);
        h_cmp = memcmp(epc_value_to_find, read_epc, 40);
      }
      else if (m_cmp > 0)
      { // EPC_value_to_find is higer than current probe in flash memory
        lidx = midx;
        memcpy(read_epc, EPC_ReadEPCAtFlash_fast(cs_pin, lidx), 40);
        l_cmp = memcmp(epc_value_to_find, read_epc, 40);
      }

      if ((lidx + 1) == hidx)
      {
        break;
      }
    }
  }
  else
  {
    ret.found = 0;
  }

  return ret;
}

/**
 * @brief This functions adds rfid record in the "last seen records array". If it is already listed it only updates its timestamp
 *
 * @param epc_new_rfid_record: Pointer to the epc string which should be added
 */
void EPC_Update_last_seen(char *epc_new_rfid_record)
{
  /* Search for EPC in list */
  volatile uint8_t i = 0;
  volatile uint8_t found = 0;
  volatile int8_t rslt = 0;

  for (i = 0; i < EPC_LAST_SEEN_COUNT; i++)
  {
    rslt = memcmp(EPC_last_seen_records[i].record.string, epc_new_rfid_record, EPC_LENGTH_MAX);

    if (rslt == 0)
    {
      EPC_last_seen_records[i].timestamp = unixtime_ms;
      EPC_last_seen_records[i].counts++;
      found = true;
      break;
    }
  }

  if (found == false)
  { // Rfid record not found

    /* If array is full, search the oldes tags and remove it to free space for a new one*/
    if (last_seen_array_entries == EPC_LAST_SEEN_COUNT)
    {
      EPC_Sort_last_seen(); // Search for the oldest entry, removes it and pack the the array again to avoid leakage */
    }

    /* Shift array */
    for (i = EPC_LAST_SEEN_COUNT - 1; i > 0; i--)
    {
      EPC_last_seen_records[i] = EPC_last_seen_records[i - 1];
    }

    /* Insert new rfid record at index 0 */
    memcpy(EPC_last_seen_records[0].record.string, epc_new_rfid_record, EPC_LENGTH_MAX);
    EPC_last_seen_records[0].timestamp = unixtime_ms;
    EPC_last_seen_records[0].counts = 1;

    if (last_seen_array_entries < EPC_LAST_SEEN_COUNT)
    {
      last_seen_array_entries++;
    }
  }
}

/*!
 * @brief This functions clears the last seen rfid record array
 */
void EPC_Clear_last_seen(void)
{
  uint8_t i = 0;

  for (i = 0; i < EPC_LAST_SEEN_COUNT; i++)
  {
    strcpy(EPC_last_seen_records[i].record.string, "0");
    EPC_last_seen_records[i].timestamp = 0LL;
    EPC_last_seen_records[i].counts = 0;
  }
}

/*!
 * @brief This functions sorts the last seen array by its time stamp and removes the oldest one
 */
void EPC_Sort_last_seen(void)
{
  uint16_t i = 0;
  uint8_t oldest_index = 0;
  int64_t timestamp_min = 4102441200000LL; // 01.01.2100

  /* Find index in array with the oldest time stamp */
  for (i = 0; i < EPC_LAST_SEEN_COUNT; i++)
  {
    if (EPC_last_seen_records[i].timestamp < timestamp_min)
    {
      timestamp_min = EPC_last_seen_records[i].timestamp;
      oldest_index = i;
    }
  }

  /* Remove oldest entry by shifting the array elements one position */
  for (i = oldest_index; i < EPC_LAST_SEEN_COUNT; i++)
  {
    EPC_last_seen_records[i] = EPC_last_seen_records[i + 1];
    last_seen_array_entries--;
  }
}

/**
 * @brief  This function takes a room epc (in ASCII format) adds it in the last seen room id array (if it is not already listed)
 *
 * @param room_wall_epc: Pointer to string which contains the room epc
 * @param type: Type number of tag
 * @param id: Tag id number
 * @param length: Length of epc string
 * @return uint8_t:  0 if epc is already listed, 1 if epc was added
 */
uint8_t update_last_seen_room_id_array(uint8_t *room_wall_epc, uint8_t type, uint32_t id, uint16_t length)
{
  uint8_t i = 0;
  int8_t rslt = 0;
  uint8_t listed = false;

  /* Search for room wall epc in list */
  for (i = 0; i < length; i++)
  {
    rslt = memcmp(room_wall_tag_last_seen[i].epc, room_wall_epc, EPC_TOTAL_HEX_LENGTH);

    if (rslt == 0)
    {
      listed = true;
    }
  }

  /* Add array list (FILO) */
  for (i = length - 1; i > 0; i--)
  {
    memcpy(&room_wall_tag_last_seen[i].epc, &room_wall_tag_last_seen[i - 1].epc, EPC_TOTAL_HEX_LENGTH);
    room_wall_tag_last_seen[i].type = room_wall_tag_last_seen[i - 1].type;
    room_wall_tag_last_seen[i].id = room_wall_tag_last_seen[i - 1].id;
  }

  /* Add new tag */
  memcpy(&room_wall_tag_last_seen[0].epc, room_wall_epc, EPC_TOTAL_HEX_LENGTH);
  room_wall_tag_last_seen[0].type = type;
  room_wall_tag_last_seen[0].id = id;

  return listed;
}

/**
 * @brief This functions clears the last seen room id array
 *
 * @param length: Number of elements to clear
 */
void clear_last_seen_room_id_array(uint16_t length)
{
  uint8_t i = 0;

  /* Clear array list (FILO) */
  for (i = 0; i < length; i++)
  {
    memset(room_wall_tag_last_seen[i].epc, 0, EPC_TOTAL_HEX_LENGTH);
    room_wall_tag_last_seen[i].type = 0;
    room_wall_tag_last_seen[i].id = 0;
  }
}

/*!
 * @brief This function checks if the current installed mop was used in antoher room before.
 * @return  0 if mop was not already used, otherwise 1
 */
uint8_t mop_check_already_used(void)
{
  /* Check if mob was used in another room before */
  if (current_room_to_mop_mapping.mop_linked_room_id != 0) // it is mendatory that one room was scanned before
  {
    if (mop_installed == true)
    {
      if (current_room_record.room_id != current_room_to_mop_mapping.mop_linked_room_id)
      {
        /* Create event for cloud and user notification (this is triggered within the event) */
        // NewEvent0x06(current_room_record.room_id, current_mop_record.mop_id);

        if (MoppingFlag[0] == 1) // User is currently mopping
        {
          NewEvent0x19(current_room_record.room_id, current_room_to_mop_mapping.current_mop_id);
        }
        // NewEvent0x1A(current_room_record.room_id, current_mop_record.mop_id);

        /* Print rfid record and room record details if listed in databases */
        if ((Parameter.epc_verbose == true || Parameter.debug == true) && datalog_ReadOutisActive == false)
        {
          rtc_print_debug_timestamp();
          shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_RED, "Same mop used in another room\n");
        }

        return 1;
      }
    }
  }
  return 0;
}

/*!
 * @brief This function checks if the current installed mop color is allowed in the current room
 * @return  1 if mop color is allowed, otherwise 0
 */
uint8_t check_allowed_mop_color(void)
{
  if (mop_installed == true)
  {
    if (current_room_record.allowed_mop_colors & new_mop_record.mop_color)
    {
      if ((Parameter.epc_verbose == true) && (datalog_ReadOutisActive == false))
      {
        rtc_print_debug_timestamp();
        shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_GREEN, "Mop COLOR is allowed in this room (room id: %d)\r\n", current_room_record.room_id);
      }
      return 1;
    }
    else
    {
      if ((Parameter.epc_verbose == true || Parameter.debug == true) && datalog_ReadOutisActive == false)
      {
        rtc_print_debug_timestamp();
        shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_RED, "Mop COLOR is NOT allowed in this room (room id: %d)\r\n", current_room_record.room_id);
      }

      /* Create event for cloud and user notification (this is triggered within the event) */
      // NewEvent0x05(current_room_record.room_id, current_room_to_mop_mapping.current_mop_id);

      if (MoppingFlag[0] == 1) // User is currently mopping
      {
        NewEvent0x19(current_room_record.room_id, current_room_to_mop_mapping.current_mop_id);
      }
      return 0;
    }
  }
}

/*!
 * @brief This function checks if the current installed mop type is allowed in the current room
 * @return  1 if mop type is allowed, otherwise 0
 */
uint8_t check_allowed_mop_type(void)
{
  if (mop_installed == true)
  {
    if (new_mop_record.mop_typegroup & current_room_record.allowed_mop_typegroups)
    {
      if ((Parameter.epc_verbose == true || Parameter.debug == true) && datalog_ReadOutisActive == false)
      {
        rtc_print_debug_timestamp();
        shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_GREEN, "Mop TYPE is allowed in this room (room id: %d)\r\n", current_room_record.room_id);
      }

      return 1;
    }
    else
    {
      if ((Parameter.epc_verbose == true || Parameter.debug == true) && datalog_ReadOutisActive == false)
      {
        rtc_print_debug_timestamp();
        shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_RED, "Mop TYPE is NOT allowed in this room (room id: %d)\r\n", current_room_record.room_id);
      }

      /* Create event for cloud and user notification (this is triggered within the event) */
      // NewEvent0x05(current_room_record.room_id, current_room_to_mop_mapping.current_mop_id);

      if (MoppingFlag[0] == 1) // User is currently mopping
      {
        NewEvent0x19(current_room_record.room_id, current_room_to_mop_mapping.current_mop_id);
      }
       return 0;     
    }
  }
}

/*!
 * @brief This function checks if the current room was left / changed
 * @return  1 if room was changed, otherwise 0
 */
uint8_t check_for_room_change(void)
{
  if (current_room_record.room_id != new_room_record.room_id)
  {
    //  if ((current_room_record.room_id != new_room_record.room_id) && (last_another_room_id != new_room_record.room_id))
    // {
    //   last_another_room_id = current_room_record.room_id;

    /* Save new current room settings */
    current_room_record.allowed_mop_colors = new_room_record.allowed_mop_colors;
    current_room_record.allowed_mop_typegroups = new_room_record.allowed_mop_typegroups;
    current_room_record.room_id = new_room_record.room_id;
    current_room_record.wall_rfid_count = new_room_record.wall_rfid_count;

    // /* Create event for cloud and user notification (this is triggered within the event) */
    //  NewEvent0x04(new_rfid_record.id);

    /* Live view on console */
    if ((Parameter.epc_verbose == true || Parameter.debug == true) && datalog_ReadOutisActive == false)
    {
      rtc_print_debug_timestamp();
      shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_CYAN, "New room detected: %d\n", current_room_record.room_id);
    }
    return 1;
  }
  else
  {
    return 0;
  }
}

/*!
 * @brief This functions links the current room id to the current mop id.
 * @return  0 if linkage successfull, otherwise 1
 */
uint8_t room_to_mop_linkage(void)
{
  if (Mopping_motion_gyr_flag == 1) //(MoppingFlag[0] == 1) // if user currently mops
  {
    if (current_room_to_mop_mapping.mop_linked_room_id == 0) // 0 means no mop is linked to any room
    {
      // uint16_t hold_previous_room_id = 0;
      // hold_previous_room_id = current_room_to_mop_mapping.mop_linked_room_id;
      current_room_to_mop_mapping.mop_linked_room_id = new_room_record.room_id;

      /* UNDER DEVELOPMENT: Create an event and send it to the cloud */
      NewEvent0x04((uint32_t)current_room_to_mop_mapping.mop_linked_room_id);

      /* Live view on console */
      if ((Parameter.epc_verbose == true || Parameter.debug == true) && datalog_ReadOutisActive == false)
      {
        rtc_print_debug_timestamp();
        shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_YELLOW, "Mop linked to room: %d\n", current_room_to_mop_mapping.mop_linked_room_id);

        //    shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_YELLOW, "Mop linked to room: %d (room before: %d)\n", current_room_to_mop_mapping.mop_linked_room_id, hold_previous_room_id);
      }
      return 0;
    }
    else
    {
      return 1;
    }
  }
  else
  {
    return 1;
  }
}

/*!
 * @brief This functions resets the current linkage between room id and mop id.
 */
void reset_room_to_mop_mapping(void)
{
  memset(current_room_to_mop_mapping.current_mop_epc, '0', 20);
  current_room_to_mop_mapping.mop_linked_room_id = 0;
  current_room_to_mop_mapping.current_mop_id = 0;  
  mop_installed = false;

  /* Print information on screen */
  if ((Parameter.epc_verbose == true || Parameter.debug == true) && datalog_ReadOutisActive == false)
  {
    rtc_print_debug_timestamp();
    shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_BLUE, "epc_mem.c: Reset room to mop mapping\n");
  }
}

/*!
 * @brief This functions progress incomming epc tags and checks, if they are listed in the external rfid database
 */
void epc_process_tags(void)
{
  static EPC_BINARY_SERACH_RESULT binary_search_result;

  uint32_t start_time;
  uint32_t stop_time;
  uint32_t cycles_spent;
  uint32_t nanoseconds_spent;

  uint8_t room_changed_flag = false;
  uint8_t room_already_seen_flag = false;
  uint8_t mop_already_used_flag = false;
  uint8_t mop_color_allowed_flag = false;
  uint8_t mop_type_allowed_flag = false;

  if (epc_head_position != epc_tail_position) // if head is equal to tail -> the buffer is empty
  {
    // shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_MAGENTA, "%d\n", epc_tail_position);

    uint8_t rfid_epc_hex[EPC_TOTAL_HEX_LENGTH];
    memset(rfid_epc_hex, 0, EPC_TOTAL_HEX_LENGTH);

    SerializeCharToHex((char *)&epc_ring_buffer[epc_tail_position].string, rfid_epc_hex, EPC_TOTAL_HEX_LENGTH);

    /* Trigger rfid confirmation led if enabled*/
    if (Parameter.rfid_blink_notification == true)
    {
      Notification.next_state = NOTIFICATION_GENERAL_RFID_CONFIRMATION_SHORT;
    }

    /* Live view on console */
    if (Parameter.epc_raw_verbose == true)
    {
      // rtc_print_debug_timestamp();
      shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_CYAN, "Process: %d, %s\n", epc_tail_position, &epc_ring_buffer[epc_tail_position].string);
    }

    /* capture initial time stamp */
    start_time = k_cycle_get_32();

    /* Search for rfid record in data base*/
    binary_search_result = EPC_BinarySearch(GPIO_PIN_FLASH_CS2, &epc_ring_buffer[epc_tail_position].string, EPC_last_rfid_record_index);

    /* capture final time stamp */
    stop_time = k_cycle_get_32();

    /* Compute how long the binary search needed */
    cycles_spent = stop_time - start_time;
    nanoseconds_spent = k_cyc_to_ns_floor64(cycles_spent);

    if (binary_search_result.found == true)
    {
      /* Read rfid record */
      EPC_Memory_Read_RFID_Record(GPIO_PIN_FLASH_CS2, &new_rfid_record, binary_search_result.epc_index);

      /* Live view on console */
      if (Parameter.binary_search_verbose == true)
      {
        rtc_print_debug_timestamp();
        shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_YELLOW, "Binary search result: RFID record index %d, record: %s, type: %d, id: %d", binary_search_result.epc_index, DeserializeHexToChar(new_rfid_record.epc, 20), new_rfid_record.type, new_rfid_record.id);
        shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_CYAN, " --- iterations: %d, time: %2.2f ms\n", binary_search_result.iterations_made, ((float)nanoseconds_spent / 1000000.0));
      }

      /***************** Read tag details from databases depending which type it has ********************************/
      if (new_rfid_record.id > 0)
      {
        switch (new_rfid_record.type)
        {

        case RESERVER_TAG:

          /* Set flag in status bit */
          System.StatusInputs |= STATUSFLAG_RO;
          break;

        case WALL_MOUNT_TAG: // room tags related to hygiene compliance - contamination control  and cleaning evidence type 1

          if (frame_lift_flag[0] == false)
          {
            /* Set flag in status bit */
            System.StatusInputs |= STATUSFLAG_RO;

            /* Read room details from room database */
            EPC_Memory_Read_Room_Record(GPIO_PIN_FLASH_CS2, &new_room_record, new_rfid_record.id - 1);
            new_room_record.room_id++; // ID is one number greater than index in array

            /* Print rfid record and room record details if listed in databases */
            if ((Parameter.epc_verbose == true || Parameter.debug == true) && datalog_ReadOutisActive == false)
            {
              rtc_print_debug_timestamp();
              shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "ROOM ID: %d, allowed_mop_colors: %X, allowed_mop_types: %X, wall_rfid_count: %d, TAG ID: %d, Mop id: %d\n", new_room_record.room_id, new_room_record.allowed_mop_colors, new_room_record.allowed_mop_typegroups, new_room_record.wall_rfid_count, binary_search_result.epc_index, new_mop_record.mop_id);
            }

            /* Check room last seen array if this room is listed */
            room_already_seen_flag = update_last_seen_room_id_array(rfid_epc_hex, new_rfid_record.type, new_rfid_record.id, EPC_LAST_SEEN_COUNT); // returns 1 if room wall tag already exists in array

            if (room_already_seen_flag == 0)
            {
              /* Create event for cloud and user notification (this is triggered within the event) */
              uint8_t rslt = 0;

              rslt = check_or_add_location_record_in_array(rfid_epc_hex, EPC_TOTAL_HEX_LENGTH);
              NewEvent0x07(last_seen_location_records_array[rslt].epc, EPC_TOTAL_HEX_LENGTH);
            }

            /* Check if user changed room */
            room_changed_flag = check_for_room_change();

            /* Live view on console */
            if ((Parameter.epc_verbose == true) && (datalog_ReadOutisActive == false) && (room_changed_flag == true))
            {
              rtc_print_debug_timestamp();
              shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "Room change\n");
            }

            if (frame_lift_flag[0] == false)
            {
              /* Create Event0x19 if the mop was already used in the shift, a new room tag was scanned and the user is currently mopping*/
              if (Flag_SameMopAlreadyUsedNotification == true)
              {
                if ((MoppingFlag[0] == 1) && (chipped_mop_installed == true)) // User is currently mopping
                {
                  NewEvent0x19(current_room_record.room_id, current_room_to_mop_mapping.current_mop_id);
                  Flag_SameMopAlreadyUsedNotification == false;
                }
              }

              if (chipped_mop_installed == true)
              {
                mop_already_used_flag = mop_check_already_used(); // return 1 if mop was already used

                /* if mop was not used in the shift and a mob is linked */
                if ((mop_already_used_flag == 0) && (current_room_record.room_id != 0))
                {
                  mop_color_allowed_flag = check_allowed_mop_color();
                  mop_type_allowed_flag = check_allowed_mop_type();

                  if ((mop_color_allowed_flag == true) && (mop_type_allowed_flag == true))
                  {
                    /* Trigger rfid confirmation led if enabled*/
                    if (Parameter.enable_rfid_confirmation_blinking == true)
                    {
                      Notification.next_state = NOTIFICATION_GENERAL_RFID_CONFIRMATION;
                    }
                  }
                }

                /* If mop is not linked to a room, link mop to current room */
                if ((current_room_to_mop_mapping.mop_linked_room_id == 0) && (current_room_record.room_id != current_room_to_mop_mapping.mop_linked_room_id))
                {
                  if ((mop_installed == true) && (mop_already_used_flag == false) && (mop_color_allowed_flag == true) && (mop_type_allowed_flag == true))
                  {
                    room_to_mop_linkage();
                  }
                }
              }
            }
          }
          break;

        case ROOM_MOUNT_TAG: //  location tags only for indoor positioning

          if (frame_lift_flag[0] == false)
          {
            /* Set flag in status bit */
            System.StatusInputs |= STATUSFLAG_RW;

            /* Trigger rfid confirmation led if enabled*/
            if (Parameter.enable_rfid_confirmation_blinking == true)
            {
              Notification.next_state = NOTIFICATION_GENERAL_RFID_CONFIRMATION;
            }

            /* Read room details from room database */
            EPC_Memory_Read_Room_Record(GPIO_PIN_FLASH_CS2, &new_room_record, new_rfid_record.id - 1);
            new_room_record.room_id++; // ID is one number greater than index in array

            /* Print rfid record and room record details if listed in databases */
            if ((Parameter.epc_verbose == true || Parameter.debug == true) && datalog_ReadOutisActive == false)
            {
              rtc_print_debug_timestamp();
              shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "Location tag (type 2), ROOM record id: %d, allowed_mop_colors: %X, allowed_mop_typegroups: %X, wall_rfid_count: %d\n", new_room_record.room_id, new_room_record.allowed_mop_colors, new_room_record.allowed_mop_typegroups, new_room_record.wall_rfid_count);
            }

            /* Check room last seen array if this room is listed */
            room_already_seen_flag = update_last_seen_room_id_array(rfid_epc_hex, new_rfid_record.type, new_rfid_record.id, EPC_LAST_SEEN_COUNT); // returns 0 if room wall tag already exists in array

            if (room_already_seen_flag == 0)
            {
              /* Create event for cloud and user notification (this is triggered within the event) */
              uint8_t rslt = 0;

              rslt = check_or_add_location_record_in_array(rfid_epc_hex, EPC_TOTAL_HEX_LENGTH);
              NewEvent0x07(last_seen_location_records_array[rslt].epc, EPC_TOTAL_HEX_LENGTH);
            }

            /* Check if user changed room */
            check_for_room_change();
          }
          break;

        case WARNING_WALL_MOUNT_TAG:

          if (frame_lift_flag[0] == false)
          {
            /* Set flag in status bit */
            System.StatusInputs |= STATUSFLAG_RO;

            /* Check room last seen array if this room is listed */
            room_already_seen_flag = update_last_seen_room_id_array(rfid_epc_hex, new_rfid_record.type, new_rfid_record.id, EPC_LAST_SEEN_COUNT); // returns 0 if room wall tag already exists in array

            if (room_already_seen_flag == 0)
            {
              /* Create event for cloud and user notification (this is triggered within the event) */
              uint8_t rslt = 0;

              rslt = check_or_add_location_record_in_array(rfid_epc_hex, EPC_TOTAL_HEX_LENGTH);
              NewEvent0x07(last_seen_location_records_array[rslt].epc, EPC_TOTAL_HEX_LENGTH);
            }

            /* Check if mop is allowed */
            mop_already_used_flag = mop_check_already_used(); // returns 1 if mop was already used, otherwise 0

            if (mop_already_used_flag == 0)
            {
              mop_color_allowed_flag = check_allowed_mop_color();
              mop_type_allowed_flag = check_allowed_mop_type();

              if ((mop_color_allowed_flag == true) && (mop_type_allowed_flag == true))
              {
                /* Trigger rfid confirmation led if enabled*/
                if (Parameter.enable_rfid_confirmation_blinking == true)
                {
                  Notification.next_state = NOTIFICATION_GENERAL_RFID_CONFIRMATION;
                }
              }
            }
          }
          break;

        case MOP_TAG:

          /* Set flag in status bit */
          System.StatusInputs |= STATUSFLAG_RM;

          /* Read mop details from room database */
          EPC_Memory_Read_Mop_Record(GPIO_PIN_FLASH_CS2, &new_mop_record, new_rfid_record.id - 1);
          new_mop_record.mop_id++; // ID is one number greater than index in array

          new_advanced_mop_record.mop_color = new_mop_record.mop_color;
          new_advanced_mop_record.mop_id = new_mop_record.mop_id;
          new_advanced_mop_record.mop_sides = new_mop_record.mop_sides;
          new_advanced_mop_record.mop_size = new_mop_record.mop_size;
          new_advanced_mop_record.mop_typegroup = new_mop_record.mop_typegroup;
          new_advanced_mop_record.timestamp = unixtime_ms;

          last_seen_mop_id = new_mop_record.mop_id;

          /* Print rfid record and mop record details if listed in databases */
          if ((((Parameter.epc_verbose == true) && (Parameter.mop_verbose == true)) || Parameter.debug == true) && datalog_ReadOutisActive == false)
          {
            rtc_print_debug_timestamp();
            shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "Mop id: %d, mop_color: %X, mop_typegroup: %X, mop_size: %d, mop_sides: %d\n", new_mop_record.mop_id, new_mop_record.mop_color, new_mop_record.mop_typegroup, new_mop_record.mop_size, new_mop_record.mop_sides);
          }

          memcpy(current_mob_string_reading, new_rfid_record.epc, 20);
          break;

        default:
          break;
        }
      }
    }

    /* Prepare tag to store in data log */
    SerializeCharToHex((char *)&epc_ring_buffer[epc_head_position].string, DataFrame.rfid_record_hex, 20);

    if (binary_search_result.found == true)
    {
      DataFrame.rfid_record_type = new_rfid_record.type;
    }
    else
    {
      if (Parameter.log_unkown_tags == true)
      {
        /* Set flag in status bit */
        System.StatusInputs |= STATUSFLAG_RO;
        DataFrame.rfid_record_type = 99; // dummy type to identify unkonw tags in log
      }

      /* Live view on console */
      if (Parameter.binary_search_verbose == true)
      {
        rtc_print_debug_timestamp();
        shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_RED, "Binary search result: unkown tag. Logging enabled: %d", Parameter.log_unkown_tags);
        shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_CYAN, " --- iterations: %d, time: %2.2f ms\n", binary_search_result.iterations_made, ((float)nanoseconds_spent / 1000000.0));
      }
    }

    /* Set tail position in the ring buffer */
    if (epc_tail_position >= (EPC_RING_BUFFER_SIZE - 1))
    {
      epc_tail_position = 0;
    }
    else
    {
      epc_tail_position++;
    }

    room_changed_flag = false;
    room_already_seen_flag = false;
    mop_already_used_flag = false;
    mop_color_allowed_flag = false;
    mop_type_allowed_flag = false;
  }
}

void epc_extract_tags_from_buffer(void)
{
  if (uart1_TransmissionLength > 0)
  {
    /* Step through uart1_InputBuffer and extract epc tag strings. The rule is:
    epc tag string starts with a <LF> (0x0A, \n) first character and stop with a
    <CR><LF>.(0x0D 0x0A, \r\n)*/

    for (int i = 0; i < uart1_TransmissionLength; i++)
    {
      /* Start of epc tag string */
      if (uart1_InputBuffer[i] == '\n')
      {
        epc_string_start_flag = true;
        j = 0;
      }

      /* End of epc tag string */
      if ((epc_string_start_flag == true) && (uart1_InputBuffer[i] == '\r') && (uart1_InputBuffer[i + 1] == '\n'))
      {
        epc_string_end_flag = true;
        epc_string_start_flag = false;

        if (Parameter.rfid_verbose == true)
        {
          shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_YELLOW, "%s\n", epc_extracted_last);
        }

        epc_length = strlen(epc_extracted_last);

        /* Set write position (head) in ring buffer */
        if ((epc_head_position + 1) != epc_tail_position) // if (head + 1) is equal to tail -> the buffer is full
        {
          if (epc_head_position >= (EPC_RING_BUFFER_SIZE - 1))
          {
            epc_head_position = 0;
          }
        }
        else
        {
          if (Parameter.epc_raw_verbose == true)
          {
            shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_RED, "epc ring buffer is full\n");
          }
        }

        /* Set each element of restulting string to "0" */
        memset(&epc_ring_buffer[epc_head_position].string, '0', EPC_STRING_LENGTH);

        /* Incomming EPC data has to be trimmed. The rule: take the string (length: epc_length), cut of the first 4 bytes (incoming "PC"), cut of the last 4 bytes and take the middle content
           as the EPC string. The resulting string has to be 20 bytes long. */
        if ((epc_length <= EPC_STRING_LENGTH) && (epc_length > (EPC_CUT_OFF_START + EPC_CUT_OFF_END)))
        {
          /* Push the trimmed epc tag into the ring buffer */
          memcpy(&epc_ring_buffer[epc_head_position].string[(EPC_STRING_LENGTH - 1) - (epc_length - (EPC_CUT_OFF_START + EPC_CUT_OFF_END))], &epc_extracted_last[EPC_CUT_OFF_START], (epc_length - (EPC_CUT_OFF_START + EPC_CUT_OFF_END)));
          epc_ring_buffer[epc_head_position].string[EPC_STRING_LENGTH - 1] = 0;

          if (Parameter.epc_raw_verbose == true)
          {
            shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_YELLOW, "New tag: %d, %d, %d, %s\n", epc_total_tag_counter, epc_session_tag_counter, epc_head_position, epc_ring_buffer[epc_head_position].string);
          }

          /* Update last seen array */
          EPC_Update_last_seen((char *)&epc_ring_buffer[epc_head_position].string);

          epc_total_tag_counter++;   // Counts every tag scaned since boot
          epc_session_tag_counter++; // Counts every tag which was seen since the last motion detection (within the imu motion reset time)
          epc_head_position++;       // Counts the number of tags in thw queue which are not yet search in the database (binary search)
        }

        memset(epc_extracted_last, 0, sizeof(epc_extracted_last));
      }

      /* Extract epc from input buffer */
      if (epc_string_start_flag == true)
      {
        if (j < EPC_STRING_LENGTH)
        {
          if ((uart1_InputBuffer[i] != '\r') && (uart1_InputBuffer[i] != '\n') && (uart1_InputBuffer[i] != 'X') && (uart1_InputBuffer[i] != 'U'))
          {
            epc_extracted_last[j++] = uart1_InputBuffer[i];
          }
        }
      }
    }

    /* Clear input buffer */
    memset(uart1_InputBuffer, 0, sizeof(uart1_InputBuffer));
    uart1_TransmissionLength = 0;
  }
}

/*!
 *  @brief This is the function description
 */
void bubblesort(LAST_SEEN_TAG array[], uint16_t length)
{
  uint16_t i, j;
  LAST_SEEN_TAG tmp;

  for (i = 1; i < length; i++)
  {
    for (j = 0; j < length - i; j++)
    {
      if (array[j].counts > array[j + 1].counts)
      {
        tmp.counts = array[j].counts;
        tmp.timestamp = array[j].timestamp;
        memcpy(&tmp.record, &array[j].record, sizeof(array[j].record));

        array[j].counts = array[j + 1].counts;
        array[j].timestamp = array[j + 1].timestamp;
        memcpy(&array[j].record, &array[j + 1].record, sizeof(array[j + 1].record));

        array[j + 1].counts = tmp.counts;
        array[j + 1].timestamp = tmp.timestamp;
        memcpy(&array[j + 1].record, &tmp.record, sizeof(tmp.record));
      }
    }
  }
}
