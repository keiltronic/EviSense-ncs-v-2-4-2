/**
 * @file algorithm.c
 * @author Thomas Keilbach (keiltronic GmbH) and Theofanis Lambrou (FHCS GmbH)
 * @date 08 Oct 2023
 * @brief This file contains functions for detecting mopping patterns, mop changes and monitors if the mop is suitable for the location
 * @version 2.3.0
 */

/*!
 * @defgroup Algorithms
 * @brief This file contains functions for detecting mopping patterns, mop changes and monitors if the mop is suitable for the location
 * @{*/
#include "algorithms.h"

// Variables Initialization
// dAccx, dAccy, dAccy, dGyrx,dGyry,dGyrz, dMagx,dMagy, dMagz : raw IMU sensor data
// a vector to keep the last Nf=10 samples (one sample every 50ms)

/* Create variables and arrays */
float Accx_Nfvec[NF];
float Accy_Nfvec[NF];
float Accz_Nfvec[NF];

float Gyrx_Nfvec[NF];
float Gyry_Nfvec[NF];
float Gyrz_Nfvec[NF];

float Magx_Nfvec[NF];
float Magy_Nfvec[NF];
float Magz_Nfvec[NF];

float Acc_dcNfvec[NF];
float Acc_acvec[NF];
float Acc_acfvec[NF];

float Gyr_fx_thr_Nfvec[NF];
float Gyr_fy_thr_Nfvec[NF];
float Gyr_fz_thr_Nfvec[NF];
float Gyrx_acvec[NF];
float Gyry_acvec[NF];

float Acc_acf_to = 0.0;
float Acc_acf_tn = 0.0;
int8_t sAacf_to = 0;
int8_t sAacf_tn = 0;
float Acc_acfvec_avg = 0.0;
float Acc_acf_peak = 0.0;
float Acc_acf_tn_abs = 0.0;
float Acc_acf_sumabs = 0.0;
float Acc_acf_adpt_thr = 0.0;
uint32_t mopcycle_duration_ms = 0UL;
int64_t lastmopcycle_tsp = 0ULL;
int64_t prev_to_lastmopcycle_tsp = 0ULL;
uint8_t handle_in_mopping_position[2] = {0, 0}; // indicates if handle is in mopping position;
uint8_t mopcyclesFlag = 0;
uint8_t MoppingFlag[2] = {0, 0};
uint16_t last_num_of_mopcycles = 0;
float velocity = 0.0;
int64_t Frame_flip_tsp = 0ULL;  // frame flip timestamp
uint8_t Frame_flip_flag = 0;    // flag indicating the a frame flip event.
uint8_t Frame_side[2] = {0, 0}; // indicates frame side

float mopping_coverage_per_handle_total = 0.0;
float mopping_coverage_per_mop = 0.0;
float mopping_coverage_side1 = 0.0;
float mopping_coverage_side2 = 0.0;
int64_t Mopping_start_tsp = 0LL;
int64_t Mopping_stop_tsp = 0LL;
uint8_t motion_state[2] = {0, 0};

float dAccx = 0.0;
float dAccy = 0.0;
float dAccz = 0.0;
float dGyrx = 0.0;
float dGyry = 0.0;
float dGyrz = 0.0;
int16_t dMagx = 0.0;
int16_t dMagy = 0.0;
int16_t dMagz = 0.0;

float Accx_f = 0.0;
float Accy_f = 0.0;
float Accz_f = 0.0;
float Gyrx_f = 0.0;
float Gyry_f = 0.0;
float Gyrz_f = 0.0;
float Magx_f = 0.0;
float Magy_f = 0.0;
float Magz_f = 0.0;

float floor_handle_angle[2] = {0.0, 0.0};
float frame_handle_angle[2] = {0.0, 0.0};

int8_t TPflag = -1;
uint16_t mopcycles = 0;
int64_t Acc_acf_min_lastupdate_tsp = 0LL;

uint8_t frame_lift_flag[2] = {1, 1};
int64_t frame_lift_tsp = 0LL;
uint8_t MopChangeFlag[2] = {0, 0};
int64_t Mop_change_tsp = 0LL;
uint8_t Mop_on_floor_after_Change_Flag = 0;
int64_t Mop_on_floor_after_Change_Flag_tsp = 0LL;

char Currentmop_RFID[EPC_TOTAL_HEX_LENGTH];
char Newmop_RFID[EPC_TOTAL_HEX_LENGTH];
uint8_t Newmop_RFID_flag = 0;
uint8_t Prevmop_RFID_replaced_flag = 0;
int64_t Newmop_RFID_tsp = 0LL;
uint32_t Newmop_RFID_readings = 0UL; // signed long int
uint16_t prev_mop_id = 0UL;
uint16_t current_mop_sides = 0UL;

uint32_t mop_rfid_readings = 0UL;
uint32_t mop_null_readings = 0UL;
uint8_t Currentmop_iswithout_RFID_Flag = 0;
uint8_t Mop_added_inlastseenmoparray_Flag = 0;

float Acc_dc_smooth = EARTH_GRAVITY;
uint8_t Free_Fall_flag = 0;
uint8_t Hit_Shock_flag = 0;
int64_t freefall_tsp = 0LL;
int64_t Hit_Shock_tsp = 0LL;
float Hit_Shock_mag = 0.0;
float fall_duration_ms = 0;
float free_fall_height = 0;

float Gyr_fx = 0.0;
float Gyr_fx_abs = 0.0;
float Gyr_fx_peak = 0.0;
int64_t Gyr_fx_peak_lastupdate_tsp = 0LL;
float Gyrx_adpt_thr = 0.0;
float Gyr_fx_thr = 0.0;
float Gyr_fx_thr_ergy = 0.0;
float Gyr_fy = 0.0;
float Gyr_fy_abs = 0.0;
float Gyr_fy_peak = 0.0;
int64_t Gyr_fy_peak_lastupdate_tsp = 0LL;
float Gyry_adpt_thr = 0.0;
float Gyr_fy_thr = 0.0;
float Gyr_fy_thr_ergy = 0.0;
float Gyr_fz = 0.0;
float Gyr_fz_abs = 0.0;
float Gyr_fz_peak = 0.0;
int64_t Gyr_fz_peak_lastupdate_tsp = 0LL;
float Gyrz_adpt_thr = 0.0;
float Gyr_fz_thr = 0.0;
float Gyr_fz_thr_ergy = 0.0;

uint8_t mopping_pattern[2] = {0, 0};
int64_t mopping_pattern_lastupdate_tsp = 0LL;
int64_t mopping_speed_lastupdate_tsp = 0LL;
float mopping_speed = 0.0;
uint32_t Total_mops_used = 0UL;
float Coverage_per_mop = 0.0;
float dt = 0.0;

float Acc_dc_avg = 0.0;
double result = 0.0;
float Acc_adpt_thr = 0.0;
int64_t Acc_acf_peak_lastupdate_tsp = 0LL;

uint8_t flag_changed = false;
uint8_t coverage_print_flag = false;
uint8_t unchipped_mop_coverage_reset_flag = false;
int64_t algo_flag_verbose_upd_tsp = 0LL;

float Acc_dc_avg_min = 0.0;
int64_t Acc_dc_avg_min_lastupdate_tsp = 0LL;

float Gyrx_acf = 0.0;
float Gyry_acf = 0.0;

uint8_t Mopping_motion_gyr_flag = 0;
int64_t Mopping_motion_gyr_flag_lastupdate_tsp = 0LL;

float gyr_snx = 0;
float gyr_sox = 0;
int8_t gTPflagx = 1;
int8_t gyr_trnx = 0;
int8_t gyr_trox = 0;
float gcycle_anglex = 0;
uint8_t gyr_activityFlagx = 0;
int64_t gyr_zerocross_lastupdate_tspx = 0LL;
int64_t lastgcycle_tspx = 0LL;
int64_t gcycle_duration_msx = 0LL;
uint8_t Mopping_motion_gyr_flagx = 0;

float gyr_sny = 0;
float gyr_soy = 0;
int8_t gTPflagy = 1;
int8_t gyr_trny = 0;
int8_t gyr_troy = 0;
float gcycle_angley = 0;
uint8_t gyr_activityFlagy = 0;
int64_t gyr_zerocross_lastupdate_tspy = 0LL;
int64_t lastgcycle_tspy = 0LL;
int64_t gcycle_duration_msy = 0LL;
uint8_t Mopping_motion_gyr_flagy = 0;
uint8_t mob_max_sqm_reached = false;
uint8_t mob_max_side1_sqm_reached = false;
uint8_t mob_max_side2_sqm_reached = false;

uint32_t time_in_idle_state = 0;
uint32_t time_in_moving_state = 0;
uint32_t time_in_mopping_state = 0;

double idle_time_in_percentage = 100.0;
double moving_time_in_percentage = 0.0;
double mopping_time_in_percentage = 0.0;

uint8_t mop_change_status = MOP_RESET_STATE;
uint8_t Flag_SameMopAlreadyUsedNotification = false;
uint8_t dirtymop_blink_flag = false;
uint8_t chipped_mop_installed = false;

uint32_t mop_id_refresh_timer = 0;

uint8_t algorithm_lock = true;

// /*!
//  * @brief This functions initalize all variables related the algorithm
//  */
// void init_algorithms(void)
// {
//   memset(Accx_Nfvec, 0, NF);
//   memset(Accy_Nfvec, 0, NF);
//   memset(Accz_Nfvec, 0, NF);

//   memset(Gyrx_Nfvec, 0, NF);
//   memset(Gyry_Nfvec, 0, NF);
//   memset(Gyrz_Nfvec, 0, NF);

//   memset(Magx_Nfvec, 0, NF);
//   memset(Magy_Nfvec, 0, NF);
//   memset(Magz_Nfvec, 0, NF);

//   memset(Acc_dcNfvec, EARTH_GRAVITY, NF);
//   memset(Acc_acvec, 0, NF);
//   memset(Acc_acfvec, 0, NF);

//   memset(Gyr_fx_thr_Nfvec, 0, NF);
//   memset(Gyr_fy_thr_Nfvec, 0, NF);
//   memset(Gyr_fz_thr_Nfvec, 0, NF);
//   memset(Gyrx_acvec, 0, NF);
//   memset(Gyry_acvec, 0, NF);

//   memset(Currentmop_RFID, '0', EPC_TOTAL_HEX_LENGTH);
//   memset(Newmop_RFID, '0', EPC_TOTAL_HEX_LENGTH);

//   dt = (float)Parameter.imu_interval / 1000.0; // Raw IMU data sampling time in sec
// }

// /*!
//  * @brief This functions resets all variables to their default value
//  */
// void algo_reset_variables(void)
// {
//   memset(Accx_Nfvec, 0, NF);
//   memset(Accy_Nfvec, 0, NF);
//   memset(Accz_Nfvec, 0, NF);

//   memset(Gyrx_Nfvec, 0, NF);
//   memset(Gyry_Nfvec, 0, NF);
//   memset(Gyrz_Nfvec, 0, NF);

//   memset(Magx_Nfvec, 0, NF);
//   memset(Magy_Nfvec, 0, NF);
//   memset(Magz_Nfvec, 0, NF);

//   memset(Acc_dcNfvec, EARTH_GRAVITY, NF);
//   memset(Acc_acvec, 0, NF);
//   memset(Acc_acfvec, 0, NF);

