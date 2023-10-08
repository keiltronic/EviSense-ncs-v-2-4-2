/**
 * @file imu.c
 * @author Thomas Keilbach | keiltronic GmbH
 * @date 07 Oct 2023
 * @brief This file contains functions to communicate with the pheripherals
 * @version 2.0.0
 */

/*!
 * @defgroup Peripherals
 * @brief This file contains functions to communicate with the pheripherals
 * @{*/

#include "imu.h"

/* function declaration */
struct bmi160_dev bmi160;
struct bmi160_sensor_data accel;
struct bmi160_sensor_data gyro;
struct bmi160_int_settg int_config;
struct bmm150_dev bmm150;

volatile uint8_t imu_IsInitialized = false;
volatile uint8_t trace_imu_flag = false;
volatile uint8_t trace_imu_reduced_flag = false;
volatile uint8_t step_interrupt_triggered = false;
volatile uint8_t motion_detected = false;
volatile uint8_t motion_detected_old = false;
volatile uint32_t motion_reset_counter = 0;
uint8_t mag_data[8] = {0};

struct i2c_dt_spec imu_i2c = I2C_DT_SPEC_GET(BMX160_NODE);
struct gpio_dt_spec imu_int1 = GPIO_DT_SPEC_GET(IMU_INT1_NODE, gpios);
struct gpio_dt_spec imu_int2 = GPIO_DT_SPEC_GET(IMU_INT2_NODE, gpios);
struct gpio_callback imu_int1_cb_data;
struct gpio_callback imu_int2_cb_data;

