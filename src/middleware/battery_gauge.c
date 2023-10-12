/**
 * @file battery_gauge.c
 * @author Thomas Keilbach | keiltronic GmbH
 * @date 27 Oct 2022
 * @brief This file contains functions to communicate with the pheripherals
 * @version 1.0.0
 */

/*!
 * @defgroup Peripherals
 * @brief This file contains functions to communicate with the pheripherals
 * @{*/

#include "battery_gauge.h"

BATTERY battery = {2400.0, 3700.0, 0.01, 2400.0, 0.0, 0.0, 0.0, 0.0, 0.0, 25.0, 0.0, 0.0, 100.0};

uint8_t battery_low_bat_notification = false;
uint8_t battery_gauge_temperature_controlled_charge_enable = true;
uint8_t battery_charge_enable_notification = false;
uint8_t battery_avoid_multiple_notifications = false; // If the device is charged to 100%, it will start charging again after the SOC has fallen below 90%. IN this case do not notify the user
uint16_t battery_charge_status_delay = BATTERY_GAUGE_CHARGE_STATUS_DELAY;
uint16_t battery_gauge_temperature_progress_delay = BATTERY_GAUGE_TEMPERATURE_PROGRESS_DELAY;

struct i2c_dt_spec battery_i2c = I2C_DT_SPEC_GET(BATTERY_NODE);

