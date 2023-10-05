/**
 * @file datalog_mem.c
 * @author Thomas Keilbach | keiltronic GmbH
 * @date 28 Sep 2022
 * @brief This file contains function to write and read log data to and from the external flash memory
 * @version 1.0.0
 */

/*!
 * @defgroup Memory
 * @brief This file contains function to write and read log data to and from the external flash memory
 * @{*/

#include "datalog_mem.h"

LOGFRAME DataFrame;

uint8_t datalog_EnableFlag = true;
uint8_t datalog_MemoryFull = false;
uint8_t datalog_EraseActive = false;
uint8_t setaddress_flag = false;
uint8_t datalog_ReadOutisActive = false;
uint32_t flash_logaddress_write = 0UL;

/*!
 * @brief This functions writes a complete data frame (structe) in the external flash.
 * @note This function talks directly over the SPI bus with the external NOR flash memory. For this it uses the API calls defined in flash.c
 * @see flash.c
 */
void datalog_StoreFrame(void)
{
  // if (datalog_ReadOutisActive == false && datalog_MemoryFull == false)
  // {
  //   /* Frame number */
  //   DataFrame.FrameNumber = System.datalogFrameNumber;

  //   /* Serialize unix time (64-bit, in seconds) with additional milli seconds */
  //   DataFrame.unixtime = unixtime;
  //   DataFrame.millisec = millisec;

  //   /* Serialize IMU data */
  //   DataFrame.raw_sens_value[0] = accel.x;
  //   DataFrame.raw_sens_value[1] = accel.y;
  //   DataFrame.raw_sens_value[2] = accel.z;
  //   DataFrame.raw_sens_value[3] = gyro.x;
  //   DataFrame.raw_sens_value[4] = gyro.y;
  //   DataFrame.raw_sens_value[5] = gyro.z;
  //   DataFrame.raw_sens_value[6] = bmm150.data.x;
  //   DataFrame.raw_sens_value[7] = bmm150.data.y;
  //   DataFrame.raw_sens_value[8] = bmm150.data.z;

  //   /* Mop angle data */
  //   DataFrame.floor_handle_angle = floor_handle_angle[0];
  //   DataFrame.frame_handle_angle = frame_handle_angle[0];

  //   /* Mopping data */
  //   DataFrame.Mopping_speed = mopping_speed;
  //   DataFrame.Coverage_per_Mop = mopping_coverage_per_mop;
  //   DataFrame.Mop_cycles = mopcycles;
  //   DataFrame.Mopping_pattern = mopping_pattern[0];
  //   DataFrame.Motion_state = motion_state[0];

  //   /* Device data */
  //   DataFrame.Total_Steps = System.TotalSteps;
  //   DataFrame.Battery_voltage = battery.Voltage;
  //   DataFrame.ChargeCycle = battery.ChargeCycle;

  //   /* Status bits */
  //   DataFrame.Input_status = System.StatusInputs;
  //   DataFrame.Output_status = System.StatusOutputs;
  //   DataFrame.Error_status = System.StatusErrors;

  //   /* Calculate address in flash */
  //   flash_logaddress_write = System.datalogFrameNumber * DATALOG_FRAME_LENGTH;

  //   /* Write data to flash */
  //   if (flash_logaddress_write < DATALOG_MEM_LENGTH)
  //   {
  //     /* Erase sector if the address points to the first element of the sector */
  //    // if (!(DATALOG_MEM + flash_logaddress_write) % FLASH_SECTOR_SIZE)
  //    if (!(DATALOG_MEM + flash_logaddress_write) % FLASH_SUBSUBSECTOR_SIZE) 
  //     {
  //       flash_EraseSector_64kB(GPIO_PIN_FLASH_CS1, DATALOG_MEM + flash_logaddress_write);
  //     }

  //     /* Write new data into flash memory */
  //     flash_write(GPIO_PIN_FLASH_CS1, DATALOG_MEM + flash_logaddress_write, &DataFrame.logging_frame[0], DATALOG_FRAME_LENGTH);
  //     System.datalogFrameNumber++;

  //     /* If verbose mode is active, print current frame to console after it was written to flash */
  //     if (Parameter.datalog_sniffFrame)
  //     {
  //       /* Print line number and unix time in millisec resolution */
  //       rtc_print_debug_timestamp();
  //       shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "%d;%d%d;", DataFrame.FrameNumber, DataFrame.unixtime, DataFrame.millisec);

  //       /* Print latest found EPC tag in HEX representation */
  //       PrintDeserializeHexToChar(&DataFrame.rfid_record_hex[0], 20);

  //       /* Print IMU and mopping data */
  //       shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;",
  //                     DataFrame.raw_sens_value[0],
  //                     DataFrame.raw_sens_value[1],
  //                     DataFrame.raw_sens_value[2],
  //                     DataFrame.raw_sens_value[3],
  //                     DataFrame.raw_sens_value[4],
  //                     DataFrame.raw_sens_value[5],
  //                     DataFrame.raw_sens_value[6],
  //                     DataFrame.raw_sens_value[7],
  //                     DataFrame.raw_sens_value[8]);

  //       shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "%f;%f;",
  //                     DataFrame.floor_handle_angle,
  //                     DataFrame.frame_handle_angle);

  //       shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "%f;%f;%d;%d;%d;%d;%d;%d;",
  //                     DataFrame.Mopping_speed,
  //                     DataFrame.Coverage_per_Mop,
  //                     DataFrame.Mop_cycles,
  //                     DataFrame.Mopping_pattern,
  //                     DataFrame.Motion_state,
  //                     DataFrame.Total_Steps,
  //                     DataFrame.Battery_voltage,
  //                     DataFrame.ChargeCycle);

  //       /* Print device status */
  //       shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "%d;%d;%d\n",
  //                     DataFrame.Input_status,
  //                     DataFrame.Output_status,
  //                     DataFrame.Error_status);
  //     }
  //   }
  //   else
  //   {
  //     /* If verbose mode is active */
  //     datalog_EnableFlag = false;
  //     datalog_MemoryFull = true;

  //     if (Parameter.datalog_sniffFrame || Parameter.debug)
  //     {
  //       rtc_print_debug_timestamp();
  //       shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "Flash memory full.");
  //     }
  //   }
  // }

  // /* Reset device status bits after the current frame was written into datalog memory*/
  // System.StatusInputs &= ~STATUSFLAG_RO;  // Reset status entry
  // System.StatusInputs &= ~STATUSFLAG_RT;  // Reset status entry
  // System.StatusInputs &= ~STATUSFLAG_RM;  // Reset status entry
  // System.StatusInputs &= ~STATUSFLAG_RW;  // Reset status entry
  // System.StatusInputs &= ~STATUSFLAG_HS;  // Reset status entry
  // System.StatusInputs &= ~STATUSFLAG_FS;  // Reset status entry
  // System.StatusInputs &= ~STATUSFLAG_FFF; // Reset status entry
  // System.StatusInputs &= ~STATUSFLAG_B2;  // Reset status entry
  // System.StatusInputs &= ~STATUSFLAG_UB;  // Reset status entry

  // memset(DataFrame.rfid_record_hex, 0, 20);
  // DataFrame.rfid_record_type = 0;
}