void imu_init(void)
{
  int16_t ret = 0;

  /* Init int 1 interrup line */
  ret = gpio_pin_interrupt_configure_dt(&imu_int1, GPIO_INT_EDGE_RISING); // Imu int 1
  if (!device_is_ready(imu_int1.port))
  {
    return;
  }

  ret = gpio_pin_configure_dt(&imu_int1, GPIO_INPUT);
  if (ret < 0)
  {
    return;
  }
  gpio_init_callback(&imu_int1_cb_data, imu_int1_cb, BIT(imu_int1.pin));
  gpio_add_callback(imu_int1.port, &imu_int1_cb_data);

  /* Init int 2 interrup line */
  ret = gpio_pin_interrupt_configure_dt(&imu_int2, GPIO_INT_EDGE_FALLING); // Imu int 2
  if (!device_is_ready(imu_int2.port))
  {
    return;
  }

  ret = gpio_pin_configure_dt(&imu_int2, GPIO_INPUT);
  if (ret < 0)
  {
    return;
  }

  gpio_init_callback(&imu_int2_cb_data, imu_int2_cb, BIT(imu_int2.pin));
  gpio_add_callback(imu_int2.port, &imu_int2_cb_data);

  /* Init i2c device */
  if (!device_is_ready(imu_i2c.bus))
  {
    printk("I2C bus %s is not ready!\n\r", imu_i2c.bus->name);
    return;
  }
  k_msleep(100);

  int8_t rslt = BMI160_OK;
  uint8_t bmm150_data_start = BMM150_DATA_X_LSB;

  imu_IsInitialized = false;

  /* Config BMI 160 */
  bmi160.id = BMI160_I2C_ADDR;
  bmi160.interface = BMI160_I2C_INTF;
  bmi160.read = user_bmi160_i2c_read;
  bmi160.write = user_bmi160_i2c_write;
  bmi160.delay_ms = user_delay_ms;

  /* Config BMM150 */
  bmm150.dev_id = BMM150_DEFAULT_I2C_ADDRESS;
  bmm150.intf = BMM150_I2C_INTF;
  bmm150.read = bmm150_aux_read;
  bmm150.write = bmm150_aux_write;
  bmm150.delay_ms = user_delay_ms;

  rslt = bmi160_init(&bmi160);

  if (bmi160.chip_id == BMI160_CHIP_ID)
  {
    if (rslt == BMI160_OK)
    {
      if (Parameter.debug)
      {
        rtc_print_debug_timestamp();
        shell_print(shell_backend_uart_get_ptr(), "BMI160 initialization success");
        shell_print(shell_backend_uart_get_ptr(), "Chip ID 0x%X\n", bmi160.chip_id);
      }
    }
    else
    {
      if (Parameter.debug)
      {
        rtc_print_debug_timestamp();
        shell_error(shell_backend_uart_get_ptr(), "BMI160 initialization failure");
      }
    }

    /* Configure the BMI160's auxiliary interface for the BMM150 */
    bmi160.aux_cfg.aux_sensor_enable = BMI160_ENABLE;
    bmi160.aux_cfg.aux_i2c_addr = BMM150_DEFAULT_I2C_ADDRESS;
    bmi160.aux_cfg.manual_enable = BMI160_ENABLE;            // manual mode
    bmi160.aux_cfg.aux_rd_burst_len = BMI160_AUX_READ_LEN_2; // 2 byte

    rslt = bmi160_aux_init(&bmi160);
    if (rslt == BMI160_OK)
    {
      if (Parameter.debug)
      {
        rtc_print_debug_timestamp();
        shell_error(shell_backend_uart_get_ptr(), "BMI160 aux. interface init success");
      }
    }
    else
    {
      if (Parameter.debug)
      {
        shell_error(shell_backend_uart_get_ptr(), "BMI160 aux. interface init failure");
      }
    }

    rslt = bmm150_init(&bmm150);
    if (rslt == BMI160_OK)
    {
      if (Parameter.debug)
      {
        rtc_print_debug_timestamp();
        shell_error(shell_backend_uart_get_ptr(), "BMM150 initialization success");
      }
    }
    else
    {
      if (Parameter.debug)
      {
        rtc_print_debug_timestamp();
        shell_error(shell_backend_uart_get_ptr(), "BMM150 initialization failure");
      }
    }

    /* Select the Output data rate, range of accelerometer sensor */
    bmi160.accel_cfg.odr = BMI160_ACCEL_ODR_100HZ;
    bmi160.accel_cfg.range = BMI160_ACCEL_RANGE_2G;
    bmi160.accel_cfg.bw = BMI160_ACCEL_BW_NORMAL_AVG4;

    /* Select the power mode of accelerometer sensor */
    bmi160.accel_cfg.power = BMI160_ACCEL_NORMAL_MODE;

    /* Select the Output data rate, range of Gyroscope sensor */
    bmi160.gyro_cfg.odr = BMI160_GYRO_ODR_100HZ;
    bmi160.gyro_cfg.range = BMI160_GYRO_RANGE_2000_DPS;
    bmi160.gyro_cfg.bw = BMI160_GYRO_BW_NORMAL_MODE;

    /* Select the power mode of Gyroscope sensor */
    bmi160.gyro_cfg.power = BMI160_GYRO_NORMAL_MODE;

    /* Set the sensor configuration */
    rslt = BMI160_OK;
    rslt = bmi160_set_sens_conf(&bmi160);

    if (rslt == BMI160_OK)
    {
      if (Parameter.debug)
      {
        rtc_print_debug_timestamp();
        shell_error(shell_backend_uart_get_ptr(), "BMI160 set configuration successful");
      }
    }
    else
    {
      if (Parameter.debug)
      {
        rtc_print_debug_timestamp();
        shell_error(shell_backend_uart_get_ptr(), "BMI160 set configuration failure");
      }
    }

    /*** Step detection interrupt on INT1 ***/

    /* Select the Interrupt channel/pin */
    int_config.int_channel = BMI160_INT_CHANNEL_1; // Interrupt channel/pin 1

    /* Select the Interrupt type */
    int_config.int_type = BMI160_STEP_DETECT_INT; // Choosing Step Detector interrupt

    /* Select the interrupt channel/pin settings */
    int_config.int_pin_settg.output_en = BMI160_ENABLE;         // Enabling interrupt pins to act as output pin
    int_config.int_pin_settg.output_mode = BMI160_DISABLE;      // Choosing push-pull mode for interrupt pin
    int_config.int_pin_settg.output_type = BMI160_ENABLE;       // Choosing active High output
    int_config.int_pin_settg.edge_ctrl = BMI160_ENABLE;         // Choosing edge triggered output
    int_config.int_pin_settg.input_en = BMI160_DISABLE;         // Disabling interrupt pin to act as input
    int_config.int_pin_settg.latch_dur = BMI160_LATCH_DUR_NONE; // non-latched output

    /* Select the Step Detector interrupt parameters, Kindly use the recommended settings for step detector */
    int_config.int_type_cfg.acc_step_detect_int.step_detector_mode = BMI160_STEP_DETECT_NORMAL;
    int_config.int_type_cfg.acc_step_detect_int.step_detector_en = BMI160_ENABLE; // 1-enable, 0-disable the step detector

    /* Set the Step Detector interrupt */
    bmi160_set_int_config(&int_config, &bmi160); /* sensor is an instance of the structure bmi160_dev */

    if (rslt != BMI160_OK)
    {
      if (Parameter.debug)
      {
        rtc_print_debug_timestamp();
        shell_error(shell_backend_uart_get_ptr(), "Unable to config step counter interrupt");
      }
    }
    else
    {
      uint8_t step_enable = 1; // enable the step counter

      rslt = bmi160_set_step_counter(step_enable, &bmi160);

      if (rslt != BMI160_OK)
      {
        if (Parameter.debug)
        {
          rtc_print_debug_timestamp();
          shell_error(shell_backend_uart_get_ptr(), "Unable to activate step counter");
        }
      }
    }

    /*** Any motion interrupt on INT2 ***/

    /* Select the Interrupt channel/pin */
    int_config.int_channel = BMI160_INT_CHANNEL_2; // Interrupt channel/pin 2

    /* Select the Interrupt type */
    int_config.int_type = BMI160_ACC_ANY_MOTION_INT; // Choosing any motion for interrupt

    /* Select the interrupt channel/pin settings */
    int_config.int_type_cfg.acc_any_motion_int.anymotion_en = BMI160_ENABLE;                          // na - 1- Enable the any-motion, 0- disable any-motion
    int_config.int_type_cfg.acc_any_motion_int.anymotion_x = BMI160_ENABLE;                           // na - Enabling x-axis for any motion interrupt - monitor x axis
    int_config.int_type_cfg.acc_any_motion_int.anymotion_y = BMI160_ENABLE;                           // na - Enabling y-axis for any motion interrupt - monitor y axis
    int_config.int_type_cfg.acc_any_motion_int.anymotion_z = BMI160_ENABLE;                           // na - Enabling z-axis for any motion interrupt - monitor z axis
    int_config.int_type_cfg.acc_any_motion_int.anymotion_dur = (uint8_t)Parameter.anymotion_duration; // na - any-motion duration. This is the consecutive datapoints -> see datasheet pg32 section 2.6.1 <int_anym_dur> and pg78
    int_config.int_type_cfg.acc_any_motion_int.anymotion_thr = (uint8_t)Parameter.anymotion_thr;      // na - An interrupt will be generated if the absolute value of two consecutive accelarion signal exceeds the threshold value -> see datasheet pg32 section 2.6.1 <int_anym_th> and pg78 INT_MOTION[1]

    /* Set the Step Detector interrupt */
    bmi160_set_int_config(&int_config, &bmi160); /* sensor is an instance of the structure bmi160_dev */

    if (rslt != BMI160_OK)
    {
      if (Parameter.debug)
      {
        rtc_print_debug_timestamp();
        shell_error(shell_backend_uart_get_ptr(), "Unable to config any motion interrupt");
      }
    }

    /*** Magnetometer Settings ***/

    /* Configure the magnetometer. The regular preset supports up to 60Hz in Forced mode */
    bmm150.settings.preset_mode = BMM150_PRESETMODE_ENHANCED;
    // bmm150.settings.preset_mode = BMM150_PRESETMODE_HIGHACCURACY;
    rslt += bmm150_set_presetmode(&bmm150);

    /* It is important that the last write to the BMM150 sets the forced mode.
     * This is because the BMI160 writes the last value to the auxiliary sensor
     * after every read */
    bmm150.settings.pwr_mode = BMM150_FORCED_MODE;
    rslt += bmm150_set_op_mode(&bmm150);

    bmi160.aux_cfg.aux_odr = BMI160_AUX_ODR_50HZ;
    rslt += bmi160_set_aux_auto_mode(&bmm150_data_start, &bmi160);

    imu_IsInitialized = true;
  }
  else
  {
    imu_IsInitialized = false;
    if (Parameter.debug)
    {
      rtc_print_debug_timestamp();
      shell_error(shell_backend_uart_get_ptr(), "BMI160 device ID does not match");
    }
  }
}

