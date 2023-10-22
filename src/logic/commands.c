/**
 * @file command.c
 * @author Thomas Keilbach | keiltronic GmbH
 * @date 04 Oct 2023
 * @brief This files provides a set terminal command to control the device behaviour and trace variables
 * @details To use the terminal it is necessary to your host pc over USB
 * @version 2.0.0
 */

/*!
 * @defgroup Commands
 * @brief This files provides a set terminal command to control the device behaviour and trace variables
 * @{*/
#include "commands.h"

/*!
 *  @brief This is the function description
 */
static int cmd_trace(const struct shell *shell, size_t argc, char **argv)
{
  ARG_UNUSED(argc);
  ARG_UNUSED(argv);
  trace_imu_flag = !trace_imu_flag;
  return 0;
}

/*!
 *  @brief This is the function description
 */
static int cmd_read_usb_volt(const struct shell *shell, size_t argc, char **argv)
{
  ARG_UNUSED(argc);
  ARG_UNUSED(argv);

  uint16_t rslt = 0;
  rslt = adc_sample(0);

  float adc_voltage = 0.0;
  float usb_voltage = 0.0;
  adc_voltage = ((float)rslt * ADC_RES_UV) / 1000.0;
  usb_voltage = adc_voltage * (15600.0 / 10000.0); // voltage divider ratio

  shell_print(shell, "digit: %d, adc voltage: %2fmV, thres: %d, USB voltage: %2fmV", rslt, adc_voltage, VUSB_THRES_DIGIT, usb_voltage);
  return 0;
}

/*!
 *  @brief This is the function description
 */
static int cmd_trace_reduced(const struct shell *shell, size_t argc, char **argv)
{
  ARG_UNUSED(argc);
  ARG_UNUSED(argv);
  trace_imu_reduced_flag = !trace_imu_reduced_flag;
  return 0;
}

/*!
 *  @brief This is the function description
 */
static int cmd_factorysettings(const struct shell *shell, size_t argc, char **argv)
{
  ARG_UNUSED(argc);
  ARG_UNUSED(argv);

  factorysettings();
  return 0;
}

/*!
 *  @brief This is the function description
 */
static int cmd_reboot(const struct shell *shell, size_t argc, char **argv)
{
  ARG_UNUSED(argc);
  ARG_UNUSED(argv);

  Device_PushRAMToFlash();
  nrf_modem_lib_shutdown();
  sys_reboot(0);
  return 0;
}

/*!
 *  @brief This is the function description
 */
static int cmd_hard_reboot(const struct shell *shell, size_t argc, char **argv)
{
  ARG_UNUSED(argc);
  ARG_UNUSED(argv);

  Device_PushRAMToFlash();
  lte_lc_power_off();
  gpio_pin_set_dt(&reset_switch, 1);

  return 0;
}

/*!
 *  @brief This is the function description

 */
static int cmd_flash_verbose(const struct shell *shell, size_t argc, char **argv)
{
  ARG_UNUSED(argc);
  ARG_UNUSED(argv);

  if (Parameter.flash_verbose == false)
  {
    Parameter.flash_verbose = true;
    shell_print(shell, "Flash memory verbose mode on");
  }
  else
  {
    Parameter.flash_verbose = false;
    shell_print(shell, "Flash memory verbose mode off");
  }
  Parameter_PushRAMToFlash();
  return 0;
}

/*!
 *  @brief This is the function description

 */
static int cmd_flash_device_id(const struct shell *shell, size_t argc, char **argv)
{
  ARG_UNUSED(argc);
  ARG_UNUSED(argv);

  uint8_t id[4];
  uint16_t err;

  flash_cs(atoi(argv[1]), 0);
  err = flash_access_register(spi_dev, &spi_cfg, FLASH_MANUFACTURER_ID_CMD, &id, sizeof(id));
  flash_cs(atoi(argv[1]), 1);

  if (err)
  {
    return -EIO;
  }

  shell_print(shell, "First 4 ID bytes: 0x%02X 0x%02X 0x%02X 0x%02X", id[0], id[1], id[2], id[3]);

  if (id[0] != 0x20)
  { // Manufacturer ID - 0x20 = Micron
    return -EIO;
  }

  if (id[1] != 0xBA)
  { //  1. byte Device ID - 0xBA = 3V version
    return -EIO;
  }

  if (id[2] != 0x21)
  { //  2. byte Device ID - 0x21 = 1GBit verion
    return -EIO;
  }

  if (id[3] != 0x10)
  { //  3. byte Device ID - 0x10 = number of remaining ID bytes (16 bytes left)
    return -EIO;
  }

  shell_print(shell, "Found Micron 3V 1Gbit NOR flash memory");

  return 0;
}

/*!
 *  @brief This is the function description

 */
static int cmd_flashtest(const struct shell *shell, size_t argc, char **argv)
{
  ARG_UNUSED(argc);
  ARG_UNUSED(argv);

  if (argc == 2)
  {
    if (flash_CommunicationTest(atoi(argv[1])))
    {
      shell_print(shell, "Test passed");
    }
    else
    {
      shell_print(shell, "Test failed");
    }
  }
  return 0;
}

/*!
 *  @brief This is the function description

 */
static int cmd_flashreset(const struct shell *shell, size_t argc, char **argv)
{
  int16_t err;

  if (argc == 2)
  {
    //! #
    // gpio_pin_set_raw(gpio_dev, atoi(argv[1]), 0);
    // err = flash_access_register(spi_dev, &spi_cfg, FLASH_RSTEN, NULL, 0);
    // gpio_pin_set_raw(gpio_dev, atoi(argv[1]), 1);

    // gpio_pin_set_raw(gpio_dev, atoi(argv[1]), 0);
    // err = flash_access_register(spi_dev, &spi_cfg, FLASH_RST, NULL, 0);
    // gpio_pin_set_raw(gpio_dev, atoi(argv[1]), 1);

    shell_print(shell, "OK");
  }
  else
  {
    shell_print(shell, "Invalid parameter list");
  }
  return 0;
}

/*!
 *  @brief This is the function description

 */
static int cmd_flashgetstatusreg(const struct shell *shell, size_t argc, char **argv)
{
  uint8_t status[3];
  uint16_t err;

  if (argc == 2)
  {
    //! #
    // gpio_pin_set_raw(gpio_dev, atoi(argv[1]), 0);
    // err = flash_access_register(spi_dev, &spi_cfg, FLASH_RDSR1, &status[0], 1);
    // gpio_pin_set_raw(gpio_dev, atoi(argv[1]), 1);

    if (err)
    {
      return -EIO;
    }
    //! #
    // gpio_pin_set_raw(gpio_dev, atoi(argv[1]), 0);
    // err = flash_access_register(spi_dev, &spi_cfg, FLASH_RDSR2, &status[1], 1);
    // gpio_pin_set_raw(gpio_dev, atoi(argv[1]), 1);

    if (err)
    {
      return -EIO;
    }
    //! #
    // gpio_pin_set_raw(gpio_dev, atoi(argv[1]), 0);
    // err = flash_access_register(spi_dev, &spi_cfg, FLASH_RDSR3, &status[2], 1);
    // gpio_pin_set_raw(gpio_dev, atoi(argv[1]), 1);

    if (err)
    {
      return -EIO;
    }

    shell_print(shell, "Status 1: 0x%02X, status 2: 0x%02X, status 3: 0x%02X", status[0], status[1], status[2]);
  }
  else
  {
    shell_print(shell, "Invalid parameter list");
  }
  return 0;
}

/*!
 *  @brief This is the function description

 */
static int cmd_flashgetflagstatusreg(const struct shell *shell, size_t argc, char **argv)
{
  uint8_t status;
  uint16_t err;

  if (argc == 2)
  {
    //! #
    // gpio_pin_set_raw(gpio_dev, atoi(argv[1]), 0);
    // err = flash_access_register(spi_dev, &spi_cfg, FLASH_RFSR, &status, 1);
    // gpio_pin_set_raw(gpio_dev, atoi(argv[1]), 1);

    if (err)
    {
      return -EIO;
    }

    shell_print(shell, "Flag stat: 0x%02X", status);
  }
  else
  {
    shell_print(shell, "Invalid parameter list");
  }
  return 0;
}

/*!
 *  @brief This is the function description

 */
static int cmd_flashgclearflagreg(const struct shell *shell, size_t argc, char **argv)
{
  uint16_t err;

  if (argc == 2)
  {
    //! #
    // gpio_pin_set_raw(gpio_dev, atoi(argv[1]), 0);
    // err = flash_access_register(spi_dev, &spi_cfg, FLASH_CLFSR, 0, 0);
    // gpio_pin_set_raw(gpio_dev, atoi(argv[1]), 1);

    if (err)
    {
      shell_print(shell, "Error: %d", -EIO);
      return -EIO;
    }
  }
  else
  {
    shell_print(shell, "Invalid parameter list");
  }
  return 0;
}

/*!
 *  @brief This is the function description

 */
static int cmd_readrange(const struct shell *shell, size_t argc, char **argv)
{
  if (argc == 4)
  {
    uint32_t start_addr = 0;

    start_addr = strtol(argv[2], NULL, 16);
    flash_MemoryViewer(atol(argv[1]), start_addr, atol(argv[3]));
  }
  else
  {
    shell_print(shell, "Address range not valid");
  }
  return 0;
}

/*!
 *  @brief This is the function description

 */
static int cmd_readreg(const struct shell *shell, size_t argc, char **argv)
{
  uint8_t rslt;
  uint16_t err;

  if (argc == 3)
  {
    //! #
    // gpio_pin_set_raw(gpio_dev, atoi(argv[1]), 0);
    // err = flash_access_register(spi_dev, &spi_cfg, (uint8_t)atoi(argv[2]), &rslt, 1);
    // gpio_pin_set_raw(gpio_dev, atoi(argv[1]), 1);

    if (err)
    {
      return -EIO;
    }

    shell_print(shell, "Result: 0x%02X", rslt);
  }
  else
  {
    shell_print(shell, "error");
  }

  return 0;
}

/*!
 *  @brief This is the function description

 */
static int cmd_writereg(const struct shell *shell, size_t argc, char **argv)
{
  uint16_t data = 0;

  if (argc == 5)
  {
    data = atoi(argv[3]);
    flash_write_register((uint8_t)atoi(argv[1]), (uint32_t)atoi(argv[2]), &data, atoi(argv[4])); // <cs-pin>,<reg>,<data>,<len>
  }
  else
  {
    shell_print(shell, "Invalid parameter list");
  }
  return 0;
}

/*!
 *  @brief This is the function description

 */
static int cmd_i2csan(const struct shell *shell, size_t argc, char **argv)
{
  ARG_UNUSED(argc);
  ARG_UNUSED(argv);

  i2c_scanner(1);
  return 0;
}

/*!
 *  @brief This is the function description
 */
static int cmd_battery_voltage(const struct shell *shell, size_t argc, char **argv)
{
  ARG_UNUSED(argc);
  ARG_UNUSED(argv);
  shell_print(shell, "%4.2fmV", battery.Voltage);
  return 0;
}

/*!
 *  @brief This is the function description
 */
static int cmd_battery_current(const struct shell *shell, size_t argc, char **argv)
{
  ARG_UNUSED(argc);
  ARG_UNUSED(argv);
  shell_print(shell, "%4.2fmA", battery.Current);
  return 0;
}

/*!
 *  @brief This is the function description
 */
static int cmd_battery_charge_cycles(const struct shell *shell, size_t argc, char **argv)
{
  ARG_UNUSED(argc);
  ARG_UNUSED(argv);

  if (argc == 1)
  {
    shell_print(shell, "%d", Device.ChargeCycles);
  }
  else
  {
    if (atoi(argv[1]) >= 0)
    {
      Device.ChargeCycles = atol(argv[1]);
      Device_PushRAMToFlash();
      shell_print(shell, "Set battery charge cycles to %d", Device.ChargeCycles);
    }
    else
    {
      shell_error(shell, "Could not set new IMU interval");
    }
  }

  return 0;
}

/*!
 *  @brief This is the function description
 */
static int cmd_battery_temperature(const struct shell *shell, size_t argc, char **argv)
{
  ARG_UNUSED(argc);
  ARG_UNUSED(argv);
  shell_print(shell, "%2.2f degree", battery.Temperature);
  return 0;
}

/*!
 *  @brief This is the function description
 */
static int cmd_battery_time_to_empty(const struct shell *shell, size_t argc, char **argv)
{
  ARG_UNUSED(argc);
  ARG_UNUSED(argv);

  if (System.charger_connected == false)
  {
    uint32_t hours = 0;
    uint32_t minutes = 0;
    uint32_t seconds = 0;

    uint32_t tte = (uint32_t)battery.TimeToEmpty;

    /* Convert seconds to hh:mm:ss */
    hours = tte / 3600;
    minutes = (tte % 3600) / 60;
    seconds = tte - ((hours * 3600) + (minutes * 60));

    shell_fprintf(shell, 0, "Estimate time to empty battery: %02d:%02d:%02d\n", hours, minutes, seconds);
  }
  else
  {
    shell_warn(shell, "USB connected, battery is charging");
  }

  return 0;
}

/*!
 *  @brief This is the function description
 */
static int cmd_battery_time_to_full(const struct shell *shell, size_t argc, char **argv)
{
  ARG_UNUSED(argc);
  ARG_UNUSED(argv);

  if (System.charger_connected == true)
  {
    uint32_t hours = 0;
    uint32_t minutes = 0;
    uint32_t seconds = 0;

    uint32_t ttf = (uint32_t)battery.TimeToFull;

    /* Convert seconds to hh:mm:ss */
    hours = ttf / 3600;
    minutes = (ttf % 3600) / 60;
    seconds = ttf - ((hours * 3600) + (minutes * 60));

    shell_fprintf(shell, 0, "Estimate time for fully charged battery: %02d:%02d:%02d\n", hours, minutes, seconds);
  }
  else
  {
    shell_warn(shell, "USB not connected, battery is not charging");
  }
  return 0;
}

/*!
 *  @brief This is the function description
 */
static int cmd_battery_state_of_charge(const struct shell *shell, size_t argc, char **argv)
{
  ARG_UNUSED(argc);
  ARG_UNUSED(argv);
  shell_print(shell, "%2.1f\%", battery.StateOfCharge);
  return 0;
}

/*!
 *  @brief This is the function description
 */
static int cmd_battery_age(const struct shell *shell, size_t argc, char **argv)
{
  ARG_UNUSED(argc);
  ARG_UNUSED(argv);

  shell_print(shell, "%2.1f\%", battery.age);
  return 0;
}

/*!
 *  @brief This is the function description
 */
static int cmd_battery_remaining_capacity(const struct shell *shell, size_t argc, char **argv)
{
  ARG_UNUSED(argc);
  ARG_UNUSED(argv);
  shell_print(shell, "%4.2fmAh", battery.RemainingCapacity);
  return 0;
}

/*!
 *  @brief This is the function description
 */
static int cmd_i2cread(const struct shell *shell, size_t argc, char **argv)
{
  uint8_t reg_read[3];
  uint8_t reg_write[3];
  uint8_t rslt = 0;

  if (argc == 3)
  {
    reg_write[0] = atoi(argv[2]);
    rslt = i2c_write_read(i2c_dev, atoi(argv[1]), reg_write, 1, reg_read, 1);

    shell_print(shell, "Result: %d, Device: 0x%X, register: 0x%X, read value: 0x%X", rslt, atoi(argv[1]), reg_write[0], reg_read[0]);
  }
  else
  {
    shell_print(shell, "Invalid I2C read command.");
  }
  return 0;
}

/*!
 *  @brief This is the function description

 */
static int cmd_i2cwrite(const struct shell *shell, size_t argc, char **argv)
{
  uint8_t rslt = 0;
  uint8_t write_data[2];

  write_data[0] = atoi(argv[2]);
  write_data[1] = atoi(argv[3]);

  // rslt = i2c_write(i2c_dev, write_data, 2UL, (uint16_t)atoi(argv[1]));

  // shell_print(shell, "Result: %d, Device: 0x%X, register: 0x%X, write value: 0x%X", rslt, atoi(argv[1]), atoi(argv[2]), atoi(argv[3]));
  return 0;
}

/*!
 *  @brief This is the function description

 */
static int cmd_datalog_clear(const struct shell *shell, size_t argc, char **argv)
{
  ARG_UNUSED(argc);
  ARG_UNUSED(argv);

  datalog_CleardatalogAll();
  datalog_MemoryFull = false;

  rtc_print_debug_timestamp();
  shell_print(shell, "Formatting done");

  return 0;
}

/*!
 *  @brief This is the function description

 */
static int cmd_datalog_format(const struct shell *shell, size_t argc, char **argv)
{
  ARG_UNUSED(argc);
  ARG_UNUSED(argv);

  rtc_print_debug_timestamp();
  shell_print(shell, "Starting formating datalog memory. This task takes a few seconds to complete");
  datalog_CleardatalogAll();
  datalog_MemoryFull = false;

  rtc_print_debug_timestamp();
  shell_print(shell, "Formatting done");
  return 0;
}

/*!
 *  @brief This is the function description
 *  @syntax: cmd_add_rfid_record <id> <epc> <type>
 */
static int cmd_add_rfid_record(const struct shell *shell, size_t argc, char **argv)
{
  if (argc == 4)
  {
    RFID_RECORD new_rfid_record;
    memset(new_rfid_record.epc, 0, 21);

    SerializeCharToHex(argv[1], new_rfid_record.epc, (strlen(argv[1]) / 2));
    new_rfid_record.type = atoi(argv[2]);
    new_rfid_record.id = atoi(argv[3]);

    EPC_Memory_Write_RFID_Record(GPIO_PIN_FLASH_CS2, &new_rfid_record, EPC_last_rfid_record_index++);
  }
  else
  {
    shell_print(shell, "Invalid command");
  }
  return 0;
}

/*!
 *  @brief This is the function description
 */
static int cmd_add_room_record(const struct shell *shell, size_t argc, char **argv)
{
  if (argc == 5)
  {
    ROOM_RECORD new_room_record;
    uint16_t index = 0;

    index = atoi(argv[1]);
    new_room_record.room_id = index;
    new_room_record.allowed_mop_colors = strtoul(argv[2], NULL, 16);     // convert the generated hex number string (base 16) to a decimal number
    new_room_record.allowed_mop_typegroups = strtoul(argv[3], NULL, 16); // convert the generated hex number string (base 16) to a decimal number
    new_room_record.wall_rfid_count = atoi(argv[4]);

    EPC_Memory_Write_Room_Record(GPIO_PIN_FLASH_CS2, &new_room_record, index);
    EPC_last_room_record_index++;
  }
  else
  {
    shell_print(shell, "Invalid command");
  }
  return 0;
}

/*!
 *  @brief This is the function description
 */
