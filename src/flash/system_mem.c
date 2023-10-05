/**
 * @file system_mem.c
 * @author Thomas Keilbach | keiltronic GmbH
 * @date 27 Oct 2022
 * @brief This file contains system wide used global variables
 * @version 1.0.0
 */

/*!
 * @defgroup Memory
 * @brief This file contains system wide used global variables
 * @{*/

#include "system_mem.h"

volatile SYSTEM System;

/*!
 * @brief This functions initalize a structure with global variables in RAM.
 */
void System_InitRAM(void)
{
  System.Steps = 0;
  System.StatusInputs = 0;
  System.StatusOutputs = 0;
  System.StatusErrors = 0;
  System.TotalSteps = 0;
  System.ChargeStatus = 0;
  System.ChargeStatus_old = 0;
  System.TemperatureDigit = 0;
  System.Temperature = 0;
  System.ConfID = 0;
  System.Logtime = 0;
  System.TotalMoppingTime = 0;
  System.AvgMoppingSpeed = 0;
  System.TotalSteps = 0;
  System.TotalVMopRFIDs = 0;
  System.TotalRoomRFIDs = 0;
  System.TotalOtherRFIDs = 0;
  System.TotalCoverageHandle = 0;
  System.AvgCoverageMop = 0;
  System.AvgdBmVmops = 0;
  System.AvgdBmWalls = 0;
  System.TotalRooms = 0;
  System.AVGRoomCleanTime = 0;
  System.charger_connected = 0;
  System.charger_connected_old = 0;
  System.StatusInputs_old = 0;
  System.StatusOutputs_old = 0;
  System.StatusErrors_old = 0;
  System.datalog_StartFrame = 0UL;
  System.datalog_EndFrame = 0UL;
  System.RFID_TransparentMode = false;
  System.datalogFrameNumber = 0UL;
  System.MillisecSinceBoot = 0UL;
  System.MillisecSincecharger_connected = 0;
  System.MillisecSinceUSBDisconnected = 0;
  System.RFID_Sniff = 0;
  System.EventNumber = 0;
  System.boot_complete = false;
}

/*!
 * @brief This functions checks if there was a change in system flags. 
 *        If a flag change happened, a datalog frame gets stored in the external memory (event based logging).
 */
void System_CheckStatusFlagChange(void)
{

  if (System.StatusInputs != System.StatusInputs_old ||
      System.StatusOutputs != System.StatusOutputs_old ||
      System.StatusErrors != System.StatusErrors_old)
  {

    // if (datalog_EnableFlag == true && datalog_ReadOutisActive == false)
    // {
    //   datalog_StoreFrame();
    // }
  }

  System.StatusInputs_old = System.StatusInputs;
  System.StatusOutputs_old = System.StatusOutputs;
  System.StatusErrors_old = System.StatusErrors;
}