void user_delay_ms(uint32_t period, void *intf_ptr)
{
  /*
   * Return control or wait,
   * for a period amount of milliseconds
   */
  k_msleep(period);
}

int8_t user_bmi160_i2c_read(uint8_t dev_id, uint8_t reg_addr, uint8_t *reg_data, uint16_t len)
{
  /*
   * Data on the bus should be like
   * |------------+---------------------|
   * | I2C action | Data                |
   * |------------+---------------------|
   * | Start      | -                   |
   * | Write      | (reg_addr)          |
   * | Stop       | -                   |
   * | Start      | -                   |
   * | Read       | (reg_data[0])       |
   * | Read       | (....)              |
   * | Read       | (reg_data[len - 1]) |
   * | Stop       | -                   |
   * |------------+---------------------|
   */

  int8_t rslt = 0; /* Return 0 for Success, non-zero for failure */
  rslt = i2c_burst_read_dt(&imu_i2c, reg_addr, reg_data, (uint32_t)len);
  return rslt;
}

int8_t user_bmi160_i2c_write(uint8_t dev_id, uint8_t reg_addr, uint8_t *reg_data, uint16_t len)
{
  /*
   * Data on the bus should be like
   * |------------+---------------------|
   * | I2C action | Data                |
   * |------------+---------------------|
   * | Start      | -                   |
   * | Write      | (reg_addr)          |
   * | Write      | (reg_data[0])       |
   * | Write      | (....)              |
   * | Write      | (reg_data[len - 1]) |
   * | Stop       | -                   |
   * |------------+---------------------|
   */

  int8_t rslt = 1; /* Return 0 for Success, non-zero for failure */
  uint16_t i = 0;

  uint8_t *data = (uint8_t *)calloc((len + 1), sizeof(uint8_t)); // write data has the reg_addr and the data included
  uint8_t *write_pointer;

  write_pointer = data;

  if (data != NULL)
  {
    *write_pointer = reg_addr;

    for (i = 1; i <= len; i++)
    {
      *++write_pointer = *reg_data++;
    }

    rslt = i2c_write_dt(&imu_i2c, data, (uint32_t)(len + 1));

    free(data);
  }
  else
  {
    shell_error(shell_backend_uart_get_ptr(), "I2C write error. Could not allocate memory.");
  }

  return rslt;
}

