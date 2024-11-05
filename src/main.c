//
// This file is part of the GNU ARM Eclipse distribution.
// Copyright (c) 2014 Liviu Ionescu.
//

// ----------------------------------------------------------------------------
// School: University of Victoria, Canada.
// Course: ECE 355 "Microprocessor-Based Systems".
// This is template code for Part 2 of Introductory Lab.
//
// See "system/include/cmsis/stm32f051x8.h" for register/bit definitions.
// See "system/src/cmsis/vectors_stm32f051x8.c" for handler declarations.
// ----------------------------------------------------------------------------

#include <stdio.h>
#include "diag/Trace.h"
#include "cmsis/cmsis_device.h"

// ----------------------------------------------------------------------------
//
// STM32F0 empty sample (trace via $(trace)).
//
// Trace support is enabled by adding the TRACE macro definition.
// By default the trace messages are forwarded to the $(trace) output,
// but can be rerouted to any device or completely suppressed, by
// changing the definitions required in system/src/diag/trace_impl.c
// (currently OS_USE_TRACE_ITM, OS_USE_TRACE_SEMIHOSTING_DEBUG/_STDOUT).
//

// ----- main() ---------------------------------------------------------------

// Sample pragmas to cope with warnings. Please note the related line at
// the end of this function, used to pop the compiler diagnostics status.
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wmissing-declarations"
#pragma GCC diagnostic ignored "-Wreturn-type"


/* Definitions of registers and their bits are
   given in system/include/cmsis/stm32f051x8.h */


/* Clock prescaler for TIM2 timer: no prescaling */
#define myTIM2_PRESCALER ((uint16_t)0x0000)
/* Maximum possible setting for overflow */
#define myTIM2_PERIOD ((uint32_t)0xFFFFFFFF)

void myGPIOA_Init(void);
void myTIM2_Init(void);
void myEXTI_Init(void);
void myTIM3_Init(void);

unsigned int Freq = 0;  // Example: measured frequency value (global variable)
unsigned int Res = 0;   // Example: measured resistance value (global variable)


void oled_Write(unsigned char);
void oled_Write_Cmd(unsigned char);
void oled_Write_Data(unsigned char);
void oled_config(void);
void refresh_OLED(void);


SPI_HandleTypeDef SPI_Handle;


//
// LED Display initialization commands
//
unsigned char oled_init_cmds[] =
{
    0xAE,
    0x20, 0x00,
    0x40,
    0xA0 | 0x01,
    0xA8, 0x40 - 1,
    0xC0 | 0x08,
    0xD3, 0x00,
    0xDA, 0x32,
    0xD5, 0x80,
    0xD9, 0x22,
    0xDB, 0x30,
    0x81, 0xFF,
    0xA4,
    0xA6,
    0xAD, 0x30,
    0x8D, 0x10,
    0xAE | 0x01,
    0xC0,
    0xA0
};