void battery_gauge_init(void)
{
  if (!device_is_ready(battery_i2c.bus))
  {
    printk("I2C bus %s is not ready!\n\r", battery_i2c.bus->name);
    return;
  }

  k_msleep(100);

  // /* This initialzation data comes from MAXIMs MAX172xx m5 Fuel Gauge PC software configuration wizard */
  // // Device = MAX17201
  // // Title = EVKIT Configurator Profile Generated on 2022 / 08 / 19 12 : 42 : 47_CheckSum_0x3 battery_gauge_write(0x180, 0X0000); // nXTable0 Register
  // battery_gauge_write(0x180, 0X0000); // nXTable0 Register
  // battery_gauge_write(0x181, 0X0000); // nXTable1 Register
  // battery_gauge_write(0x182, 0X0000); // nXTable2 Register
  // battery_gauge_write(0x183, 0X0000); // nXTable3 Register
  // battery_gauge_write(0x184, 0X0000); // nXTable4 Register
  // battery_gauge_write(0x185, 0X0000); // nXTable5 Register
  // battery_gauge_write(0x186, 0X0000); // nXTable6 Register
  // battery_gauge_write(0x187, 0X0000); // nXTable7 Register
  // battery_gauge_write(0x188, 0X0000); // nXTable8 Register
  // battery_gauge_write(0x189, 0X0000); // nXTable9 Register
  // battery_gauge_write(0x18A, 0X0000); // nXTable10 Register
  // battery_gauge_write(0x18B, 0X0000); // nXTable11 Register
  // battery_gauge_write(0x18C, 0X0000); // nUser18C Register
  // battery_gauge_write(0x18D, 0X0000); // nUser18D Register
  // battery_gauge_write(0x18E, 0X0000); // nODSCTh Register
  // battery_gauge_write(0x18F, 0X0000); // nODSCCfg Register
  // battery_gauge_write(0x190, 0X0000); // nOCVTable0 Register
  // battery_gauge_write(0x191, 0X0000); // nOCVTable1 Register
  // battery_gauge_write(0x192, 0X0000); // nOCVTable2 Register
  // battery_gauge_write(0x193, 0X0000); // nOCVTable3 Register
  // battery_gauge_write(0x194, 0X0000); // nOCVTable4 Register
  // battery_gauge_write(0x195, 0X0000); // nOCVTable5 Register
  // battery_gauge_write(0x196, 0X0000); // nOCVTable6 Register
  // battery_gauge_write(0x197, 0X0000); // nOCVTable7 Register
  // battery_gauge_write(0x198, 0X0000); // nOCVTable8 Register
  // battery_gauge_write(0x199, 0X0000); // nOCVTable9 Register
  // battery_gauge_write(0x19A, 0X0000); // nOCVTable10 Register
  // battery_gauge_write(0x19B, 0X0000); // nOCVTable11 Register
  // battery_gauge_write(0x19C, 0X0000); // nIChgTerm Register
  // battery_gauge_write(0x19D, 0X0000); // nFilterCfg Register
  // battery_gauge_write(0x19E, 0X0000); // nVEmpty Register
  // battery_gauge_write(0x19F, 0X2602); // nLearnCfg Register
  // battery_gauge_write(0x1A0, 0X3C00); // nQRTable00 Register
  // battery_gauge_write(0x1A1, 0X1B80); // nQRTable10 Register
  // battery_gauge_write(0x1A2, 0X0B04); // nQRTable20 Register
  // battery_gauge_write(0x1A3, 0X0885); // nQRTable30 Register
  // battery_gauge_write(0x1A4, 0X0000); // nCycles Register
  // battery_gauge_write(0x1A5, 0X15C4); // nFullCapNom Register
  // battery_gauge_write(0x1A6, 0X1070); // nRComp0 Register
  // battery_gauge_write(0x1A7, 0X263D); // nTempCo Register
  // battery_gauge_write(0x1A8, 0XED40); // nIAvgEmpty Register
  // battery_gauge_write(0x1A9, 0X12C0); // nFullCapRep Register
  // battery_gauge_write(0x1AA, 0X0000); // nVoltTemp Register
  // battery_gauge_write(0x1AB, 0X807F); // nMaxMinCurr Register
  // battery_gauge_write(0x1AC, 0X00FF); // nMaxMinVolt Register
  // battery_gauge_write(0x1AD, 0X807F); // nMaxMinTemp Register
  // battery_gauge_write(0x1AE, 0X0000); // nSOC Register
  // battery_gauge_write(0x1AF, 0X0000); // nTimerH Register
  // battery_gauge_write(0x1B0, 0X0000); // nConfig Register
  // battery_gauge_write(0x1B1, 0X0204); // nRippleCfg Register
  // battery_gauge_write(0x1B2, 0X3070); // nMiscCfg Register
  // battery_gauge_write(0x1B3, 0X12C0); // nDesignCap Register
  // battery_gauge_write(0x1B4, 0X0000); // nHibCfg Register
  // battery_gauge_write(0x1B5, 0X9C01); // nPackCfg Register
  // battery_gauge_write(0x1B6, 0X0000); // nRelaxCfg Register
  // battery_gauge_write(0x1B7, 0X2241); // nConvgCfg Register
  // battery_gauge_write(0x1B8, 0X0100); // nNVCfg0 Register
  // battery_gauge_write(0x1B9, 0X0006); // nNVCfg1 Register
  // battery_gauge_write(0x1BA, 0XFF0A); // nNVCfg2 Register
  // battery_gauge_write(0x1BB, 0X0002); // nSBSCfg Register
  // battery_gauge_write(0x1BC, 0X0000); // nROMID0 Register
  // battery_gauge_write(0x1BD, 0X0000); // nROMID1 Register
  // battery_gauge_write(0x1BE, 0X0000); // nROMID2 Register
  // battery_gauge_write(0x1BF, 0X0000); // nROMID3 Register
  // battery_gauge_write(0x1C0, 0X0000); // nVAlrtTh Register
  // battery_gauge_write(0x1C1, 0X0000); // nTAlrtTh Register
  // battery_gauge_write(0x1C2, 0X0000); // nSAlrtTh Register
  // battery_gauge_write(0x1C3, 0X0000); // nIAlrtTh Register
  // battery_gauge_write(0x1C4, 0X0000); // nUser1C4 Register
  // battery_gauge_write(0x1C5, 0X0000); // nUser1C5 Register
  // battery_gauge_write(0x1C6, 0X0000); // nFullSOCThr Register
  // battery_gauge_write(0x1C7, 0X0000); // nTTFCfg Register
  // battery_gauge_write(0x1C8, 0X0000); // nCGain Register
  // battery_gauge_write(0x1C9, 0X0025); // nTCurve Register
  // battery_gauge_write(0x1CA, 0X0000); // nTGain Register
  // battery_gauge_write(0x1CB, 0X0000); // nTOff Register
  // battery_gauge_write(0x1CC, 0X0000); // nManfctrName0 Register
  // battery_gauge_write(0x1CD, 0X0000); // nManfctrName1 Register
  // battery_gauge_write(0x1CE, 0X0000); // nManfctrName2 Register
  // battery_gauge_write(0x1CF, 0X03E8); // nRSense Register
  // battery_gauge_write(0x1D0, 0X0000); // nUser1D0 Register
  // battery_gauge_write(0x1D1, 0X0000); // nUser1D1 Register
  // battery_gauge_write(0x1D2, 0XD5E3); // nAgeFcCfg Register
  // battery_gauge_write(0x1D3, 0X0000); // nDesignVoltage Register
  // battery_gauge_write(0x1D4, 0X0000); // nUser1D4 Register
  // battery_gauge_write(0x1D5, 0X0000); // nRFastVShdn Register
  // battery_gauge_write(0x1D6, 0X0000); // nManfctrDate Register
  // battery_gauge_write(0x1D7, 0X0000); // nFirstUsed Register
  // battery_gauge_write(0x1D8, 0X0000); // nSerialNumber0 Register
  // battery_gauge_write(0x1D9, 0X0000); // nSerialNumber1 Register
  // battery_gauge_write(0x1DA, 0X0000); // nSerialNumber2 Register
  // battery_gauge_write(0x1DB, 0X0000); // nDeviceName0 Register
  // battery_gauge_write(0x1DC, 0X0000); // nDeviceName1 Register
  // battery_gauge_write(0x1DD, 0X0000); // nDeviceName2 Register
  // battery_gauge_write(0x1DE, 0X0000); // nDeviceName3 Register
  // battery_gauge_write(0x1DF, 0X0000); // nDeviceName4 Register

  // battery_gauge_soft_reset();
}