static int cmd_add_mop_record(const struct shell *shell, size_t argc, char **argv)
{
  if (argc == 6)
  {
    MOP_RECORD new_mop_record;
    uint16_t index = 0;

    index = atoi(argv[1]);
    new_mop_record.mop_id = index;
    new_mop_record.mop_color = (uint8_t)strtoul(argv[2], NULL, 16);     // convert the generated hex number string (base 16) to a decimal number
    new_mop_record.mop_typegroup = (uint8_t)strtoul(argv[3], NULL, 16); // convert the generated hex number string (base 16) to a decimal number
    new_mop_record.mop_size = (uint8_t)atoi(argv[4]);
    new_mop_record.mop_sides = (uint8_t)atoi(argv[5]);

    EPC_Memory_Write_Mop_Record(GPIO_PIN_FLASH_CS2, &new_mop_record, index);
    EPC_last_mop_record_index++;
  }
  else
  {
    shell_print(shell, "Invalid command");
  }
  return 0;
}

/*!
 *  @brief This is the function description
 */
static int cmd_read_rfid_record(const struct shell *shell, size_t argc, char **argv)
{
  if (argc == 2)
  {
    RFID_RECORD new_rfid_record;
    uint32_t index = 0;

    index = atol(argv[1]);

    EPC_Memory_Read_RFID_Record(GPIO_PIN_FLASH_CS2, &new_rfid_record, index);
    shell_print(shell, "epc: %s, type: %d, id: %d", DeserializeHexToChar(new_rfid_record.epc, 20), new_rfid_record.type, new_rfid_record.id);
  }
  else
  {
    shell_print(shell, "Invalid command");
  }
  return 0;
}

/*!
 *  @brief This is the function description
 */
static int cmd_read_room_record(const struct shell *shell, size_t argc, char **argv)
{
  if (argc == 2)
  {

    ROOM_RECORD new_room_record;
    uint32_t index = 0;

    index = atol(argv[1]);

    EPC_Memory_Read_Room_Record(GPIO_PIN_FLASH_CS2, &new_room_record, index);
    shell_print(shell, "id: %d, allowed_mop_colors: %X, allowed_mop_typegroups: %X, wall_rfid_count: %d", new_room_record.room_id, new_room_record.allowed_mop_colors, new_room_record.allowed_mop_typegroups, new_room_record.wall_rfid_count);
  }
  else
  {
    shell_print(shell, "Invalid command");
  }
  return 0;
}

/*!
 *  @brief This is the function description
 */
static int cmd_read_mop_record(const struct shell *shell, size_t argc, char **argv)
{

  if (argc == 2)
  {
    MOP_RECORD new_mop_record;
    uint32_t index = 0;

    index = atol(argv[1]);

    EPC_Memory_Read_Mop_Record(GPIO_PIN_FLASH_CS2, &new_mop_record, index);
    shell_print(shell, "id: %d, mop_color: %X, mop_typegroup: %X, mop_size: %d, mop_sides: %d", new_mop_record.mop_id, new_mop_record.mop_color, new_mop_record.mop_typegroup, new_mop_record.mop_size, new_mop_record.mop_sides);
  }
  else
  {
    shell_print(shell, "Invalid command");
  }
  return 0;
}

/*!
 *  @brief This is the function description
 */
static int cmd_clear_epc_database(const struct shell *shell, size_t argc, char **argv)
{
  ARG_UNUSED(argc);
  ARG_UNUSED(argv);

  /* Clear RFID records */
  EPC_Memory_Delete_All_Records(GPIO_PIN_FLASH_CS2, RFID_RECORD_REGION, RFID_RECORD_REGION_LENGTH);
  EPC_last_rfid_record_index = 0;

  k_msleep(100);

  /* Clear room records */
  EPC_Memory_Delete_All_Records(GPIO_PIN_FLASH_CS2, ROOM_RECORD_REGION, ROOM_RECORD_REGION_LENGTH);
  EPC_last_room_record_index = 0;

  k_msleep(100);

  /* Clear mop records */
  EPC_Memory_Delete_All_Records(GPIO_PIN_FLASH_CS2, MOP_RECORD_REGION, MOP_RECORD_REGION_LENGTH);
  EPC_last_mop_record_index = 0;

  return 0;
}

/*!
 *  @brief This is the function description
 */
static int cmd_clear_rfid_record(const struct shell *shell, size_t argc, char **argv)
{

  ARG_UNUSED(argc);
  ARG_UNUSED(argv);

  EPC_Memory_Delete_All_Records(GPIO_PIN_FLASH_CS2, RFID_RECORD_REGION, RFID_RECORD_REGION_LENGTH);
  EPC_last_rfid_record_index = 0;
  return 0;
}

/*!
 *  @brief This is the function description
 */
static int cmd_clear_room_record(const struct shell *shell, size_t argc, char **argv)
{
  ARG_UNUSED(argc);
  ARG_UNUSED(argv);

  EPC_Memory_Delete_All_Records(GPIO_PIN_FLASH_CS2, ROOM_RECORD_REGION, ROOM_RECORD_REGION_LENGTH);
  EPC_last_room_record_index = 0;
  return 0;
}

/*!
 *  @brief This is the function description
 */
static int cmd_clear_mop_record(const struct shell *shell, size_t argc, char **argv)
{
  ARG_UNUSED(argc);
  ARG_UNUSED(argv);

  EPC_Memory_Delete_All_Records(GPIO_PIN_FLASH_CS2, MOP_RECORD_REGION, MOP_RECORD_REGION_LENGTH);
  EPC_last_mop_record_index = 0;
  return 0;
}

/*!
 *  @brief This is the function description
 */
static int cmd_search_rfid_record(const struct shell *shell, size_t argc, char **argv)
{
  EPC_BINARY_SERACH_RESULT myresult;

  uint32_t start_time;
  uint32_t stop_time;
  uint32_t cycles_spent;
  uint32_t nanoseconds_spent;

  if (argc == 2)
  {
    /* capture initial time stamp */
    start_time = k_cycle_get_32();

    /* Search rfid database for match */
    myresult = EPC_BinarySearch(GPIO_PIN_FLASH_CS2, argv[1], EPC_last_rfid_record_index);

    /* capture final time stamp */
    stop_time = k_cycle_get_32();

    /* compute how long the work took (assumes no counter rollover) */
    cycles_spent = stop_time - start_time;
    // nanoseconds_spent = SYS_CLOCK_HW_CYCLES_TO_NS(cycles_spent);
    nanoseconds_spent = k_cyc_to_ns_floor64(cycles_spent);
  }
  else
  {
    shell_print(shell, "Invalid rfid record");
  }

  /* Print out binary search result details */
  if (myresult.found == true)
  {
    shell_print(shell, "\nBinary search result: iterations: %d, time: %d usec", myresult.iterations_made, (nanoseconds_spent / 1000));
    /* Print out rfid, room and mop details */
    RFID_RECORD new_rfid_record;
    ROOM_RECORD new_room_record;
    MOP_RECORD new_mop_record;

    EPC_Memory_Read_RFID_Record(GPIO_PIN_FLASH_CS2, &new_rfid_record, myresult.epc_index);
    shell_print(shell, "RFID record index: %d, record: %s, type: %d, id: %d", myresult.epc_index, DeserializeHexToChar(new_rfid_record.epc, 20), new_rfid_record.type, new_rfid_record.id);

    switch (new_rfid_record.type)
    {

    case RESERVER_TAG:
      break;

    case WALL_MOUNT_TAG:
      break;

    case ROOM_MOUNT_TAG:

      EPC_Memory_Read_Room_Record(GPIO_PIN_FLASH_CS2, &new_room_record, new_rfid_record.id);
      shell_print(shell, "ROOM id: %d, allowed_mop_colors: %X, allowed_mop_typegroups: %X, wall_rfid_count: %d", new_room_record.room_id, new_room_record.allowed_mop_colors, new_room_record.allowed_mop_typegroups, new_room_record.wall_rfid_count);
      break;

    case WARNING_WALL_MOUNT_TAG:
      break;

    case MOP_TAG:

      EPC_Memory_Read_Mop_Record(GPIO_PIN_FLASH_CS2, &new_mop_record, new_rfid_record.id);
      shell_print(shell, "MOP id: %d, mop_color: %X, mop_typegroup: %X, mop_size: %d, mop_sides: %d", new_mop_record.mop_id, new_mop_record.mop_color, new_mop_record.mop_typegroup, new_mop_record.mop_size, new_mop_record.mop_sides);
      break;

    default:
      break;
    }
  }
  else
  {
    shell_print(shell, "RFID record not found in rfid record database");
  }

  return 0;
}

/*!
 *  @brief This is the function description
 */
static int cmd_count_rfid_record(const struct shell *shell, size_t argc, char **argv)
{
  EPC_last_rfid_record_index = EPC_Memory_GetLastIndex(GPIO_PIN_FLASH_CS2, RFID_RECORD_REGION, RFID_RECORD_REGION_LENGTH, RFID_RECORD_BYTE_LENGTH);
  shell_print(shell, "Last index number in wall records: %d", EPC_last_rfid_record_index);
  return 0;
}

/*!
 *  @brief This is the function description
 */
static int cmd_count_room_record(const struct shell *shell, size_t argc, char **argv)
{
  EPC_last_room_record_index = EPC_Memory_GetLastIndex(GPIO_PIN_FLASH_CS2, ROOM_RECORD_REGION, ROOM_RECORD_REGION_LENGTH, ROOM_RECORD_BYTE_LENGTH);
  shell_print(shell, "Last index number in room records: %d", EPC_last_room_record_index);
  return 0;
}

/*!
 *  @brief This is the function description
 */
static int cmd_count_mop_record(const struct shell *shell, size_t argc, char **argv)
{
  EPC_last_mop_record_index = EPC_Memory_GetLastIndex(GPIO_PIN_FLASH_CS2, MOP_RECORD_REGION, MOP_RECORD_REGION_LENGTH, MOP_RECORD_BYTE_LENGTH);
  shell_print(shell, "Last index number in mop records: %d", EPC_last_mop_record_index);
  return 0;
}

/*!
 *  @brief This is the function description
 */
static int cmd_listall_rfid_record(const struct shell *shell, size_t argc, char **argv)
{
  uint16_t i = 0;

  RFID_RECORD new_rfid_record;

  for (i = 0; i < EPC_last_rfid_record_index; i++)
  {
    EPC_Memory_Read_RFID_Record(GPIO_PIN_FLASH_CS2, &new_rfid_record, i);
    shell_print(shell, "Index: %d, epc: %s, type: %d, id: %d", i, DeserializeHexToChar(new_rfid_record.epc, 20), new_rfid_record.type, new_rfid_record.id);
  }
  return 0;
}

/*!
 *  @brief This is the function description
 */
static int cmd_listall_room_record(const struct shell *shell, size_t argc, char **argv)
{
  uint16_t i = 0;

  ROOM_RECORD new_room_record;

  for (i = 0; i < EPC_last_room_record_index; i++)
  {
    EPC_Memory_Read_Room_Record(GPIO_PIN_FLASH_CS2, &new_room_record, i);
    shell_print(shell, "id: %d, allowed_mop_colors: %X, allowed_mop_typegroups: %X, wall_rfid_count: %d", new_room_record.room_id, new_room_record.allowed_mop_colors, new_room_record.allowed_mop_typegroups, new_room_record.wall_rfid_count);
  }
  return 0;
}

/*!
 *  @brief This is the function description
 */
static int cmd_listall_mop_record(const struct shell *shell, size_t argc, char **argv)
{
  uint16_t i = 0;

  MOP_RECORD new_mop_record;

  for (i = 0; i < EPC_last_mop_record_index; i++)
  {
    EPC_Memory_Read_Mop_Record(GPIO_PIN_FLASH_CS2, &new_mop_record, i);
    shell_print(shell, "id: %d, mop_color: %X, mop_typegroup: %X, mop_size: %d, mop_sides: %d", new_mop_record.mop_id, new_mop_record.mop_color, new_mop_record.mop_typegroup, new_mop_record.mop_size, new_mop_record.mop_sides);
  }
  return 0;
}

/*!
 *  @brief This is the function description
 */
static int cmd_epc_last_seen_record(const struct shell *shell, size_t argc, char **argv)
{
  ARG_UNUSED(argc);
  ARG_UNUSED(argv);

  uint16_t i = 0;
  struct tm *ptm;
  char buf[50] = {0};
  uint16_t milli = 0;

  float count_sum = 0.0;
  float percentage = 0.0;

  uint8_t sortet_array_index[EPC_LAST_SEEN_COUNT];
  memset(sortet_array_index, 0, EPC_LAST_SEEN_COUNT);

  /* Calculate total number of all seen tags */
  for (i = 0; i < EPC_LAST_SEEN_COUNT; i++)
  {
    count_sum += (float)EPC_last_seen_records[i].counts;
  }

  /* Sort array */
  bubblesort(EPC_last_seen_records, EPC_LAST_SEEN_COUNT);

  /* Print sorted array */
  for (i = (EPC_LAST_SEEN_COUNT - 1); i > 0; i--)
  {
    if (EPC_last_seen_records[i].counts != 0)
    {
      /* Fetch unix time stamp from event and convert it to human readable date and time in millisec resolution */
      int64_t timestamp = EPC_last_seen_records[i].timestamp;

      milli = timestamp % 1000LL; // Get only the millisec from the unix time stamp
      timestamp /= 1000LL;        // time.h can only handly timestamp in sec resolution, so cut the millisec

      /* convert from unix to local time */
      ptm = localtime(&timestamp);
      strftime(buf, 20, "%F %T", ptm);

      /* Calulate percentage ratio */
      percentage = (((float)EPC_last_seen_records[i].counts * 100.0) / count_sum);

      shell_print(shell, "%d: %s, last seen timestamp: %s:%03d, counts: %d, percentage: %3.2f\%", (EPC_LAST_SEEN_COUNT - i), EPC_last_seen_records[i].record.string, buf, milli, EPC_last_seen_records[i].counts, percentage);
    }
  }
  return 0;
}

/*!
 *  @brief This is the function description
 */
static int cmd_clear_last_seen_record(const struct shell *shell, size_t argc, char **argv)
{
  ARG_UNUSED(argc);
  ARG_UNUSED(argv);

  EPC_Clear_last_seen();
  return 0;
}

/*!
 *  @brief This is the function description
 */
static int cmd_rfid_blink_notification(const struct shell *shell, size_t argc, char **argv)
{
  ARG_UNUSED(argc);
  ARG_UNUSED(argv);

  if (Parameter.rfid_blink_notification == false)
  {
    Parameter.rfid_blink_notification = true;
    shell_print(shell, "Enabled rfid confirmation led for all incoming epc tags");
  }
  else
  {
    Parameter.rfid_blink_notification = false;
    shell_print(shell, "Disabled rfid confirmation led for all incoming epc tags");
  }
  Parameter_PushRAMToFlash();

  return 0;
}

/*!
 *  @brief This is the function description
 */
static int cmd_datalog_sniff_frame(const struct shell *shell, size_t argc, char **argv)
{
  ARG_UNUSED(argc);
  ARG_UNUSED(argv);

  if (Parameter.datalog_sniffFrame == false)
  {
    Parameter.datalog_sniffFrame = true;
    shell_print(shell, "Enabled datlog frame live view");
  }
  else
  {
    Parameter.datalog_sniffFrame = false;
    shell_print(shell, "Disabled datlog frame live view");
  }
  Parameter_PushRAMToFlash();

  return 0;
}

/*!
 *  @brief This is the function description
 */
static int cmd_notification_verbose(const struct shell *shell, size_t argc, char **argv)
{
  ARG_UNUSED(argc);
  ARG_UNUSED(argv);

  if (Parameter.notification_verbose == false)
  {
    Parameter.notification_verbose = true;
    shell_print(shell, "Enabled notification verbose mode");
  }
  else
  {
    Parameter.notification_verbose = false;
    shell_print(shell, "Disabled notification verbose mode");
  }
  Parameter_PushRAMToFlash();

  return 0;
}

/*!
 *  @brief This is the function description
 */
static int cmd_datalog_get_count(const struct shell *shell, size_t argc, char **argv)
{
  ARG_UNUSED(argc);
  ARG_UNUSED(argv);

  uint32_t rslt = 0UL;

  rslt = flash_GetLastFrameNumber(GPIO_PIN_FLASH_CS1, FLASH_SUBSUBSECTOR_SIZE, DATALOG_MEM, DATALOG_MEM_LENGTH, DATALOG_FRAME_LENGTH);
  shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "Last frame number: %ld, addr: %ld\n", rslt, DATALOG_MEM + (rslt * DATALOG_FRAME_LENGTH));

  return 0;
}

/*!
 *  @brief This is the function description
 */
static int cmd_datalog_interval(const struct shell *shell, size_t argc, char **argv)
{
  if (argc == 1)
  {
    shell_print(shell, "interval: %dms", Parameter.datalog_Interval);
  }
  else
  {
    if (atoi(argv[1]) >= 5 || atoi(argv[1]) == 0)
    {
      Parameter.datalog_Interval = atoi(argv[1]);
      Parameter_PushRAMToFlash();
      shell_print(shell, "Set interval to %dms", Parameter.datalog_Interval);
    }
    else
    {
      shell_error(shell, "Error. Minimum interval is 5 ms or 0 to switch off time based logging.");
    }
  }
  return 0;
}

/*!
 *  @brief This is the function description
 */
static int cmd_fully_charged_indicator_time(const struct shell *shell, size_t argc, char **argv)
{
  if (argc == 1)
  {
    shell_print(shell, "fully_charged_indicator_time: %d sec", Parameter.fully_charged_indicator_time);
  }
  else
  {
    Parameter.fully_charged_indicator_time = atoi(argv[1]);
    Parameter_PushRAMToFlash();
    shell_print(shell, "Set interval to %dms", Parameter.fully_charged_indicator_time);
  }
  return 0;
}

/*!
 *  @brief This is the function description
 */
static int cmd_datalog_enable(const struct shell *shell, size_t argc, char **argv)
{
  if (argc == 1)
  {
    if (datalog_EnableFlag == 0)
    {
      shell_print(shell, "datalog is currently disabled");
    }
    else
    {
      shell_print(shell, "datalog is currently enabled");
    }
  }
  else
  {
    if (atoi(argv[1]) > 0)
    {
      Parameter.datalogEnable = true;
      datalog_EnableFlag = true;
      shell_print(shell, "datalog enabled");
    }
    else
    {
      Parameter.datalogEnable = false;
      datalog_EnableFlag = false;
      shell_print(shell, "datalog disabled");
    }
    Parameter_PushRAMToFlash();
  }
  return 0;
}

/*!
 *  @brief This is the function description
 */
static int cmd_cloud_sync_interval_idle(const struct shell *shell, size_t argc, char **argv)
{
  if (argc == 1)
  {
    shell_print(shell, "Current cloud sync interval in idle mode: %ld sec", Parameter.cloud_sync_interval_idle);
  }
  else
  {
    if (atoi(argv[1]) > 0)
    {
      Parameter.cloud_sync_interval_idle = atol(argv[1]);
      coap_last_transmission_timer = Parameter.cloud_sync_interval_idle;
      shell_print(shell, "OK");
      Parameter_PushRAMToFlash();
    }
  }
  return 0;
}

