/**
 * PL011_T UART
 *     http://infocenter.arm.com/help/topic/com.arm.doc.ddi0183f/DDI0183.pdf
 * 
 * UARTDR: Data Register   (0x00)
 *    To read received bytes
 *    To write bytes to send
 *    Bit Fields:
 *      15:12 reserved
 *      11:08 error flags
 *       7:0  data bits
 * UARTFR:  Flag Register  (0x18)
 *    Bit Fields:
 *      7:  TXFE  transmit FIFO empty
 *      6:  RXFF  receive FIFO full
 *      5:  TXFF  transmit FIFO full
 *      4:  RXFE  receive FIFO empty
 *      3:  BUSY  set when the UART is busy transmitting data
 */

#define UART0 0x101f1000
#define UART1 0x101f2000
#define UART2 0x101f3000

#define UART_DR 0x00
#define UART_FR 0x18

#define UART_TXFE (1<<7)
#define UART_RXFF (1<<6)
#define UART_TXFF (1<<5)
#define UART_RXFE (1<<4)
#define UART_BUSY (1<<3)

int uart_receive(int uart, unsigned char *s) {
  unsigned short* uart_fr = (unsigned short*) (uart + UART_FR);
  unsigned short* uart_dr = (unsigned short*) (uart + UART_DR);
  if (*uart_fr & UART_RXFE)
    return 0;
  *s = (*uart_dr & 0xff);
  return 1;
}

void uart_send(int uart, unsigned char s) {
  unsigned short* uart_fr = (unsigned short*) (uart + UART_FR);
  unsigned short* uart_dr = (unsigned short*) (uart + UART_DR);
  while (*uart_fr & UART_TXFF)
    ;
  *uart_dr = s;
}

void uart_send_string(int uart, const unsigned char *s) {
  while (*s != '\0') {
    uart_send(uart, *s);
    s++;
  }
}


#define ECHO_ZZZ

void c_entry() {
  int i = 0;
  int count = 0;
  uart_send_string(UART0, "\nHello world!\n");
  uart_send_string(UART0, "\nQuit with \"C-a c\" and then type in \"quit\".\n");

  while (1) {
    unsigned char c;
#ifdef ECHO_ZZZ
    while (0 == uart_receive(UART0, &c)) {
      count++;
      if (count > 50000000) {
        uart_send_string(UART0, "\n\rZzzz....\n\r");
        count = 0;
      }
    }
#else
    if (0==uart_receive(UART0,&c))
    continue;
#endif
    if (c == 13) {
      uart_send(UART0, '\r');
      uart_send(UART0, '\n');
    } else {
      uart_send(UART0, c);
    }
  }
}
