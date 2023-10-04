/**
 * @file events.c
 * @author Thomas Keilbach | keiltronic GmbH
 * @date 27 Oct 2022
 * @brief This files contains function to create events which will get send to the cloud
 * @version 1.0.0
 */

/*!
 * @defgroup Events
 * @brief This files contains function to create events which will get send to the cloud
 * @{*/

#include "events.h"

EventArray myEventArray = EVENT_ARRAY__INIT;
EventArray__EventArrayEntry my_event_array_entries[EVENT_MAX_ITEMS_IN_ARRAY];
EventArray__EventArrayEntry *my_event_array_entries_pointer[EVENT_MAX_ITEMS_IN_ARRAY];
ProtobufCBinaryData binary_data;
uint8_t current_location_epc_string[20];
uint32_t Event_ItemsInArray = 0; // Counts the element in the buffer
bool event_simulation_in_progress = false;
uint16_t event1statistics_interval_timer = 0;

/**
 * @brief This function checks if the Event is already listed in event array and if it is allowed to be stored again
 * @param NewEvent: Current generic event
 * @return uint8_t: 0 (false) if event should be added to event array, 1 (true) if it is already listed
 */
uint8_t Event_filter(GenericEvent *NewEvent)
{
  uint32_t i = 0;

  for (i = 0; i < Event_ItemsInArray; i++)
  {
    /* Check if new event number is already listed somewhere in array */
    if (NewEvent->one_of_generic_event_case == my_event_array_entries[i].value->one_of_generic_event_case)
    {
      switch (NewEvent->one_of_generic_event_case)
      {

      case GENERIC_EVENT__ONE_OF_GENERIC_EVENT_FIELD_EVENT0X05:

        if (NewEvent->field_event0x05->mop_id == my_event_array_entries[i].value->field_event0x05->mop_id)
        {
          if (NewEvent->field_event0x05->room_id == my_event_array_entries[i].value->field_event0x05->room_id)
          {
            return true; // Discard adding event in event array
          }
          else
          {
            return false; // Allow adding event in event array
          }
        }
        else
        {
          return false; // Allow adding event in event array
        }
        break;

      case GENERIC_EVENT__ONE_OF_GENERIC_EVENT_FIELD_EVENT0X06:

        if (NewEvent->field_event0x06->mop_id == my_event_array_entries[i].value->field_event0x06->mop_id)
        {
          if (NewEvent->field_event0x06->room_id == my_event_array_entries[i].value->field_event0x06->room_id)
          {
            return true; // Discard adding event in event array
          }
          else
          {
            return false; // Allow adding event in event array
          }
        }
        else
        {
          return false; // Allow adding event in event array
        }
        break;

      case GENERIC_EVENT__ONE_OF_GENERIC_EVENT_FIELD_EVENT0X19:

        if (NewEvent->field_event0x19->mop_id == my_event_array_entries[i].value->field_event0x19->mop_id)
        {
          if (NewEvent->field_event0x19->room_id == my_event_array_entries[i].value->field_event0x19->room_id)
          {
            return true; // Discard adding event in event array
          }
          else
          {
            return false; // Allow adding event in event array
          }
        }
        else
        {
          return false; // Allow adding event in event array
        }
        break;

      default:
        return false;
        break;
      }
    }
  }
  return false;
}

/*!
 *  @brief This is the function description
 *  @details: 0x01 - New movement detected
 */
void Event_AddInArray(GenericEvent *NewEvent)
{
  /* Add newest event */
  if (Event_ItemsInArray < EVENT_MAX_ITEMS_IN_ARRAY)
  {
    my_event_array_entries[Event_ItemsInArray].key = ++System.EventNumber;
    my_event_array_entries[Event_ItemsInArray].value = NewEvent;
    my_event_array_entries_pointer[Event_ItemsInArray] = &my_event_array_entries[Event_ItemsInArray];
    myEventArray.event_array = my_event_array_entries_pointer;
    myEventArray.n_event_array = ++Event_ItemsInArray;
  }

  /* If local event buffer in RAm is full, pack (protobuf) and outsource the whole buffer to the external flash memory to free the local event buffer in RAM */
  if (Event_ItemsInArray >= EVENT_MAX_ITEMS_IN_ARRAY)
  {
    Event_StorePackedUsageObjectToFlash();

    /* Clear event array in RAM */
    Event_ClearArray();
    clear_last_seen_location_record_array();
  }
}

/*!
 *  @brief This funtion frees all object generated with k_malloc
 *  @details: For ech event there are two object to free: The Event0xXX and a GenericEvent object
 */
void Event_ClearArray(void)
{
  uint16_t i = 0;

  for (i = 0; i < Event_ItemsInArray; i++)
  {
    /* Free specific event */
    switch (my_event_array_entries[i].value->one_of_generic_event_case)
    {
    case GENERIC_EVENT__ONE_OF_GENERIC_EVENT_FIELD_EVENT0X01:
      free(my_event_array_entries[i].value->field_event0x01);
      break;

    case GENERIC_EVENT__ONE_OF_GENERIC_EVENT_FIELD_EVENT0X02:
      free(my_event_array_entries[i].value->field_event0x02);
      break;

    case GENERIC_EVENT__ONE_OF_GENERIC_EVENT_FIELD_EVENT0X04:
      free(my_event_array_entries[i].value->field_event0x04);
      break;

    case GENERIC_EVENT__ONE_OF_GENERIC_EVENT_FIELD_EVENT0X05:
      free(my_event_array_entries[i].value->field_event0x05);
      break;

    case GENERIC_EVENT__ONE_OF_GENERIC_EVENT_FIELD_EVENT0X06:
      free(my_event_array_entries[i].value->field_event0x06);
      break;

    case GENERIC_EVENT__ONE_OF_GENERIC_EVENT_FIELD_EVENT0X07:
      free(my_event_array_entries[i].value->field_event0x07);
      break;

    case GENERIC_EVENT__ONE_OF_GENERIC_EVENT_FIELD_EVENT0X09:
      free(my_event_array_entries[i].value->field_event0x09);
      break;

    case GENERIC_EVENT__ONE_OF_GENERIC_EVENT_FIELD_EVENT0X0_B:
      free(my_event_array_entries[i].value->field_event0x0b);
      break;

    case GENERIC_EVENT__ONE_OF_GENERIC_EVENT_FIELD_EVENT0X0_C:
      free(my_event_array_entries[i].value->field_event0x0c);
      break;

    case GENERIC_EVENT__ONE_OF_GENERIC_EVENT_FIELD_EVENT0X0_D:
      free(my_event_array_entries[i].value->field_event0x0d);
      break;

    case GENERIC_EVENT__ONE_OF_GENERIC_EVENT_FIELD_EVENT0X0_F:
      free(my_event_array_entries[i].value->field_event0x0f);
      break;

    case GENERIC_EVENT__ONE_OF_GENERIC_EVENT_FIELD_EVENT0X10:
      free(my_event_array_entries[i].value->field_event0x10);
      break;

    case GENERIC_EVENT__ONE_OF_GENERIC_EVENT_FIELD_EVENT0X11:
      free(my_event_array_entries[i].value->field_event0x11);
      break;

    case GENERIC_EVENT__ONE_OF_GENERIC_EVENT_FIELD_EVENT0X12:
      free(my_event_array_entries[i].value->field_event0x12);
      break;

    case GENERIC_EVENT__ONE_OF_GENERIC_EVENT_FIELD_EVENT0X13:
      free(my_event_array_entries[i].value->field_event0x13);
      break;

    case GENERIC_EVENT__ONE_OF_GENERIC_EVENT_FIELD_EVENT0X17:
      free(my_event_array_entries[i].value->field_event0x17);
      break;

    case GENERIC_EVENT__ONE_OF_GENERIC_EVENT_FIELD_EVENT0X18:
      free(my_event_array_entries[i].value->field_event0x18);
      break;

    case GENERIC_EVENT__ONE_OF_GENERIC_EVENT_FIELD_EVENT0X19:
      free(my_event_array_entries[i].value->field_event0x19);
      break;

    case GENERIC_EVENT__ONE_OF_GENERIC_EVENT_FIELD_EVENT0X1_A:
      free(my_event_array_entries[i].value->field_event0x1a);
      break;

    case GENERIC_EVENT__ONE_OF_GENERIC_EVENT_FIELD_EVENT0X1_B:
      free(my_event_array_entries[i].value->field_event0x1b);
      break;

    case GENERIC_EVENT__ONE_OF_GENERIC_EVENT_FIELD_EVENT0X1_C:
      free(my_event_array_entries[i].value->field_event0x1c);
      break;

    case GENERIC_EVENT__ONE_OF_GENERIC_EVENT_FIELD_EVENT0X_FF:
      free(my_event_array_entries[i].value->field_event0xff);
      break;

    default:
      break;
    }

    /* Free the GenericEvent object */
    free(my_event_array_entries[i].value);
  }

  Event_ItemsInArray = 0;
  myEventArray.n_event_array = 0;

  for (i = 0; i < EVENT_MAX_ITEMS_IN_ARRAY; i++)
  {
    event_array__event_array_entry__init(&my_event_array_entries[i]);
    my_event_array_entries_pointer[i] = NULL;
  }
  memset(current_location_epc_string, 0, EPC_TOTAL_HEX_LENGTH);

  binary_data.len = EPC_TOTAL_HEX_LENGTH;
  binary_data.data = current_location_epc_string;

  if ((Parameter.events_verbose == true) && (pcb_test_is_running == false))
  {
    rtc_print_debug_timestamp();
    shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_BLUE, "Cleared event array in RAM\n");
  }
}