/*!
 *  @brief This is the function description
 */
static int cmd_cloud_sync_interval_moving(const struct shell *shell, size_t argc, char **argv)
{
  if (argc == 1)
  {
    shell_print(shell, "Current cloud sync interval in moving mode: %ld sec", Parameter.cloud_sync_interval_moving);
  }
  else
  {
    if (atoi(argv[1]) > 0)
    {
      Parameter.cloud_sync_interval_moving = atol(argv[1]);
      coap_last_transmission_timer = Parameter.cloud_sync_interval_moving;
      shell_print(shell, "OK");
      Parameter_PushRAMToFlash();
    }
  }
  return 0;
}

/*!
 *  @brief This is the function description
 */
static int cmd_datalog_get_data(const struct shell *shell, size_t argc, char **argv)
{
  ARG_UNUSED(argc);
  ARG_UNUSED(argv);

  if (argc == 3)
  {
    if (datalog_ReadOutisActive == false)
    {
      setaddress_flag = false;

      if (atol(argv[1]) >= 0)
      {
        System.datalog_StartFrame = atol(argv[1]);
      }
      else
      {
        System.datalog_StartFrame = 0UL;
      }
      if (atol(argv[2]) >= 0)
      {
        System.datalog_EndFrame = atol(argv[2]);
      }
      else
      {
        System.datalog_EndFrame = 0UL;
      }
    }
    else
    {
      if (Parameter.datalogEnable == true)
      {
        datalog_EnableFlag = true;
      }
      else
      {
        datalog_EnableFlag = false;
      }
    }

    datalog_ReadOutisActive = !datalog_ReadOutisActive;
  }
  else
  {
    shell_print(shell, "Invalid range. Syntax: <last_line_number> <stop_line_number>");
  }

  return 0;
}

/*!
 *  @brief This is the function description
 */
static int cmd_rfid_verbose(const struct shell *shell, size_t argc, char **argv)
{
  ARG_UNUSED(argc);
  ARG_UNUSED(argv);

  if (Parameter.rfid_verbose == false)
  {
    Parameter.rfid_verbose = true;
    shell_print(shell, "Enabled RFID verbose mode");
  }
  else
  {
    Parameter.rfid_verbose = false;
    shell_print(shell, "Disabled RFID verbose mode");
  }
  Parameter_PushRAMToFlash();
  return 0;
}

/*!
 *  @brief This is the function description
 */
static int cmd_binary_search_verbose(const struct shell *shell, size_t argc, char **argv)
{
  ARG_UNUSED(argc);
  ARG_UNUSED(argv);

  if (Parameter.binary_search_verbose == false)
  {
    Parameter.binary_search_verbose = true;
    shell_print(shell, "Enabled binary search verbose mode");
  }
  else
  {
    Parameter.binary_search_verbose = false;
    shell_print(shell, "Disabled binary search verbose mode");
  }
  Parameter_PushRAMToFlash();
  return 0;
}

/*!
 *  @brief This is the function description
 */
static int cmd_mop_verbose(const struct shell *shell, size_t argc, char **argv)
{
  ARG_UNUSED(argc);
  ARG_UNUSED(argv);

  if (Parameter.mop_verbose == false)
  {
    Parameter.mop_verbose = true;
    shell_print(shell, "Enabled mop verbose mode");
  }
  else
  {
    Parameter.mop_verbose = false;
    shell_print(shell, "Disabled mop verbose mode");
  }
  Parameter_PushRAMToFlash();
  return 0;
}

/*!
 *  @brief This is the function description
 */
static int cmd_epc_raw_verbose(const struct shell *shell, size_t argc, char **argv)
{
  ARG_UNUSED(argc);
  ARG_UNUSED(argv);

  if (Parameter.epc_raw_verbose == false)
  {
    Parameter.epc_raw_verbose = true;
    shell_print(shell, "Enabled epc raw verbose mode");
  }
  else
  {
    Parameter.epc_raw_verbose = false;
    shell_print(shell, "Disabled epc raw verbose mode");
  }
  Parameter_PushRAMToFlash();
  return 0;
}

/*!
 *  @brief This is the function description
 */
static int cmd_epc_verbose(const struct shell *shell, size_t argc, char **argv)
{
  ARG_UNUSED(argc);
  ARG_UNUSED(argv);

  if (Parameter.epc_verbose == false)
  {
    Parameter.epc_verbose = true;
    shell_print(shell, "Enabled epc verbose mode");
  }
  else
  {
    Parameter.epc_verbose = false;
    shell_print(shell, "Disabled epc verbose mode");
  }
  Parameter_PushRAMToFlash();
  return 0;
}

/*!
 *  @brief This is the function description
 */
static int cmd_log_unkown_tags(const struct shell *shell, size_t argc, char **argv)
{
  ARG_UNUSED(argc);
  ARG_UNUSED(argv);

  if (Parameter.log_unkown_tags == false)
  {
    Parameter.log_unkown_tags = true;
    shell_print(shell, "Logging unkonw tags enabled.");
  }
  else
  {
    Parameter.log_unkown_tags = false;
    shell_print(shell, "Logging unkonw tags disabled.");
  }
  Parameter_PushRAMToFlash();
  return 0;
}

/*!
 *  @brief This is the function description
 */
static int cmd_list_last_seen_mop_ids(const struct shell *shell, size_t argc, char **argv)
{
  ARG_UNUSED(argc);
  ARG_UNUSED(argv);

  uint16_t i = 0;

  for (i = 0; i < last_seen_mop_count; i++)
  {
    shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "Mop ID: %d\n", last_seen_mop_records_array[i].mop_id);
  }
  return 0;
}

/*!
 *  @brief This is the function description
 */
static int cmd_clear_last_seen_mop_array(const struct shell *shell, size_t argc, char **argv)
{
  Mop_ClearLastSeenArray();
  shell_print(shell, "Last seen mop array cleard");
  return 0;
}

/*!
 *  @brief This is the function description
 */
static int cmd_mop_array_auto_reset_time(const struct shell *shell, size_t argc, char **argv)
{
  ARG_UNUSED(argc);
  ARG_UNUSED(argv);

  if (argc == 1)
  {
    shell_fprintf(shell, 0, "%d\n", Parameter.last_seen_mop_array_auto_reset_time);
  }
  else
  {
    Parameter.last_seen_mop_array_auto_reset_time = atoi(argv[1]);
    Parameter_PushRAMToFlash();
    shell_fprintf(shell, 0, "New value: %d\n", Parameter.last_seen_mop_array_auto_reset_time);
  }

  return 0;
}

/*!
 *  @brief This is the function description
 */
static int cmd_rfid_sniff(const struct shell *shell, size_t argc, char **argv)
{
  ARG_UNUSED(argc);
  ARG_UNUSED(argv);

  if (System.RFID_Sniff == false)
  {
    System.RFID_Sniff = true;
    shell_print(shell, "RFID module sniffer activated.");

    RFID_autoscan_enabled = true;
    config_RFID();
    k_msleep(50);
    RFID_ScanEnable = true;

    if (frame_lift_flag[0] == 1) // frame is lifted
    {
      shell_print(shell, "Enabled RFID auto scan. Interval: %dms", Parameter.rfid_interval_lifted);
    }
    else
    {
      shell_print(shell, "Enabled RFID auto scan. Interval: %dms", Parameter.rfid_interval);
    }
  }
  else
  {
    System.RFID_Sniff = false;
    shell_print(shell, "RFID module sniffer deactivated.");

    RFID_autoscan_enabled = false;
    RFID_TurnOff();
    RFID_ScanEnable = false;
    shell_print(shell, "Disabled RFID auto scan");
  }
  return 0;
}

/*!
 *  @brief This is the function description
 */
static int cmd_rfid_trigger(const struct shell *shell, size_t argc, char **argv)
{
  static bool rfid_trigger_enabled = false;

  if (rfid_trigger_enabled == false)
  {
    rfid_trigger_enabled = true;

    RFID_IsOn = true;
    RFID_ScanEnable = true;

    if (RFID_IsOn == false)
    {
      config_RFID();
      RFID_TurnOn();
      k_msleep(100);
    }

    gpio_pin_set_dt(&rfid_trigger_pin, 0);
    shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "RFID trigger mode activated.\n");
  }
  else
  {
    rfid_trigger_enabled = false;

    RFID_IsOn = false;
    RFID_ScanEnable = false;

    RFID_TurnOff();

    // System.RFID_TransparentMode = true;
    gpio_pin_set_dt(&rfid_trigger_pin, 1);
    shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "RFID trigger mode deactivated.\n");
  }

  Parameter_PushRAMToFlash();
  return 0;
}

/*!
 *  @brief This is the function description
 */
static int cmd_rfid_singlescan(const struct shell *shell, size_t argc, char **argv)
{
  ARG_UNUSED(argc);
  ARG_UNUSED(argv);

  config_RFID();
  k_msleep(50);
  rfid_trigger_multi_read();
  RFID_TurnOff();

  RFID_IsOn = true;
  RFID_ScanEnable = true;

  return 0;
}

/*!
 *  @brief This is the function description
 */
static int cmd_rfid_autoscan(const struct shell *shell, size_t argc, char **argv)
{
  ARG_UNUSED(argc);
  ARG_UNUSED(argv);

  if (Parameter.rfid_autoscan == false)
  {
    RFID_autoscan_enabled = true;
    Parameter.rfid_autoscan = true;
    config_RFID();
    k_msleep(50);
    RFID_ScanEnable = true;

    if (frame_lift_flag[0] == 1) // frame is lifted
    {
      shell_print(shell, "Enabled RFID auto scan. Interval: %dms", Parameter.rfid_interval_lifted);
    }
    else
    {
      shell_print(shell, "Enabled RFID auto scan. Interval: %dms", Parameter.rfid_interval);
    }

    /* For debugging prupose enable blue dev led when motion detected*/
    if (Parameter.enable_blue_dev_led == true)
    {
      gpio_pin_set_dt(&dev_led, 1);
    }
  }
  else
  {
    Parameter.rfid_autoscan = false;
    RFID_autoscan_enabled = false;
    RFID_TurnOff();
    RFID_ScanEnable = false;
    shell_print(shell, "Disabled RFID auto scan");

    /* For debugging prupose enable blue dev led when motion detected*/
    if (Parameter.enable_blue_dev_led == true)
    {
      gpio_pin_set_dt(&dev_led, 1);
    }
  }
  Parameter_PushRAMToFlash();
  return 0;
}

/*!
 *  @brief This is the function description
 */
static int cmd_rfid_settings(const struct shell *shell, size_t argc, char **argv)
{
  ARG_UNUSED(argc);
  ARG_UNUSED(argv);

  /* output power */
  shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "Output power: \t\t\t\t%d dBm\n", RFID_getOutputPower());

  /* Frequency band */
  int16_t freq = 0;
  freq = RFID_getFrequency();

  switch (freq)
  {
  case 1:
    shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "Frequency band: \t\t\t01: US 902-925 MHz\n");
    break;

  case 2:
    shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "Frequency band: \t\t\t02: TW 922-928 MHz\n");
    break;

  case 3:
    shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "Frequency band: \t\t\t03: CN 920-925 MHz\n");
    break;

  case 4:
    shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "Frequency band: \t\t\t04: CN2 840-845 MHz\n");
    break;

  case 5:
    shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "Frequency band: \t\t\t05: EU 865-868 MHz\n");
    break;

  case 6:
    shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "Frequency band: \t\t\t06: JP 916-921 MHz\n");
    break;

  case 7:
    shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "Frequency band: \t\t\t07: KR 917-921 MHz\n");
    break;

  case 8:
    shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "Frequency band: \t\t\t08: VN 918-923 MHz\n");
    break;

  default:
    shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "Unknown frequency band\n");
    break;
  }
  return 0;
}

/*!
 *  @brief This is the function description
 */
static int cmd_rfid_interval(const struct shell *shell, size_t argc, char **argv)
{
  if (argc == 1)
  {
    shell_print(shell, "RFID scanning interval: %dms", Parameter.rfid_interval);
  }
  else
  {
    Parameter.rfid_interval = atoi(argv[1]);
    Parameter_PushRAMToFlash();
    shell_print(shell, "Set RFID scanning interval to %dms", Parameter.rfid_interval);
  }
  return 0;
}

/*!
 *  @brief This is the function description
 */
static int cmd_rfid_interval_lifted(const struct shell *shell, size_t argc, char **argv)
{
  if (argc == 1)
  {
    shell_print(shell, "RFID scanning interval: %dms", Parameter.rfid_interval_lifted);
  }
  else
  {
    Parameter.rfid_interval_lifted = atoi(argv[1]);
    Parameter_PushRAMToFlash();
    shell_print(shell, "Set RFID scanning interval to %dms", Parameter.rfid_interval_lifted);
  }
  return 0;
}

/*!
 *  @brief This is the function description
 */
static int cmd_rfid_output_power(const struct shell *shell, size_t argc, char **argv)
{
  if (argc == 1)
  {
    shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "%d dBm\n", Parameter.rfid_output_power);
  }
  else
  {
    if ((atoi(argv[1]) >= -2) && (atoi(argv[1]) <= 27))
    {
      Parameter.rfid_output_power = atoi(argv[1]);
      RFID_setOutputPower(atoi(argv[1]));

      Parameter_PushRAMToFlash();
      shell_print(shell, "Set output power when NOT lifted to %ddBm", Parameter.rfid_output_power);
    }
    else
    {
      shell_error(shell, "Could not set new RFID output power");
    }
  }
  return 0;
}

/*!
 *  @brief This is the function description
 */
static int cmd_output_power_lifted(const struct shell *shell, size_t argc, char **argv)
{
  if (argc == 1)
  {
    shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "%d dBm\n", Parameter.rfid_output_power_lifted);
  }
  else
  {
    if ((atoi(argv[1]) >= -2) && (atoi(argv[1]) <= 27))
    {
      Parameter.rfid_output_power_lifted = atoi(argv[1]);

      Parameter_PushRAMToFlash();
      shell_print(shell, "Set output power when lifted to %ddBm", Parameter.rfid_output_power_lifted);
    }
    else
    {
      shell_error(shell, "Could not set new RFID output power");
    }
  }
  return 0;
}

/*!
 *  @brief This is the function description
 */
static int cmd_modem_write(const struct shell *shell, size_t argc, char **argv)
{
  static char buf[2048];
  int16_t err = 0;

  // List all available AT commands
  if (argc == 2)
  {

    shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "Sending command: %s\n", argv[1]);

    err = nrf_modem_at_cmd(buf, sizeof(buf), argv[1]);
    if (err < 0)
    {
      shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "AT command error: %d\n", err);
    }
    else
    {
      shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "%s", buf);
    }
  }
  else
  {
    shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "Invalid modem command");
  }

  return 0;
}

/*!
 *  @brief This is the function description
 */
static int cmd_modem_initialize(const struct shell *shell, size_t argc, char **argv)
{
  ARG_UNUSED(argc);
  ARG_UNUSED(argv);

  shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "Starting reset modem NVM values. Please wait.\n");
  modem_initial_setup();
  return 0;
}

/*!
 *  @brief This is the function description
 */
static int cmd_modem_imei(const struct shell *shell, size_t argc, char **argv)
{
  ARG_UNUSED(argc);
  ARG_UNUSED(argv);

  /* IMEI number - https://www.imei.info/*/
  shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "IMEI number: %s", modem.IMEI);
  return 0;
}

/*!
 *  @brief This is the function description
 */
static int cmd_modem_verbose(const struct shell *shell, size_t argc, char **argv)
{
  ARG_UNUSED(argc);
  ARG_UNUSED(argv);

  if (Parameter.modem_verbose == false)
  {
    Parameter.modem_verbose = true;
    shell_print(shell, "Enabled modem verbose mode");
  }
  else
  {
    Parameter.modem_verbose = false;
    shell_print(shell, "Disabled modem verbose mode");
  }
  Parameter_PushRAMToFlash();

  return 0;
}

/*!
 *  @brief This is the function description
 */
static int cmd_enable_blue_dev_led(const struct shell *shell, size_t argc, char **argv)
{
  ARG_UNUSED(argc);
  ARG_UNUSED(argv);

  if (Parameter.enable_blue_dev_led == false)
  {
    Parameter.enable_blue_dev_led = true;
    shell_print(shell, "Enabled blue dev led");

    /* Switch blue led on if motion is already detected when enabling the blue led*/
    if (motion_detected == true)
    {
      //   gpio_pin_set_raw(gpio_dev, GPIO_PIN_LED1, 0);
    }
  }
  else
  {
    Parameter.enable_blue_dev_led = false;
    shell_print(shell, "Disabled blue dev led");
    //   gpio_pin_set_raw(gpio_dev, GPIO_PIN_LED1, 1);
  }
  Parameter_PushRAMToFlash();

  return 0;
}

/*!
 *  @brief This is the function description
 */
static int cmd_enable_rfid_confirmation_blinking(const struct shell *shell, size_t argc, char **argv)
{
  ARG_UNUSED(argc);
  ARG_UNUSED(argv);

  if (Parameter.enable_rfid_confirmation_blinking == false)
  {
    Parameter.enable_rfid_confirmation_blinking = true;
    shell_print(shell, "Enabled rfid confirmation led for wall tags (valid epc database needed)");
  }
  else
  {
    Parameter.enable_rfid_confirmation_blinking = false;
    shell_print(shell, "Disabled rfid confirmation led for wall tags (valid epc database needed)");
  }
  Parameter_PushRAMToFlash();

  return 0;
}

/*!
 *  @brief This is the function description
 */
static int cmd_modem_version(const struct shell *shell, size_t argc, char **argv)
{
  ARG_UNUSED(argc);
  ARG_UNUSED(argv);

  memset(modem_at_recv_buf, 0, sizeof(modem_at_recv_buf));
  uint8_t err = 0;

  // enum at_cmd_state at_state;
  // err = at_cmd_write("AT+CGMR", modem_at_recv_buf, sizeof(modem_at_recv_buf), &at_state);
  err = nrf_modem_at_cmd(modem_at_recv_buf, sizeof(modem_at_recv_buf), "AT+CGMR");

  if (err)
  {
    shell_print(shell, "Error when trying to do at_cmd_write: %d", err);
    return err;
  }
  else
  {
    shell_print(shell, "%s", modem_at_recv_buf);
  }
  return 0;
}

/*!
 *  @brief This is the function description
 */
static int cmd_modem_providers(const struct shell *shell, size_t argc, char **argv)
{
  ARG_UNUSED(argc);
  ARG_UNUSED(argv);

  shell_warn(shell, "IMPORTANT: This command will block the device and shell until the modem get the result. This task may take severals minutes!");

  memset(modem_at_recv_buf, 0, sizeof(modem_at_recv_buf));
  uint8_t err = 0;

  /* Check connection status */
  err = nrf_modem_at_cmd(modem_at_recv_buf, sizeof(modem_at_recv_buf), "AT+COPS=?");
  // err = at_cmd_write("AT+COPS=?", modem_at_recv_buf, sizeof(modem_at_recv_buf), &at_state);

  if (!err)
  {
    shell_print(shell, "%s", modem_at_recv_buf);
  }

  return 0;
}

