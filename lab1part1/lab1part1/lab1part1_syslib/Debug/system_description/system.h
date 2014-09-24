/* system.h
 *
 * Machine generated for a CPU named "cpu_0" as defined in:
 * N:\ece224\myESystem\ANT\my_controller.ptf
 *
 * Generated: 2014-09-24 16:05:33.704
 *
 */

#ifndef __SYSTEM_H_
#define __SYSTEM_H_

/*

DO NOT MODIFY THIS FILE

   Changing this file will have subtle consequences
   which will almost certainly lead to a nonfunctioning
   system. If you do modify this file, be aware that your
   changes will be overwritten and lost when this file
   is generated again.

DO NOT MODIFY THIS FILE

*/

/******************************************************************************
*                                                                             *
* License Agreement                                                           *
*                                                                             *
* Copyright (c) 2003 Altera Corporation, San Jose, California, USA.           *
* All rights reserved.                                                        *
*                                                                             *
* Permission is hereby granted, free of charge, to any person obtaining a     *
* copy of this software and associated documentation files (the "Software"),  *
* to deal in the Software without restriction, including without limitation   *
* the rights to use, copy, modify, merge, publish, distribute, sublicense,    *
* and/or sell copies of the Software, and to permit persons to whom the       *
* Software is furnished to do so, subject to the following conditions:        *
*                                                                             *
* The above copyright notice and this permission notice shall be included in  *
* all copies or substantial portions of the Software.                         *
*                                                                             *
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR  *
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,    *
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE *
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER      *
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING     *
* FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER         *
* DEALINGS IN THE SOFTWARE.                                                   *
*                                                                             *
* This agreement shall be governed in all respects by the laws of the State   *
* of California and by the laws of the United States of America.              *
*                                                                             *
******************************************************************************/

/*
 * system configuration
 *
 */

#define ALT_SYSTEM_NAME "my_controller"
#define ALT_CPU_NAME "cpu_0"
#define ALT_CPU_ARCHITECTURE "altera_nios2"
#define ALT_DEVICE_FAMILY "STRATIXIII"
#define ALT_STDIN "/dev/jtag_uart_0"
#define ALT_STDIN_TYPE "altera_avalon_jtag_uart"
#define ALT_STDIN_BASE 0x01001158
#define ALT_STDIN_DEV jtag_uart_0
#define ALT_STDIN_PRESENT
#define ALT_STDOUT "/dev/jtag_uart_0"
#define ALT_STDOUT_TYPE "altera_avalon_jtag_uart"
#define ALT_STDOUT_BASE 0x01001158
#define ALT_STDOUT_DEV jtag_uart_0
#define ALT_STDOUT_PRESENT
#define ALT_STDERR "/dev/jtag_uart_0"
#define ALT_STDERR_TYPE "altera_avalon_jtag_uart"
#define ALT_STDERR_BASE 0x01001158
#define ALT_STDERR_DEV jtag_uart_0
#define ALT_STDERR_PRESENT
#define ALT_CPU_FREQ 50000000
#define ALT_IRQ_BASE NULL
#define ALT_LEGACY_INTERRUPT_API_PRESENT

/*
 * processor configuration
 *
 */

#define NIOS2_CPU_IMPLEMENTATION "small"
#define NIOS2_BIG_ENDIAN 0
#define NIOS2_INTERRUPT_CONTROLLER_ID 0

#define NIOS2_ICACHE_SIZE 4096
#define NIOS2_DCACHE_SIZE 0
#define NIOS2_ICACHE_LINE_SIZE 32
#define NIOS2_ICACHE_LINE_SIZE_LOG2 5
#define NIOS2_DCACHE_LINE_SIZE 0
#define NIOS2_DCACHE_LINE_SIZE_LOG2 0
#define NIOS2_FLUSHDA_SUPPORTED

#define NIOS2_EXCEPTION_ADDR 0x00800020
#define NIOS2_RESET_ADDR 0x00800000
#define NIOS2_BREAK_ADDR 0x01000820

#define NIOS2_HAS_DEBUG_STUB

#define NIOS2_CPU_ID_SIZE 1
#define NIOS2_CPU_ID_VALUE 0