//   memset(Gyr_fx_thr_Nfvec, 0, NF);
//   memset(Gyr_fy_thr_Nfvec, 0, NF);
//   memset(Gyr_fz_thr_Nfvec, 0, NF);
//   memset(Gyrx_acvec, 0, NF);
//   memset(Gyry_acvec, 0, NF);

//   memset(Currentmop_RFID, '0', EPC_TOTAL_HEX_LENGTH);
//   memset(Newmop_RFID, '0', EPC_TOTAL_HEX_LENGTH);

//   Acc_acf_to = 0.0;
//   Acc_acf_tn = 0.0;
//   sAacf_to = 0;
//   sAacf_tn = 0;
//   Acc_acfvec_avg = 0.0;
//   Acc_acf_peak = 0.0;
//   Acc_acf_tn_abs = 0.0;
//   Acc_acf_sumabs = 0.0;
//   Acc_acf_adpt_thr = 0.0;
//   mopcycle_duration_ms = 0UL;
//   lastmopcycle_tsp = 0ULL;
//   prev_to_lastmopcycle_tsp = 0ULL;
//   handle_in_mopping_position[0] = 0; // indicates if handle is in mopping position;
//   handle_in_mopping_position[1] = 0; // indicates if handle is in mopping position;
//   mopcyclesFlag = 0;
//   MoppingFlag[0] = 0;
//   MoppingFlag[1] = 0;
//   last_num_of_mopcycles = 0;
//   velocity = 0.0;
//   Frame_flip_tsp = 0ULL; // frame flip timestamp
//   Frame_flip_flag = 0;   // flag indicating the a frame flip event.
//   Frame_side[0] = 0;     // indicates frame side
//   Frame_side[1] = 0;     // indicates frame side

//   mopping_coverage_per_handle_total = 0.0;
//   mopping_coverage_per_mop = 0.0;
//   mopping_coverage_side1 = 0.0;
//   mopping_coverage_side2 = 0.0;
//   Mopping_start_tsp = 0LL;
//   Mopping_stop_tsp = 0LL;
//   motion_state[0] = 0;
//   motion_state[1] = 0;

//   dAccx = 0.0;
//   dAccy = 0.0;
//   dAccz = 0.0;
//   dGyrx = 0.0;
//   dGyry = 0.0;
//   dGyrz = 0.0;
//   dMagx = 0.0;
//   dMagy = 0.0;
//   dMagz = 0.0;

//   Accx_f = 0.0;
//   Accy_f = 0.0;
//   Accz_f = 0.0;
//   Gyrx_f = 0.0;
//   Gyry_f = 0.0;
//   Gyrz_f = 0.0;
//   Magx_f = 0.0;
//   Magy_f = 0.0;
//   Magz_f = 0.0;

//   floor_handle_angle[0] = 0.0;
//   floor_handle_angle[1] = 0.0;

//   frame_handle_angle[0] = 0.0;
//   frame_handle_angle[1] = 0.0;

//   TPflag = -1;
//   mopcycles = 0;
//   Acc_acf_min_lastupdate_tsp = 0LL;

//   frame_lift_flag[0] = 0;
//   frame_lift_flag[1] = 0;
//   frame_lift_tsp = 0LL;
//   MopChangeFlag[0] = 0;
//   MopChangeFlag[1] = 0;
//   Mop_change_tsp = 0LL;
//   Mop_on_floor_after_Change_Flag = 0;
//   Mop_on_floor_after_Change_Flag_tsp = 0LL;

//   Newmop_RFID_flag = 0;
//   Prevmop_RFID_replaced_flag = 0;
//   Newmop_RFID_tsp = 0LL;
//   Newmop_RFID_readings = 0UL; // signed long int
//   prev_mop_id = 0UL;
//   current_mop_sides = 2;
//   mop_rfid_readings = 0UL;
//   mop_null_readings = 0UL;
//   Currentmop_iswithout_RFID_Flag = 0;
//   Mop_added_inlastseenmoparray_Flag = 0;

//   Acc_dc_smooth = EARTH_GRAVITY;
//   Free_Fall_flag = 0;
//   Hit_Shock_flag = 0;
//   freefall_tsp = 0LL;
//   Hit_Shock_tsp = 0LL;
//   Hit_Shock_mag = 0.0;
//   fall_duration_ms = 0;
//   free_fall_height = 0;

//   Gyr_fx = 0.0;
//   Gyr_fx_abs = 0.0;
//   Gyr_fx_peak = 0.0;
//   Gyr_fx_peak_lastupdate_tsp = 0LL;
//   Gyrx_adpt_thr = 0.0;
//   Gyr_fx_thr = 0.0;
//   Gyr_fx_thr_ergy = 0.0;
//   Gyr_fy = 0.0;
//   Gyr_fy_abs = 0.0;
//   Gyr_fy_peak = 0.0;
//   Gyr_fy_peak_lastupdate_tsp = 0LL;
//   Gyry_adpt_thr = 0.0;
//   Gyr_fy_thr = 0.0;
//   Gyr_fy_thr_ergy = 0.0;
//   Gyr_fz = 0.0;
//   Gyr_fz_abs = 0.0;
//   Gyr_fz_peak = 0.0;
//   Gyr_fz_peak_lastupdate_tsp = 0LL;
//   Gyrz_adpt_thr = 0.0;
//   Gyr_fz_thr = 0.0;
//   Gyr_fz_thr_ergy = 0.0;

//   mopping_pattern[0] = 0;
//   mopping_pattern[1] = 0;
//   mopping_pattern_lastupdate_tsp = 0LL;
//   mopping_speed_lastupdate_tsp = 0LL;
//   mopping_speed = 0.0;
//   Total_mops_used = 0UL;
//   Coverage_per_mop = 0.0;
//   dt = (float)Parameter.imu_interval / 1000.0; // Raw IMU data sampling time in sec

//   Acc_dc_avg = 0.0;
//   result = 0.0;
//   Acc_adpt_thr = 0.0;
//   Acc_acf_peak_lastupdate_tsp = 0LL;

//   flag_changed = false;
//   coverage_print_flag = false;
//   unchipped_mop_coverage_reset_flag = false;
//   algo_flag_verbose_upd_tsp = 0LL;
//   Flag_SameMopAlreadyUsedNotification = false;
//   dirtymop_blink_flag = false;

//   Acc_dc_avg_min = 0.0;
//   Acc_dc_avg_min_lastupdate_tsp = 0LL;

//   Gyrx_acf = 0.0;
//   Gyry_acf = 0.0;

//   Mopping_motion_gyr_flag = 0;
//   Mopping_motion_gyr_flag_lastupdate_tsp = 0LL;

//   gyr_snx = 0;
//   gyr_sox = 0;
//   gTPflagx = 1;
//   gyr_trnx = 0;
//   gyr_trox = 0;
//   gcycle_anglex = 0;
//   gyr_activityFlagx = 0;
//   gyr_zerocross_lastupdate_tspx = 0LL;
//   lastgcycle_tspx = 0LL;
//   gcycle_duration_msx = 0LL;
//   Mopping_motion_gyr_flagx = 0;

//   gyr_sny = 0;
//   gyr_soy = 0;
//   gTPflagy = 1;
//   gyr_trny = 0;
//   gyr_troy = 0;
//   gcycle_angley = 0;
//   gyr_activityFlagy = 0;
//   gyr_zerocross_lastupdate_tspy = 0LL;
//   lastgcycle_tspy = 0LL;
//   gcycle_duration_msy = 0LL;
//   Mopping_motion_gyr_flagy = 0;

//   mob_max_side1_sqm_reached = false;
//   mob_max_side2_sqm_reached = false;
//   mob_max_sqm_reached = false;

//   time_in_idle_state = 0;
//   time_in_moving_state = 0;
//   time_in_mopping_state = 0;

//   idle_time_in_percentage = 100.0;
//   moving_time_in_percentage = 0.0;
//   mopping_time_in_percentage = 0.0;
// }

// /*!
//  * @brief Sums up all elements in an array and checks, if it is zero
//  * @param[in] array: Pointer to array
//  * @param[in] len: Length of array
//  * @return 0 if the sum of each array element is 0, otherwise 1
//  */
// uint8_t zero_sum_of_digits(uint8_t *array, uint16_t len)
// {
//   uint16_t i = 0;
//   uint32_t sum = 0;

//   for (i = 0; i < len; i++)
//   {
//     sum += array[i];
//   }

//   if (sum == 0)
//   {
//     return 0;
//   }
//   else
//   {
//     return 1;
//   }
// }

// /*!
//  * @brief This function is used to update average mop usage in for idle, moving, mopping
//  */
// void update_average_usage_numbers(void)
// {
//   if (event1statistics_interval_timer > 0) // unit: seconds, avoid division by zero
//   {
//     idle_time_in_percentage = (100.0 / (double)event1statistics_interval_timer) * (double)time_in_idle_state;       // time_in_xx has IMU interval as base unit (50ms)
//     moving_time_in_percentage = (100.0 / (double)event1statistics_interval_timer) * (double)time_in_moving_state;   // time_in_xx has IMU interval as base unit (50ms)
//     mopping_time_in_percentage = (100.0 / (double)event1statistics_interval_timer) * (double)time_in_mopping_state; // time_in_xx has IMU interval as base unit (50ms)

//     /* limit values to 100.0 percent*/
//     if (idle_time_in_percentage > 100.0)
//     {
//       idle_time_in_percentage = 100.0;
//     }

//     if (moving_time_in_percentage > 100.0)
//     {
//       moving_time_in_percentage = 100.0;
//     }

//     if (mopping_time_in_percentage > 100.0)
//     {
//       mopping_time_in_percentage = 100.0;
//     }
//   }
//   else
//   {
//     idle_time_in_percentage = 100.0;
//   }
// }

// // This function takes an array and calculates the standard derivation of the array
// // and returns the standard derivation as a float value
// float standard_deviation(float *array, int length)
// {
//   float sum = 0.0, mean, standardDeviation = 0.0;
//   int i;

//   for (i = 0; i < length; ++i)
//   {
//     sum += array[i];
//   }

//   mean = sum / length;

//   for (i = 0; i < length; ++i)
//   {
//     standardDeviation += pow(array[i] - mean, 2);
//   }

//   return sqrt(standardDeviation / length);
// }

// /*!
//  * @brief Adds a float element at array index 0 in a float array
//  * @param[in] array: Pointer to the float array
//  * @param[in] sample: Value to be added
//  * @param[in] array_length: Length of the array
//  */
// void algo_fAddArrayElement(float *array, float sample, uint16_t array_length)
// {
//   uint16_t i = 0;

//   /* Shift every element by one */
//   for (i = array_length - 1; i > 0; i--)
//   {
//     array[i] = array[i - 1];
//   }

//   /* Add new element */
//   array[0] = sample;
// }

// /*!
//  * @brief Adds a float element at array index 0 in a float array
//  * @param[in] array: Pointer to the float array
//  * @param[in] array_length: Length of the array
//  * @return Sum of each array element
//  */
// float algo_fsum(float *array, uint16_t array_length)
// {
//   uint16_t i = 0;
//   float sum = 0.0;

//   for (i = 0; i < array_length; i++)
//   {
//     sum += array[i];
//   }