/*!
 *  @brief This is the function description
 */
static int cmd_modem_bands(const struct shell *shell, size_t argc, char **argv)
{
  ARG_UNUSED(argc);
  ARG_UNUSED(argv);

  memset(modem_at_recv_buf, 0, sizeof(modem_at_recv_buf));
  uint8_t err = 0;
  // enum at_cmd_state at_state;

  /* Check connection status */
  // err = at_cmd_write("AT%XCBAND=?", modem_at_recv_buf, sizeof(modem_at_recv_buf), &at_state);
  err = nrf_modem_at_cmd(modem_at_recv_buf, sizeof(modem_at_recv_buf), "AT%XCBAND=?");

  if (!err)
  {
    shell_print(shell, "%s", modem_at_recv_buf);
  }

  return 0;
}

/*!
 *  @brief This is the function description
 */
static int cmd_modem_list_keys(const struct shell *shell, size_t argc, char **argv)
{
  ARG_UNUSED(argc);
  ARG_UNUSED(argv);

  memset(modem_at_recv_buf, 0, sizeof(modem_at_recv_buf));
  uint8_t err = 0;

  // enum at_cmd_state at_state;
  // err = at_cmd_write("AT%CMNG=1", modem_at_recv_buf, sizeof(modem_at_recv_buf), &at_state);
  err = nrf_modem_at_cmd(modem_at_recv_buf, sizeof(modem_at_recv_buf), "AT%CMNG=1");

  if (err)
  {
    shell_print(shell, "Error when trying to do at_cmd_write: %d", err);
    return err;
  }
  else
  {
    shell_print(shell, "%s", modem_at_recv_buf);
  }
  return 0;
}

/*!
 *  @brief This is the function description
 */
static int cmd_modem_mode(const struct shell *shell, size_t argc, char **argv)
{
  enum lte_lc_func_mode mode;
  uint8_t value = 0;
  int8_t err = 0;

  if (argc == 1)
  {
    err = lte_lc_func_mode_get(&mode);
    if (err == 0)
    {
      switch (mode)
      {
      case LTE_LC_FUNC_MODE_POWER_OFF:
        shell_fprintf(shell, 0, "Modem is powered off (AT+CFUN=0)\n");

        /* Add event in event array which is send to cloud in next sync interval */
        NewEvent0x0C(); // Connection down event
        break;

      case LTE_LC_FUNC_MODE_NORMAL:
        shell_fprintf(shell, 0, "Modem is in normal mode (AT+CFUN=1)\n");
        break;

      case LTE_LC_FUNC_MODE_OFFLINE:

        shell_fprintf(shell, 0, "Modem is offline (AT+CFUN=4)\n");

        /* Add event in event array which is send to cloud in next sync interval */
        NewEvent0x0C(); // Connection down event
        break;

      default:
        break;
      }
    }
  }
  else
  {
    value = atoi(argv[1]);

    if (value == 0)
    {
      lte_lc_power_off();
      shell_fprintf(shell, 0, "Set modem to powered off mode (AT+CFUN=0)\n");
    }
    else if (value == 1)
    {
      lte_lc_normal();
      shell_fprintf(shell, 0, "Set modem to normal mode (AT+CFUN=1)\n");
    }
    else if (value == 4)
    {
      lte_lc_offline();
      shell_fprintf(shell, 0, "Set modem to offline mode (AT+CFUN=4)\n");
    }
  }
  return 0;
}

/*!
 *  @brief This is the function description
 */
static int cmd_connection_type(const struct shell *shell, size_t argc, char **argv)
{
  if (argc == 1)
  {
    switch (Parameter.network_connection_type)
    {
    case LTE_M:
      shell_print(shell, "Network connection type: LTE-M");
      break;

    case NB_IOT:
      shell_print(shell, "Network connection type: NB_IoT");
      break;

    default:
      shell_print(shell, "Invalid type set.");
      break;
    }
  }
  else
  {
    switch (atoi(argv[1]))
    {

    case LTE_M:

      Parameter.network_connection_type = LTE_M;

      /* Turn modem off */
      lte_lc_power_off();
      k_msleep(500);

      /* Set modem to use LTE-M*/
      lte_lc_system_mode_set(LTE_LC_SYSTEM_MODE_LTEM, LTE_LC_SYSTEM_MODE_PREFER_AUTO);
      k_msleep(500);

      /* Turn modem on - it will automatically search for networks*/
      lte_lc_normal();

      shell_print(shell, "Set network connection type to LTE-M");
      break;

    case NB_IOT:

      Parameter.network_connection_type = NB_IOT;
      /* Turn modem off */
      lte_lc_power_off();
      k_msleep(500);

      /* Set modem to use LTE-M*/
      lte_lc_system_mode_set(LTE_LC_SYSTEM_MODE_NBIOT, LTE_LC_SYSTEM_MODE_PREFER_AUTO);
      k_msleep(500);

      /* Turn modem on - it will automatically search for networks*/
      lte_lc_normal();

      shell_print(shell, "Set network connection type to NB_IoT");
      break;

    default:
      shell_print(shell, "Invalid type.");
      break;
    }

    shell_warn(shell, "Note: It may take several minutes for a new connection to be established successfully");
    Parameter_PushRAMToFlash();
  }
  return 0;
}

/*!
 *  @brief This is the function description
 */
static int cmd_add_psk_identity(const struct shell *shell, size_t argc, char **argv)
{
  // nrf_sec_tag_t mytag = PSK_TAG;
  // char buf[200];
  // int16_t err = 0;
  // enum at_cmd_state at_state;
  // enum lte_lc_func_mode mode;

  // err = lte_lc_func_mode_get(&mode);

  // if (mode == LTE_LC_FUNC_MODE_OFFLINE)
  // {
  //   /**** WRITE PRE-SHARED KEY IDENTITY ***/
  //   strcpy(buf, argv[1]); // key
  //   err = modem_key_mgmt_write(mytag, MODEM_KEY_MGMT_CRED_TYPE_IDENTITY, buf, strlen(buf));
  //   if (err != 0)
  //   {
  //     shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "key write error: %d\n", err);
  //   }
  //   else
  //   {
  //     shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "Added psk identity successfully\n");
  //   }
  // }
  // else
  // {
  //   shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_RED, "Modem is not offline. Please execute command 'modem mode 4' before\n");
  // }
  return 0;
}

/*!
 *  @brief This is the function description
 */
static int cmd_add_psk_key(const struct shell *shell, size_t argc, char **argv)
{
  // nrf_sec_tag_t mytag = PSK_TAG;
  // char buf[100];
  // int16_t err = 0;
  // enum at_cmd_state at_state;
  // enum lte_lc_func_mode mode;

  // err = lte_lc_func_mode_get(&mode);

  // if (mode == LTE_LC_FUNC_MODE_OFFLINE)
  // {
  //   /**** WRITE PRE-SHARED KEY ***/
  //   convert_ASCII_text_in_hexadecimal_string(buf, argv[1]);
  //   shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "\n%s\n", buf);

  //   err = modem_key_mgmt_write(mytag, MODEM_KEY_MGMT_CRED_TYPE_PSK, buf, strlen(buf));
  //   if (err != 0)
  //   {
  //     shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "key write error: %d\n", err);
  //   }
  //   else
  //   {
  //     shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "Added pre shared key successfully\n");
  //   }
  // }
  // else
  // {
  //   shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_RED, "Modem is not offline. Please execute command 'modem mode 4' before\n");
  // }

  return 0;
}

/*!
 *  @brief This is the function description
 */
static int cmd_modem_settings(const struct shell *shell, size_t argc, char **argv)
{
  ARG_UNUSED(argc);
  ARG_UNUSED(argv);

  modem_print_settings();
  return 0;
}

/*!
 *  @brief This is the function description
 */
static int cmd_modem_rssi(const struct shell *shell, size_t argc, char **argv)
{
  ARG_UNUSED(argc);
  ARG_UNUSED(argv);

  if (modem.connection_stat == true)
  {
    shell_fprintf(shell, 0, "RSSI: %ddBm\n", get_signal_strength());
  }
  else
  {
    shell_fprintf(shell, 0, "LTE not connected\n");
  }
  return 0;
}

/*!
 *  @brief This is the function description
 */
static int cmd_modem_status(const struct shell *shell, size_t argc, char **argv)
{
  ARG_UNUSED(argc);
  ARG_UNUSED(argv);

  int16_t err = 0;
  enum lte_lc_nw_reg_status status;
  enum lte_lc_func_mode mode;

  err = lte_lc_func_mode_get(&mode);

  if (err == 0)
  {
    switch (mode)
    {

    case LTE_LC_FUNC_MODE_POWER_OFF:
      shell_fprintf(shell, 0, "Modem powered off (AT+CFUN=0)\n");
      break;

    case LTE_LC_FUNC_MODE_NORMAL:
      shell_fprintf(shell, 0, "Modem normal mode (AT+CFUN=1)\n");
      break;

    case LTE_LC_FUNC_MODE_OFFLINE:
      shell_fprintf(shell, 0, "Modem offline (AT+CFUN=4)\n");
      break;

    default:
      break;
    }
  }

  err = lte_lc_nw_reg_status_get(&status);

  if (err == 0)
  {
    switch (status)
    {

    case LTE_LC_NW_REG_NOT_REGISTERED:
      shell_fprintf(shell, 0, "Registration status: LTE_LC_NW_REG_NOT_REGISTERED\n");
      break;

    case LTE_LC_NW_REG_REGISTERED_HOME:
      shell_fprintf(shell, 0, "Registration status: LTE_LC_NW_REG_REGISTERED_HOME\n");
      break;

    case LTE_LC_NW_REG_SEARCHING:
      shell_fprintf(shell, 0, "Registration status: LTE_LC_NW_REG_SEARCHING\n");
      break;

    case LTE_LC_NW_REG_REGISTRATION_DENIED:
      shell_fprintf(shell, 0, "Registration status: LTE_LC_NW_REG_REGISTRATION_DENIED\n");
      break;

    case LTE_LC_NW_REG_UNKNOWN:
      shell_fprintf(shell, 0, "Registration status: LTE_LC_NW_REG_UNKNOWN\n");
      break;

    case LTE_LC_NW_REG_REGISTERED_ROAMING:
      shell_fprintf(shell, 0, "Registration status: LTE_LC_NW_REG_REGISTERED_ROAMING\n");
      break;

    case LTE_LC_NW_REG_REGISTERED_EMERGENCY:
      shell_fprintf(shell, 0, "Registration status: LTE_LC_NW_REG_REGISTERED_EMERGENCY\n");
      break;

    case LTE_LC_NW_REG_UICC_FAIL:
      shell_fprintf(shell, 0, "Registration status: LTE_LC_NW_REG_UICC_FAIL\n");
      break;

    default:
      break;
    }
  }
  else
  {
    printk("ERROR: %d. Could not read network registration status.\n", err);
  }
  return 0;
}

/*!
 *  @brief This is the function description
 */
static int cmd_rfid_command(const struct shell *shell, size_t argc, char **argv)
{
  char command[8];

  if (argc == 2)
  {

    System.RFID_TransparentMode = true;

    if (RFID_IsOn == false)
    {
      RFID_TurnOn();
      config_RFID();
      k_msleep(500);
    }

    // System.RFID_TransparentMode = true;
    memset(uart1_RFIDResponse, '\0', sizeof(uart1_RFIDResponse));
    uart1_RFIDResponseTransmissionLength = 0;
    uart1_RFIDResponseFinished = false;

    /* Build a command with \n and \r signs which the RFID reader IC can understand */
    strcpy(command, "\n");
    strcat(command, argv[1]);
    strcat(command, "\r");

    uart_fifo_fill(uart1, command, sizeof(command) - 1);

    /* Record uart input coming from rfid module for 1000 ms */
    while (!uart1_RFIDResponseFinished)
    {
    }

    System.RFID_TransparentMode = false;

    shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "%s\n", uart1_RFIDResponse);

    memset(uart1_RFIDResponse, '\0', sizeof(uart1_RFIDResponse));
    uart1_RFIDResponseTransmissionLength = 0;
    uart1_RFIDResponseFinished = false;
  }
  else
  {
    shell_error(shell, "RFID command not valid.");
  }
  return 0;
}

/*!
 *  @brief This is the function description
 */
static int cmd_rfid_frequency(const struct shell *shell, size_t argc, char **argv)
{

  if (argc == 1)
  {

    int16_t freq = 0;
    freq = RFID_getFrequency();

    switch (freq)
    {
    case 1:
      shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "01: US 902-925 MHz\n");
      break;

    case 2:
      shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "02: TW 922-928 MHz\n");
      break;

    case 3:
      shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "03: CN 920-925 MHz\n");
      break;

    case 4:
      shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "04: CN2 840-845 MHz\n");
      break;

    case 5:
      shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "05: EU 865-868 MHz\n");
      break;

    case 6:
      shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "06: JP 916-921 MHz\n");
      break;

    case 7:
      shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "07: KR 917-921 MHz\n");
      break;

    case 8:
      shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "08: VN 918-923 MHz\n");
      break;

    default:
      shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "Unknown frequency band\n");
      break;
    }
  }
  else
  {
    if ((atoi(argv[1]) > 0) && (atoi(argv[1]) <= 8))
    {
      Parameter.rfid_frequency = atoi(argv[1]);
      RFID_setFrequency(Parameter.rfid_frequency);
      Parameter_PushRAMToFlash();
    }

    switch (Parameter.rfid_frequency)
    {
    case 1:
      shell_print(shell, "Set frequency to US standard");
      break;
    case 2:
      shell_print(shell, "Set frequency to TW standard");
      break;
    case 3:
      shell_print(shell, "Set frequency to CN standard");
      break;
    case 4:
      shell_print(shell, "Set frequency to CN2 standard");
      break;
    case 5:
      shell_print(shell, "Set frequency to EU standard");
      break;
    case 6:
      shell_print(shell, "Set frequency to JP standard");
      break;
    case 7:
      shell_print(shell, "Set frequency to KR standard");
      break;
    case 8:
      shell_print(shell, "Set frequency to VN standard");
      break;
    default:
      shell_error(shell, "Could not set new frequency");
      break;
    }
  }
  return 0;
}

/*!
 *  @brief This is the function description
 */
static int cmd_eraseall(const struct shell *shell, size_t argc, char **argv)
{
  ARG_UNUSED(argc);
  ARG_UNUSED(argv);

  flash_EraseAll(GPIO_PIN_FLASH_CS1);
  shell_print(shell, "OK");
  return 0;
}

/*!
 *  @brief This is the function description
 */
static int cmd_erasesector(const struct shell *shell, size_t argc, char **argv)
{
  flash_EraseSector_4kB(GPIO_PIN_FLASH_CS1, atol(argv[1]));
  shell_print(shell, "OK");
  return 0;
}

/*!
 *  @brief This is the function description
 */
static int cmd_imu_interval(const struct shell *shell, size_t argc, char **argv)
{
  if (argc == 1)
  {
    shell_print(shell, "IMU interval: %dms", Parameter.imu_interval);
  }
  else
  {
    if (atoi(argv[1]) >= 1)
    {
      Parameter.imu_interval = atoi(argv[1]);
      Parameter_PushRAMToFlash();
      shell_print(shell, "Set IMU interval to %dms", Parameter.imu_interval);
    }
    else
    {
      shell_error(shell, "Could not set new IMU interval");
    }
  }
  return 0;
}

/*!
 *  @brief This is the function description
 */
static int cmd_motion_reset_time(const struct shell *shell, size_t argc, char **argv)
{
  ARG_UNUSED(argc);
  ARG_UNUSED(argv);

  if (argc == 1)
  {
    shell_fprintf(shell, 0, "%d sec\n", Parameter.motion_reset_time);
  }
  else
  {
    Parameter.motion_reset_time = atoi(argv[1]);
    Parameter_PushRAMToFlash();
    shell_fprintf(shell, 0, "New value: %d\n", Parameter.motion_reset_time);
  }
  return 0;
}

/*!
 *  @brief This is the function description
 */
static int cmd_anymotion_duration(const struct shell *shell, size_t argc, char **argv)
{
  ARG_UNUSED(argc);
  ARG_UNUSED(argv);

  if (argc == 1)
  {
    shell_fprintf(shell, 0, "%d\n", Parameter.anymotion_duration);
  }
  else
  {
    Parameter.anymotion_duration = atoi(argv[1]);
    Parameter_PushRAMToFlash();
    imu_init();
    shell_fprintf(shell, 0, "New value: %d\n", Parameter.anymotion_duration);
  }
  return 0;
}

/*!
 *  @brief This is the function description
 */
static int cmd_anymotion_thr(const struct shell *shell, size_t argc, char **argv)
{
  ARG_UNUSED(argc);
  ARG_UNUSED(argv);

  if (argc == 1)
  {
    shell_fprintf(shell, 0, "%d\n", Parameter.anymotion_thr);
  }
  else
  {
    Parameter.anymotion_thr = atoi(argv[1]);
    Parameter_PushRAMToFlash();
    imu_init();
    shell_fprintf(shell, 0, "New value: %d\n", Parameter.anymotion_thr);
  }
  return 0;
}

/*!
 *  @brief This is the function description
 */
static int cmd_stepdetection_trace(const struct shell *shell, size_t argc, char **argv)
{
  ARG_UNUSED(argc);
  ARG_UNUSED(argv);

  if (Parameter.stepdetection_verbose == false)
  {
    Parameter.stepdetection_verbose = true;
    shell_print(shell, "Enabled step detection verbose mode");
  }
  else
  {
    Parameter.stepdetection_verbose = false;
    shell_print(shell, "Disabled step detection verbose mode");
  }
  Parameter_PushRAMToFlash();
  return 0;
}

/*!
 *  @brief This is the function description
 */
static int cmd_notification_while_usb_connceted(const struct shell *shell, size_t argc, char **argv)
{
  ARG_UNUSED(argc);
  ARG_UNUSED(argv);

  if (Parameter.notifications_while_usb_connected == false)
  {
    Notification.current_priority = NOTIFICATION_PRIORITY_LEVEL_LOWEST; // Set priority to lowest level to allow new user notification
    Notification.next_priority = NOTIFICATION_PRIORITY_LEVEL_LOWEST;

    Parameter.notifications_while_usb_connected = true;
    shell_print(shell, "Enabled notification while usb connected");
  }
  else
  {
    Parameter.notifications_while_usb_connected = false;
    shell_print(shell, "Disabled notification while usb connected");
  }
  Parameter_PushRAMToFlash();
  return 0;
}

/*!
 *  @brief This is the function description
 */