/*
 * A define for each class of peripheral
 *
 */

#define __ALTERA_AVALON_SYSID
#define __ALTERA_AVALON_JTAG_UART
#define __ALTERA_AVALON_LCD_16207
#define __ALTERA_AVALON_PIO
#define __ALTERA_AVALON_TIMER
#define __ALTERA_AVALON_NEW_SDRAM_CONTROLLER

/*
 * sysid configuration
 *
 */

#define SYSID_NAME "/dev/sysid"
#define SYSID_TYPE "altera_avalon_sysid"
#define SYSID_BASE 0x01001150
#define SYSID_SPAN 8
#define SYSID_ID 0u
#define SYSID_TIMESTAMP 1410966645u
#define SYSID_REGENERATE_VALUES 0
#define ALT_MODULE_CLASS_sysid altera_avalon_sysid

/*
 * jtag_uart_0 configuration
 *
 */

#define JTAG_UART_0_NAME "/dev/jtag_uart_0"
#define JTAG_UART_0_TYPE "altera_avalon_jtag_uart"
#define JTAG_UART_0_BASE 0x01001158
#define JTAG_UART_0_SPAN 8
#define JTAG_UART_0_IRQ 23
#define JTAG_UART_0_IRQ_INTERRUPT_CONTROLLER_ID 0
#define JTAG_UART_0_WRITE_DEPTH 64
#define JTAG_UART_0_READ_DEPTH 64
#define JTAG_UART_0_WRITE_THRESHOLD 8
#define JTAG_UART_0_READ_THRESHOLD 8
#define JTAG_UART_0_READ_CHAR_STREAM ""
#define JTAG_UART_0_SHOWASCII 1
#define JTAG_UART_0_RELATIVEPATH 1
#define JTAG_UART_0_READ_LE 0
#define JTAG_UART_0_WRITE_LE 0
#define JTAG_UART_0_ALTERA_SHOW_UNRELEASED_JTAG_UART_FEATURES 0
#define ALT_MODULE_CLASS_jtag_uart_0 altera_avalon_jtag_uart

/*
 * lcd_display configuration
 *
 */

#define LCD_DISPLAY_NAME "/dev/lcd_display"
#define LCD_DISPLAY_TYPE "altera_avalon_lcd_16207"
#define LCD_DISPLAY_BASE 0x01001040
#define LCD_DISPLAY_SPAN 16
#define ALT_MODULE_CLASS_lcd_display altera_avalon_lcd_16207

/*
 * led_pio configuration
 *
 */

#define LED_PIO_NAME "/dev/led_pio"
#define LED_PIO_TYPE "altera_avalon_pio"
#define LED_PIO_BASE 0x01001050
#define LED_PIO_SPAN 16
#define LED_PIO_DO_TEST_BENCH_WIRING 0
#define LED_PIO_DRIVEN_SIM_VALUE 0
#define LED_PIO_HAS_TRI 0
#define LED_PIO_HAS_OUT 1
#define LED_PIO_HAS_IN 0
#define LED_PIO_CAPTURE 0
#define LED_PIO_DATA_WIDTH 8
#define LED_PIO_RESET_VALUE 0
#define LED_PIO_EDGE_TYPE "NONE"
#define LED_PIO_IRQ_TYPE "NONE"
#define LED_PIO_BIT_CLEARING_EDGE_REGISTER 0
#define LED_PIO_BIT_MODIFYING_OUTPUT_REGISTER 0
#define LED_PIO_FREQ 50000000
#define ALT_MODULE_CLASS_led_pio altera_avalon_pio

/*
 * seven_seg_pio configuration
 *
 */