/*!
 *  @brief This is the function description
 *  @details: 0x01 - New movement detected
 */
void Event_PrintPreviousEvents(void)
{
  uint16_t i = 0;
  struct tm *ptm;
  char buf[50] = {0};
  uint16_t milli = 0;
  int64_t timestamp = 0;

  for (i = 0; i < Event_ItemsInArray; i++)
  {
    /* Fetch unix time stamp from event and convert it to human readable date and time in millisec resolution */
    timestamp = my_event_array_entries[i].value->field_event0x01->event_timestamp;

    milli = timestamp % 1000LL; // Get only the millisec from the unix time stamp
    timestamp /= 1000LL;        // time.h can only handly timestamp in sec resolution, so cut the millisec

    /* convert from unix to local time */
    ptm = localtime(&timestamp);
    strftime(buf, 20, "%F %T", ptm);

    rtc_print_debug_timestamp();
    shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "Event no. %d: 0x%02X, %s:%03d\n", my_event_array_entries[i].key, my_event_array_entries[i].value->field_event0x01->event_id, buf, milli);
  }
}

/*!
 *  @brief This is the function description
 *  @details: 0x01 - New movement detected
 */
void Event_BackwardsTimeStampUpdate(time_t initial_unixtime_ms, uint32_t ms_since_boot)
{
  uint16_t i = 0;

  /* Events have a time stamp value which is the time since boot. If real time is availible, update event time stamp by substracting the time since boot from current time */
  for (i = Event_ItemsInArray; i > 0; i--)
  {
    my_event_array_entries[i - 1].value->field_event0x01->event_timestamp = (initial_unixtime_ms - (time_t)ms_since_boot) + (my_event_array_entries[i - 1].value->field_event0x01->event_timestamp - DEFAULT_UNIX_TIME);
  }

  /* Force logic to send data immediately */
  coap_last_transmission_timer = Parameter.cloud_sync_interval_idle + Parameter.cloud_sync_interval_moving;

  if ((Parameter.events_verbose == true) && (pcb_test_is_running == false))
  {
    rtc_print_debug_timestamp();
    shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_BLUE, "Updated all previous events with correct time stamp\n");
  }
}

/*!
 *  @brief This is the function description
 *  @details: 0x01 - New movement detected
 */
void NewEvent0x01(time_t interval_start, time_t interval_end, float pattern_idle, float pattern_moving, float pattern_mopping)
{
  Event0x01 *ptrNewEvent = malloc(sizeof(Event0x01));

  if (ptrNewEvent != NULL)
  {
    event0x01__init(ptrNewEvent);

    ptrNewEvent->event_id = 0x01;
    ptrNewEvent->event_timestamp = unixtime_ms;
    ptrNewEvent->interval_start = interval_start;
    ptrNewEvent->interval_end = interval_end;
    ptrNewEvent->pattern_idle = pattern_idle;
    ptrNewEvent->pattern_moving = pattern_moving;
    ptrNewEvent->pattern_mopping = pattern_mopping;

    GenericEvent *ptrNewGenericEvent = malloc(sizeof(GenericEvent));

    if (ptrNewGenericEvent != NULL)
    {
      generic_event__init(ptrNewGenericEvent);

      ptrNewGenericEvent->one_of_generic_event_case = GENERIC_EVENT__ONE_OF_GENERIC_EVENT_FIELD_EVENT0X01;
      ptrNewGenericEvent->field_event0x01 = ptrNewEvent;

      /* Add this event in event array */
      if (Event_filter(ptrNewGenericEvent) == false) // Event is not listed in event array yet
      {
        if ((Parameter.events_verbose == true) && (pcb_test_is_running == false))
        {
          rtc_print_debug_timestamp();
          shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_BLUE, "Created Event0x01 'New movement detected', Event number: %d. idle: %.2f\%, moving: %.2f\%, mopping: %.2f\%\n", System.EventNumber, pattern_idle, pattern_moving, pattern_mopping);
        }

        Event_AddInArray(ptrNewGenericEvent);
      }
      else
      {
        /* Free allocated memory */
        free(ptrNewEvent);
        free(ptrNewGenericEvent);
      }
    }
    else
    {
      if ((Parameter.events_verbose == true) && (pcb_test_is_running == false))
      {
        rtc_print_debug_timestamp();
        shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_RED, "Could not allocate memory for NewGenericEvent in Event0x01\n");
      }

      /* Free allocated memory */
      free(ptrNewEvent);
    }
  }
  else
  {
    if ((Parameter.events_verbose == true) && (pcb_test_is_running == false))
    {
      rtc_print_debug_timestamp();
      shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_RED, "Could not allocate memory for NewEvent0x01\n");
    }
  }

  /* Trigger user notification */
  if (Parameter.notification_test == false)
  {
    Notification.next_state = ActionMatrixArray[0x01];
  }
}

/*!
 *  @brief This is the function description
 *  @details: 0x02 - Mop change detected
 */