void battery_gauge_CheckChargeStatus(void)
{
  if (System.charger_connected == true)
  {
    if (battery_gauge_temperature_controlled_charge_enable == true) // if battery temperature is in valid range (0 - 45 deg), TÜV safty
    {
      if (battery.Current > ((double)Parameter.battery_charge_termination_current + BATTERY_CHARGE_CURRENT_HYSTERESIS))
      {
        System.ChargeStatus = true;
      }
      else if (battery.Current < ((double)Parameter.battery_charge_termination_current - BATTERY_CHARGE_CURRENT_HYSTERESIS))
      {
        System.ChargeStatus = false;

        if (battery_avoid_multiple_notifications == false)
        {
          battery_avoid_multiple_notifications = true;

          Device.ChargeCycles++;
          Device_PushRAMToFlash();

          battery_avoid_multiple_notifications = true;

          /* Add event in event array which is send to cloud in next sync interval */
          NewEvent0x12(); // charging stopped

          /* Update user notification led */
          Notification.next_state = NOTIFICATION_FULLY_CHARGED;
          System.StatusInputs &= ~STATUSFLAG_CHG; // Create status entry
        }
      }
    }
    else
    {
      System.ChargeStatus = false;
      System.StatusInputs &= ~STATUSFLAG_CHG; // Create status entry
    }
  }
  else
  {
    System.ChargeStatus = false;
  }

  System.ChargeStatus_old = System.ChargeStatus;
}