#define SEVEN_SEG_PIO_NAME "/dev/seven_seg_pio"
#define SEVEN_SEG_PIO_TYPE "altera_avalon_pio"
#define SEVEN_SEG_PIO_BASE 0x01001060
#define SEVEN_SEG_PIO_SPAN 16
#define SEVEN_SEG_PIO_DO_TEST_BENCH_WIRING 0
#define SEVEN_SEG_PIO_DRIVEN_SIM_VALUE 0
#define SEVEN_SEG_PIO_HAS_TRI 0
#define SEVEN_SEG_PIO_HAS_OUT 1
#define SEVEN_SEG_PIO_HAS_IN 0
#define SEVEN_SEG_PIO_CAPTURE 0
#define SEVEN_SEG_PIO_DATA_WIDTH 16
#define SEVEN_SEG_PIO_RESET_VALUE 0
#define SEVEN_SEG_PIO_EDGE_TYPE "NONE"
#define SEVEN_SEG_PIO_IRQ_TYPE "NONE"
#define SEVEN_SEG_PIO_BIT_CLEARING_EDGE_REGISTER 0
#define SEVEN_SEG_PIO_BIT_MODIFYING_OUTPUT_REGISTER 0
#define SEVEN_SEG_PIO_FREQ 50000000
#define ALT_MODULE_CLASS_seven_seg_pio altera_avalon_pio

/*
 * button_pio configuration
 *
 */

#define BUTTON_PIO_NAME "/dev/button_pio"
#define BUTTON_PIO_TYPE "altera_avalon_pio"
#define BUTTON_PIO_BASE 0x01001070
#define BUTTON_PIO_SPAN 16
#define BUTTON_PIO_IRQ 31
#define BUTTON_PIO_IRQ_INTERRUPT_CONTROLLER_ID 0
#define BUTTON_PIO_DO_TEST_BENCH_WIRING 0
#define BUTTON_PIO_DRIVEN_SIM_VALUE 0
#define BUTTON_PIO_HAS_TRI 0
#define BUTTON_PIO_HAS_OUT 0
#define BUTTON_PIO_HAS_IN 1
#define BUTTON_PIO_CAPTURE 1
#define BUTTON_PIO_DATA_WIDTH 4
#define BUTTON_PIO_RESET_VALUE 0
#define BUTTON_PIO_EDGE_TYPE "ANY"
#define BUTTON_PIO_IRQ_TYPE "EDGE"
#define BUTTON_PIO_BIT_CLEARING_EDGE_REGISTER 0
#define BUTTON_PIO_BIT_MODIFYING_OUTPUT_REGISTER 0
#define BUTTON_PIO_FREQ 50000000
#define ALT_MODULE_CLASS_button_pio altera_avalon_pio

/*
 * green_led_pio configuration
 *
 */

#define GREEN_LED_PIO_NAME "/dev/green_led_pio"
#define GREEN_LED_PIO_TYPE "altera_avalon_pio"
#define GREEN_LED_PIO_BASE 0x01001080
#define GREEN_LED_PIO_SPAN 16
#define GREEN_LED_PIO_DO_TEST_BENCH_WIRING 0
#define GREEN_LED_PIO_DRIVEN_SIM_VALUE 0
#define GREEN_LED_PIO_HAS_TRI 0
#define GREEN_LED_PIO_HAS_OUT 1
#define GREEN_LED_PIO_HAS_IN 0
#define GREEN_LED_PIO_CAPTURE 0
#define GREEN_LED_PIO_DATA_WIDTH 8
#define GREEN_LED_PIO_RESET_VALUE 0
#define GREEN_LED_PIO_EDGE_TYPE "NONE"
#define GREEN_LED_PIO_IRQ_TYPE "NONE"
#define GREEN_LED_PIO_BIT_CLEARING_EDGE_REGISTER 0
#define GREEN_LED_PIO_BIT_MODIFYING_OUTPUT_REGISTER 0
#define GREEN_LED_PIO_FREQ 50000000
#define ALT_MODULE_CLASS_green_led_pio altera_avalon_pio

/*
 * red_led_pio configuration
 *
 */