//   return sum;
// }

// /*!
//  * @brief Calculates the mean of an float array
//  * @param[in] array: Pointer to the float array
//  * @param[in] array_length: Length of the array
//  * @return Mean value of the float array
//  */
// float algo_fmean(float *array, uint16_t array_length)
// {
//   uint16_t i = 0;
//   float sum = 0.0;
//   float mean = 0.0;

//   for (i = 0; i < array_length; i++)
//   {
//     sum += array[i];
//   }

//   mean = sum / array_length;
//   return mean;
// }

// /*!
//  * @brief Function to progress/trigger the algorithm
//  */
// void algorithm_execute_process(void)
// {
//   int8_t cmp_rslt = 0;

//   if (((System.charger_connected == false) || (Parameter.notifications_while_usb_connected == true)) && (algorithm_lock == false) && (System.boot_complete == true))
//   {

//     update_average_usage_numbers();

//     /* Event0x01 auto generation timer */
//     if (event1statistics_interval_timer >= (Parameter.event1statistics_interval * (1000 / Parameter.imu_interval)))
//     {

//       NewEvent0x01(event1statistics_interval_timer, unixtime_ms, (float)idle_time_in_percentage, (float)moving_time_in_percentage, (float)mopping_time_in_percentage);
//       event1statistics_interval_timer = 0;

//       time_in_idle_state = 0;
//       time_in_moving_state = 0;
//       time_in_mopping_state = 0;
//     }
//     else
//     {
//       event1statistics_interval_timer++;
//     }

//     /********************** Intelligent RFID module activation **************************************************************************/
//     if (System.RFID_TransparentMode == false && RFID_autoscan_enabled == false && Parameter.rfid_autoscan == false && Parameter.rfid_disable == false)
//     {
//       if (motion_detected == true)
//       {
//         if (RFID_IsOn == false)
//         {
//           // config_RFID();
//           RFID_TurnOn();
//           k_msleep(5);
//           RFID_ScanEnable = true; // Starts sending EPC multitag command to rfid module in given intervall

//           /* For debugging prupose enable blue dev led when motion detected*/
//           if ((Parameter.enable_blue_dev_led == true))
//           {
//             gpio_pin_set_raw(gpio_dev, GPIO_PIN_LED1, 0);
//           }

//           if (Parameter.debug == true || Parameter.algo_verbose == true)
//           {
//             if (pcb_test_is_running == false)
//             {
//               rtc_print_debug_timestamp();
//               shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "Started triggering rfid module\n");
//             }
//           }
//         }
//       }
//       else
//       {
//         /* For debugging prupose disable blue dev led when motion detected*/
//         if (Parameter.enable_blue_dev_led == true)
//         {
//           gpio_pin_set_raw(gpio_dev, GPIO_PIN_LED1, 1);
//         }

//         if (RFID_IsOn == true)
//         {
//           /* Switch off delay */
//           RFID_TurnOff();          // Turns 5V boost converter off
//           RFID_ScanEnable = false; // Stops sending EPC multitag command to rfid module
//           epc_session_tag_counter = 0;

//           if (Parameter.debug == true || Parameter.algo_verbose == true)
//           {
//             if (pcb_test_is_running == false)
//             {
//               rtc_print_debug_timestamp();
//               shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "Stopped triggering rfid module\n");
//             }
//           }
//         }
//       }
//     }

//     /*************** Get Mop RFIDs EPCs, counters to check if a mopping is done with chipped or unchipped mops) ********************************/
//     if (RFID_TriggeredRead == true)
//     {
//       // Check if current mob string is not zero (a chipped mob is attached)
//       uint8_t chipped_mob_installed = false;
//       chipped_mob_installed = zero_sum_of_digits(current_mob_string_reading, EPC_TOTAL_HEX_LENGTH); // return 0 if sum is 0, otherwise 1

//       if ((chipped_mob_installed == 0) && (mopping_coverage_per_mop > (Parameter.mopping_coverage_per_mop_thr + 1.5)) && (Mopping_motion_gyr_flag == 1) && (Newmop_RFID_tsp < Mopping_start_tsp))
//       {
//         mop_null_readings++;
//         Newmop_RFID_flag = 0;

//         if ((mop_null_readings == (mop_rfid_readings + Parameter.mop_rfid_detection_thr)) && (unchipped_mop_coverage_reset_flag == false))
//         {
//           if (Parameter.debug == true || Parameter.algo_verbose == true)
//           {
//             if (pcb_test_is_running == false)
//             {
//               rtc_print_debug_timestamp();
//               shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "Unchipped mop coverage reset\n");
//             }
//           }
//             if ((Total_mops_used >= 1) && (mopping_coverage_per_mop > 0.5))  // Send the square meter only if there was a previous mop installed
//             {
//               current_room_to_mop_mapping.previous_mop_id = prev_mop_id;
//               NewEvent0x1C(current_room_to_mop_mapping.previous_mop_id, mopping_coverage_side1, mopping_coverage_side2);
//             }
//           mopping_coverage_per_mop = 0;
//           mopping_coverage_side1 = 0;
//           mopping_coverage_side2 = 0;
//           unchipped_mop_coverage_reset_flag = true;
//           reset_room_to_mop_mapping();
//         }
//       }

//       if (chipped_mob_installed == true)
//       {
//         mop_null_readings = 0UL;
//         mop_rfid_readings++;
//       }

//       /* checks if a new chipped mop is installed (is executed only when the device scans a valid mop epc and when this new scan is different from the past one) */
//       if (chipped_mob_installed == true)
//       {
//         cmp_rslt = memcmp(current_mob_string_reading, Newmop_RFID, EPC_TOTAL_HEX_LENGTH);

//         if (cmp_rslt == 0)
//         { // strcmp returns 0 if strings are equal; >0 is str1>str2 and <0 if str1<str2
//           mop_null_readings = 0UL;
//           Newmop_RFID_tsp = unixtime_ms;
//           Newmop_RFID_readings++; // make it signed long int to avoid overflows
//           //rtc_print_debug_timestamp(); shell_fprintf(shell_backend_uart_get_ptr(), 0, "NewmopRFID_readings=%d, Currentmop_iswithout_RFID_Flag=%d, chipped_mob_installed=%d, Reg_mopID=%d - LastmopID=%d\t mop_null_rds=%d, mop_rfid_rds=%d\n", Newmop_RFID_readings, Currentmop_iswithout_RFID_Flag, chipped_mob_installed, prev_mop_id, new_advanced_mop_record.mop_id, mop_null_readings, mop_rfid_readings);
         
//           if ((Newmop_RFID_readings == Parameter.mop_rfid_detection_thr) && (Newmop_RFID_flag == 0) && (prev_mop_id != new_advanced_mop_record.mop_id))
//           {
//             memcpy(Currentmop_RFID, Newmop_RFID, EPC_TOTAL_HEX_LENGTH); // new mop rfid reliably detected
//             Newmop_RFID_flag = 1;                                       // new mop detected
//             Prevmop_RFID_replaced_flag = 1;
//             mop_rfid_readings = Parameter.mop_rfid_detection_thr;
//             Flag_SameMopAlreadyUsedNotification = false;
//             // Notification.next_state = NOTIFICATION_CLEAR;  //TLU 10.05.23
//             //  mopping_coverage_per_mop = 0; mopping_coverage_side1 = 0;  mopping_coverage_side2 = 0;
//             //=============================================
//             /****************** MOP DETECTION ALGORITHM  *************************/
//             if ((Newmop_RFID_flag == 1) && (Prevmop_RFID_replaced_flag == 1))
//             {
//               Total_mops_used++;
//               Mop_on_floor_after_Change_Flag = 0;
//               Prevmop_RFID_replaced_flag = 0;
//               current_room_to_mop_mapping.previous_mop_id = prev_mop_id;
//                  if ((Total_mops_used > 1) && (mopping_coverage_per_mop > 0))  // Send the square meter only if there was a previous mop installed
//                   {
//                     NewEvent0x1C(current_room_to_mop_mapping.previous_mop_id, mopping_coverage_side1, mopping_coverage_side2);
//                   }
//               prev_mop_id = new_mop_record.mop_id;
//               current_mop_sides = new_mop_record.mop_sides;
//               uint8_t mop_chkrslt = 0;
//               /* Check if new mop was already used in current shift (in the last 12 hours) */
//               if (Parameter.current_shift_mop_check == true)
//               {
//                 mop_chkrslt = Mop_CheckIDInLastSeenArray(new_advanced_mop_record);
//               }
//               if (mop_chkrslt == 0) // mop is not listed in the last seen array yet
//               {
//                 /* Save current mop as new mop */
//                 if (new_mop_record.mop_id != 0)
//                 {
//                   /* Reset room to mop mapping */
//                   reset_room_to_mop_mapping();

//                   /* Set current mop as new mop in room_to_mop_mapping */
//                   memcpy(current_room_to_mop_mapping.current_mop_epc, Currentmop_RFID, 20);
//                   current_room_to_mop_mapping.current_mop_id = new_mop_record.mop_id;
//                   current_room_to_mop_mapping.timestamp = unixtime_ms;
//                   Flag_SameMopAlreadyUsedNotification = false;
//                   Mop_added_inlastseenmoparray_Flag = false;

//                   mop_installed = true;
//                   mob_max_side1_sqm_reached = false;
//                   mob_max_side2_sqm_reached = false;
//                   mob_max_sqm_reached = false;

//                   /* Create event for cloud and user notification (this is triggered within the event) */
//                   mop_change_status = NEW_CHIPPED_MOP;
//                   chipped_mop_installed = true;

//                   mop_change_status = MOP_RESET_STATE; // reset flag
//                   NewEvent0x02(current_room_to_mop_mapping.current_mop_id);
//                   mopping_coverage_per_mop = 0;
//                   mopping_coverage_side1 = 0;
//                   mopping_coverage_side2 = 0;

//                   /* Print debug info in screen */
//                   if (Parameter.debug == true || Parameter.algo_verbose == true)
//                   {
//                     if (pcb_test_is_running == false)
//                     {
//                       rtc_print_debug_timestamp();
//                       shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_GREEN, "%d New chipped Mop is placed on Frame. New mop ID: %d \t[mop_color: %X, mop_type: %X, mop_size: %d, mop_sides: %d]\n", Total_mops_used, current_room_to_mop_mapping.current_mop_id, new_advanced_mop_record.mop_color, new_advanced_mop_record.mop_typegroup, new_advanced_mop_record.mop_size, new_advanced_mop_record.mop_sides);
//                       rtc_print_debug_timestamp();
//                       shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "Mop EPC: %s\n", DeserializeHexToChar(Currentmop_RFID, 20));
//                     }
//                   }
//                 }
//               }
//               else
//               {
//                 Notification.next_state = NOTIFICATION_SAME_MOP_ALREADY_USED;
//                 Flag_SameMopAlreadyUsedNotification = true;
//                 mopping_coverage_per_mop = 0;
//                 mopping_coverage_side1 = 0;
//                 mopping_coverage_side2 = 0;
//                 mob_max_side1_sqm_reached = false;
//                 mob_max_side2_sqm_reached = false;
//                 mob_max_sqm_reached = false;