void battery_gauge_CheckLowBat(void)
{
  if (System.charger_connected == false)
  {
    if ((battery.Voltage <= Parameter.low_bat_threshold) && (battery_low_bat_notification == false))
    {
      battery_low_bat_notification = true; // This flag is used to notify the user only once (to trigger the led pattern only once)
      Notification.next_state = NOTIFICATION_LOWBAT;

      /* Disable the 5V booster and enable it again if the voltage goes above the threshold voltage */
      imu_enter_sleep();
      rfid_power_off();
      // lte_lc_offline(); // Send "AT+CFUN=4"
      //   bsd_shutdown();     // Method to gracefully shutdown the BSD library.
      nrf_modem_lib_shutdown();

      System.StatusOutputs |= STATUSFLAG_LB;

      Device_PushRAMToFlash(); // store operating time

      /* Add event in event array which is send to cloud in next sync interval */
      NewEvent0x09(); // Battery charge low
    }

    if ((battery.Voltage > Parameter.low_bat_threshold + LOW_BAT_HYSTERESIS) && (battery_low_bat_notification == true))
    {
      battery_low_bat_notification = false;

      if (System.charger_connected == false)
      {
        Notification.next_state = NOTIFICATION_CLEAR;
      }

      /* Power on IMU and rfid module */
      RFID_PowerOn();
      imu_init();

      /* Reset cellular connection flags */
      modem.connection_stat = false;
      modem.registration_status[0] = LTE_LC_NW_REG_UNKNOWN;
      modem.registration_status[1] = LTE_LC_NW_REG_UNKNOWN;

      /* Turn modem on - it will automatically search for networks*/
      lte_lc_normal();
    }
  }
}

void battery_gauge_UpdateData(void)
{
  battery.Voltage = battery_getVoltage();
  k_msleep(10);
  battery.Current = battery_getCurrent();
  k_msleep(10);
  battery.AvgCurrent = battery_getAvgCurrent();
  k_msleep(10);
  battery.Temperature = battery_getTemperature();
  k_msleep(10);
  battery.ChargeCycle = battery_getChargeCycle();
  k_msleep(10);
  battery.StateOfCharge = battery_getStateOfCharge();
  k_msleep(10);
  battery.TimeToFull = battery_getTimeToFull();
  k_msleep(10);
  battery.TimeToEmpty = battery_getTimeToEmpty();
  k_msleep(10);
  battery.RemainingCapacity = battery_getCapacity();
  k_msleep(10);
  battery.age = battery_getAge();
  k_msleep(10);
}

void battery_gauge_print_report(void)
{
  shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "Voltage: %4.2fmV\n", battery.Voltage);
  shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "Current: %4.2fmA\n", battery.Current);
  shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "AvgCurrent: %4.2fmA\n", battery.AvgCurrent);
  shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "Temperature: %4.2f°C\n", battery.Temperature);
  shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "ChargeStatus: %d\n", System.ChargeStatus);
  shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "Charge cycle: %4.0f\n", battery.ChargeCycle);
  shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "State of charge: %2.1f\%\n", battery.StateOfCharge);
  shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "Remaining capacity: %4.2fmAh\n", battery.RemainingCapacity);
}

int8_t battery_remaining_non_volatile_updates(void)
{
  /* 1. Write 0xE2FA to the Command register (060h).
     2. Wait tRECALL.
     3. Read memory address 0x1ED.
     4. Decode address 0x1ED data as shown in Table 26. Each secret update has redundant indicator flags for reliability.
        Logically OR the upper and lower bytes together then count the number of 1s to determine how many updates
        have already been used. The first update occurs in manufacturing test to clear the secret memory prior to shipping
        to the user.
  */

  // uint16_t readout;
  uint8_t val = 0;
  int8_t rslt = 0;
  uint8_t readout[2];
  int16_t ret = 0;

  battery_gauge_write(MAX1720X_COMMAND_REG, 0xE2FA); // fuel gauge reset
  k_msleep(tRECALL);

  // i2c_burst_read(i2c_dev, (uint16_t)MAX1720X_ADDR_LO, 0x1ED, &readout, 2);
  ret = i2c_burst_read_dt(&battery_i2c, 0x1ED, &readout, sizeof(readout));
  if (ret != 0)
  {
    printk("Failed to read from I2C device address 0x%x at reg. 0x%x . return value: %d\n", battery_i2c.addr, 0x1ED, ret);
    return;
  }

  if (Parameter.battery_gauge_sniff_i2c == true)
  {
    rtc_print_debug_timestamp();
    shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "Read from address 0x%X, register 0x%X the value 0x%X 0x%X\n", MAX1720X_ADDR_LO, 0x1ED, readout[0], readout[1]);
  }
  val = readout[1] | readout[0];

  switch (val)
  {
  case 0b00000001:
    rslt = 7;
    break;

  case 0b00000011:
    rslt = 6;
    break;

  case 0b00000111:
    rslt = 5;
    break;

  case 0b00001111:
    rslt = 4;
    break;

  case 0b00011111:
    rslt = 3;
    break;

  case 0b00111111:
    rslt = 2;
    break;

  case 0b01111111:
    rslt = 1;
    break;

  case 0b11111111:
    rslt = 0;
    break;

  default:
    rslt = -1;
    break;
  }

  return rslt;
}