void NewEvent0x02(uint32_t mop_id)
{
  Event0x02 *ptrNewEvent = malloc(sizeof(Event0x02));

  if (ptrNewEvent != NULL)
  {
    event0x02__init(ptrNewEvent);

    ptrNewEvent->event_id = 0x02;
    ptrNewEvent->event_timestamp = unixtime_ms;
    ptrNewEvent->mop_id = mop_id;

    GenericEvent *ptrNewGenericEvent = malloc(sizeof(GenericEvent));

    if (ptrNewGenericEvent != NULL)
    {
      generic_event__init(ptrNewGenericEvent);

      ptrNewGenericEvent->one_of_generic_event_case = GENERIC_EVENT__ONE_OF_GENERIC_EVENT_FIELD_EVENT0X02;
      ptrNewGenericEvent->field_event0x02 = ptrNewEvent;

      /* Add this event in event array */
      if (Event_filter(ptrNewGenericEvent) == false) // Event is not listed in event array yet
      {
        if ((Parameter.events_verbose == true) && (pcb_test_is_running == false))
        {
          rtc_print_debug_timestamp();

          if (mop_id > 0)
          {
            shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_BLUE, "Created Event0x02 'Mop change detected, new mop id: %d', Event number: %d\n", mop_id, System.EventNumber);
          }
          else
          {
            shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_BLUE, "Created Event0x02 'Mop change detected' , new mop id: %d, without user notification, Event number: %d\n", mop_id, System.EventNumber);
          }
        }

        Event_AddInArray(ptrNewGenericEvent);
      }
      else
      {
        /* Free allocated memory */
        free(ptrNewEvent);
        free(ptrNewGenericEvent);
      }
    }
    else
    {
      if ((Parameter.events_verbose == true) && (pcb_test_is_running == false))
      {
        rtc_print_debug_timestamp();
        shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_RED, "Could not allocate memory for NewGenericEvent in Event0x02\n");
      }

      /* Free allocated memory */
      free(ptrNewEvent);
    }
  }
  else
  {
    if ((Parameter.events_verbose == true) && (pcb_test_is_running == false))
    {
      rtc_print_debug_timestamp();
      shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_RED, "Could not allocate memory for NewEvent0x02\n");
    }
  }

  /* Trigger user notification */
  if ((Parameter.notification_test == false) && (mop_id > 0))
  {
    Notification.next_state = ActionMatrixArray[0x02];
  }
}

/*!
 *  @brief This is the function description
 *  @details: 0x04 - Another room detected
 */
void NewEvent0x04(uint32_t room_id)
{
  Event0x04 *ptrNewEvent = malloc(sizeof(Event0x04));

  if (ptrNewEvent != NULL)
  {
    event0x04__init(ptrNewEvent);

    ptrNewEvent->event_id = 0x04;
    ptrNewEvent->event_timestamp = unixtime_ms;
    ptrNewEvent->room_id = room_id;

    GenericEvent *ptrNewGenericEvent = malloc(sizeof(GenericEvent));

    if (ptrNewGenericEvent != NULL)
    {
      generic_event__init(ptrNewGenericEvent);

      ptrNewGenericEvent->one_of_generic_event_case = GENERIC_EVENT__ONE_OF_GENERIC_EVENT_FIELD_EVENT0X04;
      ptrNewGenericEvent->field_event0x04 = ptrNewEvent;

      /* Add this event in event array */
      if (Event_filter(ptrNewGenericEvent) == false) // Event is not listed in event array yet
      {
        if ((Parameter.events_verbose == true) && (pcb_test_is_running == false))
        {
          rtc_print_debug_timestamp();
          shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_BLUE, "Created Event0x04 'Another room detected', Event number: %d\n", System.EventNumber);
        }

        Event_AddInArray(ptrNewGenericEvent);
      }
      else
      {
        /* Free allocated memory */
        free(ptrNewEvent);
        free(ptrNewGenericEvent);
      }
    }
    else
    {
      if ((Parameter.events_verbose == true) && (pcb_test_is_running == false))
      {
        rtc_print_debug_timestamp();
        shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_RED, "Could not allocate memory for NewGenericEvent in Event0x04\n");
      }

      /* Free allocated memory */
      free(ptrNewEvent);
    }
  }
  else
  {
    if ((Parameter.events_verbose == true) && (pcb_test_is_running == false))
    {
      rtc_print_debug_timestamp();
      shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_RED, "Could not allocate memory for NewEvent0x04\n");
    }
  }

  /* Trigger user notification */
  if (Parameter.notification_test == false)
  {
    Notification.next_state = ActionMatrixArray[0x04];
  }
}

/*!
 *  @brief This is the function description
 *  @details: 0x05 - Wrong mop is used in the room
 */
void NewEvent0x05(uint32_t room_id, uint32_t mop_id)
{
  Event0x05 *ptrNewEvent = malloc(sizeof(Event0x05));

  if (ptrNewEvent != NULL)
  {
    event0x05__init(ptrNewEvent);

    ptrNewEvent->event_id = 0x05;
    ptrNewEvent->event_timestamp = unixtime_ms;
    ptrNewEvent->room_id = room_id;
    ptrNewEvent->mop_id = mop_id;

    GenericEvent *ptrNewGenericEvent = malloc(sizeof(GenericEvent));

    if (ptrNewGenericEvent != NULL)
    {
      generic_event__init(ptrNewGenericEvent);

      ptrNewGenericEvent->one_of_generic_event_case = GENERIC_EVENT__ONE_OF_GENERIC_EVENT_FIELD_EVENT0X05;
      ptrNewGenericEvent->field_event0x05 = ptrNewEvent;

      /* Add this event in event array */
      if (Event_filter(ptrNewGenericEvent) == false) // Event is not listed in event array yet
      {
        if ((Parameter.events_verbose == true) && (pcb_test_is_running == false))
        {
          rtc_print_debug_timestamp();
          shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_BLUE, "Created Event0x05 'Wrong mop is used in the room', Event number: %d\n", System.EventNumber);
        }

        Event_AddInArray(ptrNewGenericEvent);
      }
      else
      {
        /* Free allocated memory */
        free(ptrNewEvent);
        free(ptrNewGenericEvent);
      }
    }
    else
    {
      if ((Parameter.events_verbose == true) && (pcb_test_is_running == false))
      {
        rtc_print_debug_timestamp();
        shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_RED, "Could not allocate memory for NewGenericEvent in Event0x05\n");
      }

      /* Free allocated memory */
      free(ptrNewEvent);
    }
  }
  else
  {
    if ((Parameter.events_verbose == true) && (pcb_test_is_running == false))
    {
      rtc_print_debug_timestamp();
      shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_RED, "Could not allocate memory for NewEvent0x05\n");
    }
  }

  /* Trigger user notification */
  if (Parameter.notification_test == false)
  {
    Notification.next_state = ActionMatrixArray[0x05];
  }
}

/*!
 *  @brief This is the function description
 *  @details: 0x06 - Same mop used in another room
 */
void NewEvent0x06(uint32_t room_id, uint32_t mop_id)
{
  Event0x06 *ptrNewEvent = malloc(sizeof(Event0x06));

  if (ptrNewEvent != NULL)
  {
    event0x06__init(ptrNewEvent);

    ptrNewEvent->event_id = 0x06;
    ptrNewEvent->event_timestamp = unixtime_ms;
    ptrNewEvent->room_id = room_id;
    ptrNewEvent->mop_id = mop_id;

    GenericEvent *ptrNewGenericEvent = malloc(sizeof(GenericEvent));

    if (ptrNewGenericEvent != NULL)
    {
      generic_event__init(ptrNewGenericEvent);

      ptrNewGenericEvent->one_of_generic_event_case = GENERIC_EVENT__ONE_OF_GENERIC_EVENT_FIELD_EVENT0X06;
      ptrNewGenericEvent->field_event0x06 = ptrNewEvent;

      /* Add this event in event array */
      if (Event_filter(ptrNewGenericEvent) == false) // Event is not listed in event array yet
      {
        if ((Parameter.events_verbose == true) && (pcb_test_is_running == false))
        {
          rtc_print_debug_timestamp();
          shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_BLUE, "Created Event0x06 'Same mop used in another room', Event number: %d\n", System.EventNumber);
        }

        Event_AddInArray(ptrNewGenericEvent);
      }
      else
      {
        /* Free allocated memory */
        free(ptrNewEvent);
        free(ptrNewGenericEvent);
      }
    }
    else
    {
      if ((Parameter.events_verbose == true) && (pcb_test_is_running == false))
      {
        rtc_print_debug_timestamp();
        shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_RED, "Could not allocate memory for NewGenericEvent in Event0x06\n");
      }

      /* Free allocated memory */
      free(ptrNewEvent);
    }
  }
  else
  {
    if ((Parameter.events_verbose == true) && (pcb_test_is_running == false))
    {
      rtc_print_debug_timestamp();
      shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_RED, "Could not allocate memory for NewEvent0x06\n");
    }
  }

  /* Trigger user notification */
  if (Parameter.notification_test == false)
  {
    Notification.next_state = ActionMatrixArray[0x06];
  }
}