//                 /* Print debug info in screen */
//                 if (Parameter.debug == true || Parameter.algo_verbose == true)
//                 {
//                   if (pcb_test_is_running == false)
//                   {
//                     rtc_print_debug_timestamp();
//                     shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_RED, "%d Mop already used in the current shift, Mop ID: %d\n", Total_mops_used, new_advanced_mop_record.mop_id);
//                     rtc_print_debug_timestamp();
//                     shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "Mop EPC: %s\n", DeserializeHexToChar(Currentmop_RFID, 20));
//                   }
//                 }
//               }
//             }

//             //============================================

//             if (Parameter.debug == true || Parameter.algo_verbose == true)
//             {
//               if (pcb_test_is_running == false)
//               {
//                 rtc_print_debug_timestamp();
//                 shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_BLUE, "algorithm.c: New mop rfid reliably detected\n");
//                 buzzer.status = BUZZER_BEEP;
//                 buzzer.frequency = BUZZER_RES_FREQ;
//                 buzzer.duty_cycle = 40;
//                 buzzer.beep_on_time = 100;
//                 buzzer.beep_off_time = 1;
//                 buzzer.delay = 0;
//                 buzzer.beep_cycles = 1;
//                 buzzer.burst_repeat_periode = 0;
//                 buzzer.burst_repeat_cycles = 1;
//                 set_buzzer(&buzzer);
//               }
//             }
//           }
//         }
//         else
//         {
//           Newmop_RFID_readings = 0UL;
//           Prevmop_RFID_replaced_flag = 1; // mop is replaced by another chipped mopped
//           Newmop_RFID_flag = 0;
//         }
//         memcpy(Newmop_RFID, current_mob_string_reading, EPC_TOTAL_HEX_LENGTH);
//         memset(current_mob_string_reading, 0, EPC_TOTAL_HEX_LENGTH);
//       }
//     }
// //shell_fprintf(shell_backend_uart_get_ptr(), 0, "NewmopRFID_readings=%d, PrevmopRFID_replaced_flag=%d, Newmop_RFID_flag=%d, Currentmop_iswithout_RFID_Flag=%d, Reg_mopID=%d - LastmopID=%d\t mop_null_rds=%d, mop_rfid_rds=%d\n", Newmop_RFID_readings, Prevmop_RFID_replaced_flag, Newmop_RFID_flag, Currentmop_iswithout_RFID_Flag, prev_mop_id, new_advanced_mop_record.mop_id, mop_null_readings, mop_rfid_readings);

//     //     shell_fprintf(shell_backend_uart_get_ptr(), 0, "NewmopRFID_readings=%d, PrevmopRFID_replaced_flag=%d, Newmop_RFID_flag=%d, Currentmop_iswithout_RFID_Flag=%d, chipped_mob_installed=%d, Reg_mopID=%d - LastmopID=%d\t mop_null_rds=%d, mop_rfid_rds=%d\n", Newmop_RFID_readings, Prevmop_RFID_replaced_flag, Newmop_RFID_flag, Currentmop_iswithout_RFID_Flag, chipped_mob_installed, prev_mop_id, new_advanced_mop_record.mop_id, mop_null_readings, mop_rfid_readings);

//     /*************** get a new IMU sample, shift elements of the IMU vectors and update with the recent values.  ********************************/
//     dAccx = acc_lsb_to_ms2(accel.x);
//     dAccy = acc_lsb_to_ms2(accel.y);
//     dAccz = acc_lsb_to_ms2(accel.z);
//     dGyrx = gyro_lsb_to_dps(gyro.x);
//     dGyry = gyro_lsb_to_dps(gyro.y);
//     dGyrz = gyro_lsb_to_dps(gyro.z);
//     dMagx = bmm150.data.x;
//     dMagy = bmm150.data.y;
//     dMagz = bmm150.data.z;

//     algo_fAddArrayElement(Accx_Nfvec, dAccx, NF);
//     algo_fAddArrayElement(Accy_Nfvec, dAccy, NF);
//     algo_fAddArrayElement(Accz_Nfvec, dAccz, NF);

//     algo_fAddArrayElement(Gyrx_Nfvec, dGyrx, NF);
//     algo_fAddArrayElement(Gyry_Nfvec, dGyry, NF);
//     algo_fAddArrayElement(Gyrz_Nfvec, dGyrz, NF);

//     algo_fAddArrayElement(Magx_Nfvec, (float)dMagx, NF);
//     algo_fAddArrayElement(Magy_Nfvec, (float)dMagy, NF);
//     algo_fAddArrayElement(Magz_Nfvec, (float)dMagz, NF);

//     /* Caclulate mean values based on recent values */
//     Accx_f = algo_fmean(Accx_Nfvec, NF);
//     Accy_f = algo_fmean(Accy_Nfvec, NF);
//     Accz_f = algo_fmean(Accz_Nfvec, NF);

//     Gyrx_f = algo_fmean(Gyrx_Nfvec, NF);
//     Gyry_f = algo_fmean(Gyry_Nfvec, NF);
//     Gyrz_f = algo_fmean(Gyrz_Nfvec, NF);

//     Magx_f = algo_fmean(Magx_Nfvec, NF);
//     Magy_f = algo_fmean(Magy_Nfvec, NF);
//     Magz_f = algo_fmean(Magz_Nfvec, NF);

//     /* Complex computations */
//     result = sqrt((dAccx * dAccx) + (dAccy * dAccy) + (dAccz * dAccz));
//     algo_fAddArrayElement(Acc_dcNfvec, (float)result, NF); // shift elements of the vectors and update with the recent values.

//     Acc_dc_avg = algo_fmean(Acc_dcNfvec, NF);
//     algo_fAddArrayElement(Acc_acvec, (Acc_dcNfvec[0] - Acc_dc_avg), NF);

//     // Detect Acc_dc_avg signal min
//     if (Acc_dc_avg < Acc_dc_avg_min)
//     {
//       Acc_dc_avg_min = Acc_dc_avg;
//       Acc_dc_avg_min_lastupdate_tsp = unixtime_ms;
//     }
//     else if ((unixtime_ms - Acc_dc_avg_min_lastupdate_tsp) > 500)
//     {
//       Acc_dc_avg_min = (Acc_dc_avg_min + Acc_dc_avg) / 2.0;
//       Acc_dc_avg_min_lastupdate_tsp = unixtime_ms;
//     }
//     result = (1.0 / 16.0) * (Acc_acvec[0] + 2.0 * Acc_acvec[1] + 3.0 * Acc_acvec[2] + 4.0 * Acc_acvec[3] + 3.0 * Acc_acvec[4] + 2.0 * Acc_acvec[5] + Acc_acvec[7]); // low pass filtering with Fc=20Hz
//     algo_fAddArrayElement(Acc_acfvec, (float)result, NF);

//     Acc_acf_to = Acc_acf_tn;
//     Acc_acf_tn = Acc_acfvec[0];

//     Acc_acf_sumabs = Acc_acf_sumabs + (float)fabs((double)Acc_acf_tn);

//     if (Acc_acf_sumabs > 75.0)
//     { // to filter possible recurring shocks
//       Acc_acf_sumabs = 75.0;
//     }

//     /* Extract features, all angles should be 0 when the device is placed horizontically on the floor */
//     floor_handle_angle[0] = Parameter.angle_smooth_factor * floor_handle_angle[0] + (1.0 - Parameter.angle_smooth_factor) * (float)atan2(((double)Accy_f * -1.0), sqrt((double)Accx_f * (double)Accx_f + (double)Accz_f * (double)Accz_f)) * 180.0 / PI;

//     result = sqrt(Magx_f * Magx_f + Magy_f * Magy_f + Magz_f * Magz_f);
//     if (result > Parameter.mag_noise_thr)
//     {
//       frame_handle_angle[0] = Parameter.angle_smooth_factor * frame_handle_angle[0] + (1.0 - Parameter.angle_smooth_factor) * (float)atan2((double)Magx_f, sqrt((double)Magy_f * (double)Magy_f + (double)Magz_f * (double)Magz_f)) * 180.0 / PI;
//     }
//     else
//     {
//       frame_handle_angle[0] = 0;
//     }

//     /* Acc_acf acceleration */
//     Acc_acf_tn_abs = (float)fabs((double)Acc_acfvec[0]);

//     // Track Acc_acf max
//     if (Acc_acf_tn_abs > Acc_acf_peak)
//     {
//       Acc_acf_peak = Acc_acf_tn_abs;
//       Acc_acf_peak_lastupdate_tsp = unixtime_ms;
//     }
//     else if ((unixtime_ms - Acc_acf_peak_lastupdate_tsp) > (1000LL * Parameter.peakfollower_update_delay))
//     {
//       Acc_acf_peak = (Acc_acf_peak + Acc_acf_tn_abs) / 2;
//       if (Acc_acf_peak < Parameter.acc_noise_thr)
//       {
//         Acc_acf_peak = Parameter.acc_noise_thr;
//       }
//       Acc_acf_peak_lastupdate_tsp = unixtime_ms;
//     }

//     /* Adaptive signal thresholding */
//     Acc_adpt_thr = Parameter.acc_noise_thr + ((Acc_acf_peak - Parameter.acc_noise_thr) * (EARTH_GRAVITY / 100.0));
//     if ((floor_handle_angle[0] > Parameter.floor_handle_angle_mopping_thr_min) && (floor_handle_angle[0] < Parameter.floor_handle_angle_mopping_thr_max))
//     {
//       if (Acc_acfvec[0] > Acc_adpt_thr)
//       {
//         Acc_acf_adpt_thr = Acc_acf_tn_abs - Acc_adpt_thr;
//       }
//       else if (Acc_acfvec[0] < Acc_adpt_thr)
//       {
//         Acc_acf_adpt_thr = (Acc_acf_tn_abs * -1.0) + Acc_adpt_thr;
//       }
//       else
//       {
//         Acc_acf_adpt_thr = 0.0;
//       }
//     }
//     else
//     {
//       Acc_acf_adpt_thr = 0.0;
//     }

//     /* Detect mopping + mopping cycles + mopping m2 coverage */
//     sAacf_to = sAacf_tn;
//     mopcyclesFlag = 0;
//     if ((floor_handle_angle[0] > Parameter.floor_handle_angle_mopping_thr_min) && (floor_handle_angle[0] < Parameter.floor_handle_angle_mopping_thr_max))
//     {
//       handle_in_mopping_position[0] = 1;

//       System.StatusInputs |= STATUSFLAG_HMP; // Create status entry

//       // get sign of Aacf slope
//       if (Acc_acf_tn > Acc_acf_to)
//       {
//         sAacf_tn = 1; // positive
//       }
//       else
//       {
//         sAacf_tn = -1; // negative (include equal)
//       }

//       /* check and confirm True Max or Min Peaks */
//       if ((sAacf_to == 1) && (sAacf_tn == -1))
//       { // local Max= +=>-
//         if ((Acc_acf_tn > Acc_adpt_thr) && (TPflag == -1))
//         { // greater and comes after True Min Peak
//           TPflag = 1;
//         }
//       }

//       if ((sAacf_to == -1) && (sAacf_tn == 1))
//       { // local Min= -=>+
//         if ((Acc_acf_tn < (Acc_adpt_thr * -1.0)) && (TPflag == 1))
//         { // lesser than moving average and comes after True Max Peak
//           TPflag = -1;
//           mopcycle_duration_ms = (uint32_t)(unixtime_ms - Acc_acf_min_lastupdate_tsp);
//           Acc_acf_min_lastupdate_tsp = unixtime_ms;