void battery_gauge_soft_reset(void)
{
  battery_gauge_write(MAX1720X_CONFIG2, 0x0001); // fuel gauge reset
  k_msleep(tPOR);
}

void battery_gauge_full_reset(void)
{
  battery_gauge_write(0X000F, 0x060);
  k_msleep(tPOR);
  battery_gauge_soft_reset();
}

double battery_getVoltage(void)
{
  double rslt = 0.0;
  rslt = (double)battery_gauge_read(MAX1720X_VCELL_ADDR) * 0.078125;
  return rslt;
}

double battery_getCurrent(void)
{
  double rslt = 0.0;
  int16_t val = 0;

  val = battery_gauge_read(MAX1720X_CURENT_ADDR);
  rslt = (double)val;

  rslt *= MAX1720X_CURRENT_RES;
  rslt /= MAX1720X_SENS_RESISTOR_VALUE;
  return rslt;
}

double battery_getAvgCurrent(void)
{
  double rslt = 0.0;
  int16_t val = 0;

  val = battery_gauge_read(MAX1720X_AVG_CURENT_ADDR);
  rslt = (double)val;

  rslt *= MAX1720X_CURRENT_RES;
  rslt /= MAX1720X_SENS_RESISTOR_VALUE;
  return rslt;
}

double battery_getTemperature(void)
{
  int16_t rslt = 0;
  double val = 0.0;

  rslt = battery_gauge_read(MAX1720X_AIN1_ADDR);

  val = (double)rslt;
  val /= 10;    // shift value to have one decimal point
  val -= 273.0; // remove offset (0 Kelvin = -273°C)

  return val;
}

double battery_getCapacity(void)
{ // RepCap or reported capacity is a filtered version of the AvCap register that prevents large jumps in the reported value caused by changes in the application such as abrupt
  double rslt = 0.0;
  int16_t val = 0;

  val = battery_gauge_read(MAX1720X_REPCAP_ADDR);
  rslt = (double)val;

  rslt *= MAX1720X_CAPACITY_RES;
  rslt /= MAX1720X_SENS_RESISTOR_VALUE;
  return rslt;
}

double battery_getStateOfCharge(void)
{ // Returns the relative state of charge of the connected LiIon Polymer battery as a percentage of the full capacity w/ resolution 1/256%
  double rslt = 0.0;
  int16_t val = 0;

  val = battery_gauge_read(MAX1720X_REPSOC_ADDR);
  rslt = (double)val;

  rslt /= 256.0;
  return rslt;
}

double battery_getAge(void)
{ // Returns the relative state of charge of the connected LiIon Polymer battery as a percentage of the full capacity w/ resolution 1/256%
  double rslt = 0.0;
  int32_t val = 0;

  val = battery_gauge_read(MAX1720X_AGE_ADDR);
  rslt = (double)val;

  rslt /= 256.0;

  if (rslt > 100.0)
  {
    rslt = 100.0;
  }

  return rslt;
}

double battery_getTimeToFull(void)
{ // The TTF register holds the estimated time to full for the application under present conditions.
  double rslt = 0.0;
  int16_t val = 0;

  val = battery_gauge_read(MAX1720X_TTF_ADDR);
  rslt = (double)val;

  rslt *= MAX1720X_TIME_LSB;
  return rslt;
}