#define RED_LED_PIO_NAME "/dev/red_led_pio"
#define RED_LED_PIO_TYPE "altera_avalon_pio"
#define RED_LED_PIO_BASE 0x01001090
#define RED_LED_PIO_SPAN 16
#define RED_LED_PIO_DO_TEST_BENCH_WIRING 0
#define RED_LED_PIO_DRIVEN_SIM_VALUE 0
#define RED_LED_PIO_HAS_TRI 0
#define RED_LED_PIO_HAS_OUT 1
#define RED_LED_PIO_HAS_IN 0
#define RED_LED_PIO_CAPTURE 0
#define RED_LED_PIO_DATA_WIDTH 8
#define RED_LED_PIO_RESET_VALUE 0
#define RED_LED_PIO_EDGE_TYPE "NONE"
#define RED_LED_PIO_IRQ_TYPE "NONE"
#define RED_LED_PIO_BIT_CLEARING_EDGE_REGISTER 0
#define RED_LED_PIO_BIT_MODIFYING_OUTPUT_REGISTER 0
#define RED_LED_PIO_FREQ 50000000
#define ALT_MODULE_CLASS_red_led_pio altera_avalon_pio

/*
 * seven_seg_middle_pio configuration
 *
 */

#define SEVEN_SEG_MIDDLE_PIO_NAME "/dev/seven_seg_middle_pio"
#define SEVEN_SEG_MIDDLE_PIO_TYPE "altera_avalon_pio"
#define SEVEN_SEG_MIDDLE_PIO_BASE 0x010010a0
#define SEVEN_SEG_MIDDLE_PIO_SPAN 16
#define SEVEN_SEG_MIDDLE_PIO_DO_TEST_BENCH_WIRING 0
#define SEVEN_SEG_MIDDLE_PIO_DRIVEN_SIM_VALUE 0
#define SEVEN_SEG_MIDDLE_PIO_HAS_TRI 0
#define SEVEN_SEG_MIDDLE_PIO_HAS_OUT 1
#define SEVEN_SEG_MIDDLE_PIO_HAS_IN 0
#define SEVEN_SEG_MIDDLE_PIO_CAPTURE 0
#define SEVEN_SEG_MIDDLE_PIO_DATA_WIDTH 16
#define SEVEN_SEG_MIDDLE_PIO_RESET_VALUE 0
#define SEVEN_SEG_MIDDLE_PIO_EDGE_TYPE "NONE"
#define SEVEN_SEG_MIDDLE_PIO_IRQ_TYPE "NONE"
#define SEVEN_SEG_MIDDLE_PIO_BIT_CLEARING_EDGE_REGISTER 0
#define SEVEN_SEG_MIDDLE_PIO_BIT_MODIFYING_OUTPUT_REGISTER 0
#define SEVEN_SEG_MIDDLE_PIO_FREQ 50000000
#define ALT_MODULE_CLASS_seven_seg_middle_pio altera_avalon_pio

/*
 * seven_seg_right_pio configuration
 *
 */

#define SEVEN_SEG_RIGHT_PIO_NAME "/dev/seven_seg_right_pio"
#define SEVEN_SEG_RIGHT_PIO_TYPE "altera_avalon_pio"
#define SEVEN_SEG_RIGHT_PIO_BASE 0x010010b0
#define SEVEN_SEG_RIGHT_PIO_SPAN 16
#define SEVEN_SEG_RIGHT_PIO_DO_TEST_BENCH_WIRING 0
#define SEVEN_SEG_RIGHT_PIO_DRIVEN_SIM_VALUE 0
#define SEVEN_SEG_RIGHT_PIO_HAS_TRI 0
#define SEVEN_SEG_RIGHT_PIO_HAS_OUT 1
#define SEVEN_SEG_RIGHT_PIO_HAS_IN 0
#define SEVEN_SEG_RIGHT_PIO_CAPTURE 0
#define SEVEN_SEG_RIGHT_PIO_DATA_WIDTH 32
#define SEVEN_SEG_RIGHT_PIO_RESET_VALUE 0
#define SEVEN_SEG_RIGHT_PIO_EDGE_TYPE "NONE"
#define SEVEN_SEG_RIGHT_PIO_IRQ_TYPE "NONE"
#define SEVEN_SEG_RIGHT_PIO_BIT_CLEARING_EDGE_REGISTER 0
#define SEVEN_SEG_RIGHT_PIO_BIT_MODIFYING_OUTPUT_REGISTER 0
#define SEVEN_SEG_RIGHT_PIO_FREQ 50000000
#define ALT_MODULE_CLASS_seven_seg_right_pio altera_avalon_pio