int8_t bmm150_aux_read(uint8_t id, uint8_t reg_addr, uint8_t *aux_data, uint16_t len)
{
  (void)id; /* id is unused here */

  return bmi160_aux_read(reg_addr, aux_data, len, &bmi160);
}

int8_t bmm150_aux_write(uint8_t id, uint8_t reg_addr, uint8_t *aux_data, uint16_t len)
{
  (void)id; /* id is unused here */

  return bmi160_aux_write(reg_addr, aux_data, len, &bmi160);
}

float acc_lsb_to_ms2(int16_t val)
{
  float g_range = 0.0;

  if (bmi160.accel_cfg.range == BMI160_ACCEL_RANGE_2G)
  {
    g_range = 2.0;
  }
  else if (bmi160.accel_cfg.range == BMI160_ACCEL_RANGE_4G)
  {
    g_range = 4.0;
  }
  else if (bmi160.accel_cfg.range == BMI160_ACCEL_RANGE_8G)
  {
    g_range = 8.0;
  }
  else if (bmi160.accel_cfg.range == BMI160_ACCEL_RANGE_16G)
  {
    g_range = 16.0;
  }
  else
  {
    g_range = 0.0;
  }

  float half_scale = (float)(1 << 16) / 2.0f;

  return (GRAVITY_EARTH * val * g_range / half_scale);
}