static int cmd_fw(const struct shell *shell, size_t argc, char **argv)
{
  ARG_UNUSED(argc);
  ARG_UNUSED(argv);

  shell_print(shell, "Firmware version: v%d.%d.%d", Device.FirmwareMajorVersion, Device.FirmwareMinorVersion, Device.FirmwareInternVersion);
  return 0;
}

/*!
 *  @brief This is the function description
 */
static int cmd_hw(const struct shell *shell, size_t argc, char **argv)
{
  ARG_UNUSED(argc);
  ARG_UNUSED(argv);

  shell_print(shell, "Supported pcb version: v%d.%d", Device.HardwareMajorVersion, Device.HardwareMinorVersion);
  return 0;
}

/*!
 *  @brief This is the function description
 */
static int cmd_heap_freespace(const struct shell *shell, size_t argc, char **argv)
{
  ARG_UNUSED(argc);
  ARG_UNUSED(argv);

  shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "%d bytes free on heap.\n", memcheck_heap_freespace());

  return 0;
}

/*!
 *  @brief This is the function description
 */
static int cmd_debug(const struct shell *shell, size_t argc, char **argv)
{
  ARG_UNUSED(argc);
  ARG_UNUSED(argv);

  if (Parameter.debug == false)
  {
    Parameter.debug = true;
    shell_print(shell, "Global verbose mode on");
  }
  else
  {
    Parameter.debug = false;
    shell_print(shell, "Global verbose mode off");
  }
  Parameter_PushRAMToFlash();
  return 0;
}

/*!
 *  @brief This is the function description
 */
static int cmd_print_parameter_ram(const struct shell *shell, size_t argc, char **argv)
{
  ARG_UNUSED(argc);
  ARG_UNUSED(argv);

  Parameter_PrintValues(&Parameter);
  return 0;
}

/*!
 *  @brief This is the function description
 */
static int cmd_print_parameter_flash(const struct shell *shell, size_t argc, char **argv)
{
  ARG_UNUSED(argc);
  ARG_UNUSED(argv);

  PARAMETER readout;

  flash_read(GPIO_PIN_FLASH_CS2, PARAMETER_MEM, &readout.parameter_mem_bytes[0], PARAMETER_MEM_RAM_SIZE);

  Parameter_PrintValues(&readout);
  return 0;
}

/*!
 *  @brief This is the function description
 */
static int cmd_led_color(const struct shell *shell, size_t argc, char **argv)
{
  if (argc == 4)
  {
    uint8_t red = 0;
    uint8_t green = 0;
    uint8_t blue = 0;

    red = atoi(argv[1]);
    green = atoi(argv[2]);
    blue = atoi(argv[3]);

    if ((red <= 255) && (green <= 255) && (blue <= 255))
    {
      led_set_rgb_color(red, green, blue);
      shell_print(shell, "Red: %d, green: %d, blue: %d", red, green, blue);
    }
    else
    {
      shell_print(shell, "Invalid rgb color (colors must be in range of 0 to 255)");
    }
  }
  else
  {
    shell_print(shell, "Invalid parameters");
  }
}

/*!
 *  @brief This is the function description
 */
static int cmd_led_brightness(const struct shell *shell, size_t argc, char **argv)
{
  if (argc == 1)
  {
    shell_print(shell, "LED brightness: %d", Parameter.led_brightness);
  }
  else
  {
    if ((atoi(argv[1]) >= 0) && (atoi(argv[1]) <= 255))
    {
      Parameter.led_brightness = atoi(argv[1]);
      led_set_rgb_brightness(Parameter.led_brightness);
      Parameter_PushRAMToFlash();
      shell_print(shell, "Set LED brightness to %d", Parameter.led_brightness);
    }
    else
    {
      shell_error(shell, "Could not set LED brightness");
    }
  }
  return 0;
}

/*!
 *  @brief This is the function description
 */
static int cmd_buzzer_duty_cycle(const struct shell *shell, size_t argc, char **argv)
{
  if (argc == 1)
  {
    shell_print(shell, "Buzzer duty cycle: %d", Parameter.buzzer_duty_cycle);
  }
  else
  {
    if ((atoi(argv[1]) >= 0) && (atoi(argv[1]) <= 50))
    {
      Parameter.buzzer_duty_cycle = atoi(argv[1]);
      Parameter_PushRAMToFlash();
      shell_print(shell, "Buzzer duty cycle to %d", Parameter.buzzer_duty_cycle);
    }
    else
    {
      shell_error(shell, "Could not set duty cycle");
    }
  }
  return 0;
}

/*!
 *  @brief This is the function description
 */
static int cmd_timestamp(const struct shell *shell, size_t argc, char **argv)
{
  rtc_print_timestamp();
  return 0;
}

/*!
 *  @brief This is the function description
 */
static int cmd_time(const struct shell *shell, size_t argc, char **argv)
{
  if (argc == 1)
  {
    rtc_print_time();
    shell_print(shell, " ");
  }
  else
  {
    if ((strlen(argv[1]) >= 5) && (strlen(argv[1]) <= 8))
    { // Valid langth:   h:m:s and hh:mm:ss

      char delimiter[] = ":"; // This character in string gets replaced by \0
      char *ptr = NULL;       // Pointer to string section

      volatile uint8_t hour = 0;
      volatile uint8_t min = 0;
      volatile uint8_t sec = 0;

      /* Find first segment in string */
      ptr = strtok(argv[1], delimiter); // strtok cuts the string into pieces sperated by delimiter characters
      if (ptr != NULL)
      {
        hour = atoi(ptr);
      }

      /* Find second segment in string */
      ptr = strtok(NULL, delimiter);
      if (ptr != NULL)
      {
        min = atoi(ptr);
      }

      /* Find third segment in string */
      ptr = strtok(NULL, delimiter);
      if (ptr != NULL)
      {
        sec = atoi(ptr);
      }

      /* Validate user input and if correct set new system time*/
      if (((hour >= 0) && (hour <= 24)) &&
          ((min >= 0) && (min <= 60)) &&
          ((sec >= 0) && (sec <= 60)))
      {
        ptm->tm_hour = hour;
        ptm->tm_min = min;
        ptm->tm_sec = sec;

        /* convert local time to unix */
        unixtime = gmtime(&ptm);

        /* Print new settings on console */
        rtc_print_time();
      }
      else
      {
        shell_error(shell, "Incorrect time format");
      }
    }
  }
  return 0;
}

/*!
 *  @brief This is the function description
 */
static int cmd_date(const struct shell *shell, size_t argc, char **argv)
{
  if (argc == 1)
  {
    rtc_print_date();
    shell_print(shell, " ");
  }
  else
  {
    if ((strlen(argv[1]) >= 6) && (strlen(argv[1]) <= 10))
    { // Valid langth:   d:m:yy and dd:mm:yyyy

      char delimiter[] = "."; // This character in string gets replaced by \0
      char *ptr = NULL;       // Pointer to string section

      uint8_t day = 0;
      uint8_t month = 0;
      uint16_t year = 0;

      /* Find first segment in string */
      ptr = strtok(argv[1], delimiter); // strtok cuts the string into pieces sperated by delimiter characters
      if (ptr != NULL)
      {
        day = atoi(ptr);
      }

      /* Find second segment in string */
      ptr = strtok(NULL, delimiter);
      if (ptr != NULL)
      {
        month = atoi(ptr);
      }

      /* Find third segment in string */
      ptr = strtok(NULL, delimiter);
      if (ptr != NULL)
      {
        year = atoi(ptr);
      }

      /* Validate user input and if correct set new system time*/
      if ((day >= 1 && day <= 31) &&
          (month >= 1 && month <= 12) &&
          ((year >= 20 && year <= 99) || (year >= 2020 && year <= 2099)))
      {
        ptm->tm_mday = day;
        ptm->tm_mon = month;
        ptm->tm_year = year;

        /* convert local time to unix */
        unixtime = gmtime(&ptm);

        /* Print new settings on console */
        rtc_print_date();
      }
      else
      {
        shell_error(shell, "Incorrect date format");
      }
    }
  }
  return 0;
}

/*!
 *  @brief This is the function description
 */
static int cmd_weekday(const struct shell *shell, size_t argc, char **argv)
{
  rtc_print_weekday();
  shell_print(shell, "\r\n");
  return 0;
}

/*!
 *  @brief This is the function description
 */
static int cmd_unixtime(const struct shell *shell, size_t argc, char **argv)
{
  if (argc == 1)
  {
    rtc_print_unixtime(unixtime);
    shell_print(shell, "\n");
  }
  else
  {
    unixtime = atol(argv[1]);
    ptm = localtime(&unixtime);
    shell_print(shell, "OK");
  }
  return 0;
}

/*!
 *  @brief This is the function description
 */
static int cmd_sessionuptime(const struct shell *shell, size_t argc, char **argv)
{
  ARG_UNUSED(argc);
  ARG_UNUSED(argv);

  uint16_t hours = 0;
  uint16_t minutes = 0;
  uint16_t seconds = 0;

  /* Convert seconds to hh:mm:ss */
  hours = ((uint32_t)System.MillisecSinceBoot / 1000) / 3600;
  minutes = (((uint32_t)System.MillisecSinceBoot / 1000) % 3600) / 60;
  seconds = ((uint32_t)System.MillisecSinceBoot / 1000) - ((hours * 3600) + (minutes * 60));
  shell_fprintf(shell, 0, "%d sec equals %02d:%02d:%02d\n", ((uint32_t)System.MillisecSinceBoot / 1000), hours, minutes, seconds);
  return 0;
}

/*!
 *  @brief This is the function description
 */
static int cmd_totaluptime(const struct shell *shell, size_t argc, char **argv)
{
  ARG_UNUSED(argc);
  ARG_UNUSED(argv);

  uint16_t hours = 0;
  uint16_t minutes = 0;
  uint16_t seconds = 0;

  /* Convert seconds to hh:mm:ss */
  hours = Device.OpertingTime / 3600;
  minutes = (Device.OpertingTime % 3600) / 60;
  seconds = Device.OpertingTime - ((hours * 3600) + (minutes * 60));

  shell_fprintf(shell, 0, "%ld sec equals %02d:%02d:%02d\n", Device.OpertingTime, hours, minutes, seconds);
  Device_PushRAMToFlash();

  return 0;
}

/*!
 *  @brief This is the function description
 */
static int cmd_stepdetection_totalsteps(const struct shell *shell, size_t argc, char **argv)
{
  ARG_UNUSED(argc);
  ARG_UNUSED(argv);

  shell_fprintf(shell, 0, "%d steps\n", System.TotalSteps);
  return 0;
}

/*!
 *  @brief This is the function description
 */
static int cmd_algo_verbose(const struct shell *shell, size_t argc, char **argv)
{
  ARG_UNUSED(argc);
  ARG_UNUSED(argv);

  if (Parameter.algo_verbose == false)
  {
    Parameter.algo_verbose = true;
    shell_print(shell, "Algorithm debug verbose mode on");
  }
  else
  {
    Parameter.algo_verbose = false;
    shell_print(shell, "Algorithm debug verbose mode off");
  }
  Parameter_PushRAMToFlash();
  return 0;
}

/*!
 *  @brief This is the function description
 */
static int cmd_enable_coveraged_per_mop_notification(const struct shell *shell, size_t argc, char **argv)
{
  ARG_UNUSED(argc);
  ARG_UNUSED(argv);

  if (Parameter.enable_coveraged_per_mop_notification == false)
  {
    Parameter.enable_coveraged_per_mop_notification = true;
    shell_print(shell, "Enabled notification if coverage per mop is reached.");
  }
  else
  {
    Parameter.enable_coveraged_per_mop_notification = false;
    shell_print(shell, "Disabled notification if coverage per mop is reached.");
  }
  Parameter_PushRAMToFlash();
  return 0;
}

/*!
 *  @brief This is the function description
 */
static int cmd_algo_flag_verbose(const struct shell *shell, size_t argc, char **argv)
{
  ARG_UNUSED(argc);
  ARG_UNUSED(argv);

  if (Parameter.algo_flag_verbose == false)
  {
    Parameter.algo_flag_verbose = true;
    shell_print(shell, "Algorithm flag verbose mode on");
  }
  else
  {
    Parameter.algo_flag_verbose = false;
    shell_print(shell, "Algorithm flag verbose mode off");
  }
  Parameter_PushRAMToFlash();
  return 0;
}

/*!
 *  @brief This is the function description
 */
static int cmd_current_shift_mop_check(const struct shell *shell, size_t argc, char **argv)
{
  ARG_UNUSED(argc);
  ARG_UNUSED(argv);

  if (Parameter.current_shift_mop_check == false)
  {
    Parameter.current_shift_mop_check = true;
    shell_print(shell, "Used mop check in current shift activated.");
  }
  else
  {
    Parameter.current_shift_mop_check = false;
    shell_print(shell, "Used mop check in current shift deactivated.");
  }
  Parameter_PushRAMToFlash();
  return 0;
}

/*!
 *  @brief This is the function description
 */
static int cmd_mop_id_refresh_timer(const struct shell *shell, size_t argc, char **argv)
{
  if (argc == 1)
  {
    shell_fprintf(shell, 0, "%d\n", Parameter.mop_id_refresh_timer);
  }
  else
  {
    Parameter.mop_id_refresh_timer = atoi(argv[1]);
    Parameter_PushRAMToFlash();
    shell_fprintf(shell, 0, "New value: %d\n", Parameter.mop_id_refresh_timer);
  }

  return 0;
}

/*!
 *  @brief This is the function description
 */
static int cmd_hit_shock_mag_thr(const struct shell *shell, size_t argc, char **argv)
{
  if (argc == 1)
  {
    shell_fprintf(shell, 0, "%f\n", Parameter.hit_shock_mag_thr);
  }
  else
  {
    Parameter.hit_shock_mag_thr = atof(argv[1]);
    Parameter_PushRAMToFlash();
    shell_fprintf(shell, 0, "hit_shock_mag_thr new value: %f\n", Parameter.hit_shock_mag_thr);
  }

  return 0;
}

/*!
 *  @brief This is the function description
 */
static int cmd_algocontrol_bymag_det(const struct shell *shell, size_t argc, char **argv)
{
  if (argc == 1)
  {
    shell_fprintf(shell, 0, "%d\n", Parameter.algocontrol_bymag_det);
  }
  else
  {
    if (atoi(argv[1]) > 0)
    {
      Parameter.algocontrol_bymag_det = 1;
    }
    else
    {
      Parameter.algocontrol_bymag_det = 0;
      algorithm_lock = false;
    }
    Parameter_PushRAMToFlash();
    shell_fprintf(shell, 0, "algocontrol_bymag_det new value: %d\n", Parameter.algocontrol_bymag_det);
  }

  return 0;
}

/*!
 *  @brief This is the function description
 */
static int cmd_mag_det_threshold(const struct shell *shell, size_t argc, char **argv)
{
  if (argc == 1)
  {
    shell_print(shell, "mag_det_threshold: %d uT", Parameter.mag_det_threshold);
  }
  else
  {
    Parameter.mag_det_threshold = atoi(argv[1]);
    Parameter_PushRAMToFlash();
    shell_print(shell, "New mag_det_threshold: %d uT", Parameter.mag_det_threshold);
  }
  return 0;
}

/*!
 *  @brief This is the function description
 */
static int cmd_mag_det_consecutive_samples(const struct shell *shell, size_t argc, char **argv)
{
  if (argc == 1)
  {
    shell_print(shell, "mag_det_consecutive_samples: %d", Parameter.mag_det_consecutive_samples);
  }
  else
  {
    Parameter.mag_det_consecutive_samples = atoi(argv[1]);
    Parameter_PushRAMToFlash();
    shell_print(shell, "New mag_det_consecutive_samples: %d", Parameter.mag_det_consecutive_samples);
  }
  return 0;
}

/*!
 *  @brief This is the function description
 */
static int cmd_event1statistics_interval(const struct shell *shell, size_t argc, char **argv)
{
  if (argc == 1)
  {
    shell_print(shell, "event1statistics_interval: %d", Parameter.event1statistics_interval);
  }
  else
  {
    Parameter.event1statistics_interval = atoi(argv[1]);
    Parameter_PushRAMToFlash();
    shell_print(shell, "New event1statistics_interval: %d", Parameter.event1statistics_interval);
  }
  return 0;
}

/*!
 *  @brief This is the function description
 */
static int cmd_algo_settings(const struct shell *shell, size_t argc, char **argv)
{
  ARG_UNUSED(argc);
  ARG_UNUSED(argv);

  shell_print(shell, "acc_noise_thr = %.2f", Parameter.acc_noise_thr);
  shell_print(shell, "gyr_noise_thr = %.2f", Parameter.gyr_noise_thr);
  shell_print(shell, "gyr_spin_thr = %.2f", Parameter.gyr_spin_thr);
  shell_print(shell, "mag_noise_thr = %.2f", Parameter.mag_noise_thr);
  shell_print(shell, "frame_handle_angle_thr = %.2f", Parameter.frame_handle_angle_thr);
  shell_print(shell, "floor_handle_angle_mopping_thr_min = %.2f", Parameter.floor_handle_angle_mopping_thr_min);
  shell_print(shell, "floor_handle_angle_mopping_thr_max = %.2f", Parameter.floor_handle_angle_mopping_thr_max);
  shell_print(shell, "floor_handle_angle_mopchange_thr = %.2f", Parameter.floor_handle_angle_mopchange_thr);
  shell_print(shell, "min_mopchange_duration = %ld", (uint32_t)Parameter.min_mopchange_duration);
  shell_print(shell, "min_mopframeflip_duration = %ld", (uint32_t)Parameter.min_mopframeflip_duration);
  shell_print(shell, "angle_smooth_factor = %.2f", Parameter.angle_smooth_factor);
  shell_print(shell, "gyr_smooth_factor = %.2f", Parameter.gyr_smooth_factor);
  shell_print(shell, "min_mopcycle_duration = %.2f", Parameter.min_mopcycle_duration);
  shell_print(shell, "max_mopcycle_duration = %.2f", Parameter.max_mopcycle_duration);
  shell_print(shell, "mop_width = %.2f", Parameter.mop_width);
  shell_print(shell, "mop_overlap = %.2f", Parameter.mop_overlap);
  shell_print(shell, "mopcycle_sequence_thr = %.2f", Parameter.mopcycle_sequence_thr);
  shell_print(shell, "peakfollower_update_delay = %ld", (uint32_t)Parameter.peakfollower_update_delay);
  shell_print(shell, "mop_rfid_detection_thr = %ld", (uint32_t)Parameter.mop_rfid_detection_thr);
  shell_print(shell, "mopping_coverage_per_mop_thr = %.2f", Parameter.mopping_coverage_per_mop_thr);

  return 0;
}

/*!
 *  @brief This is the function description
 */
static int cmd_battery_charge_termination_current(const struct shell *shell, size_t argc, char **argv)
{
  if (argc == 1)
  {
    shell_fprintf(shell, 0, "%f\n", Parameter.battery_charge_termination_current);
  }
  else
  {

    Parameter.battery_charge_termination_current = atof(argv[1]);
    Parameter_PushRAMToFlash();
    shell_fprintf(shell, 0, "battery_charge_termination_current new value: %f\n", Parameter.battery_charge_termination_current);
  }
  return 0;
}