/*
 * switch_pio configuration
 *
 */

#define SWITCH_PIO_NAME "/dev/switch_pio"
#define SWITCH_PIO_TYPE "altera_avalon_pio"
#define SWITCH_PIO_BASE 0x010010c0
#define SWITCH_PIO_SPAN 16
#define SWITCH_PIO_DO_TEST_BENCH_WIRING 0
#define SWITCH_PIO_DRIVEN_SIM_VALUE 0
#define SWITCH_PIO_HAS_TRI 0
#define SWITCH_PIO_HAS_OUT 0
#define SWITCH_PIO_HAS_IN 1
#define SWITCH_PIO_CAPTURE 0
#define SWITCH_PIO_DATA_WIDTH 16
#define SWITCH_PIO_RESET_VALUE 0
#define SWITCH_PIO_EDGE_TYPE "NONE"
#define SWITCH_PIO_IRQ_TYPE "NONE"
#define SWITCH_PIO_BIT_CLEARING_EDGE_REGISTER 0
#define SWITCH_PIO_BIT_MODIFYING_OUTPUT_REGISTER 0
#define SWITCH_PIO_FREQ 50000000
#define ALT_MODULE_CLASS_switch_pio altera_avalon_pio

/*
 * timer_0 configuration
 *
 */

#define TIMER_0_NAME "/dev/timer_0"
#define TIMER_0_TYPE "altera_avalon_timer"
#define TIMER_0_BASE 0x01001000
#define TIMER_0_SPAN 32
#define TIMER_0_IRQ 0
#define TIMER_0_IRQ_INTERRUPT_CONTROLLER_ID 0
#define TIMER_0_ALWAYS_RUN 0
#define TIMER_0_FIXED_PERIOD 0
#define TIMER_0_SNAPSHOT 1
#define TIMER_0_PERIOD 1
#define TIMER_0_PERIOD_UNITS "ms"
#define TIMER_0_RESET_OUTPUT 0
#define TIMER_0_TIMEOUT_PULSE_OUTPUT 0
#define TIMER_0_LOAD_VALUE 49999
#define TIMER_0_COUNTER_SIZE 32
#define TIMER_0_MULT 0.0010
#define TIMER_0_TICKS_PER_SEC 1000
#define TIMER_0_FREQ 50000000
#define ALT_MODULE_CLASS_timer_0 altera_avalon_timer

/*
 * timer_1 configuration
 *
 */

#define TIMER_1_NAME "/dev/timer_1"
#define TIMER_1_TYPE "altera_avalon_timer"
#define TIMER_1_BASE 0x01001020
#define TIMER_1_SPAN 32
#define TIMER_1_IRQ 1
#define TIMER_1_IRQ_INTERRUPT_CONTROLLER_ID 0
#define TIMER_1_ALWAYS_RUN 0
#define TIMER_1_FIXED_PERIOD 0
#define TIMER_1_SNAPSHOT 1
#define TIMER_1_PERIOD 10
#define TIMER_1_PERIOD_UNITS "us"
#define TIMER_1_RESET_OUTPUT 0
#define TIMER_1_TIMEOUT_PULSE_OUTPUT 0
#define TIMER_1_LOAD_VALUE 499
#define TIMER_1_COUNTER_SIZE 32
#define TIMER_1_MULT "1.0E-6"
#define TIMER_1_TICKS_PER_SEC 100000
#define TIMER_1_FREQ 50000000
#define ALT_MODULE_CLASS_timer_1 altera_avalon_timer

/*
 * sdram_0 configuration
 *
 */