/*!
 *  @brief This is the function description
 *  @details: 0x07 - New location detected
 */
extern void NewEvent0x07(uint8_t *location_epc, uint8_t epc_len) // 0x07 - New location detected
{
  Event0x07 *ptrNewEvent = malloc(sizeof(Event0x07));

  if (ptrNewEvent != NULL)
  {
    event0x07__init(ptrNewEvent);

    ptrNewEvent->event_id = 0x07;
    ptrNewEvent->event_timestamp = unixtime_ms;

    /* Copy current location epc string also into a variable which is used in DeviceStatus object */
    memcpy(current_location_epc_string, location_epc, epc_len);

    /* Copy current location in allocated memory */
    binary_data.len = epc_len;
    binary_data.data = location_epc;

    ptrNewEvent->location_epc = binary_data;

    /* Create a new GeneraticEvent */
    GenericEvent *ptrNewGenericEvent = malloc(sizeof(GenericEvent));

    if (ptrNewGenericEvent != NULL)
    {
      generic_event__init(ptrNewGenericEvent);

      ptrNewGenericEvent->one_of_generic_event_case = GENERIC_EVENT__ONE_OF_GENERIC_EVENT_FIELD_EVENT0X07;
      ptrNewGenericEvent->field_event0x07 = ptrNewEvent;

      /* Add this event in event array */
      if (Event_filter(ptrNewGenericEvent) == false) // Event is not listed in event array yet
      {
        if ((Parameter.events_verbose == true) && (pcb_test_is_running == false))
        {
          rtc_print_debug_timestamp();
          shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_BLUE, "Created Event0x07 'New location detected, tag epc: %s', Event number: %d\n", DeserializeHexToChar(ptrNewGenericEvent->field_event0x07->location_epc.data, 20), System.EventNumber);
        }

        Event_AddInArray(ptrNewGenericEvent);
      }
      else
      {
        /* Free allocated memory */
        free(ptrNewEvent);
        free(ptrNewGenericEvent);
      }
    }
    else
    {
      if ((Parameter.events_verbose == true) && (pcb_test_is_running == false))
      {
        rtc_print_debug_timestamp();
        shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_RED, "Could not allocate memory for NewGenericEvent in Event0x07, Event number: %d\n", System.EventNumber);
      }

      /* Free allocated memory */
      free(ptrNewEvent);
    }
  }

  /* Trigger user notification */
  if (Parameter.notification_test == false)
  {
    Notification.next_state = ActionMatrixArray[0x07];
  }
}

/*!
 *  @brief This is the function description
 *  @details: From Hub to device -> notifications are only used to activate LED and / or sound on the Device remotely from the Hub
 */
void NewEvent0x08(uint32_t notification_id, uint64_t notification_ttl, uint8_t notficiation_pattern_id, uint8_t notification_led_power, uint8_t notification_sound_power)
{
}

/*!
 *  @brief This is the function description
 *  @details: 0x09 - Battery charge low
 */
void NewEvent0x09(void)
{
  Event0x09 *ptrNewEvent = malloc(sizeof(Event0x09));

  if (ptrNewEvent != NULL)
  {
    event0x09__init(ptrNewEvent);

    ptrNewEvent->event_id = 0x09;
    ptrNewEvent->event_timestamp = unixtime_ms;

    GenericEvent *ptrNewGenericEvent = malloc(sizeof(GenericEvent));

    if (ptrNewGenericEvent != NULL)
    {
      generic_event__init(ptrNewGenericEvent);

      ptrNewGenericEvent->one_of_generic_event_case = GENERIC_EVENT__ONE_OF_GENERIC_EVENT_FIELD_EVENT0X09;
      ptrNewGenericEvent->field_event0x09 = ptrNewEvent;

      /* Add this event in event array */
      if (Event_filter(ptrNewGenericEvent) == false) // Event is not listed in event array yet
      {
        if ((Parameter.events_verbose == true) && (pcb_test_is_running == false))
        {
          rtc_print_debug_timestamp();
          shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_BLUE, "Created Event0x09 'Battery charge low'\n");
        }

        Event_AddInArray(ptrNewGenericEvent);
      }
      else
      {
        /* Free allocated memory */
        free(ptrNewEvent);
        free(ptrNewGenericEvent);
      }
    }
    else
    {
      if ((Parameter.events_verbose == true) && (pcb_test_is_running == false))
      {
        rtc_print_debug_timestamp();
        shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_RED, "Could not allocate memory for NewGenericEvent in Event0x09, Event number: %d\n", System.EventNumber);
      }

      /* Free allocated memory */
      free(ptrNewEvent);
    }
  }
  else
  {
    if ((Parameter.events_verbose == true) && (pcb_test_is_running == false))
    {
      rtc_print_debug_timestamp();
      shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_RED, "Could not allocate memory for NewEvent0x09\n");
    }
  }

  /* Trigger user notification */
  if (Parameter.notification_test == false)
  {
    Notification.next_state = ActionMatrixArray[0x09];
  }
}

/*!
 *  @brief This is the function description
 *  @details: 0x0B - Connection up
 */
void NewEvent0x0B(void)
{
  Event0x0B *ptrNewEvent = malloc(sizeof(Event0x0B));

  if (ptrNewEvent != NULL)
  {
    event0x0_b__init(ptrNewEvent);

    ptrNewEvent->event_id = 0x0B;
    ptrNewEvent->event_timestamp = unixtime_ms;

    GenericEvent *ptrNewGenericEvent = malloc(sizeof(GenericEvent));

    if (ptrNewGenericEvent != NULL)
    {
      generic_event__init(ptrNewGenericEvent);

      ptrNewGenericEvent->one_of_generic_event_case = GENERIC_EVENT__ONE_OF_GENERIC_EVENT_FIELD_EVENT0X0_B;
      ptrNewGenericEvent->field_event0x0b = ptrNewEvent;

      /* Add this event in event array */
      if (Event_filter(ptrNewGenericEvent) == false) // Event is not listed in event array yet
      {
        if ((Parameter.events_verbose == true) && (pcb_test_is_running == false))
        {
          rtc_print_debug_timestamp();
          shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_BLUE, "Created Event0x0B 'Connection up', Event number: %d\n", System.EventNumber);
        }

        Event_AddInArray(ptrNewGenericEvent);
      }
      else
      {
        /* Free allocated memory */
        free(ptrNewEvent);
        free(ptrNewGenericEvent);
      }
    }
    else
    {
      if ((Parameter.events_verbose == true) && (pcb_test_is_running == false))
      {
        rtc_print_debug_timestamp();
        shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_RED, "Could not allocate memory for NewGenericEvent in Event0x0B, Event number: %d\n", System.EventNumber);
      }

      /* Free allocated memory */
      free(ptrNewEvent);
    }
  }
  else
  {
    if ((Parameter.events_verbose == true) && (pcb_test_is_running == false))
    {
      rtc_print_debug_timestamp();
      shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_RED, "Could not allocate memory for NewEvent0x0B\n");
    }
  }

  /* Trigger user notification */
  if (Parameter.notification_test == false)
  {
    Notification.next_state = ActionMatrixArray[0x0B];
  }
}

/*!
 *  @brief This is the function description
 *  @details: 0x0C - Connection down
 */
