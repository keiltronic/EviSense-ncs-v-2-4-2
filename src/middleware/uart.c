/**
 * @file uart.c
 * @author Thomas Keilbach | keiltronic GmbH
 * @date 27 Oct 2022
 * @brief This file contains functions to communicate with the pheripherals
 * @version 1.0.0
 */

/*!
 * @defgroup Peripherals
 * @brief This file contains functions to communicate with the pheripherals
 * @{*/
#include "uart.h"

//struct device *uart1;
struct uart_data_t
{
  void *fifo_reserved;
  uint8_t data[UART1_BUFFERSIZE];
  uint16_t len;
};

volatile char uart1_InputBuffer[UART1_BUFFERSIZE];
volatile char uart1_RFIDResponse[UART1_BUFFERSIZE];

static char uart_buf[32];
static K_FIFO_DEFINE(fifo_uart_tx_data);

volatile uint8_t uart1_TransmissionReady = false;
volatile uint16_t uart1_TransmissionLength = 0;
volatile uint16_t uart1_RFIDResponseTransmissionLength = 0;
volatile uint8_t uart1_RFIDResponseFinished = false;
volatile uint8_t EPC_string_start = false;
volatile uint8_t EPC_string_end = false;

struct device *uart1 = DEVICE_DT_GET(DT_NODELABEL(uart1));

/*!
 *  @brief This is the function description
 */
// void uart1_cb(const struct device *rfid_module, struct uart_event *evt)
void uart1_cb(const struct device *rfid_module, struct uart_event *evt, void *user_data)
{
  uart_irq_update(rfid_module);
  volatile int16_t data_length = 0;

  /* ----------- RX handling --------------------------------------------------------- */

  /* Received data */
  if (uart_irq_rx_ready(rfid_module))
  {
    data_length = uart_fifo_read(rfid_module, uart_buf, sizeof(uart_buf));

    /* Append incoming byte-wise charackters to a full string to process it outside this ISR */
    if (uart1_TransmissionLength < UART1_BUFFERSIZE)
    {
      uart1_InputBuffer[uart1_TransmissionLength++] = uart_buf[0];
    }
  }

  /* Fill input buffer, extract EPC tags from raw input buffer data */
  if (System.RFID_TransparentMode == true && System.RFID_Sniff == false)
  {
    if (uart_buf[0] != '\n' && uart_buf[0] != '\r')
    {
      uart1_RFIDResponse[uart1_RFIDResponseTransmissionLength++] = uart_buf[0]; // Append character to a full string
    }

    if (uart_buf[0] == '\r')
    {
      uart1_RFIDResponseFinished = true;
    }

    /* Print out raw data coming from rfid module */
  }
  else if (System.RFID_TransparentMode == false && System.RFID_Sniff == true)
  {
    /* Echo received data at UART1 to console (UART0) */
    if (suppress_rfid_command_charcaters == true)
    {
      if ((uart_buf[0] != 'U') && (uart_buf[0] != 'x')) // supress charackter 'U' and 'X' and 'CR'
      {
        if ((EPC_string_start == false) && (uart_buf[0] != 0x0D) && (uart_buf[0] != 0x0A))
        {
          EPC_string_start = true;
          rfid_ok = true;
        }

        if ((EPC_string_start == true) && (uart_buf[0] == 0x0D))
        {
          EPC_string_start = false;
          printk("\n");
        }

        if (EPC_string_start == true)
        {
          printk("%s", &uart_buf[0]);
        }
      }
    }
    else
    {
      printk("%s", &uart_buf[0]);
    }
  }

  /* ----------- TX handling --------------------------------------------------------- */

  /* Data to send */
  if (uart_irq_tx_ready(rfid_module))
  {
    struct uart_data_t *buf = k_fifo_get(&fifo_uart_tx_data, K_NO_WAIT);
    uint16_t written = 0;

    /* Nothing in the FIFO, nothing to send */
    if (!buf)
    {
      uart_irq_tx_disable(rfid_module);
      return;
    }

    while (buf->len > written)
    {
      written += uart_fifo_fill(rfid_module, &buf->data[written], buf->len - written);
    }

    while (!uart_irq_tx_complete(rfid_module))
    {
      /* Wait for the last byte to get shifted out of the module */
    }

    if (k_fifo_is_empty(&fifo_uart_tx_data))
    {
      uart_irq_tx_disable(rfid_module);
    }

    free(buf);
  }
}

/*!
 *  @brief This is the function description
 */
void uart1_init(void)
{
  // uart1 = device_get_binding("UART_1");

  // uart_irq_callback_set(uart1, uart1_cb);
  // uart_irq_rx_enable(uart1);

  int16_t ret = 0;

  if (!device_is_ready(uart1))
  {
    printk("UART1 not ready\n\r");
    return;
  }

  ret = uart_irq_callback_set(uart1, uart1_cb);
  if (ret)
  {
    printk("UART1 cant install uart1 callback\n\r");
    return 1;
  }

  uart_irq_rx_enable(uart1);
}