#define SDRAM_0_NAME "/dev/sdram_0"
#define SDRAM_0_TYPE "altera_avalon_new_sdram_controller"
#define SDRAM_0_BASE 0x00800000
#define SDRAM_0_SPAN 8388608
#define SDRAM_0_REGISTER_DATA_IN 1
#define SDRAM_0_SIM_MODEL_BASE 0
#define SDRAM_0_SDRAM_DATA_WIDTH 16
#define SDRAM_0_SDRAM_ADDR_WIDTH 12
#define SDRAM_0_SDRAM_ROW_WIDTH 12
#define SDRAM_0_SDRAM_COL_WIDTH 8
#define SDRAM_0_SDRAM_NUM_CHIPSELECTS 1
#define SDRAM_0_SDRAM_NUM_BANKS 4
#define SDRAM_0_REFRESH_PERIOD 15.625
#define SDRAM_0_POWERUP_DELAY 100.0
#define SDRAM_0_CAS_LATENCY 3
#define SDRAM_0_T_RFC 70.0
#define SDRAM_0_T_RP 20.0
#define SDRAM_0_T_MRD 3
#define SDRAM_0_T_RCD 20.0
#define SDRAM_0_T_AC 5.5
#define SDRAM_0_T_WR 14.0
#define SDRAM_0_INIT_REFRESH_COMMANDS 2
#define SDRAM_0_INIT_NOP_DELAY 0.0
#define SDRAM_0_SHARED_DATA 0
#define SDRAM_0_SDRAM_BANK_WIDTH 2
#define SDRAM_0_TRISTATE_BRIDGE_SLAVE ""
#define SDRAM_0_STARVATION_INDICATOR 0
#define SDRAM_0_IS_INITIALIZED 1
#define ALT_MODULE_CLASS_sdram_0 altera_avalon_new_sdram_controller

/*
 * pio_pulse configuration
 *
 */

#define PIO_PULSE_NAME "/dev/pio_pulse"
#define PIO_PULSE_TYPE "altera_avalon_pio"
#define PIO_PULSE_BASE 0x010010d0
#define PIO_PULSE_SPAN 16
#define PIO_PULSE_IRQ 4
#define PIO_PULSE_IRQ_INTERRUPT_CONTROLLER_ID 0
#define PIO_PULSE_DO_TEST_BENCH_WIRING 0
#define PIO_PULSE_DRIVEN_SIM_VALUE 0
#define PIO_PULSE_HAS_TRI 0
#define PIO_PULSE_HAS_OUT 0
#define PIO_PULSE_HAS_IN 1
#define PIO_PULSE_CAPTURE 1
#define PIO_PULSE_DATA_WIDTH 1
#define PIO_PULSE_RESET_VALUE 0
#define PIO_PULSE_EDGE_TYPE "ANY"
#define PIO_PULSE_IRQ_TYPE "EDGE"
#define PIO_PULSE_BIT_CLEARING_EDGE_REGISTER 0
#define PIO_PULSE_BIT_MODIFYING_OUTPUT_REGISTER 0
#define PIO_PULSE_FREQ 50000000
#define ALT_MODULE_CLASS_pio_pulse altera_avalon_pio

/*
 * pio_latency configuration
 *
 */

#define PIO_LATENCY_NAME "/dev/pio_latency"
#define PIO_LATENCY_TYPE "altera_avalon_pio"
#define PIO_LATENCY_BASE 0x010010e0
#define PIO_LATENCY_SPAN 16
#define PIO_LATENCY_DO_TEST_BENCH_WIRING 0
#define PIO_LATENCY_DRIVEN_SIM_VALUE 0
#define PIO_LATENCY_HAS_TRI 0
#define PIO_LATENCY_HAS_OUT 0
#define PIO_LATENCY_HAS_IN 1
#define PIO_LATENCY_CAPTURE 0
#define PIO_LATENCY_DATA_WIDTH 16
#define PIO_LATENCY_RESET_VALUE 0
#define PIO_LATENCY_EDGE_TYPE "NONE"
#define PIO_LATENCY_IRQ_TYPE "NONE"
#define PIO_LATENCY_BIT_CLEARING_EDGE_REGISTER 0
#define PIO_LATENCY_BIT_MODIFYING_OUTPUT_REGISTER 0
#define PIO_LATENCY_FREQ 50000000
#define ALT_MODULE_CLASS_pio_latency altera_avalon_pio

/*
 * pio_missed configuration
 *
 */