/*!
 *  @brief This is the function description
 */
static int cmd_battery_charge_termination_temp_min(const struct shell *shell, size_t argc, char **argv)
{
  if (argc == 1)
  {
    shell_fprintf(shell, 0, "%f\n", Parameter.battery_gauge_charge_temp_min);
  }
  else
  {

    Parameter.battery_gauge_charge_temp_min = atof(argv[1]);
    Parameter_PushRAMToFlash();
    shell_fprintf(shell, 0, "battery_gauge_charge_temp_min new value: %f\n", Parameter.battery_gauge_charge_temp_min);
  }
  return 0;
}

/*!
 *  @brief This is the function description
 */
static int cmd_battery_charge_termination_temp_max(const struct shell *shell, size_t argc, char **argv)
{
  if (argc == 1)
  {
    shell_fprintf(shell, 0, "%f\n", Parameter.battery_gauge_charge_temp_max);
  }
  else
  {

    Parameter.battery_gauge_charge_temp_max = atof(argv[1]);
    Parameter_PushRAMToFlash();
    shell_fprintf(shell, 0, "battery_gauge_charge_temp_max new value: %f\n", Parameter.battery_gauge_charge_temp_max);
  }
  return 0;
}

/*!
 *  @brief This is the function description
 */
static int cmd_battery_gauge_sniff_i2c(const struct shell *shell, size_t argc, char **argv)
{
  if (Parameter.battery_gauge_sniff_i2c == false)
  {
    Parameter.battery_gauge_sniff_i2c = true;
    shell_print(shell, "Enabled sniffing battery gauge i2c communication.");
  }
  else
  {
    Parameter.battery_gauge_sniff_i2c = false;
    shell_print(shell, "Disabled sniffing battery gauge i2c communication.");
  }
  Parameter_PushRAMToFlash();
  return 0;
}

/*!
 *  @brief This is the function description
 */
static int cmd_battery_gauge_report(const struct shell *shell, size_t argc, char **argv)
{
  battery_gauge_print_report();
  return 0;
}

/*!
 *  @brief This is the function description
 */
static int cmd_acc_noise_thr(const struct shell *shell, size_t argc, char **argv)
{

  if (argc == 1)
  {
    shell_fprintf(shell, 0, "%f\n", Parameter.acc_noise_thr);
  }
  else
  {

    Parameter.acc_noise_thr = atof(argv[1]);
    Parameter_PushRAMToFlash();
    shell_fprintf(shell, 0, "New value: %f\n", Parameter.acc_noise_thr);
  }
  return 0;
}

/*!
 *  @brief This is the function description
 */
static int cmd_gyr_noise_thr(const struct shell *shell, size_t argc, char **argv)
{
  if (argc == 1)
  {
    shell_fprintf(shell, 0, "%f\n", Parameter.gyr_noise_thr);
  }
  else
  {

    Parameter.gyr_noise_thr = atof(argv[1]);
    Parameter_PushRAMToFlash();
    shell_fprintf(shell, 0, "New value: %f\n", Parameter.gyr_noise_thr);
  }
  return 0;
}

/*!
 *  @brief This is the function description
 */
static int cmd_mag_noise_thr(const struct shell *shell, size_t argc, char **argv)
{
  if (argc == 1)
  {
    shell_fprintf(shell, 0, "%f\n", Parameter.mag_noise_thr);
  }
  else
  {

    Parameter.mag_noise_thr = atof(argv[1]);
    Parameter_PushRAMToFlash();
    shell_fprintf(shell, 0, "New value: %f\n", Parameter.mag_noise_thr);
  }
  return 0;
}

/*!
 *  @brief This is the function description
 */
static int cmd_gyr_spin_thr(const struct shell *shell, size_t argc, char **argv)
{
  if (argc == 1)
  {
    shell_fprintf(shell, 0, "%f\n", Parameter.gyr_spin_thr);
  }
  else
  {

    Parameter.gyr_spin_thr = atof(argv[1]);
    Parameter_PushRAMToFlash();
    shell_fprintf(shell, 0, "New value: %f\n", Parameter.gyr_spin_thr);
  }
  return 0;
}

/*!
 *  @brief This is the function description
 */
static int cmd_frame_handle_angle_thr(const struct shell *shell, size_t argc, char **argv)
{
  if (argc == 1)
  {
    shell_fprintf(shell, 0, "%f\n", Parameter.frame_handle_angle_thr);
  }
  else
  {

    Parameter.frame_handle_angle_thr = atof(argv[1]);
    Parameter_PushRAMToFlash();
    shell_fprintf(shell, 0, "New value: %f\n", Parameter.frame_handle_angle_thr);
  }
  return 0;
}

/*!
 *  @brief This is the function description
 */
static int cmd_floor_handle_angle_mopping_thr_min(const struct shell *shell, size_t argc, char **argv)
{
  if (argc == 1)
  {
    shell_fprintf(shell, 0, "%f\n", Parameter.floor_handle_angle_mopping_thr_min);
  }
  else
  {

    Parameter.floor_handle_angle_mopping_thr_min = atof(argv[1]);
    Parameter_PushRAMToFlash();
    shell_fprintf(shell, 0, "New value: %f\n", Parameter.floor_handle_angle_mopping_thr_min);
  }
  return 0;
}

/*!
 *  @brief This is the function description
 */
static int cmd_floor_handle_angle_mopping_thr_max(const struct shell *shell, size_t argc, char **argv)
{
  if (argc == 1)
  {
    shell_fprintf(shell, 0, "%f\n", Parameter.floor_handle_angle_mopping_thr_max);
  }
  else
  {

    Parameter.floor_handle_angle_mopping_thr_max = atof(argv[1]);
    Parameter_PushRAMToFlash();
    shell_fprintf(shell, 0, "New value: %f\n", Parameter.floor_handle_angle_mopping_thr_max);
  }
  return 0;
}

/*!
 *  @brief This is the function description
 */
static int cmd_floor_handle_angle_mopchange_thr(const struct shell *shell, size_t argc, char **argv)
{
  if (argc == 1)
  {
    shell_fprintf(shell, 0, "%f\n", Parameter.floor_handle_angle_mopchange_thr);
  }
  else
  {

    Parameter.floor_handle_angle_mopchange_thr = atof(argv[1]);
    Parameter_PushRAMToFlash();
    shell_fprintf(shell, 0, "New value: %f\n", Parameter.floor_handle_angle_mopchange_thr);
  }
  return 0;
}

/*!
 *  @brief This is the function description
 */
static int cmd_min_mopchange_duration(const struct shell *shell, size_t argc, char **argv)
{
  if (argc == 1)
  {
    shell_fprintf(shell, 0, "%d\n", (uint32_t)Parameter.min_mopchange_duration);
  }
  else
  {

    Parameter.min_mopchange_duration = (int64_t)atol(argv[1]);
    Parameter_PushRAMToFlash();
    shell_fprintf(shell, 0, "New value: %d\n", (uint32_t)Parameter.min_mopchange_duration);
  }
  return 0;
}

/*!
 *  @brief This is the function description
 */
static int cmd_min_mopframeflip_duration(const struct shell *shell, size_t argc, char **argv)
{
  if (argc == 1)
  {
    shell_fprintf(shell, 0, "%d\n", (uint32_t)Parameter.min_mopframeflip_duration);
  }
  else
  {

    Parameter.min_mopframeflip_duration = (int64_t)atol(argv[1]);
    Parameter_PushRAMToFlash();
    shell_fprintf(shell, 0, "New value: %d\n", (uint32_t)Parameter.min_mopframeflip_duration);
  }
  return 0;
}

/*!
 *  @brief This is the function description
 */
static int cmd_angle_smooth_factor(const struct shell *shell, size_t argc, char **argv)
{
  if (argc == 1)
  {
    shell_fprintf(shell, 0, "%f\n", Parameter.angle_smooth_factor);
  }
  else
  {
    Parameter.angle_smooth_factor = atof(argv[1]);
    Parameter_PushRAMToFlash();
    shell_fprintf(shell, 0, "New value: %f\n", Parameter.angle_smooth_factor);
  }
  return 0;
}

/*!
 *  @brief This is the function description
 */
static int cmd_gyr_smooth_factor(const struct shell *shell, size_t argc, char **argv)
{
  if (argc == 1)
  {
    shell_fprintf(shell, 0, "%f\n", Parameter.gyr_smooth_factor);
  }
  else
  {
    Parameter.gyr_smooth_factor = atof(argv[1]);
    Parameter_PushRAMToFlash();
    shell_fprintf(shell, 0, "New value: %f\n", Parameter.gyr_smooth_factor);
  }
  return 0;
}

/*!
 *  @brief This is the function description
 */
static int cmd_min_mopcycle_duration(const struct shell *shell, size_t argc, char **argv)
{
  if (argc == 1)
  {
    shell_fprintf(shell, 0, "%f\n", Parameter.min_mopcycle_duration);
  }
  else
  {
    Parameter.min_mopcycle_duration = atof(argv[1]);
    Parameter_PushRAMToFlash();
    shell_fprintf(shell, 0, "New value: %f\n", Parameter.min_mopcycle_duration);
  }
  return 0;
}

/*!
 *  @brief This is the function description
 */
static int cmd_max_mopcycle_duration(const struct shell *shell, size_t argc, char **argv)
{
  if (argc == 1)
  {
    shell_fprintf(shell, 0, "%f\n", Parameter.max_mopcycle_duration);
  }
  else
  {

    Parameter.max_mopcycle_duration = atof(argv[1]);
    Parameter_PushRAMToFlash();
    shell_fprintf(shell, 0, "New value: %f\n", Parameter.max_mopcycle_duration);
  }
  return 0;
}

/*!
 *  @brief This is the function description
 */
static int cmd_max_sqm_coveraged_per_mop(const struct shell *shell, size_t argc, char **argv)
{
  if (argc == 1)
  {
    shell_fprintf(shell, 0, "%f\n", Parameter.max_sqm_coveraged_per_mop);
  }
  else
  {
    Parameter.max_sqm_coveraged_per_mop = atof(argv[1]);
    Parameter_PushRAMToFlash();
    shell_fprintf(shell, 0, "New value: %f\n", Parameter.max_sqm_coveraged_per_mop);
  }
  return 0;
}

/*!
 *  @brief This is the function description
 */
static int cmd_mop_width(const struct shell *shell, size_t argc, char **argv)
{
  if (argc == 1)
  {
    shell_fprintf(shell, 0, "%f\n", Parameter.mop_width);
  }
  else
  {
    Parameter.mop_width = atof(argv[1]);
    Parameter_PushRAMToFlash();
    shell_fprintf(shell, 0, "New value: %f\n", Parameter.mop_width);
  }
  return 0;
}

/*!
 *  @brief This is the function description
 */
static int cmd_usb_plugin_reset_time(const struct shell *shell, size_t argc, char **argv)
{
  if (argc == 1)
  {
    shell_fprintf(shell, 0, "%ld sec\n", Parameter.usb_plugin_reset_time);
  }
  else
  {
    Parameter.usb_plugin_reset_time = atoi(argv[1]);
    Parameter_PushRAMToFlash();
    shell_fprintf(shell, 0, "New value: %ld\n", Parameter.usb_plugin_reset_time);
  }
  return 0;
}

/*!
 *  @brief This is the function description
 */
static int cmd_usb_auto_reset_time(const struct shell *shell, size_t argc, char **argv)
{
  if (argc == 1)
  {
    shell_fprintf(shell, 0, "%ld sec\n", Parameter.usb_auto_reset_time);
  }
  else
  {
    Parameter.usb_auto_reset_time = atoi(argv[1]);
    Parameter_PushRAMToFlash();
    shell_fprintf(shell, 0, "New value: %ld\n", Parameter.usb_auto_reset_time);
  }
  return 0;
}

/*!
 *  @brief This is the function description
 */
static int cmd_modem_disable(const struct shell *shell, size_t argc, char **argv)
{
  int16_t err = 0;

  if (argc == 1)
  {
    shell_fprintf(shell, 0, "%d\n", Parameter.modem_disable);
  }
  else
  {
    Parameter.modem_disable = atoi(argv[1]);
    Parameter_PushRAMToFlash();
    shell_fprintf(shell, 0, "New value: %d\n", Parameter.modem_disable);

    if (Parameter.modem_disable == true)
    { /* Turn modem off */
      lte_lc_power_off();
      k_msleep(500);
    }
    else
    {
      /* Configure modem to use either LTE-M or NB-IoT */
      if (Parameter.network_connection_type == NB_IOT)
      {
        // err = lte_lc_system_mode_set(LTE_LC_SYSTEM_MODE_NBIOT);

        rtc_print_debug_timestamp();
        shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_YELLOW, "Note: Device will use NB-IoT connection. It may take several minutes for a NB-IoT connection to be established successfully\n");

        if (err)
        {
          rtc_print_debug_timestamp();
          shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_RED, "Error: Setting modem to NB-IoT failed\n");
        }
      }
      else
      {
        //     err = lte_lc_system_mode_set(LTE_LC_SYSTEM_MODE_LTEM);

        if (err)
        {
          rtc_print_debug_timestamp();
          shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_RED, "Error: Setting modem to LTE-M failed\n");
        }
      }
      k_msleep(100);

      /* Turn modem on - it will automatically search for networks*/
      //  lte_lc_normal();
    }
  }
  return 0;
}

/*!
 *  @brief This is the function description
 */
static int cmd_low_bat_thres(const struct shell *shell, size_t argc, char **argv)
{
  if (argc == 1)
  {
    shell_fprintf(shell, 0, "%d mV\n", Parameter.low_bat_threshold);
  }
  else
  {
    Parameter.low_bat_threshold = atoi(argv[1]);
    Parameter_PushRAMToFlash();
    shell_fprintf(shell, 0, "New value: %d mV\n", Parameter.low_bat_threshold);
  }
  return 0;
}

/*!
 *  @brief This is the function description
 */
static int cmd_rfid_disable(const struct shell *shell, size_t argc, char **argv)
{
  if (argc == 1)
  {
    shell_fprintf(shell, 0, "%d\n", Parameter.rfid_disable);
  }
  else
  {
    Parameter.rfid_disable = atoi(argv[1]);
    Parameter_PushRAMToFlash();
    shell_fprintf(shell, 0, "New value: %d\n", Parameter.rfid_disable);

    if (Parameter.rfid_disable == 1)
    {
      //    rfid_power_off();
    }
    else
    {
      //   RFID_PowerOn();
    }
  }
  return 0;
}

/*!
 *  @brief This is the function description
 */
static int cmd_mop_overlap(const struct shell *shell, size_t argc, char **argv)
{
  if (argc == 1)
  {
    shell_fprintf(shell, 0, "%f\n", Parameter.mop_overlap);
  }
  else
  {

    Parameter.mop_overlap = atof(argv[1]);
    Parameter_PushRAMToFlash();
    shell_fprintf(shell, 0, "New value: %f\n", Parameter.mop_overlap);
  }
  return 0;
}

/*!
 *  @brief This is the function description
 */
static int cmd_mopcycle_sequence_thr(const struct shell *shell, size_t argc, char **argv)
{
  if (argc == 1)
  {
    shell_fprintf(shell, 0, "%f\n", Parameter.mopcycle_sequence_thr);
  }
  else
  {

    Parameter.mopcycle_sequence_thr = atof(argv[1]);
    Parameter_PushRAMToFlash();
    shell_fprintf(shell, 0, "New value: %f\n", Parameter.mopcycle_sequence_thr);
  }
  return 0;
}

/*!
 *  @brief This is the function description
 */
static int cmd_peakfollower_update_delay(const struct shell *shell, size_t argc, char **argv)
{
  if (argc == 1)
  {
    shell_fprintf(shell, 0, "%d\n", (uint32_t)Parameter.peakfollower_update_delay);
  }
  else
  {

    Parameter.peakfollower_update_delay = (int64_t)atol(argv[1]);
    Parameter_PushRAMToFlash();
    shell_fprintf(shell, 0, "New value: %d\n", (uint32_t)Parameter.peakfollower_update_delay);
  }
  return 0;
}

/*!
 *  @brief This is the function description
 */
static int cmd_mop_rfid_detection_thr(const struct shell *shell, size_t argc, char **argv)
{
  if (argc == 1)
  {
    shell_fprintf(shell, 0, "%d\n", Parameter.mop_rfid_detection_thr);
  }
  else
  {
    Parameter.mop_rfid_detection_thr = atol(argv[1]);
    Parameter_PushRAMToFlash();
    shell_fprintf(shell, 0, "New value: %d\n", Parameter.mop_rfid_detection_thr);
  }
  return 0;
}

/*!
 *  @brief This is the function description
 */
static int cmd_mopping_coverage_per_mop_thr(const struct shell *shell, size_t argc, char **argv)
{
  if (argc == 1)
  {
    shell_fprintf(shell, 0, "%f\n", Parameter.mopping_coverage_per_mop_thr);
  }
  else
  {
    Parameter.mopping_coverage_per_mop_thr = atof(argv[1]);
    Parameter_PushRAMToFlash();
    shell_fprintf(shell, 0, "New value: %f\n", Parameter.mopping_coverage_per_mop_thr);
  }
  return 0;
}

/*!
 *  @brief This is the function description
 */
static int cmd_coap_verbose(const struct shell *shell, size_t argc, char **argv)
{
  ARG_UNUSED(argc);
  ARG_UNUSED(argv);

  if (Parameter.coap_verbose == false)
  {
    Parameter.coap_verbose = true;
    shell_print(shell, "Enabled CoAP verbose mode");
  }
  else
  {
    Parameter.coap_verbose = false;
    shell_print(shell, "Disabled CoAP verbose mode");
  }
  Parameter_PushRAMToFlash();
  return 0;
}

/*!
 *  @brief This is the function description
 */
static int cmd_protobuf_verbose(const struct shell *shell, size_t argc, char **argv)
{
  ARG_UNUSED(argc);
  ARG_UNUSED(argv);

  if (Parameter.protobuf_verbose == false)
  {
    Parameter.protobuf_verbose = true;
    shell_print(shell, "Enabled protobuf verbose mode");
  }
  else
  {
    Parameter.protobuf_verbose = false;
    shell_print(shell, "Disabled protobuf verbose mode");
  }
  Parameter_PushRAMToFlash();
  return 0;
}

/*!
 *  @brief This is the function description
 */
static int cmd_trigger_transmit(const struct shell *shell, size_t argc, char **argv)
{
  ARG_UNUSED(argc);
  ARG_UNUSED(argv);

  trigger_tx = true;
  shell_print(shell, "Triggered sending data");

  return 0;
}

/*!
 *  @brief This is the function description
 */