void NewEvent0x0C(void)
{
  Event0x0C *ptrNewEvent = malloc(sizeof(Event0x0C));

  if (ptrNewEvent != NULL)
  {
    event0x0_c__init(ptrNewEvent);

    ptrNewEvent->event_id = 0x0C;
    ptrNewEvent->event_timestamp = unixtime_ms;

    GenericEvent *ptrNewGenericEvent = malloc(sizeof(GenericEvent));

    if (ptrNewGenericEvent != NULL)
    {
      generic_event__init(ptrNewGenericEvent);

      ptrNewGenericEvent->one_of_generic_event_case = GENERIC_EVENT__ONE_OF_GENERIC_EVENT_FIELD_EVENT0X0_C;
      ptrNewGenericEvent->field_event0x0c = ptrNewEvent;

      /* Add this event in event array */
      if (Event_filter(ptrNewGenericEvent) == false) // Event is not listed in event array yet
      {
        if ((Parameter.events_verbose == true) && (pcb_test_is_running == false))
        {
          rtc_print_debug_timestamp();
          shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_BLUE, "Created Event0x0C 'Connection down', Event number: %d\n", System.EventNumber);
        }

        Event_AddInArray(ptrNewGenericEvent);
      }
      else
      {
        /* Free allocated memory */
        free(ptrNewEvent);
        free(ptrNewGenericEvent);
      }
    }
    else
    {
      if ((Parameter.events_verbose == true) && (pcb_test_is_running == false))
      {
        rtc_print_debug_timestamp();
        shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_RED, "Could not allocate memory for NewGenericEvent in Event0x0C\n");
      }

      /* Free allocated memory */
      free(ptrNewEvent);
    }
  }
  else
  {
    if ((Parameter.events_verbose == true) && (pcb_test_is_running == false))
    {
      rtc_print_debug_timestamp();
      shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_RED, "Could not allocate memory for NewEvent0x0C\n");
    }
  }

  /* Trigger user notification */
  if (Parameter.notification_test == false)
  {
    Notification.next_state = ActionMatrixArray[0x0C];
  }
}

/*!
 *  @brief This is the function description
 *  @details: 0x0D - Button pressed
 */
void NewEvent0x0D(void)
{
  Event0x0D *ptrNewEvent = malloc(sizeof(Event0x0D));

  if (ptrNewEvent != NULL)
  {
    event0x0_d__init(ptrNewEvent);

    ptrNewEvent->event_id = 0x0D;
    ptrNewEvent->event_timestamp = unixtime_ms;

    GenericEvent *ptrNewGenericEvent = malloc(sizeof(GenericEvent));

    if (ptrNewGenericEvent != NULL)
    {
      generic_event__init(ptrNewGenericEvent);

      ptrNewGenericEvent->one_of_generic_event_case = GENERIC_EVENT__ONE_OF_GENERIC_EVENT_FIELD_EVENT0X0_D;
      ptrNewGenericEvent->field_event0x0d = ptrNewEvent;

      /* Add this event in event array */
      if (Event_filter(ptrNewGenericEvent) == false) // Event is not listed in event array yet
      {
        if ((Parameter.events_verbose == true) && (pcb_test_is_running == false))
        {
          rtc_print_debug_timestamp();
          shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_BLUE, "Created Event0x0D 'Button pressed', Event number: %d\n", System.EventNumber);
        }

        Event_AddInArray(ptrNewGenericEvent);
      }
      else
      {
        /* Free allocated memory */
        free(ptrNewEvent);
        free(ptrNewGenericEvent);
      }
    }
    else
    {
      if ((Parameter.events_verbose == true) && (pcb_test_is_running == false))
      {
        rtc_print_debug_timestamp();
        shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_RED, "Could not allocate memory for NewGenericEvent in Event0x0D\n");
      }

      /* Free allocated memory */
      free(ptrNewEvent);
    }
  }
  else
  {
    if ((Parameter.events_verbose == true) && (pcb_test_is_running == false))
    {
      rtc_print_debug_timestamp();
      shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_RED, "Could not allocate memory for NewEvent0x0D\n");
    }
  }

  /* Trigger user notification */
  if (Parameter.notification_test == false)
  {
    Notification.next_state = ActionMatrixArray[0x0D];
  }
}

/*!
 *  @brief This is the function description
 *  @details: 0x0F - USB plugged in
 */
void NewEvent0x0F(void)
{
  Event0x0F *ptrNewEvent = malloc(sizeof(Event0x0F));

  if (ptrNewEvent != NULL)
  {
    event0x0_f__init(ptrNewEvent);

    ptrNewEvent->event_id = 0x0F;
    ptrNewEvent->event_timestamp = unixtime_ms;

    GenericEvent *ptrNewGenericEvent = malloc(sizeof(GenericEvent));

    if (ptrNewGenericEvent != NULL)
    {
      generic_event__init(ptrNewGenericEvent);

      ptrNewGenericEvent->one_of_generic_event_case = GENERIC_EVENT__ONE_OF_GENERIC_EVENT_FIELD_EVENT0X0_F;
      ptrNewGenericEvent->field_event0x0f = ptrNewEvent;

      /* Add this event in event array */
      if (Event_filter(ptrNewGenericEvent) == false) // Event is not listed in event array yet
      {
        if ((Parameter.events_verbose == true) && (pcb_test_is_running == false))
        {
          rtc_print_debug_timestamp();
          shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_BLUE, "Created Event0x0F 'USB plugged in', Event number: %d\n", System.EventNumber);
        }

        Event_AddInArray(ptrNewGenericEvent);
      }
      else
      {
        /* Free allocated memory */
        free(ptrNewEvent);
        free(ptrNewGenericEvent);
      }
    }
    else
    {
      if ((Parameter.events_verbose == true) && (pcb_test_is_running == false))
      {
        rtc_print_debug_timestamp();
        shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_RED, "Could not allocate memory for NewGenericEvent in Event0x0F\n");
      }

      /* Free allocated memory */
      free(ptrNewEvent);
    }
  }
  else
  {
    if ((Parameter.events_verbose == true) && (pcb_test_is_running == false))
    {
      rtc_print_debug_timestamp();
      shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_RED, "Could not allocate memory for NewEvent0x0F\n");
    }
  }

  /* Trigger user notification */
  if (Parameter.notification_test == false)
  {
    Notification.next_state = ActionMatrixArray[0x0F];
  }
}

/*!
 *  @brief This is the function description
 *  @details: 0x10 - USB plugged off
 */
void NewEvent0x10(void)
{
  Event0x10 *ptrNewEvent = malloc(sizeof(Event0x10));

  if (ptrNewEvent != NULL)
  {
    event0x10__init(ptrNewEvent);

    ptrNewEvent->event_id = 0x10;
    ptrNewEvent->event_timestamp = unixtime_ms;

    GenericEvent *ptrNewGenericEvent = malloc(sizeof(GenericEvent));

    if (ptrNewGenericEvent != NULL)
    {
      generic_event__init(ptrNewGenericEvent);

      ptrNewGenericEvent->one_of_generic_event_case = GENERIC_EVENT__ONE_OF_GENERIC_EVENT_FIELD_EVENT0X10;
      ptrNewGenericEvent->field_event0x10 = ptrNewEvent;

      /* Add this event in event array */
      if (Event_filter(ptrNewGenericEvent) == false) // Event is not listed in event array yet
      {
        if ((Parameter.events_verbose == true) && (pcb_test_is_running == false))
        {
          rtc_print_debug_timestamp();
          shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_BLUE, "Created Event0x10 'USB plugged off', Event number: %d\n", System.EventNumber);
        }

        Event_AddInArray(ptrNewGenericEvent);
      }
      else
      {
        /* Free allocated memory */
        free(ptrNewEvent);
        free(ptrNewGenericEvent);
      }
    }
    else
    {
      if ((Parameter.events_verbose == true) && (pcb_test_is_running == false))
      {
        rtc_print_debug_timestamp();
        shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_RED, "Could not allocate memory for NewGenericEvent in Event0x10\n");
      }

      /* Free allocated memory */
      free(ptrNewEvent);
    }
  }
  else
  {
    if ((Parameter.events_verbose == true) && (pcb_test_is_running == false))
    {
      rtc_print_debug_timestamp();
      shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_RED, "Could not allocate memory for NewEvent0x10\n");
    }
  }

  /* Trigger user notification */
  if (Parameter.notification_test == false)
  {
    Notification.next_state = ActionMatrixArray[0x10];
  }
}