float gyro_lsb_to_dps(int16_t val)
{
  float dps_range = 0.0;

  if (bmi160.gyro_cfg.range == BMI160_GYRO_RANGE_2000_DPS)
  {
    dps_range = 2000.0;
  }
  else if (bmi160.gyro_cfg.range == BMI160_GYRO_RANGE_1000_DPS)
  {
    dps_range = 1000.0;
  }
  else if (bmi160.gyro_cfg.range == BMI160_GYRO_RANGE_500_DPS)
  {
    dps_range = 500.0;
  }
  else if (bmi160.gyro_cfg.range == BMI160_GYRO_RANGE_250_DPS)
  {
    dps_range = 250.0;
  }
  else if (bmi160.gyro_cfg.range == BMI160_GYRO_RANGE_125_DPS)
  {
    dps_range = 125.0;
  }
  else
  {
    dps_range = 0.0;
  }

  float half_scale = (float)(1 << 16) / 2.0f;

  return (val * dps_range / half_scale);
}

void imu_fetch_data(void)
{
  int8_t rslt = 0;

  if (imu_IsInitialized)
  {
    /* To read only Accel data */
    rslt = bmi160_get_sensor_data((BMI160_ACCEL_SEL | BMI160_GYRO_SEL | BMI160_TIME_SEL), &accel, &gyro, &bmi160);
    if (rslt != 0)
    {
      if (Parameter.debug)
      {
        rtc_print_debug_timestamp();
        shell_error(shell_backend_uart_get_ptr(), "Could not fetch acceleromter and gyro data");
      }
    }

    /* Reading data from BMI160 data registers */
    rslt = bmi160_read_aux_data_auto_mode(mag_data, &bmi160);
    if (rslt != 0)
    {
      if (Parameter.debug)
      {
        rtc_print_debug_timestamp();
        shell_error(shell_backend_uart_get_ptr(), "Could not set magnetometer auto mode");
      }
    }

    /* Compensating the raw mag data available from the BMM150 API */
    rslt = bmm150_aux_mag_data(mag_data, &bmm150);
    if (rslt != 0)
    {
      if (Parameter.debug)
      {
        rtc_print_debug_timestamp();
        shell_error(shell_backend_uart_get_ptr(), "Could not read magnetometer data");
      }
    }
  }
}

void trace_imu(void)
{
  if (imu_IsInitialized)
  {
    rtc_print_debug_timestamp();

    shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "Sensor time:%8.3fs; Acc x: %4.2f; y: %4.2f; z: %4.2f  m/s^2;\t Gyro x: %4.2f; y: %4.2f; z: %4.2f dps;\t Mag: x: %3d; y: %3d; z: %3d\n",
                  ((float)accel.sensortime * 0.000039),
                  acc_lsb_to_ms2(accel.x),
                  acc_lsb_to_ms2(accel.y),
                  acc_lsb_to_ms2(accel.z),
                  gyro_lsb_to_dps(gyro.x),
                  gyro_lsb_to_dps(gyro.y),
                  gyro_lsb_to_dps(gyro.z),
                  bmm150.data.x,
                  bmm150.data.y,
                  bmm150.data.z);

    // uint8_t index = 0;
    // for (index = 0; index < 8; index++) {
    //   shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_RED, "MAG DATA[%d] : %4d, ", index, mag_data[index]);
    // }
    // shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_RED, "\n");
  }
}

