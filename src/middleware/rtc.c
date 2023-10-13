/**
 * @file rtc.c
 * @author Thomas Keilbach | keiltronic GmbH
 * @date 27 Oct 2022
 * @brief This file contains functions to communicate with the pheripherals
 * @version 1.0.0
 */

/*!
 * @defgroup Peripherals
 * @brief This file contains functions to communicate with the pheripherals
 * @{*/

#include "rtc.h"

#define AUTOSAVE_TIME 15 // 3600

struct tm *ptm;
time_t unixtime = 0LL;
time_t unixtime_ms = 0LL;
uint16_t millisec = 0;
uint16_t autosave_timer = 0;
uint8_t initial_time_update = false;

struct k_timer rtc_timer;
struct k_timer millisec_timer;

/*!
 *  @brief This is the function description
 */
void rtc_update_handler(struct k_work *work)
{
    unixtime++; // updates system time every second
         //        System.TimeSinceBoot++;
     Device.OpertingTime++;
    millisec = 0;
}
K_WORK_DEFINE(my_work, rtc_update_handler);

/*!
 *  @brief This is the function description
 */
void millisec_update_handler(struct k_work *work)
{
    if (millisec < 999)
    {
        millisec++;
    }
    else
    {
        millisec = 999;
    }
    unixtime_ms++;

    if (System.MillisecSinceBoot < 0xFFFFFFFF)
    { // Prevent overflow and starting again from 0
        System.MillisecSinceBoot++;
    }

    if (System.charger_connected == true)
    {
        System.MillisecSincecharger_connected++;
    }
    else
    {
        System.MillisecSinceUSBDisconnected++;
    }
}
K_WORK_DEFINE(my_work1, millisec_update_handler);

/*!
 *  @brief This is the function description
 */
void rtc_timer_handler(struct k_timer *dummy)
{
    k_work_submit(&my_work);
}
K_TIMER_DEFINE(rtc_timer, rtc_timer_handler, NULL);

/*!
 *  @brief This is the function description
 */
void millisec_timer_handler(struct k_timer *dummy)
{
    k_work_submit(&my_work1);
}
K_TIMER_DEFINE(millisec_timer, millisec_timer_handler, NULL);

/*!
 *  @brief This is the function description
 */
void init_rtc(void)
{
    k_timer_init(&rtc_timer, rtc_update_handler, NULL);
    k_timer_init(&millisec_timer, millisec_update_handler, NULL);

    /* start periodic timer that expires once every second */
    k_timer_start(&rtc_timer, K_SECONDS(1), K_SECONDS(1));
    k_timer_start(&millisec_timer, K_MSEC(1), K_MSEC(1));

    /* Set defult unix time and time zone */
    unixtime = 1609459200LL; // 01.01.2021, 00:00:00
    unixtime_ms = DEFAULT_UNIX_TIME;

    /* convert from unix to local time */
    ptm = localtime(&unixtime);
}

/*!
 *  @brief This is the function description
 */
void rtc_print_timestamp(void)
{
    char buf[50] = {0};

    /* convert from unix to local time */
    ptm = localtime(&unixtime);

    strftime(buf, 50, "%FT%T", ptm);
    shell_print(shell_backend_uart_get_ptr(), "%s:%d", buf, millisec);
}

/*!
 *  @brief This is the function description
 */
void rtc_print_time(void)
{
    char buf[30] = {0};

    /* convert from unix to local time */
    ptm = localtime(&unixtime);

    strftime(buf, 15, "%T", ptm);
    shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "%s:%03d", buf, millisec);
}

/*!
 *  @brief This is the function description
 */
void rtc_print_date(void)
{
    char buf[15] = {0};

    /* convert from unix to local time */
    ptm = localtime(&unixtime);

    strftime(buf, 15, "%F", ptm);
    shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "%s", buf);
}

/*!
 *  @brief This is the function description
 */
void rtc_print_weekday(void)
{
    char buf[15] = {0};

    /* convert from unix to local time */
    ptm = localtime(&unixtime);

    strftime(buf, 15, "%A", ptm);
    shell_print(shell_backend_uart_get_ptr(), "%s", buf);
}

/*!
 *  @brief This is the function description
 */
void rtc_print_timezone(void)
{
    char buf[15] = {0};

    /* convert from unix to local time */
    ptm = localtime(&unixtime);

    strftime(buf, 15, "%z %Z", ptm);
    shell_print(shell_backend_uart_get_ptr(), "%s", buf);
}

/*!
 *  @brief This is the function description
 */
void rtc_print_unixtime(uint64_t unixtime)
{
    printk("%lld", (uint64_t)unixtime);
}

/*!
 *  @brief This is the function description
 */
void rtc_print_unixtime_ms(uint64_t unixtime, uint16_t millisec)
{
    printk("%lld%d", unixtime, millisec);
}

/*!
 *  @brief This is the function description
 */
bool rtc_fetch_date_time(void)
{
    int8_t err = 0;

    /* Get the current date from network provider or NTP sever */
    err = date_time_now(&unixtime_ms);

    if (err == 0)
    {
        unixtime = unixtime_ms / 1000ULL;
        millisec = unixtime_ms % 1000;

        if ((initial_time_update == false) && (modem.connection_stat == true))
        {
            if (datalog_ReadOutisActive == false)
            {
                /* Debug message */
                if (pcb_test_is_running == false)
                {
                    rtc_print_debug_timestamp();
                    shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "Initial time and date update successful\n");
                }

                /* After the real time is known update all previous created time stamps with correct time */
                Event_BackwardsTimeStampUpdate(unixtime_ms, System.MillisecSinceBoot);

                initial_time_update = true;
            }
        }

        return 0;
    }
    else
    {
        return 1;
    }
}

/*!
 *  @brief This is the function description
 */
void rtc_print_debug_timestamp(void)
{
    shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "[ ");
    rtc_print_date();
    shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, " ");
    rtc_print_time();
    shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, " ] ");
}