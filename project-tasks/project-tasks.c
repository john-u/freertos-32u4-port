//
// Created by John Undersander 04/2018
//

#include "FreeRTOS.h"
#include "task.h"

#include <avr/io.h>
#include <util/delay.h>

#include "basic-analog.h"
#include "hough.h"
#include "project-tasks.h"

void toggle_red(void *params) {
  const TickType_t xFrequency = 100;
  TickType_t xLastWakeTime = xTaskGetTickCount();
  for (;;) {
    PORTB ^= BIT(PORTB4);
    vTaskDelayUntil(&xLastWakeTime, xFrequency);
  }
}

void toggle_yellow(void *params) {
  const TickType_t xFrequency = 100;
  TickType_t xLastWakeTime = xTaskGetTickCount();
  for (;;) {
    PORTD ^= BIT(PORTD6);
    vTaskDelayUntil(&xLastWakeTime, xFrequency);
  }
}

void toggle_green(void *params) {
  const TickType_t xFrequency = 40;
  TickType_t xLastWakeTime = xTaskGetTickCount();
  for (;;) {
    PORTB ^= BIT(PORTB6);
    vTaskDelayUntil(&xLastWakeTime, xFrequency);
  }
}

void hough(void *params) {
  const TickType_t xFrequency = 100;
  TickType_t xLastWakeTime = xTaskGetTickCount();
  char dummyVar;
  uint8_t i;
  for (;;) {
    PORTD |= BIT(PORTD5);

    i = 2;  // 2 times gives around 58ms WCET
    while (i--) {
      dummyVar = houghTransform((uint16_t) &red, (uint16_t) &green, (uint16_t) &blue);
    }

    PORTD &= ~BIT(PORTD5);
    vTaskDelayUntil(&xLastWakeTime, xFrequency);
  }
}

void semaphore_hold(void *params) {
  // Yellow on-board, pre-configured for sanity check in main
  PORTC |= BIT(PORTC7);
  _delay_ms(500);
  PORTC &= ~BIT(PORTC7);

  // Deletes itself to free resources for other tasks
  vTaskDelete(NULL);
}

void poll_adc(void *params) {
  const TickType_t xFrequency = 5000;
  TickType_t xLastWakeTime = xTaskGetTickCount();
  uint16_t last_adc = INFINITY;
  uint16_t adc;
  for (;;) {
    adc = adc_read();

    if (adc < last_adc - 1 || adc > last_adc + 1) {
      last_adc = adc;
      xTaskCreate(semaphore_hold, "Semaphore", configMINIMAL_STACK_SIZE, NULL, 6, NULL);
    }

    vTaskDelayUntil(&xLastWakeTime, xFrequency);
  }
}