//
// Character specifications for LED Display (1 row = 8 bytes = 1 ASCII character)
// Example: to display '4', retrieve 8 data bytes stored in Characters[52][X] row
//          (where X = 0, 1, ..., 7) and send them one by one to LED Display.
// Row number = character ASCII code (e.g., ASCII code of '4' is 0x34 = 52)
//
unsigned char Characters[][8] = {
    {0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // SPACE
    {0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // SPACE
    {0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // SPACE
    {0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // SPACE
    {0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // SPACE
    {0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // SPACE
    {0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // SPACE
    {0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // SPACE
    {0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // SPACE
    {0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // SPACE
    {0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // SPACE
    {0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // SPACE
    {0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // SPACE
    {0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // SPACE
    {0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // SPACE
    {0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // SPACE
    {0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // SPACE
    {0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // SPACE
    {0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // SPACE
    {0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // SPACE
    {0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // SPACE
    {0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // SPACE
    {0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // SPACE
    {0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // SPACE
    {0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // SPACE
    {0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // SPACE
    {0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // SPACE
    {0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // SPACE
    {0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // SPACE
    {0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // SPACE
    {0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // SPACE
    {0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // SPACE
    {0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // SPACE
    {0b00000000, 0b00000000, 0b01011111, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // !
    {0b00000000, 0b00000111, 0b00000000, 0b00000111, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // "
    {0b00010100, 0b01111111, 0b00010100, 0b01111111, 0b00010100,0b00000000, 0b00000000, 0b00000000},  // #
    {0b00100100, 0b00101010, 0b01111111, 0b00101010, 0b00010010,0b00000000, 0b00000000, 0b00000000},  // $
    {0b00100011, 0b00010011, 0b00001000, 0b01100100, 0b01100010,0b00000000, 0b00000000, 0b00000000},  // %
    {0b00110110, 0b01001001, 0b01010101, 0b00100010, 0b01010000,0b00000000, 0b00000000, 0b00000000},  // &
    {0b00000000, 0b00000101, 0b00000011, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // '
    {0b00000000, 0b00011100, 0b00100010, 0b01000001, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // (
    {0b00000000, 0b01000001, 0b00100010, 0b00011100, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // )
    {0b00010100, 0b00001000, 0b00111110, 0b00001000, 0b00010100,0b00000000, 0b00000000, 0b00000000},  // *
    {0b00001000, 0b00001000, 0b00111110, 0b00001000, 0b00001000,0b00000000, 0b00000000, 0b00000000},  // +
    {0b00000000, 0b01010000, 0b00110000, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // ,
    {0b00001000, 0b00001000, 0b00001000, 0b00001000, 0b00001000,0b00000000, 0b00000000, 0b00000000},  // -
    {0b00000000, 0b01100000, 0b01100000, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // .
    {0b00100000, 0b00010000, 0b00001000, 0b00000100, 0b00000010,0b00000000, 0b00000000, 0b00000000},  // /
    {0b00111110, 0b01010001, 0b01001001, 0b01000101, 0b00111110,0b00000000, 0b00000000, 0b00000000},  // 0
    {0b00000000, 0b01000010, 0b01111111, 0b01000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // 1
    {0b01000010, 0b01100001, 0b01010001, 0b01001001, 0b01000110,0b00000000, 0b00000000, 0b00000000},  // 2
    {0b00100001, 0b01000001, 0b01000101, 0b01001011, 0b00110001,0b00000000, 0b00000000, 0b00000000},  // 3
    {0b00011000, 0b00010100, 0b00010010, 0b01111111, 0b00010000,0b00000000, 0b00000000, 0b00000000},  // 4
    {0b00100111, 0b01000101, 0b01000101, 0b01000101, 0b00111001,0b00000000, 0b00000000, 0b00000000},  // 5
    {0b00111100, 0b01001010, 0b01001001, 0b01001001, 0b00110000,0b00000000, 0b00000000, 0b00000000},  // 6
    {0b00000011, 0b00000001, 0b01110001, 0b00001001, 0b00000111,0b00000000, 0b00000000, 0b00000000},  // 7
    {0b00110110, 0b01001001, 0b01001001, 0b01001001, 0b00110110,0b00000000, 0b00000000, 0b00000000},  // 8
    {0b00000110, 0b01001001, 0b01001001, 0b00101001, 0b00011110,0b00000000, 0b00000000, 0b00000000},  // 9
    {0b00000000, 0b00110110, 0b00110110, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // :
    {0b00000000, 0b01010110, 0b00110110, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // ;
    {0b00001000, 0b00010100, 0b00100010, 0b01000001, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // <
    {0b00010100, 0b00010100, 0b00010100, 0b00010100, 0b00010100,0b00000000, 0b00000000, 0b00000000},  // =
    {0b00000000, 0b01000001, 0b00100010, 0b00010100, 0b00001000,0b00000000, 0b00000000, 0b00000000},  // >
    {0b00000010, 0b00000001, 0b01010001, 0b00001001, 0b00000110,0b00000000, 0b00000000, 0b00000000},  // ?
    {0b00110010, 0b01001001, 0b01111001, 0b01000001, 0b00111110,0b00000000, 0b00000000, 0b00000000},  // @
    {0b01111110, 0b00010001, 0b00010001, 0b00010001, 0b01111110,0b00000000, 0b00000000, 0b00000000},  // A
    {0b01111111, 0b01001001, 0b01001001, 0b01001001, 0b00110110,0b00000000, 0b00000000, 0b00000000},  // B
    {0b00111110, 0b01000001, 0b01000001, 0b01000001, 0b00100010,0b00000000, 0b00000000, 0b00000000},  // C
    {0b01111111, 0b01000001, 0b01000001, 0b00100010, 0b00011100,0b00000000, 0b00000000, 0b00000000},  // D
    {0b01111111, 0b01001001, 0b01001001, 0b01001001, 0b01000001,0b00000000, 0b00000000, 0b00000000},  // E
    {0b01111111, 0b00001001, 0b00001001, 0b00001001, 0b00000001,0b00000000, 0b00000000, 0b00000000},  // F
    {0b00111110, 0b01000001, 0b01001001, 0b01001001, 0b01111010,0b00000000, 0b00000000, 0b00000000},  // G
    {0b01111111, 0b00001000, 0b00001000, 0b00001000, 0b01111111,0b00000000, 0b00000000, 0b00000000},  // H
    {0b01000000, 0b01000001, 0b01111111, 0b01000001, 0b01000000,0b00000000, 0b00000000, 0b00000000},  // I
    {0b00100000, 0b01000000, 0b01000001, 0b00111111, 0b00000001,0b00000000, 0b00000000, 0b00000000},  // J
    {0b01111111, 0b00001000, 0b00010100, 0b00100010, 0b01000001,0b00000000, 0b00000000, 0b00000000},  // K
    {0b01111111, 0b01000000, 0b01000000, 0b01000000, 0b01000000,0b00000000, 0b00000000, 0b00000000},  // L
    {0b01111111, 0b00000010, 0b00001100, 0b00000010, 0b01111111,0b00000000, 0b00000000, 0b00000000},  // M
    {0b01111111, 0b00000100, 0b00001000, 0b00010000, 0b01111111,0b00000000, 0b00000000, 0b00000000},  // N
    {0b00111110, 0b01000001, 0b01000001, 0b01000001, 0b00111110,0b00000000, 0b00000000, 0b00000000},  // O
    {0b01111111, 0b00001001, 0b00001001, 0b00001001, 0b00000110,0b00000000, 0b00000000, 0b00000000},  // P
    {0b00111110, 0b01000001, 0b01010001, 0b00100001, 0b01011110,0b00000000, 0b00000000, 0b00000000},  // Q
    {0b01111111, 0b00001001, 0b00011001, 0b00101001, 0b01000110,0b00000000, 0b00000000, 0b00000000},  // R
    {0b01000110, 0b01001001, 0b01001001, 0b01001001, 0b00110001,0b00000000, 0b00000000, 0b00000000},  // S
    {0b00000001, 0b00000001, 0b01111111, 0b00000001, 0b00000001,0b00000000, 0b00000000, 0b00000000},  // T
    {0b00111111, 0b01000000, 0b01000000, 0b01000000, 0b00111111,0b00000000, 0b00000000, 0b00000000},  // U
    {0b00011111, 0b00100000, 0b01000000, 0b00100000, 0b00011111,0b00000000, 0b00000000, 0b00000000},  // V
    {0b00111111, 0b01000000, 0b00111000, 0b01000000, 0b00111111,0b00000000, 0b00000000, 0b00000000},  // W
    {0b01100011, 0b00010100, 0b00001000, 0b00010100, 0b01100011,0b00000000, 0b00000000, 0b00000000},  // X
    {0b00000111, 0b00001000, 0b01110000, 0b00001000, 0b00000111,0b00000000, 0b00000000, 0b00000000},  // Y
    {0b01100001, 0b01010001, 0b01001001, 0b01000101, 0b01000011,0b00000000, 0b00000000, 0b00000000},  // Z
    {0b01111111, 0b01000001, 0b00000000, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // [
    {0b00010101, 0b00010110, 0b01111100, 0b00010110, 0b00010101,0b00000000, 0b00000000, 0b00000000},  // back slash
    {0b00000000, 0b00000000, 0b00000000, 0b01000001, 0b01111111,0b00000000, 0b00000000, 0b00000000},  // ]
    {0b00000100, 0b00000010, 0b00000001, 0b00000010, 0b00000100,0b00000000, 0b00000000, 0b00000000},  // ^
    {0b01000000, 0b01000000, 0b01000000, 0b01000000, 0b01000000,0b00000000, 0b00000000, 0b00000000},  // _
    {0b00000000, 0b00000001, 0b00000010, 0b00000100, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // `
    {0b00100000, 0b01010100, 0b01010100, 0b01010100, 0b01111000,0b00000000, 0b00000000, 0b00000000},  // a
    {0b01111111, 0b01001000, 0b01000100, 0b01000100, 0b00111000,0b00000000, 0b00000000, 0b00000000},  // b
    {0b00111000, 0b01000100, 0b01000100, 0b01000100, 0b00100000,0b00000000, 0b00000000, 0b00000000},  // c
    {0b00111000, 0b01000100, 0b01000100, 0b01001000, 0b01111111,0b00000000, 0b00000000, 0b00000000},  // d
    {0b00111000, 0b01010100, 0b01010100, 0b01010100, 0b00011000,0b00000000, 0b00000000, 0b00000000},  // e
    {0b00001000, 0b01111110, 0b00001001, 0b00000001, 0b00000010,0b00000000, 0b00000000, 0b00000000},  // f
    {0b00001100, 0b01010010, 0b01010010, 0b01010010, 0b00111110,0b00000000, 0b00000000, 0b00000000},  // g
    {0b01111111, 0b00001000, 0b00000100, 0b00000100, 0b01111000,0b00000000, 0b00000000, 0b00000000},  // h
    {0b00000000, 0b01000100, 0b01111101, 0b01000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // i
    {0b00100000, 0b01000000, 0b01000100, 0b00111101, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // j
    {0b01111111, 0b00010000, 0b00101000, 0b01000100, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // k
    {0b00000000, 0b01000001, 0b01111111, 0b01000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // l
    {0b01111100, 0b00000100, 0b00011000, 0b00000100, 0b01111000,0b00000000, 0b00000000, 0b00000000},  // m
    {0b01111100, 0b00001000, 0b00000100, 0b00000100, 0b01111000,0b00000000, 0b00000000, 0b00000000},  // n
    {0b00111000, 0b01000100, 0b01000100, 0b01000100, 0b00111000,0b00000000, 0b00000000, 0b00000000},  // o
    {0b01111100, 0b00010100, 0b00010100, 0b00010100, 0b00001000,0b00000000, 0b00000000, 0b00000000},  // p
    {0b00001000, 0b00010100, 0b00010100, 0b00011000, 0b01111100,0b00000000, 0b00000000, 0b00000000},  // q
    {0b01111100, 0b00001000, 0b00000100, 0b00000100, 0b00001000,0b00000000, 0b00000000, 0b00000000},  // r
    {0b01001000, 0b01010100, 0b01010100, 0b01010100, 0b00100000,0b00000000, 0b00000000, 0b00000000},  // s
    {0b00000100, 0b00111111, 0b01000100, 0b01000000, 0b00100000,0b00000000, 0b00000000, 0b00000000},  // t
    {0b00111100, 0b01000000, 0b01000000, 0b00100000, 0b01111100,0b00000000, 0b00000000, 0b00000000},  // u
    {0b00011100, 0b00100000, 0b01000000, 0b00100000, 0b00011100,0b00000000, 0b00000000, 0b00000000},  // v
    {0b00111100, 0b01000000, 0b00111000, 0b01000000, 0b00111100,0b00000000, 0b00000000, 0b00000000},  // w
    {0b01000100, 0b00101000, 0b00010000, 0b00101000, 0b01000100,0b00000000, 0b00000000, 0b00000000},  // x
    {0b00001100, 0b01010000, 0b01010000, 0b01010000, 0b00111100,0b00000000, 0b00000000, 0b00000000},  // y
    {0b01000100, 0b01100100, 0b01010100, 0b01001100, 0b01000100,0b00000000, 0b00000000, 0b00000000},  // z
    {0b00000000, 0b00001000, 0b00110110, 0b01000001, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // {
    {0b00000000, 0b00000000, 0b01111111, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // |
    {0b00000000, 0b01000001, 0b00110110, 0b00001000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // }
    {0b00001000, 0b00001000, 0b00101010, 0b00011100, 0b00001000,0b00000000, 0b00000000, 0b00000000},  // ~
    {0b00001000, 0b00011100, 0b00101010, 0b00001000, 0b00001000,0b00000000, 0b00000000, 0b00000000}   // <-
};

/*** Call this function to boost the STM32F0xx clock to 48 MHz ***/

void SystemClock48MHz( void )
{
//
// Disable the PLL
//
    RCC->CR &= ~(RCC_CR_PLLON);
//
// Wait for the PLL to unlock
//
    while (( RCC->CR & RCC_CR_PLLRDY ) != 0 );
//
// Configure the PLL for 48-MHz system clock
//
    RCC->CFGR = 0x00280000;
//
// Enable the PLL
//
    RCC->CR |= RCC_CR_PLLON;
//
// Wait for the PLL to lock
//
    while (( RCC->CR & RCC_CR_PLLRDY ) != RCC_CR_PLLRDY );
//
// Switch the processor to the PLL clock source
//
    RCC->CFGR = ( RCC->CFGR & (~RCC_CFGR_SW_Msk)) | RCC_CFGR_SW_PLL;
//
// Update the system with the new clock frequency
//
    SystemCoreClockUpdate();

}

/*****************************************************************/

//
volatile int cycles;
volatile int cycles_1;
volatile int firstEdge;
volatile int firstEdge_1;

int main(int argc, char* argv[])
{

	firstEdge = 0;
	firstEdge_1 = 0;
	cycles = 0;
	cycles_1 = 0;

	SystemClock48MHz();

	trace_printf("This is Part 2 of Introductory Lab...\n");
	trace_printf("System clock: %u Hz\n", SystemCoreClock);

	myGPIOA_Init();		/* Initialize I/O port PA */
	oled_config();
	myTIM3_Init();
	myTIM2_Init();		/* Initialize timer TIM2 */
	myEXTI_Init();		/* Initialize EXTI */

    trace_printf("init compeltete \n");

	int active = 1;

	while (1)
	{
		while ((GPIOA->IDR & 0x1) == 0);
		trace_printf("Latch Open\n");
		if(active == 1) {
			NVIC_EnableIRQ(EXTI2_3_IRQn);
			NVIC_DisableIRQ(EXTI0_1_IRQn);
			trace_printf("Active = 2 \n");
			active = 2;
		}
		else if (active == 2) {
			NVIC_DisableIRQ(EXTI2_3_IRQn);
			NVIC_EnableIRQ(EXTI0_1_IRQn);
			trace_printf("Active = 1 \n");
			active = 1;
		}
		while ((GPIOA->IDR & 0x1) != 0);
		trace_printf("Latch Close\n");
		//trace_printf("asfsda: %d\n", GPIOA->IDR & 0x1); // read user in
	}

	return 0;

}

void myGPIOA_Init()
{
	/* Enable clock for GPIOA peripheral */
	RCC->AHBENR |= RCC_AHBENR_GPIOAEN;

	/* Configure PA2 as input */
	GPIOA->MODER &= ~(GPIO_MODER_MODER2);
	GPIOA->MODER &= ~(GPIO_MODER_MODER0 | GPIO_MODER_MODER1);
	/* Ensure no pull-up/pull-down for PA0 */
	GPIOA->PUPDR &= ~(GPIO_PUPDR_PUPDR2);
	GPIOA->PUPDR |= (GPIO_PUPDR_PUPDR0_1 | GPIO_PUPDR_PUPDR1_1); // Set PA0 and PA1 to pull-down

}

void myTIM2_Init()
{
	/* Enable clock for TIM2 peripheral */
	// Relevant register: RCC->APB1ENR
	RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;


	/* Configure TIM2: buffer auto-reload, count up, stop on overflow,
	 * enable update events, interrupt on overflow only */
	TIM2->CR1 = ((uint16_t)0x008C);
	// Relevant register: TIM2->CR1

	/* Set clock prescaler value */
	TIM2->PSC = myTIM2_PRESCALER;
	/* Set auto-reloaded delay */
	TIM2->ARR = myTIM2_PERIOD;

	/* Update timer registers */
	// Relevant register: TIM2->EGR
	TIM2->EGR = ((uint16_t)0x0001);


	/* Assign TIM2 interrupt priority = 0 in NVIC */
	// Relevant register: NVIC->IP[3], or use NVIC_SetPriority
	NVIC_SetPriority(TIM2_IRQn, 0);


	/* Enable TIM2 interrupts in NVIC */
	// Relevant register: NVIC->ISER[0], or use NVIC_EnableIRQ
	NVIC_EnableIRQ(TIM2_IRQn);

	/* Enable update interrupt generation */
	// Relevant register: TIM2->DIER
	TIM2->DIER |= TIM_DIER_UIE;
	//TIM2->CR1 |= TIM_CR1_CEN;
}
void myTIM3_Init()
{
	/* Enable clock for TIM3 peripheral */
	// Relevant register: RCC->APB1ENR
	RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;


	/* Configure TIM3: buffer auto-reload, count up, stop on overflow,
	 * enable update events, interrupt on overflow only */
	TIM3->CR1 = ((uint16_t)0x008C);
	// Relevant register: TIM3->CR1

	/* Set clock prescaler value */
	TIM3->PSC = 480;
	/* Set auto-reloaded delay */
	TIM3->ARR = 1000000;

	/* Update timer registers */
	// Relevant register: TIM3->EGR
	TIM3->EGR = ((uint16_t)0x0001);


	/* Assign TIM3 interrupt priority = 0 in NVIC */
	// Relevant register: NVIC->IP[3], or use NVIC_SetPriority
	NVIC_SetPriority(TIM3_IRQn, 1);


	/* Enable TIM3 interrupts in NVIC */
	// Relevant register: NVIC->ISER[0], or use NVIC_EnableIRQ
	NVIC_EnableIRQ(TIM3_IRQn);

	/* Enable update interrupt generation */
	// Relevant register: TIM3->DIER
	TIM3->DIER |= TIM_DIER_UIE;
	TIM3->CR1 |= TIM_CR1_CEN;
}

void TIM3_IRQHandler()
{
	if ((TIM3->SR & TIM_SR_UIF) != 0)
	{

		/* Clear update interrupt flag */
		// Relevant register: TIM3->SR

		/* Restart stopped timer */
		refresh_OLED();
		// Relevant register: TIM3->CR1
		TIM3->SR &= ~TIM_SR_UIF;
		TIM3->CNT =0x0;
		TIM3->CR1 |= TIM_CR1_CEN;
	}
	// Check if the update interrupt flag is set (UIF)
//	if (TIM3->SR & TIM_SR_UIF)
//	{
//		TIM3->SR &= ~TIM_SR_UIF;
//		Freq ++;
//		trace_printf("asfsda: \n"); // read user in
//		//refresh_OLED();
//	}
}

/* This handler is declared in system/src/cmsis/vectors_stm32f051x8.c */
void TIM2_IRQHandler()
{
	/* Check if update interrupt flag is indeed set */
	if ((TIM2->SR & TIM_SR_UIF) != 0)
	{
		trace_printf("\n*** Overflow! ***\n");

		/* Clear update interrupt flag */
		// Relevant register: TIM2->SR

		/* Restart stopped timer */
		// Relevant register: TIM2->CR1
		TIM2->CNT &= 0x0;
		TIM2->SR &= ~TIM_SR_UIF;
	}
}
void myEXTI_Init()
{



	//Map EXTI2 line to PA2
	// Relevant register: SYSCFG->EXTICR[0]
	SYSCFG->EXTICR[0] &= ~SYSCFG_EXTICR1_EXTI2; // Clear EXTI2 configuration
	SYSCFG->EXTICR[0] |= SYSCFG_EXTICR1_EXTI2_PA; // Select PA2 as the source for EXTI2

	/* Map EXTI1 line to PA1 */
	SYSCFG->EXTICR[0] &= ~SYSCFG_EXTICR1_EXTI1; // Clear EXTI1 configuration
	SYSCFG->EXTICR[0] |= SYSCFG_EXTICR1_EXTI1_PA; // Select PA1 as the source for EXTI1

	//EXTI2 line interrupts: set rising-edge trigger
	// Relevant register: EXTI->RTSR
	EXTI->RTSR |= EXTI_RTSR_TR2;
	EXTI->RTSR |= EXTI_RTSR_TR1; // EXTI1

	//Unmask interrupts from EXTI2 line
	// Relevant register: EXTI->IMR
    EXTI->IMR |= EXTI_IMR_IM2;
    EXTI->IMR |= EXTI_IMR_IM1; // EXTI1

	//Assign EXTI2 interrupt priority = 0 in NVIC
	// Relevant register: NVIC->IP[2], or use NVIC_SetPriority
	NVIC_SetPriority(EXTI2_3_IRQn, 0);
	NVIC_SetPriority(EXTI0_1_IRQn, 0);

	//Enable EXTI2 interrupts in NVIC
	// Relevant register: NVIC->ISER[0], or use NVIC_EnableIRQ
	NVIC_DisableIRQ(EXTI2_3_IRQn);
	NVIC_EnableIRQ(EXTI0_1_IRQn);

	//NVIC_DisableIQR(EXTI2_3_IRQn);

}
/* This handler is declared in system/src/cmsis/vectors_stm32f051x8.c */
void EXTI0_1_IRQHandler() {
	if ((EXTI->PR & EXTI_PR_PR1) != 0) {

		if(firstEdge_1 == 0) {
			TIM2->CNT = 0x0;
			TIM2->CR1 |= TIM_CR1_CEN;
			cycles_1 = 0;
			firstEdge_1 ++;
		} else if(firstEdge_1 == 1) {
			cycles_1 = TIM2->CNT - 5;
			float p = ((float)cycles_1)/48.0 * 1000.0;
			int ns = p;
			float f = 48000000.0/(cycles_1);
			Freq = f;
			firstEdge_1 = 0;
		}
		EXTI->PR |= EXTI_PR_PR1; // Clear the EXTI2 pending bit

	}
	if ((EXTI->PR & EXTI_PR_PR0) != 0) {
		trace_printf("PIN 0\n");
		EXTI->PR |= EXTI_PR_PR0; // Clear the EXTI2 pending bit
	}


}
void EXTI2_3_IRQHandler()
{
	// Declare/initialize your local variables here...


	/* Check if EXTI2 interrupt pending flag is indeed set */
	if ((EXTI->PR & EXTI_PR_PR2) != 0)
	{

		if(firstEdge == 0) {
			TIM2->CNT = 0;
			TIM2->CR1 |= TIM_CR1_CEN;
			cycles = 0;
			firstEdge ++;
		} else if(firstEdge == 1) {
			cycles = TIM2->CNT - 5;
			float p = ((float)cycles)/48.0 * 1000.0;
			int ns = p;
			float f = 48000000.0/(cycles);
			Freq = f;
			firstEdge = 0;
		} else if (firstEdge == 2) {

		}
		//trace_printf("ms: %d \n", cycles/80000);
		//
		// 1. If this is the first edge:
		//	- Clear count register (TIM2->CNT).
		//	- Start timer (TIM2->CR1).
		//    Else (this is the second edge):
		//	- Stop timer (TIM2->CR1).
		//	- Read out count register (TIM2->CNT).
		//	- Calculate signal period and frequency.
		//	- Print calculated values to the console.
		//	  NOTE: Function trace_printf does not work
		//	  with floating-point numbers: you must use
		//	  "unsigned int" type to print your signal
		//	  period and frequency.
		//
		// 2. Clear EXTI2 interrupt pending flag (EXTI->PR).
		// NOTE: A pending register (PR) bit is cleared
		// by writing 1 to it.
		//

		EXTI->PR |= EXTI_PR_PR2; // Clear the EXTI2 pending bit
	}
}

//
// LED Display Functions
//


void refresh_OLED( void )
{
//    // Buffer size = at most 16 characters per PAGE + terminating '\0'
    unsigned char Buffer[17];

	for (int i = 0; i < 16; i++) {
		Buffer[i] = 0;
	}
	oled_Write_Cmd(0xB0 | 3);  // Command to select the page (0xB0 to 0xB7)
	// Set the column (SEG) to 0 (start from the left-most column)
	oled_Write_Cmd(0x00);  // Set lower nibble of the column address (0x00)
	oled_Write_Cmd(0x10);  // Set upper nibble of the column address (0x10)

    snprintf( Buffer, sizeof( Buffer ), "  R: %5u Ohms", Res );

    for(int id = 0; id < 16; id ++) {

		for(int i = 0; i < 8; i++) {
			oled_Write_Data(Characters[Buffer[id]][i]);
		}
    }
//    /* Buffer now contains your character ASCII codes for LED Display
//       - select PAGE (LED Display line) and set starting SEG (column)
//       - for each c = ASCII code = Buffer[0], Buffer[1], ...,
//           send 8 bytes in Characters[c][0-7] to LED Display
//    */
//
//    ...
//
//
	oled_Write_Cmd(0xB0 | 5);  // Command to select the page (0xB0 to 0xB7)
	// Set the column (SEG) to 0 (start from the left-most column)
	oled_Write_Cmd(0x00);  // Set lower nibble of the column address (0x00)
	oled_Write_Cmd(0x10);  // Set upper nibble of the column address (0x10)

	for (int i = 0; i < 16; i++) {
		Buffer[i] = 0;
	}
    snprintf( Buffer, sizeof( Buffer ), "  F: %5u Hz", Freq );

    for(int id = 0; id < 16; id ++) {

		for(int i = 0; i < 8; i++) {
			oled_Write_Data(Characters[Buffer[id]][i]);
		}
    }
//    /* Buffer now contains your character ASCII codes for LED Display
//       - select PAGE (LED Display line) and set starting SEG (column)
//       - for each c = ASCII code = Buffer[0], Buffer[1], ...,
//           send 8 bytes in Characters[c][0-7] to LED Display
//    */
//
//    ...
//
//
//	/* Wait for ~100 ms (for example) to get ~10 frames/sec refresh rate
//       - You should use TIM3 to implement this delay (e.g., via polling)
//    */
//
//    ...

}


void oled_Write_Cmd( unsigned char cmd )
{
    // Step 1: Make PB6 = CS# = 1 (deselect the OLED)
    GPIOB->ODR |= GPIO_ODR_6;  // Set PB6 high

    // Step 2: Make PB7 = D/C# = 0 (set to command mode)
    GPIOB->ODR &= ~GPIO_ODR_7; // Set PB7 low

    // Step 3: Make PB6 = CS# = 0 (select the OLED)
    GPIOB->ODR &= ~GPIO_ODR_6; // Set PB6 low

    // Step 4: Send the command via SPI
    oled_Write(cmd);  // This function will handle SPI transmission

    // Step 5: Make PB6 = CS# = 1 (deselect the OLED)
    GPIOB->ODR |= GPIO_ODR_6;  // Set PB6 high
}

void oled_Write_Data(unsigned char data)
{
    // Step 1: Make PB6 = CS# = 1 (deselect the OLED)
    GPIOB->ODR |= GPIO_ODR_6;  // Set CS high (deselect OLED)

    // Step 2: Make PB7 = D/C# = 1 (set to data mode, as we're sending data)
    GPIOB->ODR |= GPIO_ODR_7;  // Set D/C high (indicating data mode)

    // Step 3: Make PB6 = CS# = 0 (select the OLED)
    GPIOB->ODR &= ~GPIO_ODR_6; // Set CS low (select OLED)

    // Step 4: Send the data via SPI
    oled_Write(data);  // Transmit the data via SPI

    // Step 5: Make PB6 = CS# = 1 (deselect the OLED)
    GPIOB->ODR |= GPIO_ODR_6;  // Set CS high (deselect OLED)
}


void oled_Write( unsigned char Value )
{

	  while (!(SPI1->SR & SPI_SR_TXE));
//    /* Wait until SPI1 is ready for writing (TXE = 1 in SPI1_SR) */
//
//    ...
//
//    /* Send one 8-bit character:
//       - This function also sets BIDIOE = 1 in SPI1_CR1
//    */
	  HAL_SPI_Transmit( &SPI_Handle, &Value, 1, HAL_MAX_DELAY );
//
//
//    /* Wait until transmission is complete (TXE = 1 in SPI1_SR) */
//
//    ...

	  while (!(SPI1->SR & SPI_SR_TXE));
}


void oled_config( void )
{
	RCC->AHBENR |= RCC_AHBENR_GPIOBEN;
	RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;

	GPIOB->MODER = (0x2 << (2 * 3)) | (0x2 << (2 * 5)); // set PB 4,5 to 10
	GPIOB->AFR[0] &= ~(0xF << (3 * 4));  // Clear AF setting for PB3 (AF0 is the default)
	GPIOB->AFR[0] |= (0x0 << (3 * 4));   // Set AF0 for PB3

	GPIOB->AFR[0] &= ~(0xF << (5 * 4));  // Clear AF setting for PB3 (AF0 is the default)
	GPIOB->AFR[0] |= (0x0 << (5 * 4));   // Set AF0 for PB3

	GPIOB->MODER |= (GPIO_MODER_MODER6_0 | GPIO_MODER_MODER7_0 | GPIO_MODER_MODER4_0);
	/* Ensure push-pull mode selected for PC8 and PC9 */
	GPIOB->OTYPER &= ~(GPIO_OTYPER_OT_6 | GPIO_OTYPER_OT_7 | GPIO_MODER_MODER4_0);
	/* Ensure high-speed mode for PC8 and PC9 */
	GPIOB->OSPEEDR |= (GPIO_OSPEEDER_OSPEEDR6 | GPIO_OSPEEDER_OSPEEDR7 | GPIO_MODER_MODER4_0);
	/* Ensure no pull-up/pull-down for PC8 and PC9 */
	GPIOB->PUPDR &= ~(GPIO_PUPDR_PUPDR6 | GPIO_PUPDR_PUPDR7 | GPIO_MODER_MODER4_0);

// Don't forget to enable GPIOB clock in RCC
// Don't forget to configure PB3/PB5 as AF0
// Don't forget to enable SPI1 clock in RCC

    SPI_Handle.Instance = SPI1;

    SPI_Handle.Init.Direction = SPI_DIRECTION_1LINE;
    SPI_Handle.Init.Mode = SPI_MODE_MASTER;
    SPI_Handle.Init.DataSize = SPI_DATASIZE_8BIT;
    SPI_Handle.Init.CLKPolarity = SPI_POLARITY_LOW;
    SPI_Handle.Init.CLKPhase = SPI_PHASE_1EDGE;
    SPI_Handle.Init.NSS = SPI_NSS_SOFT;
    SPI_Handle.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_256;
    SPI_Handle.Init.FirstBit = SPI_FIRSTBIT_MSB;
    SPI_Handle.Init.CRCPolynomial = 7;

    //
    // Initialize the SPI interface
    //
    HAL_SPI_Init( &SPI_Handle );

    //
    // Enable the SPI
    //
    __HAL_SPI_ENABLE( &SPI_Handle );


    // Reset the LED Display (RES# = PB4):
	// 1. Set PB4 (RES#) low for a brief period (reset pulse)
	GPIOB->ODR &= ~GPIO_IDR_4;  // Set PB4 low (reset pulse)
	for (int i = 0; i < 1000000; i ++) {
		__NOP();
	}

	// 2. Set PB4 (RES#) high to release the reset
	GPIOB->ODR |= GPIO_IDR_4;   // Set PB4 high (release reset)
	for (int i = 0; i < 1000000; i ++) {
			__NOP();
	}


    //
    // Send initialization commands to LED Display
    //
    for ( unsigned int i = 0; i < sizeof( oled_init_cmds ); i++ )
    {
        oled_Write_Cmd( oled_init_cmds[i] );
    }


    /* Fill LED Display data memory (GDDRAM) with zeros:
       - for each PAGE = 0, 1, ..., 7
           set starting SEG = 0
           call oled_Write_Data( 0x00 ) 128 times
    */
    for (unsigned char page = 0; page < 8; page++) {
		// Select the page (assuming the OLED supports page addressing)
		oled_Write_Cmd(0xB0 | page);  // Command to select the page (0xB0 to 0xB7)

		// Set the column (SEG) to 0 (start from the left-most column)
		oled_Write_Cmd(0x00);  // Set lower nibble of the column address (0x00)
		oled_Write_Cmd(0x10);  // Set upper nibble of the column address (0x10)

		// Fill the page with 0x00 (128 columns)
		for (unsigned char col = 0; col < 130; col++) {
			oled_Write_Data(0x00);  // Send 0x00 to clear the segment (turn off all pixels)
		}
	}


}

#pragma GCC diagnostic pop

// ----------------------------------------------------------------------------