#define PIO_MISSED_NAME "/dev/pio_missed"
#define PIO_MISSED_TYPE "altera_avalon_pio"
#define PIO_MISSED_BASE 0x010010f0
#define PIO_MISSED_SPAN 16
#define PIO_MISSED_DO_TEST_BENCH_WIRING 0
#define PIO_MISSED_DRIVEN_SIM_VALUE 0
#define PIO_MISSED_HAS_TRI 0
#define PIO_MISSED_HAS_OUT 0
#define PIO_MISSED_HAS_IN 1
#define PIO_MISSED_CAPTURE 0
#define PIO_MISSED_DATA_WIDTH 16
#define PIO_MISSED_RESET_VALUE 0
#define PIO_MISSED_EDGE_TYPE "NONE"
#define PIO_MISSED_IRQ_TYPE "NONE"
#define PIO_MISSED_BIT_CLEARING_EDGE_REGISTER 0
#define PIO_MISSED_BIT_MODIFYING_OUTPUT_REGISTER 0
#define PIO_MISSED_FREQ 50000000
#define ALT_MODULE_CLASS_pio_missed altera_avalon_pio

/*
 * pio_dutycycle configuration
 *
 */

#define PIO_DUTYCYCLE_NAME "/dev/pio_dutycycle"
#define PIO_DUTYCYCLE_TYPE "altera_avalon_pio"
#define PIO_DUTYCYCLE_BASE 0x01001100
#define PIO_DUTYCYCLE_SPAN 16
#define PIO_DUTYCYCLE_DO_TEST_BENCH_WIRING 0
#define PIO_DUTYCYCLE_DRIVEN_SIM_VALUE 0
#define PIO_DUTYCYCLE_HAS_TRI 0
#define PIO_DUTYCYCLE_HAS_OUT 1
#define PIO_DUTYCYCLE_HAS_IN 0
#define PIO_DUTYCYCLE_CAPTURE 0
#define PIO_DUTYCYCLE_DATA_WIDTH 4
#define PIO_DUTYCYCLE_RESET_VALUE 0
#define PIO_DUTYCYCLE_EDGE_TYPE "NONE"
#define PIO_DUTYCYCLE_IRQ_TYPE "NONE"
#define PIO_DUTYCYCLE_BIT_CLEARING_EDGE_REGISTER 0
#define PIO_DUTYCYCLE_BIT_MODIFYING_OUTPUT_REGISTER 0
#define PIO_DUTYCYCLE_FREQ 50000000
#define ALT_MODULE_CLASS_pio_dutycycle altera_avalon_pio

/*
 * pio_period configuration
 *
 */

#define PIO_PERIOD_NAME "/dev/pio_period"
#define PIO_PERIOD_TYPE "altera_avalon_pio"
#define PIO_PERIOD_BASE 0x01001110
#define PIO_PERIOD_SPAN 16
#define PIO_PERIOD_DO_TEST_BENCH_WIRING 0
#define PIO_PERIOD_DRIVEN_SIM_VALUE 0
#define PIO_PERIOD_HAS_TRI 0
#define PIO_PERIOD_HAS_OUT 1
#define PIO_PERIOD_HAS_IN 0
#define PIO_PERIOD_CAPTURE 0
#define PIO_PERIOD_DATA_WIDTH 4
#define PIO_PERIOD_RESET_VALUE 0
#define PIO_PERIOD_EDGE_TYPE "NONE"
#define PIO_PERIOD_IRQ_TYPE "NONE"
#define PIO_PERIOD_BIT_CLEARING_EDGE_REGISTER 0
#define PIO_PERIOD_BIT_MODIFYING_OUTPUT_REGISTER 0
#define PIO_PERIOD_FREQ 50000000
#define ALT_MODULE_CLASS_pio_period altera_avalon_pio

/*
 * pio_response configuration
 *
 */