static int cmd_last_upload(const struct shell *shell, size_t argc, char **argv)
{
  ARG_UNUSED(argc);
  ARG_UNUSED(argv);

  if (initial_time_update == false)
  {
    shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "No data uploaded yet\n");
  }
  else
  {
    uint16_t i = 0;
    struct tm *ptm;
    char buf[50] = {0};
    uint16_t milli = 0;
    int64_t timestamp = 0;

    timestamp = timestamp_last_cloud_transmission;
    shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "Last upload %d sec ago, at timestamp: ", coap_last_transmission_timer);

    milli = timestamp % 1000LL; // Get only the millisec from the unix time stamp
    timestamp /= 1000LL;        // time.h can only handly timestamp in sec resolution, so cut the millisec

    /* convert from unix to local time */
    ptm = localtime(&timestamp);
    strftime(buf, 20, "%F %T", ptm);

    shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "%s:%03d\n", buf, milli);
  }
  return 0;
}

/*!
 *  @brief This is the function description
 */
static int cmd_event_verbose(const struct shell *shell, size_t argc, char **argv)
{
  ARG_UNUSED(argc);
  ARG_UNUSED(argv);

  if (Parameter.events_verbose == false)
  {
    Parameter.events_verbose = true;
    shell_print(shell, "Enabled event verbose mode");
  }
  else
  {
    Parameter.events_verbose = false;
    shell_print(shell, "Disabled event verbose mode");
  }
  Parameter_PushRAMToFlash();
  return 0;
}

/*!
 *  @brief This is the function description
 */
static int cmd_list_events(const struct shell *shell, size_t argc, char **argv)
{
  ARG_UNUSED(argc);
  ARG_UNUSED(argv);

  Event_PrintPreviousEvents();
  return 0;
}

/*!
 *  @brief This is the function description
 */
static int cmd_count_events(const struct shell *shell, size_t argc, char **argv)
{
  ARG_UNUSED(argc);
  ARG_UNUSED(argv);

  shell_print(shell, "Total events: %d, events to upload: %d", System.EventNumber, (Event_ItemsInArray + (Event_NumberOfOutsourcedMessages * EVENT_MAX_ITEMS_IN_ARRAY)));
  return 0;
}

/*!
 *  @brief This is the function description
 */
static int cmd_clear_events(const struct shell *shell, size_t argc, char **argv)
{
  ARG_UNUSED(argc);
  ARG_UNUSED(argv);

  Event_ClearArray();
  shell_print(shell, "Cleared event list.");
  return 0;
}

/*!
 *  @brief This is the function description
 */
static int cmd_list_last_seen_location_records(const struct shell *shell, size_t argc, char **argv)
{
  ARG_UNUSED(argc);
  ARG_UNUSED(argv);

  uint16_t i = 0;

  for (i = 0; i < EPC_LAST_SEEN_COUNT; i++)
  {
    shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "No: %d, epc: %s, type: %d:, id: %d\n", i, DeserializeHexToChar(room_wall_tag_last_seen[i].epc, 20), room_wall_tag_last_seen[i].type, room_wall_tag_last_seen[i].id);
  }
  return 0;
}

/*!
 *  @brief This is the function description
 */
static int cmd_list_last_seen_location_reset_time(const struct shell *shell, size_t argc, char **argv)
{
  ARG_UNUSED(argc);
  ARG_UNUSED(argv);

  if (argc == 1)
  {
    shell_fprintf(shell, 0, "%d\n", Parameter.last_seen_locations_auto_reset_time);
  }
  else
  {
    Parameter.last_seen_locations_auto_reset_time = atoi(argv[1]);
    Parameter_PushRAMToFlash();
    shell_fprintf(shell, 0, "New value: %d\n", Parameter.last_seen_locations_auto_reset_time);
  }
  return 0;
}

/*!
 *  @brief This is the function description
 */
static int cmd_list_room_and_mop(const struct shell *shell, size_t argc, char **argv)
{
  ARG_UNUSED(argc);
  ARG_UNUSED(argv);

  shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_CYAN, "Mop id: %d linked to room id: %d\n", current_room_to_mop_mapping.current_mop_id, current_room_to_mop_mapping.mop_linked_room_id);
  shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_CYAN, "Current mop epc: %s\n", DeserializeHexToChar(current_room_to_mop_mapping.current_mop_epc, 20));
  return 0;
}

// /*!
//  *  @brief This is the function description
//  */
// static int cmd_fota_connection_status(const struct shell *shell, size_t argc, char **argv)
// {
//   ARG_UNUSED(argc);
//   ARG_UNUSED(argv);

//   shell_print(shell, "Device fota connection status: %d\n", fota_is_connected);

//   return 0;
// }

// /*!
//  *  @brief This is the function description
//  */
// static int cmd_fota_enable(const struct shell *shell, size_t argc, char **argv)
// {
//   ARG_UNUSED(argc);
//   ARG_UNUSED(argv);

//   if (Parameter.fota_enable == false)
//   {
//     Parameter.fota_enable = true;
//     shell_print(shell, "Enabled fota feature");
//   }
//   else
//   {
//     Parameter.fota_enable = false;
//     shell_print(shell, "Disabled fota feature");
//   }
//   Parameter_PushRAMToFlash();
//   return 0;
// }

// /*!
//  *  @brief This is the function description
//  */
// static int cmd_fota_verbose(const struct shell *shell, size_t argc, char **argv)
// {
//   ARG_UNUSED(argc);
//   ARG_UNUSED(argv);

//   if (Parameter.fota_verbose == false)
//   {
//     Parameter.fota_verbose = true;
//     shell_print(shell, "Enabled fota verbose");
//   }
//   else
//   {
//     Parameter.fota_verbose = false;
//     shell_print(shell, "Disabled fota verbose");
//   }
//   Parameter_PushRAMToFlash();
//   return 0;
// }

// /*!
//  *  @brief This is the function description
//  */
// static int cmd_fota_connect(const struct shell *shell, size_t argc, char **argv)
// {
//   ARG_UNUSED(argc);
//   ARG_UNUSED(argv);

// //  aws_fota_process_state = AWS_FOTA_PROCESS_CONNECT;
//   return 0;
// }

// /*!
//  *  @brief This is the function description
//  */
// static int cmd_fota_disconnect(const struct shell *shell, size_t argc, char **argv)
// {
//   ARG_UNUSED(argc);
//   ARG_UNUSED(argv);

// //  aws_fota_process_state = AWS_FOTA_PROCESS_DISCONNECT;

//   return 0;
// }

/*!
 *  @brief This is the function description
 */
static int cmd_test_notification(const struct shell *shell, size_t argc, char **argv)
{
  ARG_UNUSED(argc);
  ARG_UNUSED(argv);

  if (Parameter.notification_test == false)
  {
    notification_set_priority(NOTIFICATION_PRIORITY_LEVEL_LOWEST);
    Parameter.notification_test = true;
    Parameter.notifications_while_usb_connected = true;
    Parameter.notification_verbose = true;

    shell_print(shell, "Enabled notification test.");
  }
  else
  {
    Parameter.notification_test = false;
    Parameter.notifications_while_usb_connected = false;
    Parameter.notification_verbose = false;
    Notification.next_state = NOTIFICATION_CLEAR;

    shell_print(shell, "Disabled notification test.");
  }

  Parameter_PushRAMToFlash();
  return 0;
}

/*!
 *  @brief This is the function description
 */
static int cmd_test0(const struct shell *shell, size_t argc, char **argv)
{
  ARG_UNUSED(argc);
  ARG_UNUSED(argv);

  uint16_t i = 0;

  for (i = 0; i < atoi(argv[1]); i++)
  {
    NewEvent0x0D();
    k_msleep(10);
    event_simulation_in_progress = true;
  }

  event_simulation_in_progress = false;
  return 0;
}

/*!
 *  @brief This is the function description
 */
static int cmd_test1(const struct shell *shell, size_t argc, char **argv)
{
  ARG_UNUSED(argc);
  ARG_UNUSED(argv);

  return 0;
}

/*!
 *  @brief This is the function description
 */
static int cmd_test2(const struct shell *shell, size_t argc, char **argv)
{
  ARG_UNUSED(argc);
  ARG_UNUSED(argv);

  network_info_log();
}

/*!
 *  @brief This is the function description
 */
static int cmd_test3(const struct shell *shell, size_t argc, char **argv)
{
  ARG_UNUSED(argc);
  ARG_UNUSED(argv);

	printk("LTE Link Connecting ...\n");
	err = lte_lc_init_and_connect();
	__ASSERT(err == 0, "LTE link could not be established.");
	printk("LTE Link Connected!\n");

  return 0;
}

/*!
 *  @brief This is the function description
 */
static int cmd_pcbtest(const struct shell *shell, size_t argc, char **argv)
{
  ARG_UNUSED(argc);
  ARG_UNUSED(argv);

  // test_pcb();
  return 0;
}

/*!
 *  @brief This is the function description
 */