/*!
 * @brief This functions reads data frames from the external flash memory and prints it on console.
 * @details: To set the adress to read from, the start and  end frame number has to be set (System.datalog_StartFrame and System.datalog_EndFrame)
 * @note This function talks directly over the SPI bus with the external NOR flash memory. For this it uses the API calls defined in flash.c
 * @see flash.c
 */
void datalog_GetData(void)
{
  // uint32_t currentDataFrame = 0UL;
  // uint32_t lastDataFrame = 0UL;
  // uint16_t byte_pos = DATALOG_FRAME_LENGTH;

  // /* Stop data logging */
  // datalog_EnableFlag = false;

  // /* Caclulate start end end frame address based on user input */
  // if (!setaddress_flag)
  // {
  //   if (System.datalog_StartFrame >= System.datalog_EndFrame)
  //   {
  //     currentDataFrame = System.datalog_StartFrame;
  //     if ((System.datalog_StartFrame - System.datalog_EndFrame) > 0UL)
  //     {
  //       lastDataFrame = System.datalog_EndFrame;
  //     }
  //     else
  //     {
  //       lastDataFrame = 0UL;
  //     }
  //   }
  //   else
  //   {
  //     currentDataFrame = System.datalog_EndFrame;
  //     if ((System.datalog_EndFrame - System.datalog_StartFrame) > 0UL)
  //     {
  //       lastDataFrame = System.datalog_StartFrame;
  //     }
  //     else
  //     {
  //       lastDataFrame = 0UL;
  //     }
  //   }

  //   setaddress_flag = true;
  // }

  // /* Read stored frames from external flash */
  // shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "\r\n");
  // printk("\r\n");

  // while ((datalog_ReadOutisActive == true) && (currentDataFrame > lastDataFrame))
  // {
  //   wdt_reset();

  //   flash_read(GPIO_PIN_FLASH_CS1, DATALOG_MEM + (currentDataFrame * DATALOG_FRAME_LENGTH), &DataFrame.logging_frame[0], DATALOG_FRAME_LENGTH);

  //   /* Print linenumber and unixtime in millisec resolution */
  //   printf("%d;%d%03d;", DataFrame.FrameNumber, DataFrame.unixtime, DataFrame.millisec);

  //   /* Print latest found EPC tag in HEX representation */
  //   PrintDeserializeHexToChar(&DataFrame.rfid_record_hex[0], 20);
  //   printf("%d;", DataFrame.rfid_record_type);

  //   /* Print IMU and mopping data */
  //   printf("%4.2f;%4.2f;%4.2f;%4.2f;%4.2f;%4.2f;%d;%d;%d;",
  //          acc_lsb_to_ms2((int16_t)DataFrame.raw_sens_value[0]),  // Acc x
  //          acc_lsb_to_ms2((int16_t)DataFrame.raw_sens_value[1]),  // Acc y
  //          acc_lsb_to_ms2((int16_t)DataFrame.raw_sens_value[2]),  // Acc z
  //          gyro_lsb_to_dps((int16_t)DataFrame.raw_sens_value[3]), // Gyro x
  //          gyro_lsb_to_dps((int16_t)DataFrame.raw_sens_value[4]), // Gyro y
  //          gyro_lsb_to_dps((int16_t)DataFrame.raw_sens_value[5]), // Gyro z
  //          DataFrame.raw_sens_value[6],                           // Mag x
  //          DataFrame.raw_sens_value[7],                           // Mag y
  //          DataFrame.raw_sens_value[8]);                          // Mag z

  //   printf("%3.2f;%3.2f;", DataFrame.floor_handle_angle, DataFrame.frame_handle_angle);

  //   printf("%2.2f;%2.2f;%d;%d;%d;%d;%d;%d;",
  //          DataFrame.Mopping_speed,
  //          DataFrame.Coverage_per_Mop,
  //          DataFrame.Mop_cycles,
  //          DataFrame.Mopping_pattern,
  //          DataFrame.Motion_state,
  //          DataFrame.Total_Steps,
  //          DataFrame.Battery_voltage,
  //          DataFrame.ChargeCycle);

  //   /* Print device status */
  //   printf("%d;%d;%d\n", DataFrame.Input_status,
  //          DataFrame.Output_status,
  //          DataFrame.Error_status);

  //   if (currentDataFrame != 0UL)
  //   {
  //     currentDataFrame--;
  //   }
  // }

  // setaddress_flag = false;
  // byte_pos = DATALOG_FRAME_LENGTH;
  // datalog_ReadOutisActive = false;
  // currentDataFrame = 0UL;
  // lastDataFrame = 0UL;

  // if (Parameter.datalogEnable == true)
  // {
  //   datalog_EnableFlag = true;
  // }
  // else
  // {
  //   datalog_EnableFlag = false;
  // }

  // shell_print(shell_backend_uart_get_ptr(), "EOF");
}

/*!
 * @brief This functions clears  the data log memory in the external flash and reset the write address.
 * @note This function talks directly over the SPI bus with the external NOR flash memory. For this it uses the API calls defined in flash.c
 * @see flash.c
 */
void datalog_CleardatalogAll(void)
{
  // datalog_EnableFlag = false;
  // datalog_EraseActive = true;
  // flash_ClearMemAll(GPIO_PIN_FLASH_CS1, DATALOG_MEM, DATALOG_MEM_LENGTH);
  // datalog_EraseActive = false;

  // System.TotalSteps = 0;
  // System.Steps = 0;
  // System.datalogFrameNumber = 0UL;

  // if (Parameter.datalogEnable == true)
  // {
  //   datalog_EnableFlag = true;
  // }
  // else
  // {
  //   datalog_EnableFlag = false;
  // }
}