/*!
 *  @brief This is the function description
 *  @details: 0x11 - Charging started
 */
void NewEvent0x11(void)
{
  Event0x11 *ptrNewEvent = malloc(sizeof(Event0x11));

  if (ptrNewEvent != NULL)
  {
    event0x11__init(ptrNewEvent);

    ptrNewEvent->event_id = 0x11;
    ptrNewEvent->event_timestamp = unixtime_ms;

    GenericEvent *ptrNewGenericEvent = malloc(sizeof(GenericEvent));

    if (ptrNewGenericEvent != NULL)
    {
      generic_event__init(ptrNewGenericEvent);

      ptrNewGenericEvent->one_of_generic_event_case = GENERIC_EVENT__ONE_OF_GENERIC_EVENT_FIELD_EVENT0X11;
      ptrNewGenericEvent->field_event0x11 = ptrNewEvent;

      /* Add this event in event array */
      if (Event_filter(ptrNewGenericEvent) == false) // Event is not listed in event array yet
      {
        if ((Parameter.events_verbose == true) && (pcb_test_is_running == false))
        {
          rtc_print_debug_timestamp();
          shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_BLUE, "Created Event0x11 'Charging started', Event number: %d\n", System.EventNumber);
        }

        Event_AddInArray(ptrNewGenericEvent);
      }
      else
      {
        /* Free allocated memory */
        free(ptrNewEvent);
        free(ptrNewGenericEvent);
      }
    }
    else
    {
      if ((Parameter.events_verbose == true) && (pcb_test_is_running == false))
      {
        rtc_print_debug_timestamp();
        shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_RED, "Could not allocate memory for NewGenericEvent in Event0x11\n");
      }

      /* Free allocated memory */
      free(ptrNewEvent);
    }
  }
  else
  {
    if ((Parameter.events_verbose == true) && (pcb_test_is_running == false))
    {
      rtc_print_debug_timestamp();
      shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_RED, "Could not allocate memory for NewEvent0x11\n");
    }
  }

  /* Trigger user notification */
  if (Parameter.notification_test == false)
  {
    Notification.next_state = ActionMatrixArray[0x11];
  }
}

/*!
 *  @brief This is the function description
 *  @details: 0x12 - Charging stopped
 */
void NewEvent0x12(void)
{
  Event0x12 *ptrNewEvent = malloc(sizeof(Event0x12));

  if (ptrNewEvent != NULL)
  {
    event0x12__init(ptrNewEvent);

    ptrNewEvent->event_id = 0x12;
    ptrNewEvent->event_timestamp = unixtime_ms;

    GenericEvent *ptrNewGenericEvent = malloc(sizeof(GenericEvent));

    if (ptrNewGenericEvent != NULL)
    {
      generic_event__init(ptrNewGenericEvent);

      ptrNewGenericEvent->one_of_generic_event_case = GENERIC_EVENT__ONE_OF_GENERIC_EVENT_FIELD_EVENT0X12;
      ptrNewGenericEvent->field_event0x12 = ptrNewEvent;

      /* Add this event in event array */
      if (Event_filter(ptrNewGenericEvent) == false) // Event is not listed in event array yet
      {

        if ((Parameter.events_verbose == true) && (pcb_test_is_running == false))
        {
          rtc_print_debug_timestamp();
          shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_BLUE, "Created Event0x12 'Charging stopped', Event number: %d\n", System.EventNumber);
        }

        Event_AddInArray(ptrNewGenericEvent);
      }
      else
      {
        /* Free allocated memory */
        free(ptrNewEvent);
        free(ptrNewGenericEvent);
      }
    }
    else
    {
      if ((Parameter.events_verbose == true) && (pcb_test_is_running == false))
      {
        rtc_print_debug_timestamp();
        shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_RED, "Could not allocate memory for NewGenericEvent in Event0x12\n");
      }

      /* Free allocated memory */
      free(ptrNewEvent);
    }
  }
  else
  {
    if ((Parameter.events_verbose == true) && (pcb_test_is_running == false))
    {
      rtc_print_debug_timestamp();
      shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_RED, "Could not allocate memory for NewEvent0x12\n");
    }
  }

  /* Trigger user notification */
  if (Parameter.notification_test == false)
  {
    Notification.next_state = ActionMatrixArray[0x12];
  }
}

/*!
 *  @brief This is the function description
 *  @details: 0x13 - Power on
 */
void NewEvent0x13(void)
{
  Event0x13 *ptrNewEvent = malloc(sizeof(Event0x13));

  if (ptrNewEvent != NULL)
  {
    event0x13__init(ptrNewEvent);

    ptrNewEvent->event_id = 0x13;
    ptrNewEvent->event_timestamp = unixtime_ms;

    GenericEvent *ptrNewGenericEvent = malloc(sizeof(GenericEvent));

    if (ptrNewGenericEvent != NULL)
    {
      generic_event__init(ptrNewGenericEvent);

      ptrNewGenericEvent->one_of_generic_event_case = GENERIC_EVENT__ONE_OF_GENERIC_EVENT_FIELD_EVENT0X13;
      ptrNewGenericEvent->field_event0x13 = ptrNewEvent;

      /* Add this event in event array */
      if (Event_filter(ptrNewGenericEvent) == false) // Event is not listed in event array yet
      {
        if ((Parameter.events_verbose == true) && (pcb_test_is_running == false))
        {
          rtc_print_debug_timestamp();
          shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_BLUE, "Created Event0x13 'Power on', Event number: %d\n", System.EventNumber);
        }

        Event_AddInArray(ptrNewGenericEvent);
      }
      else
      {
        /* Free allocated memory */
        free(ptrNewEvent);
        free(ptrNewGenericEvent);
      }
    }
    else
    {
      if ((Parameter.events_verbose == true) && (pcb_test_is_running == false))
      {
        rtc_print_debug_timestamp();
        shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_RED, "Could not allocate memory for NewGenericEvent in Event0x13\n");
      }

      /* Free allocated memory */
      free(ptrNewEvent);
    }
  }
  else
  {
    if ((Parameter.events_verbose == true) && (pcb_test_is_running == false))
    {
      rtc_print_debug_timestamp();
      shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_RED, "Could not allocate memory for NewEvent0x13\n");
    }
  }

  /* Trigger user notification */
  if (Parameter.notification_test == false)
  {
    Notification.next_state = ActionMatrixArray[0x13];
  }
}

/*!
 *  @brief This is the function description
 *  @details: 0x17 - Hit shock
 */
