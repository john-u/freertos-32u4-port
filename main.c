/*
 * Created by John Undersander 04/2018
 */

/* Hardware includes */
#include <avr/io.h>
#include <util/delay.h>

/* Kernel includes */
#include "FreeRTOS.h"
#include "task.h"

/* Project includes */
#include "project-tasks.h"

void sanity_check_board() {
  // Configure Yellow LED
  DDRC |= BIT(DDC7);
  // Configure Green LED
  DDRD |= BIT(DDD5);
  // Configure Red LED
  DDRB |= BIT(DDB0);
  // RGRGYY * 2
  int i, j, k;
  for (i = 0; i < 2; i++) {
    for (j = 0; j < 2; j++) {
      for (k = 0; k < 2; k++) {
        PORTB ^= BIT(PORTB0);
        _delay_ms(50);
      }

      for (k = 0; k < 2; k++) {
        PORTD ^= BIT(PORTD5);
        _delay_ms(50);
      }
    }

    for (j = 0; j < 4; j++) {
      PORTC ^= BIT(PORTC7);
      _delay_ms(100);
    }
  }
}

void sanity_check_gpio() {
  // Green LED port B, pin 6 (arduino pin 10)
  DDRB |= BIT(PB6);

  // Yellow LED port D, pin 6 (arduino pin 12)
  DDRD |= BIT(PD6);

  // Red LED port B, pin 4 (arduino pin 8)
  DDRB |= BIT(PB4);

  // confirm function
  PORTB |= BIT(PORTB4) | BIT(PORTB6);
  PORTD |= BIT(PORTD6);
  _delay_ms(1000);
  PORTB &= ~BIT(PORTB4) & ~BIT(PORTB6);
  PORTD &= ~BIT(PORTD6);
}

int main() {
  // This prevents the need to reset after flashing
  USBCON = 0;

  sanity_check_board();
  sanity_check_gpio();

  adc_init();

  xTaskCreate(toggle_red, "Red", configMINIMAL_STACK_SIZE, NULL, 5, NULL);
  xTaskCreate(hough, "Hough", configMINIMAL_STACK_SIZE, NULL, 4, NULL);
  xTaskCreate(toggle_yellow, "Yellow", configMINIMAL_STACK_SIZE, NULL, 3, NULL);
  xTaskCreate(toggle_green, "Green", configMINIMAL_STACK_SIZE, NULL, 2, NULL);
  xTaskCreate(poll_adc, "Poll ADC", configMINIMAL_STACK_SIZE, NULL, 1, NULL);

  vTaskStartScheduler();

  return 0;
}
