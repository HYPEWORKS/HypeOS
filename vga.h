/*
 * Copyright (C) 2025 HYPEWORKS Research and Development
 * This file is part of HypeOS.
 *
 * HypeOS is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * HypeOS is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with HypeOS. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef HYPEOS_VGA_H
#define HYPEOS_VGA_H

#include <stdint.h>

/* VGA text mode mmio */
extern volatile uint16_t* const VGA;
extern const int VGA_COLS;
extern const int VGA_ROWS;

/* VGA color constants */
enum {
  VGA_COLOR_BLACK = 0, VGA_COLOR_BLUE, VGA_COLOR_GREEN, VGA_COLOR_CYAN,
  VGA_COLOR_RED, VGA_COLOR_MAGENTA, VGA_COLOR_BROWN, VGA_COLOR_LIGHT_GRAY,
  VGA_COLOR_DARK_GRAY, VGA_COLOR_LIGHT_BLUE, VGA_COLOR_LIGHT_GREEN, VGA_COLOR_LIGHT_CYAN,
  VGA_COLOR_LIGHT_RED, VGA_COLOR_LIGHT_MAGENTA, VGA_COLOR_YELLOW, VGA_COLOR_WHITE
};

/* Shared cursor + color (defined in vga.c) */
extern int g_row, g_col;
extern uint8_t g_color;

/* Small helper available to users */
static inline uint16_t vga_entry(char c, uint8_t color) {
  return ((uint16_t)color << 8) | (uint8_t)c;
}

/* API */
void vga_clear(uint8_t color);
void vga_clearex(int fg, int bg);
void vga_puts(const char* s);
void vga_puts_at(const char* s, int row, int col, uint8_t color);
void vga_set_cursor(int row, int col);
void vga_get_cursor(int* row, int* col);
void vga_set_color(uint8_t color);
void vga_set_colorex(int fg, int bg);
uint8_t vga_get_color(void);
void vga_get_colorex(int* fg, int* bg);
void vga_scroll_if_needed(void);

#endif // HYPEOS_VGA_H