void trace_imu_reduced(void)
{
  if (imu_IsInitialized)
  {
    rtc_print_debug_timestamp();
    shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "%4.2f;%4.2f;%4.2f;%4.2f;%4.2f;%4.2f;%d;%d;%d\n",
                  acc_lsb_to_ms2(accel.x),
                  acc_lsb_to_ms2(accel.y),
                  acc_lsb_to_ms2(accel.z),
                  gyro_lsb_to_dps(gyro.x),
                  gyro_lsb_to_dps(gyro.y),
                  gyro_lsb_to_dps(gyro.z),
                  bmm150.data.x,
                  bmm150.data.y,
                  bmm150.data.z);
  }
}

void motion_detection(void)
{
  /* If a motion was detcted (or mopping started without a previous motion), check to reset motion event (no motion) */
  if (imu_IsInitialized)
  {
    if (motion_detected == true)
    {
      System.StatusInputs |= STATUSFLAG_MD; // Create status entry

      if (motion_reset_counter >= Parameter.motion_reset_time)
      {
        motion_detected = false;
        motion_reset_counter = 0;

        /* For debugging prupose */
        if (Parameter.enable_blue_dev_led == true)
        {
          gpio_pin_set_dt(&dev_led, 1);
        }
        System.StatusInputs &= ~STATUSFLAG_MD; // Delete status entry
      }
      else
      {
        motion_reset_counter++;
      }
    }
    motion_detected_old = motion_detected;
  }
}

int8_t magnet_detection(void)
{
  static uint8_t mag_x_sample_counter = 0;
  static uint8_t mag_y_sample_counter = 0;
  static uint8_t mag_z_sample_counter = 0;

  if (abs(bmm150.data.x) > Parameter.mag_det_threshold)
  {
    if (mag_x_sample_counter < Parameter.mag_det_consecutive_samples)
    {
      mag_x_sample_counter++;
    }
  }
  else
  {
    if (mag_x_sample_counter > 0)
    {
      mag_x_sample_counter--;
    }
  }

  if (abs(bmm150.data.y) > Parameter.mag_det_threshold)
  {
    if (mag_y_sample_counter < Parameter.mag_det_consecutive_samples)
    {
      mag_y_sample_counter++;
    }
  }
  else
  {
    if (mag_y_sample_counter > 0)
    {
      mag_y_sample_counter--;
    }
  }

  if (abs(bmm150.data.z) > Parameter.mag_det_threshold)
  {
    if (mag_z_sample_counter < Parameter.mag_det_consecutive_samples)
    {
      mag_z_sample_counter++;
    }
  }
  else
  {
    if (mag_z_sample_counter > 0)
    {
      mag_z_sample_counter--;
    }
  }

  if ((mag_x_sample_counter == 0) && (mag_y_sample_counter == 0) && (mag_z_sample_counter == 0))
  {
    return false;
  }
  else if ((mag_x_sample_counter == Parameter.mag_det_consecutive_samples) ||
           (mag_y_sample_counter == Parameter.mag_det_consecutive_samples) ||
           (mag_z_sample_counter == Parameter.mag_det_consecutive_samples))
  {
    return true;
  }
  else
  {
    return -1;
  }

  // if ((abs(bmm150.data.x) > 130) || (abs(bmm150.data.z) > 130))
  // {
  //   return true;
  // }
  // else
  // {
  //   return false;
  // }
}

