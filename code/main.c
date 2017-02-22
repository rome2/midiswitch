#include <avr/io.h>
#include <util/delay.h>

#define BTN_CH1 0b10000000
#define BTN_CH2 0b01000000
#define BTN_CH3 0b00100000
#define BTN_MASK (BTN_CH1 | BTN_CH2 | BTN_CH3)

#define LED_OFF 0b00000000
#define LED_CH1 0b00010000
#define LED_CH2 0b00001000
#define LED_CH3 0b00000100
#define LED_MASK (LED_CH1 | LED_CH2 | LED_CH3)

#define IO_DIR DDRD
#define IO_PORT PORTD
#define IO_PINS PIND
#define UART_BAUD_RATE 31250

#define LED_DELAY 500
#define PRG_CHN 0
#define PRG_CH1 0
#define PRG_CH2 1
#define PRG_CH3 2

void send_byte(unsigned char byte)
{
  loop_until_bit_is_set(UCSRA, UDRE);
  UDR = byte;
}

void send_prg_change(unsigned char channel, unsigned char program)
{
  send_byte(0xC0 | (channel & 0x0F));
  send_byte(program & 0x7F);
}

void led_on(unsigned char which)
{
  IO_PORT = (IO_PORT & ~LED_MASK) | which;
}

int btn_pressed(unsigned char which)
{
  return (IO_PINS & which) == 0 ? 1 : 0;
}

int main(void)
{
  // Init IO:
  IO_DIR = LED_MASK;

  // Show sign of life:
  led_on(LED_CH1);
  _delay_ms(LED_DELAY);

  // Enable pullup on buttons:
  IO_PORT = BTN_MASK;

  // Show sign of life:
  led_on(LED_CH2);
  _delay_ms(LED_DELAY);

  // Setup UART:
  unsigned int ubbr_val = (F_CPU / (UART_BAUD_RATE * 16L)) - 1;
  UBRRH = (uint8_t)(ubbr_val >> 8);
  UBRRL = (uint8_t)ubbr_val;
  UCSRB = 1 << TXEN;

  // Show sign of life:
  led_on(LED_CH3);
  _delay_ms(LED_DELAY);

  // Main loop:
  led_on(LED_OFF);
  static int cur_program = -1;
  while (1)
  {
    if (btn_pressed(BTN_CH1) && cur_program != PRG_CH1)
    {
      led_on(LED_CH1);
      send_prg_change(PRG_CHN, PRG_CH1);
      cur_program = PRG_CH1;
    }
    else if (btn_pressed(BTN_CH2) && cur_program != PRG_CH2)
    {
      led_on(LED_CH2);
      send_prg_change(PRG_CHN, PRG_CH2);
      cur_program = PRG_CH2;
    }
    else if (btn_pressed(BTN_CH3) && cur_program != PRG_CH3)
    {
      led_on(LED_CH3);
      send_prg_change(PRG_CHN, PRG_CH3);
      cur_program = PRG_CH3;
    }
  }

  // Just to please the compiler:
  return 0;
}