void NewEvent0x17(uint32_t schock_acc)
{
  Event0x17 *ptrNewEvent = malloc(sizeof(Event0x17));

  if (ptrNewEvent != NULL)
  {
    event0x17__init(ptrNewEvent);

    ptrNewEvent->event_id = 0x17;
    ptrNewEvent->event_timestamp = unixtime_ms;
    ptrNewEvent->schock_acc = schock_acc;

    GenericEvent *ptrNewGenericEvent = malloc(sizeof(GenericEvent));

    if (ptrNewGenericEvent != NULL)
    {
      generic_event__init(ptrNewGenericEvent);

      ptrNewGenericEvent->one_of_generic_event_case = GENERIC_EVENT__ONE_OF_GENERIC_EVENT_FIELD_EVENT0X17;
      ptrNewGenericEvent->field_event0x17 = ptrNewEvent;

      /* Add this event in event array */
      if (Event_filter(ptrNewGenericEvent) == false) // Event is not listed in event array yet
      {
        if ((Parameter.events_verbose == true) && (pcb_test_is_running == false))
        {
          rtc_print_debug_timestamp();
          shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_BLUE, "Created Event0x17 'Hit shock', Event number: %d\n", System.EventNumber);
        }

        Event_AddInArray(ptrNewGenericEvent);
      }
      else
      {
        /* Free allocated memory */
        free(ptrNewEvent);
        free(ptrNewGenericEvent);
      }
    }
    else
    {
      if ((Parameter.events_verbose == true) && (pcb_test_is_running == false))
      {
        rtc_print_debug_timestamp();
        shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_RED, "Could not allocate memory for NewGenericEvent in Event0x17\n");
      }

      /* Free allocated memory */
      free(ptrNewEvent);
    }
  }
  else
  {
    if ((Parameter.events_verbose == true) && (pcb_test_is_running == false))
    {
      rtc_print_debug_timestamp();
      shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_RED, "Could not allocate memory for NewEvent0x17\n");
    }
  }

  /* Trigger user notification */
  if (Parameter.notification_test == false)
  {
    Notification.next_state = ActionMatrixArray[0x17];
  }
}

/*!
 *  @brief This is the function description
 *  @details: 0x18 - Frame side flip
 */
void NewEvent0x18(uint32_t frame_side)
{
  Event0x18 *ptrNewEvent = malloc(sizeof(Event0x18));

  if (ptrNewEvent != NULL)
  {
    event0x18__init(ptrNewEvent);

    ptrNewEvent->event_id = 0x18;
    ptrNewEvent->event_timestamp = unixtime_ms;
    ptrNewEvent->frame_side = frame_side;

    GenericEvent *ptrNewGenericEvent = malloc(sizeof(GenericEvent));

    if (ptrNewGenericEvent != NULL)
    {
      generic_event__init(ptrNewGenericEvent);

      ptrNewGenericEvent->one_of_generic_event_case = GENERIC_EVENT__ONE_OF_GENERIC_EVENT_FIELD_EVENT0X18;
      ptrNewGenericEvent->field_event0x18 = ptrNewEvent;

      /* Add this event in event array */
      if (Event_filter(ptrNewGenericEvent) == false) // Event is not listed in event array yet
      {
        if ((Parameter.events_verbose == true) && (pcb_test_is_running == false))
        {
          rtc_print_debug_timestamp();
          shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_BLUE, "Created Event0x18 'Frame side flip', Event number: %d\n", System.EventNumber);
        }

        Event_AddInArray(ptrNewGenericEvent);
      }
      else
      {
        /* Free allocated memory */
        free(ptrNewEvent);
        free(ptrNewGenericEvent);
      }
    }
    else
    {
      if ((Parameter.events_verbose == true) && (pcb_test_is_running == false))
      {
        rtc_print_debug_timestamp();
        shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_RED, "Could not allocate memory for NewGenericEvent in Event0x18\n");
      }

      /* Free allocated memory */
      free(ptrNewEvent);
    }
  }
  else
  {
    if ((Parameter.events_verbose == true) && (pcb_test_is_running == false))
    {
      rtc_print_debug_timestamp();
      shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_RED, "Could not allocate memory for NewEvent0x18\n");
    }
  }

  /* Trigger user notification */
  if (Parameter.notification_test == false)
  {
    Notification.next_state = ActionMatrixArray[0x18];
  }
}

/*!
 *  @brief This is the function description
 *  @details: 0x19 - Inappropriate mop is used in the room in mopping state
 */
void NewEvent0x19(uint32_t room_id, uint32_t mop_id)
{
  Event0x19 *ptrNewEvent = malloc(sizeof(Event0x19));

  if (ptrNewEvent != NULL)
  {
    event0x19__init(ptrNewEvent);

    ptrNewEvent->event_id = 0x19;
    ptrNewEvent->event_timestamp = unixtime_ms;
    ptrNewEvent->room_id = room_id;
    ptrNewEvent->mop_id = mop_id;

    GenericEvent *ptrNewGenericEvent = malloc(sizeof(GenericEvent));

    if (ptrNewGenericEvent != NULL)
    {
      generic_event__init(ptrNewGenericEvent);

      ptrNewGenericEvent->one_of_generic_event_case = GENERIC_EVENT__ONE_OF_GENERIC_EVENT_FIELD_EVENT0X19;
      ptrNewGenericEvent->field_event0x19 = ptrNewEvent;

      /* Add this event in event array */
      if (Event_filter(ptrNewGenericEvent) == false) // Check if event should be added multiple times
      {
        if ((Parameter.events_verbose == true) && (pcb_test_is_running == false))
        {
          rtc_print_debug_timestamp();
          shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_BLUE, "Created Event0x19 'Inappropriate mop is used in the room in mopping state', Event number: %d\n", System.EventNumber);
        }

        Event_AddInArray(ptrNewGenericEvent);
      }
      else
      {
        /* Free allocated memory */
        free(ptrNewEvent);
        free(ptrNewGenericEvent);
      }
    }
    else
    {
      if ((Parameter.events_verbose == true) && (pcb_test_is_running == false))
      {
        rtc_print_debug_timestamp();
        shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_RED, "Could not allocate memory for NewGenericEvent in Event0x19\n");
      }

      /* Free allocated memory */
      free(ptrNewEvent);
    }
  }
  else
  {
    if ((Parameter.events_verbose == true) && (pcb_test_is_running == false))
    {
      rtc_print_debug_timestamp();
      shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_RED, "Could not allocate memory for NewEvent0x19\n");
    }
  }

  /* Trigger user notification */
  if (Parameter.notification_test == false)
  {
    Notification.next_state = ActionMatrixArray[0x19];
  }
}

/*!
 *  @brief This is the function description
 *  @details: 0x19 - Inappropriate mop is used in the room in mopping state
 */
void NewEvent0x1A(uint32_t room_id, uint32_t mop_id)
{
  Event0x1A *ptrNewEvent = malloc(sizeof(Event0x1A));

  if (ptrNewEvent != NULL)
  {
    event0x1_a__init(ptrNewEvent);

    ptrNewEvent->event_id = 0x1A;
    ptrNewEvent->event_timestamp = unixtime_ms;
    ptrNewEvent->room_id = room_id;
    ptrNewEvent->mop_id = mop_id;

    GenericEvent *ptrNewGenericEvent = malloc(sizeof(GenericEvent));

    if (ptrNewGenericEvent != NULL)
    {
      generic_event__init(ptrNewGenericEvent);

      ptrNewGenericEvent->one_of_generic_event_case = GENERIC_EVENT__ONE_OF_GENERIC_EVENT_FIELD_EVENT0X1_A;
      ptrNewGenericEvent->field_event0x1a = ptrNewEvent;

      /* Add this event in event array */
      if (Event_filter(ptrNewGenericEvent) == false) // Event is not listed in event array yet
      {
        if ((Parameter.events_verbose == true) && (pcb_test_is_running == false))
        {
          rtc_print_debug_timestamp();
          shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_BLUE, "Created Event0x1A 'Inappropriate mop is near the room', Event number: %d\n", System.EventNumber);
        }

        Event_AddInArray(ptrNewGenericEvent);
      }
      else
      {
        /* Free allocated memory */
        free(ptrNewEvent);
        free(ptrNewGenericEvent);
      }
    }
    else
    {
      if ((Parameter.events_verbose == true) && (pcb_test_is_running == false))
      {
        rtc_print_debug_timestamp();
        shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_RED, "Could not allocate memory for NewGenericEvent in Event0x1A\n");
      }

      /* Free allocated memory */
      free(ptrNewEvent);
    }
  }
  else
  {
    if ((Parameter.events_verbose == true) && (pcb_test_is_running == false))
    {
      rtc_print_debug_timestamp();
      shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_RED, "Could not allocate memory for NewEvent0x1A\n");
    }
  }

  /* Trigger user notification */
  if (Parameter.notification_test == false)
  {
    Notification.next_state = ActionMatrixArray[0x1A];
  }
}