//           if ((Mopping_motion_gyr_flag == 1) && ((mopcycle_duration_ms > (1000 * Parameter.min_mopcycle_duration)) && (mopcycle_duration_ms < (1000 * 2.0 * Parameter.max_mopcycle_duration))))
//           {
//             mopcycles++; // if True Min Peak after True Max Peak => 1 cycle
//             mopcyclesFlag = 1;
//             prev_to_lastmopcycle_tsp = lastmopcycle_tsp;
//             lastmopcycle_tsp = unixtime_ms;
//             velocity = Acc_acf_sumabs * dt;

//             if (MoppingFlag[0] == 0)
//             {
//               velocity = 0;
//               mopping_speed = 0;
//             }
//             mopping_speed = 0.5 * mopping_speed + (1.0 - 0.5) * velocity;
//             if ((mopping_speed > 0.5) && (mopping_speed < 1.0))
//             {
//               velocity = velocity / mopping_speed;
//             }

//             mopping_coverage_per_handle_total = mopping_coverage_per_handle_total + velocity * ((float)mopcycle_duration_ms / 1000.0) * (Parameter.mop_width * (100.0 - Parameter.mop_overlap) / 100);
//             mopping_coverage_per_mop = mopping_coverage_per_mop + velocity * ((float)mopcycle_duration_ms / 1000.0) * (Parameter.mop_width * (100.0 - Parameter.mop_overlap) / 100.0);

//             if (Frame_side[0] == 0)
//             {
//               mopping_coverage_side1 = mopping_coverage_side1 + velocity * ((float)mopcycle_duration_ms / 1000.0) * (Parameter.mop_width * (100.0 - Parameter.mop_overlap) / 100.0);
//             }

//             if (Frame_side[0] == 1)
//             {
//               mopping_coverage_side2 = mopping_coverage_side2 + velocity * ((float)mopcycle_duration_ms / 1000.0) * (Parameter.mop_width * (100.0 - Parameter.mop_overlap) / 100.0);
//             }
//           }
//           Acc_acf_sumabs = 0;
//           velocity = 0;
//         }
//       }
//     }
//     else
//     {
//       handle_in_mopping_position[0] = 0;
//       velocity = 0;
//       System.StatusInputs &= ~STATUSFLAG_HMP; // Reset status entry
//     }

//     // Check if mob has reached the maximum allowed square meter
//     if (Parameter.enable_coveraged_per_mop_notification == true)
//     {
//       check_max_sqm_coveraged_per_mop();
//     }

//     /* Check if mopping started */
//     if ((MoppingFlag[0] == 0) && ((gcycle_angley > (4.0 * Parameter.gyr_spin_thr)) || ((mopcyclesFlag == 1) && (Mopping_motion_gyr_flag == 1) && ((unixtime_ms - Mopping_stop_tsp) > 1000 * Parameter.max_mopcycle_duration) && ((float)(mopcycles - last_num_of_mopcycles) >= Parameter.mopcycle_sequence_thr))))
//     {
//       MoppingFlag[0] = 1;
//       Mopping_start_tsp = unixtime_ms;
//       System.StatusInputs |= STATUSFLAG_MF; // Create status entry

//       if ((MoppingFlag[0] == 1) && (Total_mops_used == 0)) // start shift without lift, only for first mop
//       {
//         Mop_on_floor_after_Change_Flag = 1;
//         Prevmop_RFID_replaced_flag = 1;
//       }

//       if (Parameter.debug == true || Parameter.algo_verbose == true)
//       {
//         if (pcb_test_is_running == false)
//         {
//           rtc_print_debug_timestamp();
//           shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_YELLOW, "Mopping has started\n");
//         }
//       }
//     }

//     /* Check if mopping stopped */
//     if ((MoppingFlag[0] == 1) && (((Mopping_motion_gyr_flag == 0) && ((unixtime_ms - Mopping_start_tsp) > 1000 * Parameter.max_mopcycle_duration)) || (handle_in_mopping_position[0] == 0)))
//     {
//       MoppingFlag[0] = 0;
//       last_num_of_mopcycles = mopcycles;
//       mopping_speed = 0;
//       Mopping_stop_tsp = unixtime_ms;

//       System.StatusInputs &= ~STATUSFLAG_MF; // Reset status entry

//       if (Parameter.debug == true || Parameter.algo_verbose == true)
//       {
//         if (pcb_test_is_running == false)
//         {
//           rtc_print_debug_timestamp();
//           shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_YELLOW, "Mopping has stopped\n");
//         }
//       }
//     }

//     /* classify motion state, New movement pattern detected event */
//     if (motion_detected == false)
//     {
//       motion_state[0] = IDLE_STATE; // idle on wall or trolley

//       /* Create event for cloud and user notification (this is triggered within the event) */
//       if (motion_state[0] != motion_state[1])
//       {
//         //  NewEvent0x01(timestamp_last_cloud_transmission, unixtime_ms, (float)idle_time_in_percentage, (float)moving_time_in_percentage, (float)mopping_time_in_percentage);

//         /* If switch from MOVING to IDLE state allow immediatelly data sync*/
//         if ((motion_state[0] == IDLE_STATE) && (motion_state[1] == MOVING_STATE))
//         {
//           /* Force logic to send data immediately */
//           coap_last_transmission_timer = Parameter.cloud_sync_interval_idle + Parameter.cloud_sync_interval_moving;
//         }

//         if (Parameter.algo_verbose == true)
//         {
//           if (pcb_test_is_running == false)
//           {
//             rtc_print_debug_timestamp();
//             shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "Motion state: IDLE\n");
//           }
//         }
//       }
//     }
//     else
//     {
//       if (MoppingFlag[0] == 1)
//       {
//         motion_state[0] = MOPPING_STATE; // mopping

//         if (mop_change_status == NEW_CHIPPED_MOP)
//         {
//           // mop_change_status = MOP_RESET_STATE; // reset flag
//         }
//         if (mop_change_status == NEW_UNCHIPPED_MOP)
//         {
//           // mop_change_status = MOP_RESET_STATE; // reset flag
//         }
//         if (motion_state[0] != motion_state[1])
//         {
//           if (Parameter.algo_verbose == true)
//           {
//             if (pcb_test_is_running == false)
//             {
//               rtc_print_debug_timestamp();
//               shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "Motion state: MOPPING\n");
//             }
//           }
//           // NewEvent0x01(timestamp_last_cloud_transmission, unixtime_ms, (float)idle_time_in_percentage, (float)moving_time_in_percentage, (float)mopping_time_in_percentage);
//         }
//       }
//       else
//       {
//         motion_state[0] = MOVING_STATE; // moving (e.g. at hands and walking)

//         /* Create event for cloud and user notification (this is triggered within the event) */
//         if (motion_state[0] != motion_state[1])
//         {
//           //  NewEvent0x01(timestamp_last_cloud_transmission, unixtime_ms, (float)idle_time_in_percentage, (float)moving_time_in_percentage, (float)mopping_time_in_percentage);

//           if (Parameter.algo_verbose == true)
//           {
//             if (pcb_test_is_running == false)
//             {
//               rtc_print_debug_timestamp();
//               shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "Motion state: MOVING\n");
//             }
//           }
//         }
//       }
//     }

//     /* Detect frame side and frame flip time */
//     Frame_flip_flag = 0;

//     if ((frame_handle_angle[0] > Parameter.frame_handle_angle_thr) && (floor_handle_angle[0] > Parameter.floor_handle_angle_mopping_thr_min) && (unixtime_ms - Frame_flip_tsp) > (1000ULL * Parameter.min_mopframeflip_duration))
//     {
//       if (((MoppingFlag[0] == 1) || ((unixtime_ms - Mopping_stop_tsp) < (1000ULL * Parameter.min_mopframeflip_duration))) && (Frame_flip_flag == 0) && (Frame_side[0] == 1) && (frame_lift_flag[0] == 0) && (mopping_coverage_per_mop > Parameter.mopping_coverage_per_mop_thr))
//       {
//         Frame_flip_flag = 1;
//         Frame_flip_tsp = unixtime_ms;
//         System.StatusInputs |= STATUSFLAG_FFF; // Create status entry

//         /* Create event for cloud and user notification (this is triggered within the event) */
//         NewEvent0x18(Frame_side[0]);

//         Notification.next_state = NOTIFICATION_CLEAR;

//         if (Parameter.debug == true || Parameter.algo_verbose == true)
//         {
//           if (pcb_test_is_running == false)
//           {
//             rtc_print_debug_timestamp();
//             shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "Frame flip 1->0\n");
//           }
//         }
//       }

//       Frame_side[0] = 0;

//       if (Frame_side[0] != Frame_side[1])
//       {
//         System.StatusInputs |= STATUSFLAG_FS; // Create status entry
//       }
//     }
//     else if ((frame_handle_angle[0] < (-1.0 * Parameter.frame_handle_angle_thr)) && (floor_handle_angle[0] > Parameter.floor_handle_angle_mopping_thr_min) && (unixtime_ms - Frame_flip_tsp) > (1000ULL * Parameter.min_mopframeflip_duration))
//     {
//       if (((MoppingFlag[0] == 1) || ((unixtime_ms - Mopping_stop_tsp) < (1000ULL * Parameter.min_mopframeflip_duration))) && (Frame_flip_flag == 0) && (Frame_side[0] == 0) && (frame_lift_flag[0] == 0) && (mopping_coverage_per_mop > Parameter.mopping_coverage_per_mop_thr))
//       {
//         Frame_flip_flag = 1;
//         Frame_flip_tsp = unixtime_ms;
//         System.StatusInputs |= STATUSFLAG_FFF; // Create status entry

//         /* Create event for cloud and user notification (this is triggered within the event) */
//         NewEvent0x18(Frame_side[0]);

//         Notification.next_state = NOTIFICATION_CLEAR;

//         if (Parameter.debug == true || Parameter.algo_verbose == true)
//         {
//           if (pcb_test_is_running == false)
//           {
//             rtc_print_debug_timestamp();
//             shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "Frame flip 0->1\n");
//           }
//         }
//       }
//       Frame_side[0] = 1;
//       if (Frame_side[0] != Frame_side[1])
//       {
//         System.StatusInputs |= STATUSFLAG_FS; // Create status entry
//       }
//     }

//     /* Detect Mop change */
//     //=======================================================
//     if ((floor_handle_angle[0] < Parameter.floor_handle_angle_mopchange_thr))
//     {
//       // if the frame is lifted up and a chipped mop has been installed or an unchipped mop has been installed
//       if ((frame_lift_flag[0] == 0) && ((unixtime_ms - frame_lift_tsp) > 1000 * Parameter.min_mopchange_duration))
//       {
//         frame_lift_tsp = unixtime_ms;
//         frame_lift_flag[0] = 1;
//         System.StatusInputs |= STATUSFLAG_FLF; // Create status entry
//         coverage_print_flag = true;
//         Notification.next_state = NOTIFICATION_CLEAR;

//         /* Change rfid TX output power when lifted */
//         RFID_setOutputPower(Parameter.rfid_output_power_lifted);
// //Parameter.rfid_interval = 120;

