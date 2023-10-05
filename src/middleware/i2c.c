/**
 * @file i2c.c
 * @author Thomas Keilbach | keiltronic GmbH
 * @date 05 Oct 2023
 * @brief This file contains functions to communicate with the pheripherals
 * @version 2.0.0
 */

/*!
 * @defgroup Peripherals
 * @brief This file contains functions to communicate with the pheripherals
 * @{*/

/* I2C slaves on board
Addr: 0xXX: Battery gauge
Addr: 0x c: nRF52 ?
Addr: 0x14: LED controller
Addr: 0x20: IO expander
Addr: 0x48: Temp sensor
Addr: 0x68: Accelerometer
*/

#include "i2c.h"

#define I2C2_NODE DT_NODELABEL(bmx160)

struct i2c_dt_spec dev_i2c = I2C_DT_SPEC_GET(I2C2_NODE);

void i2c_init(void)
{
  if (!device_is_ready(dev_i2c.bus))
  {
    printk("I2C bus %s is not ready!\n\r", dev_i2c.bus->name);
    return;
  }
}

// struct device *i2c_dev;
// struct i2c_msg msgs[1];

// void init_i2c(void)
// {
//   /* Init I2C */
//   i2c_dev = device_get_binding("I2C_2");

//   if (!i2c_dev)
//   {
//     if (Parameter.debug)
//     {
//       rtc_print_debug_timestamp();
//       shell_print(shell_backend_uart_get_ptr(), "I2C: Device driver not found");
//     }
//     return 0;
//   }
//   else
//   {
//     i2c_configure(i2c_dev, I2C_SPEED_SET(I2C_SPEED_FAST)); // 400 kHz
//   }
// }

// /*!
//  *  @brief This function search for I2C slave device on bus
//  *  @param[in] print_message: If this is set to true, the total count is printed at console
//  *  @return: Returns the count of found I2C slave devices
//  */
// uint16_t i2c_scanner(uint8_t print_message)
// {
//   uint8_t dst = 1;
//   uint8_t error = 0u;
//   uint8_t slave_count = 0;

//   /* Poll all addresses from 0 to 127 on I2C bus */
//   for (uint8_t i = 0; i <= 0x7F; i++)
//   {
//     /* Send the address to read from */
//     msgs[0].buf = &dst;
//     msgs[0].len = 1U;
//     msgs[0].flags = I2C_MSG_WRITE | I2C_MSG_STOP;

//     error = i2c_transfer(i2c_dev, &msgs[0], 1, i);

//     if (!error)
//     {
//       if (print_message == true)
//       {
//         shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "Addr: 0x%02X FOUND, Chip: ", i);

//         switch (i)
//         {
//         case 0x0B:
//           shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "Battery gauge (Maxim MAX17201G)\n");
//           break;

//         case 0x0C:
//           shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "LED driver (TI LP5009)\n");
//           break;

//         case 0x14:
//           shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "LED driver (TI LP5009)\n");
//           break;

//         case 0x36:
//           shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "Battery gauge (Maxim MAX17201G)\n");
//           break;

//         case 0x68:
//           shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "IMU (Bosch BMI160)\n");
//           break;

//         default:
//           break;
//         }
//       }
//       slave_count++;
//     }
//     else
//     {
//       //   shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "Addr: 0x%2x no slave", i);
//     }
//   }

//   if (print_message == true)
//   {
//     shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "Total I2C devices on pcb: %d. Total device found: %d\n", I2C_SLAVE_COUNT, slave_count);
//   }

//   return slave_count;
// }

// /*!
//  *  @brief This function checks if the I2C bus is functional
//  *  @details To test the bus, it gets scanned for slaves. If there are two known slaves found,
//  *            the bus is considered as working.
//  *  @return: Returns the count of found I2C slave devices
//  */
// uint8_t i2c_test(void)
// {
//   uint8_t imu = false;
//   uint8_t led_driver = false;

//   uint8_t dst = 1;
//   uint8_t error = 0u;

//   /* Poll all addresses from 0 to 127 on I2C bus */
//   for (uint8_t i = 0; i <= 0x7F; i++)
//   {
//     /* Send the address to read from */
//     msgs[0].buf = &dst;
//     msgs[0].len = 1U;
//     msgs[0].flags = I2C_MSG_WRITE | I2C_MSG_STOP;

//     error = i2c_transfer(i2c_dev, &msgs[0], 1, i);

//     if (!error)
//     {
//       switch (i)
//       {
//       case 0x0C: // LED driver TI LP5009
//         led_driver = true;
//         break;

//       case 0x14: // LED driver TI LP5009
//         led_driver = true;
//         break;

//       case 0x68: // IMU Bosch BMI160
//         imu = true;
//         break;

//       default:
//         break;
//       }
//     }
//   }
//   /* Return result */
//   if ((imu == true) && (led_driver == true))
//   {
//     return true;
//   }
//   else
//   {
//     return false;
//   }
// }