/**
 * @file battery_gauge.h
 * @author Thomas Keilbach | keiltronic GmbH
 * @date 27 Oct 2022
 * @brief This file contains functions headers to communicate with the pheripherals
 * @version 1.0.0
 */

#ifndef BATTERY_GAUGE_H
#define BATTERY_GAUGE_H

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/i2c.h>
#include "i2c.h"
#include "system_mem.h"
#include "notification.h"

/* Device address */
#define MAX1720X_ADDR_LO 0x36 // 7-bit address -> in 8-bit: 0x6C, address range 0x000 - 0x0FF
#define MAX1720X_ADDR_HI 0x0B // 7-bit address -> in 8-bit: 0x16, address range 0x180 - 0x1FF

#define MAX1720X_ADDR 0x36
#define MAX1720X_STATUS_ADDR 0x00     // Contains alert status and chip status
#define MAX1720X_VCELL_ADDR 0x09      // Lowest cell voltage of a pack, or the cell voltage for a single cell
#define MAX1720X_REPSOC_ADDR 0x06     // Reported state of charge
#define MAX1720X_REPCAP_ADDR 0x05     // Reported remaining capacity
#define MAX1720X_AGE_ADDR 0x07        // Reported estimated age in percent
#define MAX1720X_TEMP_ADDR 0x08       // Temperature
#define MAX1720X_CURENT_ADDR 0x0A     // Battery current
#define MAX1720X_AVG_CURENT_ADDR 0x0B // Battery average current
#define MAX1720X_TTE_ADDR 0x11        // Time to empty
#define MAX1720X_CYCLE_ADDR 0x17      // Charge cycle counter
#define MAX1720X_TTF_ADDR 0x20        // Time to full
#define MAX1720X_DEVNAME_ADDR 0x21    // Time to full
#define MAX1720X_CAPACITY_ADDR 0x10   // Full capacity estimation
#define MAX1720X_VBAT_ADDR 0xDA       // Battery pack voltage
#define MAX1720X_AVCELL_ADDR 0x17     // Battery cycles
#define MAX1720X_COMMAND_ADDR 0x60    // Command register
#define MAX1720X_CONFIG2_ADDR 0xbb    // Command register
#define MAX1720X_AVG_TEMP1_ADDR 0x137 // Command register

#define MAX1720X_PACKCFG 0x0BD
#define MAX1720X_NPACKCFG 0x1B5
#define MAX1720X_NDESIGNCAP 0x1B3
#define MAX1720X_NCONFIG 0x1B0
#define MAX1720X_NNVCFG0 0x1B8
#define MAX1720X_CONFIG 0x01D
#define MAX1720X_CONFIG2 0x0BB
#define MAX1720X_AIN1_ADDR  0x134 

//#define MAX1720X_CONFIG2_ADDR           0x1b4   // Hibernate register
#define BATTERY_NODE DT_ALIAS(batterygauge)

/* Resolutions */
#define MAX1720X_CAPACITY_RES 0.005          // Vh with 10mOhm shunt
#define MAX1720X_CURRENT_RES 0.0015625       // mA with 10mOhm shunt
#define MAX1720X_SENS_RESISTOR_VALUE 0.01    // Ohms
#define MAX1720X_TIME_LSB 5.625              // sec
#define MAX1720X_PERCENT_LSB 0.00390625      // 1/256%
#define MAX1720X_AIN_TEMP_RESOLUTION 0.00391 // Internal Temperature Measurement Resolution at AIN1/2
#define MAX1720X_COMMAND_REG 060 // fuel gauge reset
#define tRECALL 10
#define tPOR 10
#define tBLOCK 8000


/* Battery percentage level */
#define BATTERY_GOOD 60 // 3650
#define BATTERY_LOW 20  // 3400

#define VUSB_THRES_DIGIT 3000 // 850
#define LOW_BAT_THRES 10      // in percent % (State of charge)
//#define LOW_BAT_VOLTAGE_THRES 3450
#define LOW_BAT_HYSTERESIS 100
#define BATTERY_CHARGE_CURRENT_HYSTERESIS 2.0  // mA
#define BATTERY_CHARGE_TEMPERATURE_HYSTERESIS 1.0 // deg
#define BATTERY_GAUGE_CHARGE_STATUS_DELAY 10 // unit: 100ms  -> 10 = 10*100ms = 1sec
#define BATTERY_GAUGE_TEMPERATURE_PROGRESS_DELAY 5; // sec

typedef struct
{
  double NominalCapacity;   // mAh
  double NominalVoltage;    // mV
  double Shunt;             // mOhm
  double RemainingCapacity; // mAh  
  double TimeToFull;        // s
  double TimeToEmpty;       // s
  double Current;           // mA
  double AvgCurrent;        // mA
  double Voltage;           // mV
  double Temperature;       // deg
  double ChargeCycle;       // counts
  double StateOfCharge;     // %
  double age;               // %
} BATTERY;

extern BATTERY battery;

extern void battery_gauge_init(void);
extern void battery_gauge_soft_reset(void);
extern void battery_gauge_full_reset(void);
extern void battery_gauge_write(uint16_t reg, uint16_t val);
extern uint16_t battery_gauge_read(uint16_t reg);
extern void battery_gauge_UpdateData(void);
extern double battery_getCurrent(void);
extern double battery_getAvgCurrent(void);
extern double battery_getVoltage(void);
extern double battery_getTemperature(void);
extern double battery_getCapacity(void);
extern double battery_getStateOfCharge(void);
extern double battery_getTimeToFull(void);
extern double battery_getTimeToEmpty(void);
extern double battery_getChargeCycle(void);
extern double battery_getAge(void);
extern void battery_PrintDetails(void);
extern int8_t battery_remaining_non_volatile_updates(void);
extern void battery_gauge_CheckChargeStatus(void);
extern void battery_gauge_CheckLowBat(void);

extern uint8_t battery_low_bat_notification;
extern uint8_t battery_gauge_temperature_controlled_charge_enable;
extern uint8_t battery_charge_enable_notification;
extern uint8_t battery_avoid_multiple_notifications;
extern uint16_t battery_charge_status_delay;
extern uint16_t battery_gauge_temperature_progress_delay;

#endif