//         if (Parameter.debug == true || Parameter.algo_verbose == true)
//         {
//           if (pcb_test_is_running == false)
//           {
//             rtc_print_debug_timestamp();
//             shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "frame has lifted. Set rfid putput power to: %d dBm\n", Parameter.rfid_output_power_lifted);
//           }
//         }
//       }

//       if ((frame_lift_flag[0] == 1) && ((unixtime_ms - frame_lift_tsp) > 1000 * Parameter.min_mopchange_duration))
//       {
//         MopChangeFlag[0] = 1;
//         Mop_change_tsp = unixtime_ms;
//         System.StatusInputs |= STATUSFLAG_MCF; // Create status entry
//         mop_null_readings = 0UL;
//         mop_rfid_readings = 0UL;

//         if (Total_mops_used > 0)
//         {
//           unchipped_mop_coverage_reset_flag = false;

//           // first mop will be installed in the shift
//           if ((Parameter.debug == true || Parameter.algo_verbose == true) && (coverage_print_flag))
//           {
//             if (pcb_test_is_running == false)
//             {
//               rtc_print_debug_timestamp();
//               shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_YELLOW, "Mop ID: %d \t Mop Coverage = %4.1fm2 (side1 = %4.2fm2, side2 = %4.1fm2)\n", current_room_to_mop_mapping.current_mop_id, mopping_coverage_per_mop, mopping_coverage_side1, mopping_coverage_side2);
//             }
//             coverage_print_flag = false;
//           }
//         }
//       }
//     }
//     else
//     {
//       frame_lift_flag[0] = 0;
//     }

//     /* Mop change and frame is back on the floor   */

//     if ((floor_handle_angle[0] > Parameter.floor_handle_angle_mopchange_thr) && (MopChangeFlag[0] == 1) && (unixtime_ms - Mop_change_tsp) > 100ULL)
//     {
//       Mop_on_floor_after_Change_Flag = 1;
//       Mop_on_floor_after_Change_Flag_tsp = unixtime_ms;
//       frame_lift_flag[0] = 0;
//       MopChangeFlag[0] = 0;
//       dirtymop_blink_flag = true;

//       System.StatusInputs &= ~STATUSFLAG_FLF; // Create status entry
//       System.StatusInputs &= ~STATUSFLAG_MCF; // Create status entry

//       /* Create event for cloud and user notification (this is triggered within the event) */
//       NewEvent0x1B();

//       /* Change rfid TX output power when not lifted */
//       RFID_setOutputPower(Parameter.rfid_output_power);
//   //Parameter.rfid_interval = 300;

//       if (Parameter.debug == true || Parameter.algo_verbose == true)
//       {
//         if (pcb_test_is_running == false)
//         {
//           rtc_print_debug_timestamp();
//           shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "Frame is back on the floor. Set rfid output power to %d dBm\n", Parameter.rfid_output_power);
//         }
//       }
//     }

//     // Reactivite dirty mop blinks every time the handle is placed on the floor
//     if ((dirtymop_blink_flag == true) && (Flag_SameMopAlreadyUsedNotification == true) && (prev_mop_id == new_advanced_mop_record.mop_id) && (new_advanced_mop_record.timestamp > Mop_on_floor_after_Change_Flag_tsp))
//     {
//       Notification.next_state = NOTIFICATION_SAME_MOP_ALREADY_USED;
//       dirtymop_blink_flag = false;
//     }

//     /* Refresh mop id which is registered in the device */
//     if (RFID_IsOn == true) // do this only if rfid module is turned on (scans for tags)
//     {
//       if (current_room_to_mop_mapping.current_mop_id != 0) // If mop id is >0 there is a chipped mop registered in the device
//       {
//         if (current_room_to_mop_mapping.current_mop_id == last_seen_mop_id)
//         {
//           mop_id_refresh_timer = Parameter.mop_id_refresh_timer * (1000 / Parameter.imu_interval); // This function is executed 20 times per second. Timers need to be converted to be 20 times higher to match 1sec loops.
//           last_seen_mop_id = 0;
//           // rtc_print_debug_timestamp();
//           //  shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "Registered mop id %d seens. Reset mop_id_refresh_timer\n", current_room_to_mop_mapping.current_mop_id);
//         }
//         else
//         {
//           mop_id_refresh_timer--;

//           if (mop_id_refresh_timer == 0)
//           {
//             rtc_print_debug_timestamp();
//             shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_RED, "Registered mop id %d is absent for %d sec\n", current_room_to_mop_mapping.current_mop_id, Parameter.mop_id_refresh_timer);
//             rtc_print_debug_timestamp();
//             shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_RED, "Mop is released and room to mop mapping is reset.\n");
//             if ((Total_mops_used >= 1) && (mopping_coverage_per_mop > 0.5))
//             {
//               NewEvent0x1C(current_room_to_mop_mapping.current_mop_id, mopping_coverage_side1, mopping_coverage_side2);
//             }
//             NewEvent0x02(0); // Mop id 0 creates an Event for cloud upload but triggers no user notification

//             reset_room_to_mop_mapping();         // Reset room to mop mapping
//             mop_change_status = MOP_RESET_STATE; // reset flag
//             memset(Newmop_RFID, '0', EPC_TOTAL_HEX_LENGTH);
//             prev_mop_id = 0;
//             mopping_coverage_per_mop = 0;
//           mopping_coverage_side1 = 0;
//           mopping_coverage_side2 = 0;

//             //  shell_fprintf(shell_backend_uart_get_ptr(), 0, "NewmopRFID_readings=%d, PrevmopRFID_replaced_flag=%d, Newmop_RFID_flag=%d, Currentmop_iswithout_RFID_Flag=%d, Reg_mopID=%d - LastmopID=%d\t mop_null_rds=%d, mop_rfid_rds=%d\n", Newmop_RFID_readings, Prevmop_RFID_replaced_flag, Newmop_RFID_flag, Currentmop_iswithout_RFID_Flag, prev_mop_id, new_advanced_mop_record.mop_id, mop_null_readings, mop_rfid_readings);
//           }
//         }
//       }
//     }

//     /* Check if current mop is new, was used for some sqm and is not listed in the last seen mop array yet
//     If  mopping_coverage_per_mop >= Parameter.mopping_coverage_per_mop_thr and there is a chipped mop on the frame (mop_null_readings < mop_rfid_readings ) then the current mop scan is registered in the used mop list (i.e. the last mop scan must occur after Mopping_start_tsp).
//     */
//     uint8_t rslt = 0;

//     if (RFID_TriggeredRead == true)
//     {
//       if (MoppingFlag[0] == 1)
//       {
//         /* To improve reliability of registering mops in the used list we should trigger the import function (Mop_AddItemInLastSeenArray()) after some mopping has been done with this mop*/
//         if ((mopping_coverage_per_mop >= Parameter.mopping_coverage_per_mop_thr) && (mop_null_readings < mop_rfid_readings) && (Mopping_start_tsp < new_advanced_mop_record.timestamp))
//         {
//           /* We should also check if this last last mop scan is equal to the Currentmop_RFID detected by the mop detection algorithm and issue a terminal warning is not the same */
//           if ((current_room_to_mop_mapping.current_mop_id != 0) && (current_room_to_mop_mapping.current_mop_id == new_advanced_mop_record.mop_id))
//           {
//             /* Check if new mop was already used in current shift (in the last 12 hours, array gets auto reset after this time periode) */
//             rslt = Mop_CheckIDInLastSeenArray(new_advanced_mop_record);

//             if (rslt == 0) // mop is not listed in the last seen array yet
//             {
//               Mop_AddItemInLastSeenArray(new_advanced_mop_record);
//               Mop_added_inlastseenmoparray_Flag = 1;

//               /* Print rfid record and room record details if listed in rf databases */
//               if (pcb_test_is_running == false)
//               {
//                 rtc_print_debug_timestamp();
//                 shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_YELLOW, "Mop ID: %d was added in 'last seen mop array'\n", new_advanced_mop_record.mop_id);
//               }
//             }
//           }
//           else
//           {
//             if ((Mop_added_inlastseenmoparray_Flag == 0) && (current_room_to_mop_mapping.current_mop_id != 0) && (new_advanced_mop_record.mop_id != current_room_to_mop_mapping.current_mop_id)) // Do not print out this message if there is no mop registered
//             {
//               if (pcb_test_is_running == false)
//               {
//                 rtc_print_debug_timestamp();
//                 shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_RED, "Mop ID: %d was NOT added in 'last seen mop array'\n", current_room_to_mop_mapping.current_mop_id);
//                 rtc_print_debug_timestamp();
//                 shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_RED, "last mop scan ID: %d, is not identical to mop ID detected during mop change\n", new_advanced_mop_record.mop_id);
//               }
//             }
//           }
//         }
//       }
//     }

//     /** ALGORITHM TO DETECTED THAT USER USES CURRENTLY AN UNCHIPPED MOP ***/
//     if ((Mop_on_floor_after_Change_Flag == 1) && (mopping_coverage_per_mop > (Parameter.mopping_coverage_per_mop_thr + 0.1)) && (unchipped_mop_coverage_reset_flag) && (Newmop_RFID_flag == 0))
//     {
//       Total_mops_used++;
//       mob_max_side1_sqm_reached = false;
//       mob_max_side2_sqm_reached = false;
//       mob_max_sqm_reached = false;
//       Mop_on_floor_after_Change_Flag = 0;
//       Currentmop_iswithout_RFID_Flag = 1; // The user mops with an un-chipped mop (mop is not chipped or chip is not working)
//       prev_mop_id = 0;
//       current_mop_sides = 2;
//       mop_change_status = NEW_UNCHIPPED_MOP;
//       chipped_mop_installed = false;
//       Flag_SameMopAlreadyUsedNotification = false;
//       mop_change_status = MOP_RESET_STATE; // reset flag

//       reset_room_to_mop_mapping(); /* Reset room to mop mapping */
//       memset(Newmop_RFID, '0', EPC_TOTAL_HEX_LENGTH);
//       NewEvent0x02(0); // Mop id 0 creates an Event for cloud upload but triggers no user notification

//       if (Parameter.debug == true || Parameter.algo_verbose == true)
//       {
//         if (pcb_test_is_running == false)
//         {
//           rtc_print_debug_timestamp();
//           shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_YELLOW, "%d Mop without chip is placed on Frame\n", Total_mops_used);
//         }
//       }
//     }
//     else
//     {
//       Currentmop_iswithout_RFID_Flag = 0;
//     }
//     //========================================================

//     /* Detect free fall or hit */
//     Acc_dc_smooth = (0.7 * Acc_dcNfvec[0]) + (0.3 * Acc_dc_smooth);
//     if ((Free_Fall_flag == 0) && (Acc_dcNfvec[0] < (EARTH_GRAVITY / 10.0)) && (Acc_dc_smooth < (EARTH_GRAVITY / 5.0)))
//     { // free fall detected
//       freefall_tsp = unixtime_ms;
//       Free_Fall_flag = 1;
//     }

//     if (((Free_Fall_flag == 1) && (Acc_dc_avg > (0.95 * EARTH_GRAVITY))) || ((unixtime_ms - freefall_tsp) > 5000LL))
//     {
//       Free_Fall_flag = 0;
//     }