/*!
 *  @brief This is the function description
 *  @details: 0x19 - Inappropriate mop is used in the room in mopping state
 */
void NewEvent0x1B(void)
{
  Event0x1B *ptrNewEvent = malloc(sizeof(Event0x1B));

  if (ptrNewEvent != NULL)
  {
    event0x1_b__init(ptrNewEvent);

    ptrNewEvent->event_id = 0x1B;
    ptrNewEvent->event_timestamp = unixtime_ms;

    GenericEvent *ptrNewGenericEvent = malloc(sizeof(GenericEvent));

    if (ptrNewGenericEvent != NULL)
    {
      generic_event__init(ptrNewGenericEvent);

      ptrNewGenericEvent->one_of_generic_event_case = GENERIC_EVENT__ONE_OF_GENERIC_EVENT_FIELD_EVENT0X1_B;
      ptrNewGenericEvent->field_event0x1b = ptrNewEvent;

      /* Add this event in event array */
      if (Event_filter(ptrNewGenericEvent) == false) // Event is not listed in event array yet
      {
        if ((Parameter.events_verbose == true) && (pcb_test_is_running == false))
        {
          rtc_print_debug_timestamp();
          shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_BLUE, "Created Event0x1B 'Mop change', Event number: %d\n", System.EventNumber);
        }

        Event_AddInArray(ptrNewGenericEvent);
      }
      else
      {
        /* Free allocated memory */
        free(ptrNewEvent);
        free(ptrNewGenericEvent);
      }
    }
    else
    {
      if ((Parameter.events_verbose == true) && (pcb_test_is_running == false))
      {
        rtc_print_debug_timestamp();
        shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_RED, "Could not allocate memory for NewGenericEvent in Event0x1B\n");
      }

      /* Free allocated memory */
      free(ptrNewEvent);
    }
  }
  else
  {
    if ((Parameter.events_verbose == true) && (pcb_test_is_running == false))
    {
      rtc_print_debug_timestamp();
      shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_RED, "Could not allocate memory for NewEvent0x1B\n");
    }
  }

  /* Trigger user notification */
  if (Parameter.notification_test == false)
  {
    Notification.next_state = ActionMatrixArray[0x1B];
  }
}

/*!
 *  @brief This is the function description
 *  @details: 0x1C - Square meters
 */
void NewEvent0x1C(uint32_t mop_id, float sqm_side_0, float sqm_side_1)
{
  Event0x1C *ptrNewEvent = malloc(sizeof(Event0x1C));

  if (ptrNewEvent != NULL)
  {
    event0x1_c__init(ptrNewEvent);

    ptrNewEvent->event_id = 0x1C;
    ptrNewEvent->event_timestamp = unixtime_ms;
    ptrNewEvent->mop_id = mop_id;
    ptrNewEvent->sqm_side_0 = sqm_side_0;
    ptrNewEvent->sqm_side_1 = sqm_side_1;

    GenericEvent *ptrNewGenericEvent = malloc(sizeof(GenericEvent));

    if (ptrNewGenericEvent != NULL)
    {
      generic_event__init(ptrNewGenericEvent);

      ptrNewGenericEvent->one_of_generic_event_case = GENERIC_EVENT__ONE_OF_GENERIC_EVENT_FIELD_EVENT0X1_C;
      ptrNewGenericEvent->field_event0x1c = ptrNewEvent;

      /* Add this event in event array */
      if (Event_filter(ptrNewGenericEvent) == false) // Event is not listed in event array yet
      {
        if ((Parameter.events_verbose == true) && (pcb_test_is_running == false))
        {
          rtc_print_debug_timestamp();
          shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_BLUE, "Created Event0x1C 'MopID:%d, sqm_side_0: %.1f, sqm_side_1 : %.1f, Event number: %d\n", mop_id, sqm_side_0, sqm_side_1, System.EventNumber);
        }

        Event_AddInArray(ptrNewGenericEvent);
      }
      else
      {
        /* Free allocated memory */
        free(ptrNewEvent);
        free(ptrNewGenericEvent);
      }
    }
    else
    {
      if ((Parameter.events_verbose == true) && (pcb_test_is_running == false))
      {
        rtc_print_debug_timestamp();
        shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_RED, "Could not allocate memory for NewGenericEvent in Event0x1C\n");
      }

      /* Free allocated memory */
      free(ptrNewEvent);
    }
  }
  else
  {
    if ((Parameter.events_verbose == true) && (pcb_test_is_running == false))
    {
      rtc_print_debug_timestamp();
      shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_RED, "Could not allocate memory for NewEvent0x1C\n");
    }
  }

  /* Trigger user notification */
  //  if (Parameter.notification_test == false)
  // {
  //   Notification.next_state = ActionMatrixArray[0x1B];
  // }
}

/*!
 *  @brief This is the function description
 *  @details: 0xFF - Debug info
 */
void NewEvent0xFF(uint32_t msg_id, uint8_t *msg, uint16_t len)
{
  Event0xFF *ptrNewEvent = malloc(sizeof(Event0xFF));

  if (ptrNewEvent != NULL)
  {
    event0x_ff__init(ptrNewEvent);

    ptrNewEvent->event_id = 0xFF;
    ptrNewEvent->event_timestamp = unixtime_ms;
    ptrNewEvent->msg_id = msg_id;

    /* Copy current location in allocated memory */
    binary_data.len = len;
    binary_data.data = msg;

    ptrNewEvent->msg = binary_data;

    GenericEvent *ptrNewGenericEvent = malloc(sizeof(GenericEvent));

    if (ptrNewGenericEvent != NULL)
    {
      generic_event__init(ptrNewGenericEvent);

      ptrNewGenericEvent->one_of_generic_event_case = GENERIC_EVENT__ONE_OF_GENERIC_EVENT_FIELD_EVENT0X_FF;
      ptrNewGenericEvent->field_event0xff = ptrNewEvent;

      /* Add this event in event array */
      if (Event_filter(ptrNewGenericEvent) == false) // Event is not listed in event array yet
      {
        if ((Parameter.events_verbose == true) && (pcb_test_is_running == false))
        {
          rtc_print_debug_timestamp();
          shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_BLUE, "Created Event0xFF 'Debug info', Event number: %d\n", System.EventNumber);
        }

        Event_AddInArray(ptrNewGenericEvent);
      }
      else
      {
        /* Free allocated memory */
        free(ptrNewEvent);
        free(ptrNewGenericEvent);
      }
    }
    else
    {
      if ((Parameter.events_verbose == true) && (pcb_test_is_running == false))
      {
        rtc_print_debug_timestamp();
        shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_RED, "Could not allocate memory for NewGenericEvent in Event0xFF\n");
      }

      /* Free allocated memory */
      free(ptrNewEvent);
    }
  }
  else
  {
    if ((Parameter.events_verbose == true) && (pcb_test_is_running == false))
    {
      rtc_print_debug_timestamp();
      shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_RED, "Could not allocate memory for NewEvent0xFF\n");
    }
  }

  /* Trigger user notification */
  //  if (Parameter.notification_test == false)
  // {
  //   Notification.next_state = ActionMatrixArray[0x1B];
  // }
}