double battery_getTimeToEmpty(void)
{ // The TTE register holds the estimated time to empty for the application under present temperature and load conditions
  double rslt = 0.0;
  int16_t val = 0;

  val = battery_gauge_read(MAX1720X_TTE_ADDR);
  rslt = (double)val;

  rslt *= MAX1720X_TIME_LSB;
  return rslt;
}

double battery_getChargeCycle(void)
{ // The TTE register holds the estimated time to empty for the application under present temperature and load conditions
  double rslt = 0.0;
  int16_t val = 0;

  val = battery_gauge_read(MAX1720X_CYCLE_ADDR);
  rslt = (double)val;
  return rslt;
}

void battery_gauge_write(uint16_t reg, uint16_t val)
{
  uint8_t data[3];
  int16_t ret = 0;

  if (reg >= 0x100)
  {
    data[0] = (reg & 0xFF);
    data[1] = (uint8_t)val;
    data[2] = (uint8_t)(val >> 8);
    // i2c_write(i2c_dev, data, 3, (uint16_t)MAX1720X_ADDR_HI);

    ret = i2c_write_dt(&battery_i2c, data, sizeof(data));
    if (ret != 0)
    {
      printk("Failed to write to I2C device address 0x%x at reg. 0x%x . return value: %d\n", battery_i2c.addr, data[0], ret);
      return;
    }

    if (Parameter.battery_gauge_sniff_i2c == true)
    {
      rtc_print_debug_timestamp();
      shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "Write to address 0x%X, register 0x%X the value 0x%X 0x%X\n", MAX1720X_ADDR_HI, data[0], data[1], data[2]);
    }
  }
  else
  {
    data[0] = reg;
    data[1] = (uint8_t)val;
    data[2] = (uint8_t)(val >> 8);
    // i2c_write(i2c_dev, data, 3, (uint16_t)MAX1720X_ADDR_LO);

    ret = i2c_write_dt(&battery_i2c, data, sizeof(data));
    if (ret != 0)
    {
      printk("Failed to write to I2C device address 0x%x at reg. 0x%x . return value: %d\n", battery_i2c.addr, data[0], ret);
      return;
    }

    if (Parameter.battery_gauge_sniff_i2c == true)
    {
      rtc_print_debug_timestamp();
      shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "Write to address 0x%X, register 0x%X the value 0x%X 0x%X\n", MAX1720X_ADDR_LO, data[0], data[1], data[2]);
    }
  }
}

uint16_t battery_gauge_read(uint16_t reg)
{
  uint8_t readout[2];
  int16_t ret = 0;

  if (reg >= 0x100)
  {
    // i2c_burst_read(i2c_dev, (uint16_t)MAX1720X_ADDR_HI, (reg & 0xFF), &readout, 2);
    ret = i2c_burst_read_dt(&battery_i2c, (reg & 0xFF), &readout, sizeof(readout));
    if (ret != 0)
    {
      printk("Failed to read from I2C device address 0x%x at reg. 0x%x . return value: %d\n", battery_i2c.addr, (reg & 0xFF), ret);
      return;
    }

    if (Parameter.battery_gauge_sniff_i2c == true)
    {
      rtc_print_debug_timestamp();
      shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "Read from address 0x%X, register 0x%X the value 0x%X 0x%X\n", MAX1720X_ADDR_HI, reg, readout[0], readout[1]);
    }
  }
  else
  {
    // i2c_burst_read(i2c_dev, (uint16_t)MAX1720X_ADDR_LO, reg, &readout, 2);
    ret = i2c_burst_read_dt(&battery_i2c, (reg & 0xFF), &readout, sizeof(readout));
    {
      printk("Failed to read from I2C device address 0x%x at reg. 0x%x . return value: %d\n", battery_i2c.addr, (reg & 0xFF), ret);
      return;
    }

    if (Parameter.battery_gauge_sniff_i2c == true)
    {
      rtc_print_debug_timestamp();
      shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "Read from address 0x%X, register 0x%X the value 0x%X 0x%X\n", MAX1720X_ADDR_LO, reg, readout[0], readout[1]);
    }
  }

  return (((uint16_t)readout[1] << 8) + (uint16_t)readout[0]);
}