void command_init(void)
{
  SHELL_STATIC_SUBCMD_SET_CREATE(adc,
                                 SHELL_CMD(usb, NULL, "Reads the usb voltage", cmd_read_usb_volt),
                                 SHELL_SUBCMD_SET_END /* Array terminated. */
  );
  SHELL_CMD_REGISTER(adc, &adc, "Command set to control and read out adc data", NULL);

  SHELL_STATIC_SUBCMD_SET_CREATE(algorithm,
                                 SHELL_CMD(acc_noise_thr, NULL, "m/s^2 - Noise thres for Acc data", cmd_acc_noise_thr),
                                 SHELL_CMD(gyr_noise_thr, NULL, "deg/s  - Noise thres for Gyr data", cmd_gyr_noise_thr),
                                 SHELL_CMD(mag_noise_thr, NULL, "uT - thres to remove earth magnetic field", cmd_mag_noise_thr),
                                 SHELL_CMD(gyr_spin_thr, NULL, "deg - min handle rotation degrees for S-shape mopping detection", cmd_gyr_spin_thr),
                                 SHELL_CMD(frame_handle_angle_thr, NULL, "deg - thres to detect frame flip.", cmd_frame_handle_angle_thr),
                                 SHELL_CMD(floor_handle_angle_mopping_thr_min, NULL, "deg - min handle inclination for mopping detection.", cmd_floor_handle_angle_mopping_thr_min),
                                 SHELL_CMD(floor_handle_angle_mopping_thr_max, NULL, "deg - max handle inclination for mopping detection.", cmd_floor_handle_angle_mopping_thr_max),
                                 SHELL_CMD(floor_handle_angle_mopchange_thr, NULL, "deg - thres to enable mop change detection.", cmd_floor_handle_angle_mopchange_thr),
                                 SHELL_CMD(min_mopchange_duration, NULL, "sec - thres to enable mop change detection.", cmd_min_mopchange_duration),
                                 SHELL_CMD(min_mopframeflip_duration, NULL, "sec - thres to enable mop frame flip", cmd_min_mopframeflip_duration),
                                 SHELL_CMD(angle_smooth_factor, NULL, "exp filter factor to estimate inclination angles (ie 0.1->recent)", cmd_angle_smooth_factor),
                                 SHELL_CMD(gyr_smooth_factor, NULL, "exp filter factor for Gyro based feature values", cmd_gyr_smooth_factor),
                                 SHELL_CMD(min_mopcycle_duration, NULL, "sec - min duration of mop cycle - fast mopping", cmd_min_mopcycle_duration),
                                 SHELL_CMD(max_mopcycle_duration, NULL, "sec - max duration of mop cycle - slow mopping", cmd_max_mopcycle_duration),
                                 SHELL_CMD(enable_coveraged_per_mop_notification, NULL, "Enable notification if mop reached max sqm coverage", cmd_enable_coveraged_per_mop_notification),
                                 SHELL_CMD(max_sqm_coveraged_per_mop, NULL, "m2 - max m2 to trigger notifications (for 2-sided mops)", cmd_max_sqm_coveraged_per_mop),
                                 SHELL_CMD(mop_width, NULL, "m - width of the mop frame", cmd_mop_width),
                                 SHELL_CMD(mop_overlap, NULL, "in precentage during mopping", cmd_mop_overlap),
                                 SHELL_CMD(mopcycle_sequence_thr, NULL, "num of mop cycles needed to set mopping flag", cmd_mopcycle_sequence_thr),
                                 SHELL_CMD(peakfollower_update_delay, NULL, "secs - period to update signal peak", cmd_peakfollower_update_delay),
                                 SHELL_CMD(mop_rfid_detection_thr, NULL, "num of sequential scans to confirm mop", cmd_mop_rfid_detection_thr),
                                 SHELL_CMD(mopping_coverage_per_mop_thr, NULL, "m - min thr to confirm unchipped mop", cmd_mopping_coverage_per_mop_thr),
                                 SHELL_CMD(verbose, NULL, "Show algorithm debug information", cmd_algo_verbose),
                                 SHELL_CMD(flag_verbose, NULL, "Show algorithm flag information", cmd_algo_flag_verbose),
                                 SHELL_CMD(settings, NULL, "Show all algorithm parameters", cmd_algo_settings),
                                 SHELL_CMD(current_shift_mop_check, NULL, "enables check for 'mop was already used in current shift'", cmd_current_shift_mop_check),
                                 SHELL_CMD(mop_id_refresh_timer, NULL, "sec to release the current registered mop id", cmd_mop_id_refresh_timer),
                                 SHELL_CMD(hit_shock_mag_thr, NULL, "thres for shock detection", cmd_hit_shock_mag_thr),
                                 SHELL_CMD(algocontrol_bymag_det, NULL, "Enables magnet detection to unlock algorithm", cmd_algocontrol_bymag_det),
                                 SHELL_CMD(mag_det_threshold, NULL, "uT - mag field strength for magnet detection", cmd_mag_det_threshold),
                                 SHELL_CMD(mag_det_consecutive_samples, NULL, "Consecutive samples for mag detection", cmd_mag_det_consecutive_samples),
                                 SHELL_CMD(event1statistics_interval, NULL, "Calculation interval for Event0x01", cmd_event1statistics_interval),
                                 SHELL_SUBCMD_SET_END /* Array terminated. */
  );
  SHELL_CMD_REGISTER(algorithm, &algorithm, "Command set to control moping algorithms", NULL);

  SHELL_STATIC_SUBCMD_SET_CREATE(battery,
                                 SHELL_CMD(voltage, NULL, "Returns the battery voltage", cmd_battery_voltage),
                                 SHELL_CMD(current, NULL, "Returns the battery current", cmd_battery_current),
                                 SHELL_CMD(cycles, NULL, "Returns the battery charge cycles", cmd_battery_charge_cycles),
                                 SHELL_CMD(soc, NULL, "Returns the state of charge(SOC) in percentage of the battery", cmd_battery_state_of_charge),
                                 SHELL_CMD(age, NULL, "Returns the estimated age of the battery", cmd_battery_age),
                                 SHELL_CMD(temperature, NULL, "Returns the measured temperature of the battery (10k NTC inside battery, AIN1 port battery gauge)", cmd_battery_temperature),
                                 SHELL_CMD(cap, NULL, "Returns the remaining capacity of the battery", cmd_battery_remaining_capacity),
                                 SHELL_CMD(tte, NULL, "Returns the estimated time to empty under present temperature and load conditions", cmd_battery_time_to_empty),
                                 SHELL_CMD(ttf, NULL, "Returns the estimated time to fully charge the battery", cmd_battery_time_to_full),
                                 SHELL_CMD(fully_charged_indicator_time, NULL, "Time to keep the green led is ON (when usb is connected and device is fully charged)", cmd_fully_charged_indicator_time),
                                 SHELL_CMD(charge_termination_current, NULL, "Charge current threshold to switch from white to green led (when charging ends).", cmd_battery_charge_termination_current),
                                 SHELL_CMD(charge_termination_temp_min, NULL, "Minimum temperature to allow charging (default: 0°C)", cmd_battery_charge_termination_temp_min),
                                 SHELL_CMD(charge_termination_temp_max, NULL, "Maximum temperature to allow charging (default: 45°C)", cmd_battery_charge_termination_temp_max),
                                 SHELL_CMD(sniff, NULL, "Sniff I2C communiction to battery gauge.", cmd_battery_gauge_sniff_i2c),
                                 SHELL_CMD(report, NULL, "Sniff I2C communiction to battery gauge.", cmd_battery_gauge_report),
                                 SHELL_SUBCMD_SET_END /* Array terminated. */
  );
  SHELL_CMD_REGISTER(battery, &battery, "Command set to control and read out battery data", NULL);

  SHELL_STATIC_SUBCMD_SET_CREATE(modem,
                                 SHELL_CMD(initialize, NULL, "Initialize all modem parameters", cmd_modem_initialize),
                                 SHELL_CMD(mode, NULL, "Change modem mode", cmd_modem_mode),
                                 SHELL_CMD(connection_type, NULL, "0 = LTE-M, 1 = NB-IoT", cmd_connection_type),
                                 SHELL_CMD(settings, NULL, "Returns current modem settings", cmd_modem_settings),
                                 SHELL_CMD(status, NULL, "Returns the network registration status", cmd_modem_status),
                                 SHELL_CMD(version, NULL, "Returns firmware version of the modem", cmd_modem_version),
                                 SHELL_CMD(command, NULL, "Send a AT command and prints out the response", cmd_modem_write),
                                 SHELL_CMD(providers, NULL, "Lists all availible providers which can be used for connection", cmd_modem_providers),
                                 SHELL_CMD(bands, NULL, "Lists all availible bands which can be used for connection", cmd_modem_bands),
                                 SHELL_CMD(verbose, NULL, "Displays modem debug information", cmd_modem_verbose),
                                 SHELL_CMD(rssi, NULL, "Returns the current rssi", cmd_modem_rssi),
                                 SHELL_CMD(imei, NULL, "Returns the IMEI number of the modem", cmd_modem_imei),
                                 SHELL_CMD(list_keys, NULL, "List all stored psk keys", cmd_modem_list_keys),
                                 SHELL_CMD(add_psk, NULL, "List all stored psk keys", cmd_add_psk_key),
                                 SHELL_CMD(add_psk_identity, NULL, "List all stored psk keys", cmd_add_psk_identity),
                                 SHELL_SUBCMD_SET_END /* Array terminated. */
  );
  SHELL_CMD_REGISTER(modem, &modem, "Command set to control the modem and cellular connectivity", NULL);

    SHELL_STATIC_SUBCMD_SET_CREATE(cloud,
                                   SHELL_CMD(sync_interval_idle, NULL, "Cloud sync interval while in idle mode.", cmd_cloud_sync_interval_idle),
                                   SHELL_CMD(sync_interval_moving, NULL, "Cloud sync interval while in moving mode.", cmd_cloud_sync_interval_moving),
                                   SHELL_CMD(coap_verbose, NULL, "Shows coap debug information.", cmd_coap_verbose),
                                   SHELL_CMD(protobuf_verbose, NULL, "Shows protobuf debug information.", cmd_protobuf_verbose),
                                   SHELL_CMD(trigger_transmit, NULL, "Force device to send data now", cmd_trigger_transmit),
                                   SHELL_CMD(last_upload, NULL, "Force device to send data now", cmd_last_upload),
                                   SHELL_SUBCMD_SET_END /* Array terminated. */
    );
    SHELL_CMD_REGISTER(cloud, &cloud, "Command set to cloud connectivity", NULL);

  SHELL_STATIC_SUBCMD_SET_CREATE(events,
                                 SHELL_CMD(verbose, NULL, "Shows event debug information.", cmd_event_verbose),
                                 SHELL_CMD(list, NULL, "Displays the last events.", cmd_list_events),
                                 SHELL_CMD(count, NULL, "Returns event count stored int the event array.", cmd_count_events),
                                 SHELL_CMD(clear, NULL, "Clear event list.", cmd_clear_events),
                                 SHELL_SUBCMD_SET_END /* Array terminated. */
  );
  SHELL_CMD_REGISTER(events, &events, "Commands to monitor events", NULL);

  SHELL_STATIC_SUBCMD_SET_CREATE(datetime,
                                 SHELL_CMD(timestamp, NULL, "Prints out the current timestamp in ISO8601 format", cmd_timestamp),
                                 SHELL_CMD(unixtime, NULL, "Gets or sets unix epoch time. Changes in this value will effect all other time commands", cmd_unixtime),
                                 SHELL_CMD(time, NULL, "Get or set time. <hh>:<mm>:<ss>", cmd_time),
                                 SHELL_CMD(date, NULL, "Get or set date. <dd>:<mm>:<yyyy>", cmd_date),
                                 SHELL_CMD(weekday, NULL, "Get the weekday", cmd_weekday),
                                 SHELL_SUBCMD_SET_END /* Array terminated. */
  );
  SHELL_CMD_REGISTER(datetime, &datetime, "Command set to control date and time)", NULL);

  SHELL_STATIC_SUBCMD_SET_CREATE(epc,
                                 SHELL_CMD(add_rfid_record, NULL, "Add a new tag rfid record memory. Syntax: epc add_rfid_record <id> <epc> <type>", cmd_add_rfid_record),
                                 SHELL_CMD(add_room_record, NULL, "Add a new tag room record memory. Syntax: epc add_room_record <id> <color> <type> <rfid_count>", cmd_add_room_record),
                                 SHELL_CMD(add_mop_record, NULL, "Add a new tag mop record memory. Syntax: epc add_mop_record <id> <color> <type> <size> <sides>", cmd_add_mop_record),
                                 SHELL_CMD(read_rfid_record, NULL, "Read rfid tag at index. Syntax: epc read_rfid_record <index>", cmd_read_rfid_record),
                                 SHELL_CMD(read_room_record, NULL, "Read room tag at index. Syntax: epc read_room_record <index>", cmd_read_room_record),
                                 SHELL_CMD(read_mop_record, NULL, "Read mop tag at index. Syntax: epc read_mop_record <index>", cmd_read_mop_record),
                                 SHELL_CMD(clear_rfid_record, NULL, "Clears complete rfid record list", cmd_clear_rfid_record),
                                 SHELL_CMD(clear_room_record, NULL, "Clears complete room record list", cmd_clear_room_record),
                                 SHELL_CMD(clear_mop_record, NULL, "Clears complete mop record list", cmd_clear_mop_record),
                                 SHELL_CMD(clear_databases, NULL, "Clears complete epc data base (rfid records, room records and mop records)", cmd_clear_epc_database),
                                 SHELL_CMD(search_rfid_record, NULL, "Search for rfid record and output related room and mop data.", cmd_search_rfid_record),
                                 SHELL_CMD(count_rfid_record, NULL, "Returns the number of rfid records in rfid record database", cmd_count_rfid_record),
                                 SHELL_CMD(count_room_record, NULL, "Returns the number of room records in room record database", cmd_count_room_record),
                                 SHELL_CMD(count_mop_record, NULL, "Returns the number of mop records in mop record database", cmd_count_mop_record),
                                 SHELL_CMD(listall_rfid_record, NULL, "Displays all rfid records entries in rfid record database", cmd_listall_rfid_record),
                                 SHELL_CMD(listall_room_record, NULL, "Displays all room records entries in room record database", cmd_listall_room_record),
                                 SHELL_CMD(listall_mop_record, NULL, "Displays all mop records entries in mop record database", cmd_listall_mop_record),
                                 SHELL_CMD(last_seen_records, NULL, "Displays the last seen records", cmd_epc_last_seen_record),
                                 SHELL_CMD(last_seen_location_tags, NULL, "Displays the last seen wall and room rfids", cmd_list_last_seen_location_records),
                                 SHELL_CMD(last_seen_location_reset_time, NULL, "Auto reset time for last seen location array", cmd_list_last_seen_location_reset_time),
                                 SHELL_CMD(clear_last_seen_records, NULL, "Clears the last seen records array", cmd_clear_last_seen_record),
                                 SHELL_CMD(current_room_mop_linking, NULL, "Returns current mop and current room", cmd_list_room_and_mop),
                                 SHELL_CMD(log_unkown_tags, NULL, "Returns current mop and current room", cmd_log_unkown_tags),
                                 SHELL_CMD(list_last_seen_mop_ids, NULL, "Returns current mop and current room", cmd_list_last_seen_mop_ids),
                                 SHELL_CMD(clear_last_seen_mop_array, NULL, "Clears complete room record list", cmd_clear_last_seen_mop_array),
                                 SHELL_CMD(mop_array_auto_reset_time, NULL, "Auto reset time for last seen mob array", cmd_mop_array_auto_reset_time),
                                 SHELL_CMD(verbose, NULL, "Shows the trimmed epc tag data comming from rfid module (40 byte long string)", cmd_epc_verbose),
                                 SHELL_CMD(tag_verbose, NULL, "Shows the trimmed epc tag data comming from rfid module (40 byte long string)", cmd_epc_raw_verbose),
                                 SHELL_CMD(binary_search_verbose, NULL, "Shows the statistics of the binary search algorithm", cmd_binary_search_verbose),
                                 SHELL_CMD(mop_verbose, NULL, "Shows the live reading of the actually seen mop", cmd_mop_verbose),
                                 SHELL_SUBCMD_SET_END /* Array terminated. */
  );
  SHELL_CMD_REGISTER(epc, &epc, "Command set to control the epc log data", NULL);

  SHELL_STATIC_SUBCMD_SET_CREATE(datalog,
                                 SHELL_CMD(count, NULL, "Requests the total data log frame count", cmd_datalog_get_count),
                                 SHELL_CMD(read, NULL, "Prints out the entire log data to console. Parameter: <Start frame number> <End frame number>", cmd_datalog_get_data),
                                 SHELL_CMD(clear, NULL, "Clear only data log area in which data is stored", cmd_datalog_clear),
                                 SHELL_CMD(format, NULL, "Clear the entire data log", cmd_datalog_format),
                                 SHELL_CMD(interval, NULL, "Changes the storing interval of log frames (default 1000ms)", cmd_datalog_interval),
                                 SHELL_CMD(verbose, NULL, "Prints out every new frame which is stored in external flash also at console", cmd_datalog_sniff_frame),
                                 SHELL_CMD(enable, NULL, "Enables or disables data logging (time and event based)", cmd_datalog_enable),
                                 SHELL_SUBCMD_SET_END /* Array terminated. */
  );
  SHELL_CMD_REGISTER(datalog, &datalog, "Command set to control the log data", NULL);

  SHELL_STATIC_SUBCMD_SET_CREATE(i2c,
                                 SHELL_CMD(scan, NULL, "Scans the I2C bus for slave devices and returns their addresses", cmd_i2csan),
                                 SHELL_CMD(read, NULL, "Read a byte from a specific slave. Parameter <SLAVE_ADDR>, <REG_ADDR>", cmd_i2cread),
                                 SHELL_CMD(write, NULL, "Write a byte to a specific slave. Parameter <SLAVE_ADDR>, <REG_ADDR>, <VALUE>", cmd_i2cwrite),
                                 SHELL_SUBCMD_SET_END /* Array terminated. */
  );
  SHELL_CMD_REGISTER(i2c, &i2c, "Command set for evaluating I2C bus slaves", NULL);

  SHELL_STATIC_SUBCMD_SET_CREATE(flash,
                                 SHELL_CMD(verbose, NULL, "Displays flash information", cmd_flash_verbose),
                                 SHELL_CMD(id, NULL, "Returns the device ID", cmd_flash_device_id),
                                 SHELL_CMD(check, NULL, "Start an integrety check.  Parameter: <CS_pin_no>", cmd_flashtest),
                                 SHELL_CMD(erasesector, NULL, "Erase the sector at a specific address.  Parameter: <CS_pin_no>", cmd_erasesector),
                                 SHELL_CMD(eraseall, NULL, "Erase the whole flash.  Parameter: <CS_pin_no>", cmd_eraseall),
                                 SHELL_CMD(read, NULL, "Reads an address range from flash. Parameter: <CS_pin_no> <start_adr in hex (e.g. 0x8000)> <length>", cmd_readrange),
                                 SHELL_CMD(read_reg, NULL, "Reads an address range from flash. Parameter: <CS_pin_no> <reg_adr>", cmd_readreg),
                                 SHELL_CMD(write_reg, NULL, "Reads an address range from flash. Parameter: <CS_pin_no> <reg> <data>", cmd_writereg),
                                 SHELL_CMD(status, NULL, "Prints out status register values.  Parameter: <CS_pin_no>", cmd_flashgetstatusreg),
                                 SHELL_CMD(flag_status, NULL, "Prints out status register values.  Parameter: <CS_pin_no>", cmd_flashgetflagstatusreg),
                                 SHELL_CMD(clear_flag_reg, NULL, "Prints out status register values.  Parameter: <CS_pin_no>", cmd_flashgclearflagreg),
                                 SHELL_CMD(reset, NULL, "Initialze a soft reset of flash memory.  Parameter: <CS_pin_no>", cmd_flashreset),
                                 SHELL_SUBCMD_SET_END /* Array terminated. */
  );
  SHELL_CMD_REGISTER(flash, &flash, "Command set to check, erase and readout external flash memory", NULL);

  SHELL_STATIC_SUBCMD_SET_CREATE(led,
                                 SHELL_CMD(color, NULL, "Color for rgb LED. Thre values need: <RED> <GREEN> <BLUE> (range 0-255)", cmd_led_color),
                                 SHELL_CMD(brightness, NULL, "Brightness rgb LED (range 0-255)", cmd_led_brightness),
                                 SHELL_SUBCMD_SET_END /* Array terminated. */
  );
  SHELL_CMD_REGISTER(led, &led, "Command set to change the settings of each LED", NULL);

  SHELL_STATIC_SUBCMD_SET_CREATE(buzzer,
                                 SHELL_CMD(duty_cycle, NULL, "Sets the buzzer volume (range 0-50).", cmd_buzzer_duty_cycle),
                                 SHELL_SUBCMD_SET_END /* Array terminated. */
  );
  SHELL_CMD_REGISTER(buzzer, &buzzer, "Command set to change the buzzer settings", NULL);

  SHELL_STATIC_SUBCMD_SET_CREATE(rfid,
                                 SHELL_CMD(settings, NULL, "Inquire the current settings from RFID module", cmd_rfid_settings),
                                 SHELL_CMD(interval, NULL, "Read interval for EPC in mopping state", cmd_rfid_interval),
                                 SHELL_CMD(interval_lifted, NULL, "Read interval for EPC when frame is lifted", cmd_rfid_interval_lifted),
                                 SHELL_CMD(output_power, NULL, "Sets the output power of the RFID module when not lifted (-2 to 25dBm)", cmd_rfid_output_power),
                                 SHELL_CMD(output_power_lifted, NULL, "Sets the output power of the RFID module when lifted (-2 to 25dBm)", cmd_output_power_lifted),
                                 SHELL_CMD(interval, NULL, "Sets the scan interval of the RFID module when not lifted (-2 to 25dBm)", cmd_rfid_interval),
                                 SHELL_CMD(frequency, NULL, "Sets the reader frequency.", cmd_rfid_frequency),
                                 SHELL_CMD(verbose, NULL, "Print found EPC tag to console", cmd_rfid_verbose),
                                 SHELL_CMD(sniff, NULL, "Displays the raw unfiltered incomming data from rfid module", cmd_rfid_sniff),
                                 SHELL_CMD(trigger, NULL, "Reads automatically EPC tags by settings the autotrigger pin at RFID module", cmd_rfid_trigger),
                                 SHELL_CMD(singlescan, NULL, "Triggers a single scan for EPC tags", cmd_rfid_singlescan),
                                 SHELL_CMD(autoscan, NULL, "Reads automatically EPC tags with given interval", cmd_rfid_autoscan),
                                 SHELL_CMD(command, NULL, "Writes a direct command to RFID module", cmd_rfid_command),
                                 SHELL_CMD(blink_notification, NULL, "Main user led will blink blue when ever a tag was read", cmd_rfid_blink_notification),
                                 SHELL_SUBCMD_SET_END /* Array terminated. */
  );
  SHELL_CMD_REGISTER(rfid, &rfid, "Command set to change RFID settings", NULL);

  SHELL_STATIC_SUBCMD_SET_CREATE(imu,
                                 SHELL_CMD(verbose, NULL, "Prints out the current acc, gyro and mag readings to console", cmd_trace),
                                 SHELL_CMD(trace, NULL, "Prints out the shorten list acc, gyro and mag readings to console", cmd_trace_reduced),
                                 SHELL_CMD(interval, NULL, "Set the interval for gathering new imu data", cmd_imu_interval),
                                 SHELL_CMD(motion_reset_time, NULL, "Set the time after which the device went to IDLE mode after a movement was detected.", cmd_motion_reset_time),
                                 SHELL_CMD(anymotion_dur, NULL, "Duration for any motion interrupt at IMU", cmd_anymotion_duration),
                                 SHELL_CMD(anymotion_thr, NULL, "Threshold for any motion interrupt at IMU", cmd_anymotion_thr),
                                 SHELL_SUBCMD_SET_END /* Array terminated. */
  );
  SHELL_CMD_REGISTER(imu, &imu, "Command set to change IMU settings", NULL);

  SHELL_STATIC_SUBCMD_SET_CREATE(stepdetection,
                                 SHELL_CMD(verbose, NULL, "Prints current readings from step detection to console", cmd_stepdetection_trace),
                                 SHELL_CMD(totalsteps, NULL, "Prints total steps in this session to console", cmd_stepdetection_totalsteps),
                                 SHELL_SUBCMD_SET_END /* Array terminated. */
  );
  SHELL_CMD_REGISTER(stepdetection, &stepdetection, "Command set to control and trace step detection algorithm", NULL);

  //   SHELL_STATIC_SUBCMD_SET_CREATE(fota,
  //                                  SHELL_CMD(enable, NULL, "Enables or disables the fota option", cmd_fota_enable),
  //                                  SHELL_CMD(verbose, NULL, "Enables or disables the fota option", cmd_fota_verbose),
  //                                  SHELL_CMD(status, NULL, "Reports the connection status to fota server.", cmd_fota_connection_status),
  //                                  SHELL_CMD(connect, NULL, "Connects the device to the fota server to be able to receive firmware updates", cmd_fota_connect),
  //                                  SHELL_CMD(disconnect, NULL, "Disconnects the device from the fota server", cmd_fota_disconnect),
  //                                  SHELL_SUBCMD_SET_END /* Array terminated. */
  //   );
  //   SHELL_CMD_REGISTER(fota, &fota, "Command set to control firmware-over-the-air updates (FOTA)", NULL);

  SHELL_STATIC_SUBCMD_SET_CREATE(device,
                                 SHELL_CMD(factory_reset, NULL, "For development pruposes.", cmd_factorysettings),
                                 SHELL_CMD(reboot, NULL, "Reboots the device", cmd_reboot),
                                 SHELL_CMD(hard_reboot, NULL, "Reboots the device", cmd_hard_reboot),
                                 SHELL_CMD(fw, NULL, "Get the firmware version", cmd_fw),
                                 SHELL_CMD(hw, NULL, "Get the hardware version", cmd_hw),
                                 SHELL_CMD(heap, NULL, "Returns the available space in heap", cmd_heap_freespace),
                                 SHELL_CMD(sessionuptime, NULL, "Get the hardware version", cmd_sessionuptime),
                                 SHELL_CMD(totaluptime, NULL, "Get the hardware version", cmd_totaluptime),
                                 SHELL_CMD(debug, NULL, "For development only", cmd_debug),
                                 SHELL_CMD(parameter_ram, NULL, "Displays all parameter from RAM.", cmd_print_parameter_ram),
                                 SHELL_CMD(parameter_flash, NULL, "Displays all parameter from flash.", cmd_print_parameter_flash),
                                 SHELL_CMD(blue_dev_led, NULL, "Controlles the blue on board dev led for debugging purposes.", cmd_enable_blue_dev_led),
                                 SHELL_CMD(rfid_confirmation, NULL, "Controlles led confirmation if a new wall tag was read.", cmd_enable_rfid_confirmation_blinking),
                                 SHELL_CMD(usb_plugin_reset_time, NULL, "Reset time in sec after the device reboots if it gets connected with the charger", cmd_usb_plugin_reset_time),
                                 SHELL_CMD(usb_auto_reset_time, NULL, "Reset time in sec if the device is continouse conected with the charger", cmd_usb_auto_reset_time),
                                 SHELL_CMD(modem_disable, NULL, "Disables modem (switchs it off permanently)", cmd_modem_disable),
                                 SHELL_CMD(rfid_disable, NULL, "Disables rfid module (switchs it off permanently)", cmd_rfid_disable),
                                 SHELL_CMD(low_bat_thres, NULL, "mV - Threshold to activate low bat notificatiopn", cmd_low_bat_thres),
                                 SHELL_SUBCMD_SET_END /* Array terminated. */
  );
  SHELL_CMD_REGISTER(device, &device, "help description", NULL);

  SHELL_STATIC_SUBCMD_SET_CREATE(notification,
                                 SHELL_CMD(verbose, NULL, "Displays all user notification after they gets triggered.", cmd_notification_verbose),
                                 SHELL_CMD(allow_while_usb_connected, NULL, "Allow user notification signals while device is connected to USB. Otherwise notification are disabled and only red/green charge led will appear.", cmd_notification_while_usb_connceted),
                                 SHELL_SUBCMD_SET_END /* Array terminated. */
  );
  SHELL_CMD_REGISTER(notification, &notification, "help description", NULL);

  SHELL_STATIC_SUBCMD_SET_CREATE(test,
                                 SHELL_CMD(notification, NULL, "PCB self test", cmd_test_notification),
                                 SHELL_CMD(pcb, NULL, "PCB self test", cmd_pcbtest),
                                 SHELL_CMD(0, NULL, "For development only", cmd_test0),
                                 SHELL_CMD(1, NULL, "For development only", cmd_test1),
                                 SHELL_CMD(2, NULL, "For development only", cmd_test2),
                                 SHELL_CMD(3, NULL, "For development only", cmd_test3),
                                 SHELL_SUBCMD_SET_END /* Array terminated. */
  );
  SHELL_CMD_REGISTER(test, &test, "For development only", NULL);
}