//     if ((Free_Fall_flag == 1) && (Acc_dcNfvec[0] > (Parameter.hit_shock_mag_thr * EARTH_GRAVITY)))
//     {
//       Hit_Shock_tsp = unixtime_ms;
//       Hit_Shock_flag = 1;
//       Free_Fall_flag = 0;
//       Hit_Shock_mag = Acc_dcNfvec[0];
//       fall_duration_ms = unixtime_ms - freefall_tsp;
//     }

//     if ((Hit_Shock_flag == 1) && (Acc_dc_avg > (0.95 * EARTH_GRAVITY)))
//     {
//       Hit_Shock_flag = 0;
//       free_fall_height = 1.0 / 2.0 * EARTH_GRAVITY * (fall_duration_ms / 1000.0) * (fall_duration_ms / 1000.0); // free_fall_height = 1 / 2 * EARTH_GRAVITY * (fall_duration_ms / 1000) ^ 2;

//       if (Parameter.debug == true || Parameter.algo_verbose == true)
//       {
//         if (pcb_test_is_running == false)
//         {
//           rtc_print_debug_timestamp();
//           shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "Hit Shock detected, Mag= %4.2fm/s^2  , Duration= %4.2fs, free_fall_height= %4.2fm \n", Hit_Shock_mag, fall_duration_ms, free_fall_height);
//         }
//       }

//       NewEvent0x17(Hit_Shock_mag);

//       System.StatusInputs |= STATUSFLAG_HS; // Create status entry
//     }

//     ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//     /* Detect Gyr_fx signal peak */
//     Gyr_fx_abs = (float)fabs((double)Gyrx_f);

//     if (Gyr_fx_abs > Gyr_fx_peak)
//     {
//       Gyr_fx_peak = Gyr_fx_abs;
//       Gyr_fx_peak_lastupdate_tsp = unixtime_ms;
//     }
//     else if ((unixtime_ms - Gyr_fx_peak_lastupdate_tsp) > (1000 * Parameter.peakfollower_update_delay))
//     {
//       Gyr_fx_peak = (Gyr_fx_peak + Gyr_fx_abs) / 2.0;
//       Gyr_fx_peak_lastupdate_tsp = unixtime_ms;
//     }

//     /* Adaptive signal thresholding */
//     Gyrx_adpt_thr = ((Gyr_fx_peak) / 10) + (Parameter.gyr_noise_thr) / 2.0;
//     Gyr_fx_thr = Gyr_fx_abs - Gyrx_adpt_thr;

//     if (Gyr_fx_thr < 0)
//     {
//       Gyr_fx_thr = 0;
//     }

//     /* shift values and compute energy */
//     algo_fAddArrayElement(Gyr_fx_thr_Nfvec, Gyr_fx_thr, NF);
//     Gyr_fx_thr_ergy = (Parameter.gyr_smooth_factor * Gyr_fx_thr_ergy) + ((1 - Parameter.gyr_smooth_factor) * algo_fsum(Gyr_fx_thr_Nfvec, NF)); // Gyr_fx_thr_ergy=sum(Gyr_fx_thr_Nfvec);

//     /* Detect Gyr_fy signal peak */
//     Gyr_fy_abs = (float)fabs((double)Gyry_f);

//     if (Gyr_fy_abs > Gyr_fy_peak)
//     {
//       Gyr_fy_peak = Gyr_fy_abs;
//       Gyr_fy_peak_lastupdate_tsp = unixtime_ms;
//     }
//     else if ((unixtime_ms - Gyr_fy_peak_lastupdate_tsp) > 1000 * 2 * Parameter.peakfollower_update_delay)
//     {
//       Gyr_fy_peak = (Gyr_fy_peak + Gyr_fy_abs) / 2.0;
//       Gyr_fy_peak_lastupdate_tsp = unixtime_ms;
//     }

//     /* Adaptive signal thresholding */
//     Gyry_adpt_thr = (Gyr_fy_peak) / 2.0 + Parameter.gyr_noise_thr;
//     Gyr_fy_thr = Gyr_fy_abs - Gyry_adpt_thr;

//     if (Gyr_fy_thr < 0)
//     {
//       Gyr_fy_thr = 0;
//     }

//     /* shift values and compute energy */
//     algo_fAddArrayElement(Gyr_fy_thr_Nfvec, Gyr_fy_thr, NF);
//     Gyr_fy_thr_ergy = (Parameter.gyr_smooth_factor * Gyr_fy_thr_ergy) + ((1 - Parameter.gyr_smooth_factor) * algo_fsum(Gyr_fy_thr_Nfvec, NF)); //   Gyr_fy_thr_ergy= sum(Gyr_fy_thr_Nfvec);

//     /* Detect Gyr_fz signal peak */
//     Gyr_fz_abs = (float)fabs((double)Gyrz_f);

//     if (Gyr_fz_abs > Gyr_fz_peak)
//     {
//       Gyr_fz_peak = Gyr_fz_abs;
//       Gyr_fz_peak_lastupdate_tsp = unixtime_ms;
//     }
//     else if ((unixtime_ms - Gyr_fz_peak_lastupdate_tsp) > 1000 * 3 * Parameter.peakfollower_update_delay)
//     {
//       Gyr_fz_peak = (Gyr_fz_peak + Gyr_fz_abs) / 2.0;
//       Gyr_fz_peak_lastupdate_tsp = unixtime_ms;
//     }

//     /* Adaptive signal thresholding */
//     Gyrz_adpt_thr = (Gyr_fz_peak) / 3.0 + (Parameter.gyr_noise_thr) / 2.0;
//     Gyr_fz_thr = Gyr_fz_abs - Gyrz_adpt_thr;

//     if (Gyr_fz_thr < 0)
//     {
//       Gyr_fz_thr = 0;
//     }

//     /* shift values and compute energy */
//     algo_fAddArrayElement(Gyr_fz_thr_Nfvec, Gyr_fz_thr, NF);
//     Gyr_fz_thr_ergy = (Parameter.gyr_smooth_factor * Gyr_fz_thr_ergy) + ((1 - Parameter.gyr_smooth_factor) * algo_fsum(Gyr_fz_thr_Nfvec, NF)); //   Gyr_fy_thr_ergy= sum(Gyr_fy_thr_Nfvec);

//     ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//     //// gyro x axis
//     algo_fAddArrayElement(Gyrx_acvec, (dGyrx - Gyrx_f), NF);                                                                                                                 // remove dc offset-bias//
//     Gyrx_acf = (1.0 / 16.0) * (Gyrx_acvec[0] + 2.0 * Gyrx_acvec[1] + 3.0 * Gyrx_acvec[2] + 4.0 * Gyrx_acvec[3] + 3.0 * Gyrx_acvec[4] + 2.0 * Gyrx_acvec[5] + Gyrx_acvec[7]); // low pass filtering with Fc=20Hz

//     gyr_snx = Gyrx_acf; // Gyrx_f;
//     // get sign of gyr signal slope
//     if (gyr_snx > gyr_sox)
//     {
//       gyr_trnx = 1; // positive
//     }
//     else
//     {
//       gyr_trnx = -1; // negative (include equal)
//     }
//     // check and confirm True Max or Min Peaks
//     if ((gyr_trox == 1) && (gyr_trnx == -1) && (gTPflagx == -1) && (gyr_snx > (Parameter.gyr_noise_thr / 2)))
//     { // local Max= +=>-
//       gcycle_duration_msx = unixtime_ms - gyr_zerocross_lastupdate_tspx;
//       lastgcycle_tspx = unixtime_ms;
//       if ((gcycle_duration_msx > (1000 * Parameter.min_mopcycle_duration / 2.0)) && (gcycle_duration_msx < (1000 * Parameter.max_mopcycle_duration / 2.0)))
//       {
//         gTPflagx = 1;
//       }
//     }
//     if ((gyr_trox == -1) && (gyr_trnx == 1) && (gTPflagx == 1) && (gyr_snx < -1 * Parameter.gyr_noise_thr / 2))
//     { // local Min= -=>+
//       gcycle_duration_msx = unixtime_ms - gyr_zerocross_lastupdate_tspx;
//       lastgcycle_tspx = unixtime_ms;
//       if ((gcycle_duration_msx > (1000 * Parameter.min_mopcycle_duration / 2.0)) && (gcycle_duration_msx < (1000 * Parameter.max_mopcycle_duration / 2.0)))
//       {
//         gTPflagx = -1;
//         gyr_activityFlagx = 1;
//       }
//     }
//     if (((gyr_snx > gyr_sox) && (gyr_snx > 0) && (gyr_sox < 0)) || ((gyr_snx < gyr_sox) && (gyr_snx < 0) && (gyr_sox > 0)))
//     { // cross zero line
//       gyr_zerocross_lastupdate_tspx = unixtime_ms;
//     }

//     if ((unixtime_ms - lastgcycle_tspx) > (1000 * (Parameter.max_mopcycle_duration)))
//     {
//       gcycle_anglex = 0;
//       gyr_activityFlagx = 0;
//     }
//     if ((gyr_activityFlagx == 1) && ((unixtime_ms - Mop_on_floor_after_Change_Flag_tsp) > 1000ULL * Parameter.max_mopcycle_duration))
//     {
//       gcycle_anglex = gcycle_anglex + (float)fabs((double)gyr_snx) * dt;
//     }
//     else
//     {
//       gcycle_anglex = 0;
//     }
//     if (gcycle_anglex > (Parameter.gyr_spin_thr / 4.0))
//     {
//       Mopping_motion_gyr_flagx = 1;
//     }
//     else
//     {
//       Mopping_motion_gyr_flagx = 0;
//     }
//     gyr_sox = gyr_snx;
//     gyr_trox = gyr_trnx;
//     ////////////////////////////////////////////////////////////////////////////////////////////////////
//     // //// gyro y axis

//     algo_fAddArrayElement(Gyry_acvec, (dGyry - Gyry_f), NF);
//     Gyry_acf = (1.0 / 16.0) * (Gyry_acvec[0] + 2.0 * Gyry_acvec[1] + 3.0 * Gyry_acvec[2] + 4.0 * Gyry_acvec[3] + 3.0 * Gyry_acvec[4] + 2.0 * Gyry_acvec[5] + Gyry_acvec[7]); // low pass filtering with Fc=20Hz

//     gyr_sny = Gyry_acf; // Gyry_f;