void imu_enter_sleep(void)
{
  int8_t rslt = BMI160_OK;
  uint8_t bmm150_data_start = BMM150_DATA_X_LSB;

  /* Config BMI 160 */
  bmi160.id = BMI160_I2C_ADDR;
  bmi160.interface = BMI160_I2C_INTF;
  bmi160.read = user_bmi160_i2c_read;
  bmi160.write = user_bmi160_i2c_write;
  bmi160.delay_ms = user_delay_ms;

  /* Config BMM150 */
  bmm150.dev_id = BMM150_DEFAULT_I2C_ADDRESS;
  bmm150.intf = BMM150_I2C_INTF;
  bmm150.read = bmm150_aux_read;
  bmm150.write = bmm150_aux_write;
  bmm150.delay_ms = user_delay_ms;

  rslt = bmi160_init(&bmi160);

  if (bmi160.chip_id == BMI160_CHIP_ID)
  {
    /* Configure the BMI160's auxiliary interface for the BMM150 */
    bmi160.aux_cfg.aux_sensor_enable = BMI160_ENABLE;
    bmi160.aux_cfg.aux_i2c_addr = BMM150_DEFAULT_I2C_ADDRESS;
    bmi160.aux_cfg.manual_enable = BMI160_ENABLE;            // manual mode
    bmi160.aux_cfg.aux_rd_burst_len = BMI160_AUX_READ_LEN_2; // 2 byte

    rslt = bmi160_aux_init(&bmi160);
    rslt = bmm150_init(&bmm150);

    /* Select the Output data rate, range of accelerometer sensor */
    bmi160.accel_cfg.odr = BMI160_ACCEL_ODR_100HZ;
    bmi160.accel_cfg.range = BMI160_ACCEL_RANGE_2G;
    bmi160.accel_cfg.bw = BMI160_ACCEL_BW_NORMAL_AVG4;

    /* Select the power mode of accelerometer sensor */
    bmi160.accel_cfg.power = BMI160_ACCEL_LOWPOWER_MODE; // SUSPEND_MODE;

    /* Select the Output data rate, range of Gyroscope sensor */
    bmi160.gyro_cfg.odr = BMI160_GYRO_ODR_100HZ;
    bmi160.gyro_cfg.range = BMI160_GYRO_RANGE_2000_DPS;
    bmi160.gyro_cfg.bw = BMI160_GYRO_BW_NORMAL_MODE;

    /* Select the power mode of Gyroscope sensor */
    bmi160.gyro_cfg.power = BMI160_GYRO_SUSPEND_MODE;

    /* Set the sensor configuration */
    rslt = BMI160_OK;
    rslt = bmi160_set_sens_conf(&bmi160);

    /*** Magnetometer Settings ***/

    /* Configure the magnetometer. The regular preset supports up to 60Hz in Forced mode */
    bmm150.settings.preset_mode = BMM150_SUSPEND_MODE;
    rslt += bmm150_set_presetmode(&bmm150);

    /* It is important that the last write to the BMM150 sets the forced mode.
     * This is because the BMI160 writes the last value to the auxiliary sensor
     * after every read */
    bmm150.settings.pwr_mode = BMM150_SUSPEND_MODE;
    rslt += bmm150_set_op_mode(&bmm150);

    bmi160.aux_cfg.aux_odr = BMI160_AUX_ODR_50HZ;
    rslt += bmi160_set_aux_auto_mode(&bmm150_data_start, &bmi160);
  }
}

/*!
 *  @brief This is the function description
 */
void imu_int1_cb(const struct device *dev, struct gpio_callback *cb, uint32_t pins)
{
  step_interrupt_triggered = true;
  System.TotalSteps++;
  System.Steps++;
}

/*!
 *  @brief This is the function description
 */
void imu_int2_cb(const struct device *dev, struct gpio_callback *cb, uint32_t pins)
{
  motion_detected = true;
  motion_reset_counter = 0;
}