#define PIO_RESPONSE_NAME "/dev/pio_response"
#define PIO_RESPONSE_TYPE "altera_avalon_pio"
#define PIO_RESPONSE_BASE 0x01001120
#define PIO_RESPONSE_SPAN 16
#define PIO_RESPONSE_DO_TEST_BENCH_WIRING 0
#define PIO_RESPONSE_DRIVEN_SIM_VALUE 0
#define PIO_RESPONSE_HAS_TRI 0
#define PIO_RESPONSE_HAS_OUT 1
#define PIO_RESPONSE_HAS_IN 0
#define PIO_RESPONSE_CAPTURE 0
#define PIO_RESPONSE_DATA_WIDTH 1
#define PIO_RESPONSE_RESET_VALUE 0
#define PIO_RESPONSE_EDGE_TYPE "NONE"
#define PIO_RESPONSE_IRQ_TYPE "NONE"
#define PIO_RESPONSE_BIT_CLEARING_EDGE_REGISTER 0
#define PIO_RESPONSE_BIT_MODIFYING_OUTPUT_REGISTER 0
#define PIO_RESPONSE_FREQ 50000000
#define ALT_MODULE_CLASS_pio_response altera_avalon_pio

/*
 * pio_egmenable configuration
 *
 */

#define PIO_EGMENABLE_NAME "/dev/pio_egmenable"
#define PIO_EGMENABLE_TYPE "altera_avalon_pio"
#define PIO_EGMENABLE_BASE 0x01001130
#define PIO_EGMENABLE_SPAN 16
#define PIO_EGMENABLE_DO_TEST_BENCH_WIRING 0
#define PIO_EGMENABLE_DRIVEN_SIM_VALUE 0
#define PIO_EGMENABLE_HAS_TRI 0
#define PIO_EGMENABLE_HAS_OUT 1
#define PIO_EGMENABLE_HAS_IN 0
#define PIO_EGMENABLE_CAPTURE 0
#define PIO_EGMENABLE_DATA_WIDTH 1
#define PIO_EGMENABLE_RESET_VALUE 0
#define PIO_EGMENABLE_EDGE_TYPE "NONE"
#define PIO_EGMENABLE_IRQ_TYPE "NONE"
#define PIO_EGMENABLE_BIT_CLEARING_EDGE_REGISTER 0
#define PIO_EGMENABLE_BIT_MODIFYING_OUTPUT_REGISTER 0
#define PIO_EGMENABLE_FREQ 50000000
#define ALT_MODULE_CLASS_pio_egmenable altera_avalon_pio

/*
 * pio_egmreset configuration
 *
 */

#define PIO_EGMRESET_NAME "/dev/pio_egmreset"
#define PIO_EGMRESET_TYPE "altera_avalon_pio"
#define PIO_EGMRESET_BASE 0x01001140
#define PIO_EGMRESET_SPAN 16
#define PIO_EGMRESET_DO_TEST_BENCH_WIRING 0
#define PIO_EGMRESET_DRIVEN_SIM_VALUE 0
#define PIO_EGMRESET_HAS_TRI 0
#define PIO_EGMRESET_HAS_OUT 1
#define PIO_EGMRESET_HAS_IN 0
#define PIO_EGMRESET_CAPTURE 0
#define PIO_EGMRESET_DATA_WIDTH 1
#define PIO_EGMRESET_RESET_VALUE 0
#define PIO_EGMRESET_EDGE_TYPE "NONE"
#define PIO_EGMRESET_IRQ_TYPE "NONE"
#define PIO_EGMRESET_BIT_CLEARING_EDGE_REGISTER 0
#define PIO_EGMRESET_BIT_MODIFYING_OUTPUT_REGISTER 0
#define PIO_EGMRESET_FREQ 50000000
#define ALT_MODULE_CLASS_pio_egmreset altera_avalon_pio

/*
 * system library configuration
 *
 */

#define ALT_MAX_FD 32
#define ALT_SYS_CLK TIMER_0
#define ALT_TIMESTAMP_CLK none

/*
 * Devices associated with code sections.
 *
 */

#define ALT_TEXT_DEVICE       SDRAM_0
#define ALT_RODATA_DEVICE     SDRAM_0
#define ALT_RWDATA_DEVICE     SDRAM_0
#define ALT_EXCEPTIONS_DEVICE SDRAM_0
#define ALT_RESET_DEVICE      SDRAM_0

/*
 * The text section is initialised so no bootloader will be required.
 * Set a variable to tell crt0.S to provide code at the reset address and
 * to initialise rwdata if appropriate.
 */

#define ALT_NO_BOOTLOADER


#endif /* __SYSTEM_H_ */