//     // get sign of gyr signal slope
//     if (gyr_sny > gyr_soy)
//     {
//       gyr_trny = 1; // positive
//     }
//     else
//     {
//       gyr_trny = -1; // negative (include equal)
//     }
//     // check and confirm True Max or Min Peaks
//     if ((gyr_troy == 1) && (gyr_trny == -1) && (gTPflagy == -1) && (gyr_sny > Parameter.gyr_noise_thr))
//     { // local Max= +=>-
//       gcycle_duration_msy = unixtime_ms - gyr_zerocross_lastupdate_tspy;
//       lastgcycle_tspy = unixtime_ms;
//       if ((gcycle_duration_msy > (1000 * Parameter.min_mopcycle_duration / 2.0)) && (gcycle_duration_msy < (1000 * Parameter.max_mopcycle_duration / 2.0)))
//       {
//         gTPflagy = 1;
//       }
//     }
//     if ((gyr_troy == -1) && (gyr_trny == 1) && (gTPflagy == 1) && (gyr_sny < -1 * Parameter.gyr_noise_thr))
//     { // local Min= -=>+
//       gcycle_duration_msy = unixtime_ms - gyr_zerocross_lastupdate_tspy;
//       lastgcycle_tspy = unixtime_ms;
//       if ((gcycle_duration_msy > (1000 * Parameter.min_mopcycle_duration / 2.0)) && (gcycle_duration_msy < (1000 * Parameter.max_mopcycle_duration / 2.0)))
//       {
//         gTPflagy = -1;
//         gyr_activityFlagy = 1;
//       }
//     }
//     if (((gyr_sny > gyr_soy) && (gyr_sny > 0) && (gyr_soy < 0)) || ((gyr_sny < gyr_soy) && (gyr_sny < 0) && (gyr_soy > 0)))
//     { // cross zero line
//       gyr_zerocross_lastupdate_tspy = unixtime_ms;
//     }
//     if ((unixtime_ms - lastgcycle_tspy) > (1000 * Parameter.max_mopcycle_duration))
//     {
//       gcycle_angley = 0;
//       gyr_activityFlagy = 0;
//     }
//     if ((gyr_activityFlagy == 1) && ((unixtime_ms - Mop_on_floor_after_Change_Flag_tsp) > 1000ULL * Parameter.max_mopcycle_duration))
//     {
//       gcycle_angley = gcycle_angley + (float)fabs((double)gyr_sny) * dt;
//     }
//     else
//     {
//       gcycle_angley = 0;
//     }
//     if (gcycle_angley > Parameter.gyr_spin_thr)
//     {
//       Mopping_motion_gyr_flagy = 1;
//     }
//     else
//     {
//       Mopping_motion_gyr_flagy = 0;
//     }
//     gyr_soy = gyr_sny;
//     gyr_troy = gyr_trny;

//     if ((handle_in_mopping_position[0] == 0) || ((Gyr_fy_thr_ergy + Gyr_fx_thr_ergy) <= (1.5 * Gyr_fz_thr_ergy)))
//     {
//       gcycle_angley = 0;
//       gcycle_anglex = 0;
//       Gyr_fx_thr_ergy = 0;
//       Gyr_fy_thr_ergy = 0;
//       Gyr_fz_thr_ergy = 0;
//     }

//     ///////////////////////////////////

//     if (((Mopping_motion_gyr_flagy == 1) || (Mopping_motion_gyr_flagx == 1)) && (handle_in_mopping_position[0] == 1) && (Acc_dc_avg_min > 0.9 * EARTH_GRAVITY) && ((Gyr_fy_thr_ergy + Gyr_fx_thr_ergy) > 1.5 * Gyr_fz_thr_ergy))
//     {
//       Mopping_motion_gyr_flag = 1;
//       Mopping_motion_gyr_flag_lastupdate_tsp = unixtime_ms;
//     }
//     else if (((unixtime_ms - Mopping_motion_gyr_flag_lastupdate_tsp) > 1000LL) || (handle_in_mopping_position[0] == 0))
//     {
//       Mopping_motion_gyr_flag = 0;
//       Mopping_motion_gyr_flag_lastupdate_tsp = unixtime_ms;
//     }

//     /* find the prominent axis and clasify mopping activity type 1:unknown 2:back and forth 3: pro s-shape */
//     if ((unixtime_ms - mopping_pattern_lastupdate_tsp) > (1000LL * 2 * Parameter.peakfollower_update_delay))
//     {
//       mopping_pattern_lastupdate_tsp = unixtime_ms;

//       if ((MoppingFlag[0] == 1) && (Mopping_motion_gyr_flagy == 1) && (gcycle_angley >= 1.5 * gcycle_anglex) && (Gyr_fy_thr_ergy > 0.5 * Gyr_fx_thr_ergy))
//       {
//         // user mops with s or 8 shape movement
//         mopping_pattern[0] = 3;
//         if (pcb_test_is_running == false)
//         {
//           rtc_print_debug_timestamp();
//           shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "Mopping with S-shape movement\n");
//         }
//       }
//       else if ((MoppingFlag[0] == 1) && (Mopping_motion_gyr_flagx == 1) && (Gyr_fy_thr_ergy < 0.5 * Gyr_fx_thr_ergy))
//       {
//         // user mops with back and forth movement
//         mopping_pattern[0] = 2;
//         if (pcb_test_is_running == false)
//         {
//           rtc_print_debug_timestamp();
//           shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "Mopping with back-forth movement\n");
//         }
//       }
//       else if ((MoppingFlag[0] == 1) && (Acc_dc_avg_min >= 1.1 * EARTH_GRAVITY))
//       {
//         // user mops with unknown pattern
//         mopping_pattern[0] = 1;
//         if (pcb_test_is_running == false)
//         {
//           rtc_print_debug_timestamp();
//           shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "Mopping with unknown movement\n");
//         }
//       }
//       else
//       {
//         // user is not mopping
//         mopping_pattern[0] = 0;
//       }
//     }

//     /* Check if any flag changed */
//     if ((Frame_side[1] != Frame_side[0]) || (MoppingFlag[1] != MoppingFlag[0]) || (motion_state[1] != motion_state[0]) || (handle_in_mopping_position[1] != handle_in_mopping_position[0]) ||
//         (frame_lift_flag[1] != frame_lift_flag[0]) || (MopChangeFlag[1] != MopChangeFlag[0]) || (mopping_pattern[1] != mopping_pattern[0]))
//     {
//       flag_changed = true;
//     }

//     /* Print out all information on console */

//     if (Parameter.algo_flag_verbose)
//     {
//       if ((flag_changed == true) || ((unixtime_ms - algo_flag_verbose_upd_tsp) > 1000))
//       {
//         algo_flag_verbose_upd_tsp = unixtime_ms;

//         shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "Frame:\t FS: %d, MotionState: %d, FLF: %d, FrameHA: %f, FloorHA: %f \n",
//                       Frame_side[0],
//                       motion_state[0],
//                       frame_lift_flag[0],
//                       frame_handle_angle[0],
//                       floor_handle_angle[0]);

//         shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "Mopping:\t MF: %d, MP: %d, MC: %d, MS: %f, MCPM: %f, MCS1: %f, MCS2: %f, MCF: %d, HMP: %d MMGyrF: %d\n",
//                       MoppingFlag[0],
//                       mopping_pattern[0],
//                       mopcycles,
//                       mopping_speed,
//                       mopping_coverage_per_mop,
//                       mopping_coverage_side1,
//                       mopping_coverage_side2,
//                       MopChangeFlag[0],
//                       handle_in_mopping_position[0],
//                       Mopping_motion_gyr_flag);
//       }

//       flag_changed = false;
//     }

//     /* Save states from current cycle to compare it in the next cycle and check for any change */
//     Frame_side[1] = Frame_side[0];
//     MoppingFlag[1] = MoppingFlag[0];
//     motion_state[1] = motion_state[0];
//     handle_in_mopping_position[1] = handle_in_mopping_position[0];
//     frame_lift_flag[1] = frame_lift_flag[0];
//     MopChangeFlag[1] = MopChangeFlag[0];
//     mopping_pattern[1] = mopping_pattern[0];

//     /* Count time in each state (idle, moving, mopping */
//     if (motion_state[0] == 0)
//     {
//       time_in_idle_state++;
//     }
//     else if (motion_state[0] == 1)
//     {
//       time_in_moving_state++;
//     }
//     else if (motion_state[0] == 2)
//     {
//       time_in_mopping_state++;
//     }
//   }
//   else
//   {
//     if (RFID_IsOn == true)
//     {
//       /* Switch off delay */
//       RFID_TurnOff();          // Turns 5V boost converter off
//       RFID_ScanEnable = false; // Stops sending EPC multitag command to rfid module

//       if (Parameter.debug == true || Parameter.algo_verbose == true)
//       {
//         if (pcb_test_is_running == false)
//         {
//           rtc_print_debug_timestamp();
//           shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "Switch OFF rfid module\n");
//         }
//       }
//     }
//   }

//   /* New flag which is set every time a new RFID reading was triggered. Must be reset here*/
//   RFID_TriggeredRead = false;

//   time_since_last_cloud_transmission++;
// }

// /*!
//  * @brief Checks if the current mop already reached its maximum allowed square meter value
//  */
// void check_max_sqm_coveraged_per_mop(void)
// {
//   /* Check sqm coverage for each side (and total) */
//   if (current_mop_sides == 2)
//   {
//     if ((mob_max_sqm_reached == false) && (mopping_coverage_side1 >= (Parameter.max_sqm_coveraged_per_mop / 2.0)) && (mopping_coverage_side2 >= (Parameter.max_sqm_coveraged_per_mop / 2.0)))
//     {
//       Notification.next_state = NOTIFICATION_MAX_SQM_COVERAGE_REACHED;
//       mob_max_sqm_reached = true;

//       if (Parameter.debug == true || Parameter.algo_verbose == true)
//       {
//         if (pcb_test_is_running == false)
//         {
//           rtc_print_debug_timestamp();
//           shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_YELLOW, "Max total sqm reached\n");
//         }
//       }
//     }
//     else if ((mob_max_side1_sqm_reached == false) && (mopping_coverage_side1 >= (Parameter.max_sqm_coveraged_per_mop / 2.0)) && (mopping_coverage_per_mop < Parameter.max_sqm_coveraged_per_mop))
//     {
//       Notification.next_state = NOTIFICATION_SIDE_MAX_SQM_COVERAGE_REACHED;
//       mob_max_side1_sqm_reached = true;

//       if (Parameter.debug == true || Parameter.algo_verbose == true)
//       {
//         if (pcb_test_is_running == false)
//         {
//           rtc_print_debug_timestamp();
//           shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_YELLOW, "Max sqm side 1 reached\n");
//         }
//       }
//     }
//     else if ((mob_max_side2_sqm_reached == false) && (mopping_coverage_side2 >= (Parameter.max_sqm_coveraged_per_mop / 2.0)) && (mopping_coverage_per_mop < Parameter.max_sqm_coveraged_per_mop))
//     {
//       Notification.next_state = NOTIFICATION_SIDE_MAX_SQM_COVERAGE_REACHED;
//       mob_max_side2_sqm_reached = true;

//       if (Parameter.debug == true || Parameter.algo_verbose == true)
//       {
//         if (pcb_test_is_running == false)
//         {
//           rtc_print_debug_timestamp();
//           shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_YELLOW, "Max sqm side 2 reached\n");
//         }
//       }
//     }
//   }
//   if (current_mop_sides == 1)
//   {
//     if ((mob_max_sqm_reached == false) && (mopping_coverage_per_mop >= (Parameter.max_sqm_coveraged_per_mop / 2.0)))
//     {
//       Notification.next_state = NOTIFICATION_MAX_SQM_COVERAGE_REACHED;
//       mob_max_sqm_reached = true;

//       if (Parameter.debug == true || Parameter.algo_verbose == true)
//       {
//         if (pcb_test_is_running == false)
//         {
//           rtc_print_debug_timestamp();
//           shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_YELLOW, "Max total sqm reached for single side mop\n");
//         }
//       }
//     }
//   }
// }
