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

#include "vga.h"

#include <stdint.h>
#include <stddef.h>

/* Definitions for the externs */
volatile uint16_t* const VGA = (uint16_t*)0xB8000;
const int VGA_COLS = 80;
const int VGA_ROWS = 25;

/* Shared cursor/color */
int g_row = 0, g_col = 0;
uint8_t g_color = (VGA_COLOR_BLACK << 4) | VGA_COLOR_LIGHT_GRAY;

void vga_clear(uint8_t color) {
  for (int i = 0; i < VGA_COLS * VGA_ROWS; i++) {
    VGA[i] = vga_entry(' ', color);
  }
  g_row = 0;
  g_col = 0;
  g_color = color;
}

void vga_clearex(int fg, int bg) {
  uint8_t color = ((bg & 0x0F) << 4) | (fg & 0x0F);
  vga_clear(color);
}

void vga_puts(const char* s) {
  while (*s) {
    char ch = *s++;
    if (ch == '\n') {
      g_col = 0; g_row++;
      vga_scroll_if_needed();
      continue;
    }
    VGA[g_row * VGA_COLS + g_col] = vga_entry(ch, g_color);
    if (++g_col >= VGA_COLS) { g_col = 0; g_row++; vga_scroll_if_needed(); }
  }
}

void vga_puts_at(const char* s, int row, int col, uint8_t color) {
  int idx = row * VGA_COLS + col;
  while (*s) VGA[idx++] = vga_entry(*s++, color);
}

void vga_set_cursor(int row, int col) {
  if (row >= 0 && row < VGA_ROWS && col >= 0 && col < VGA_COLS) {
    g_row = row; g_col = col;
  }
}

void vga_get_cursor(int* row, int* col) { if (row) *row = g_row; if (col) *col = g_col; }
void vga_set_color(uint8_t color) { g_color = color; }
void vga_set_colorex(int fg, int bg) { g_color = ((bg & 0x0F) << 4) | (fg & 0x0F); }
uint8_t vga_get_color(void) { return g_color; }
void vga_get_colorex(int* fg, int* bg){ if (fg) *fg=g_color&0x0F; if (bg) *bg=(g_color>>4)&0x0F; }

void vga_scroll_if_needed(void) {
  if (g_row < VGA_ROWS) return;
  for (int r = 1; r < VGA_ROWS; ++r) {
    for (int c = 0; c < VGA_COLS; ++c) {
      VGA[(r - 1) * VGA_COLS + c] = VGA[r * VGA_COLS + c];
    }
  }
  for (int c = 0; c < VGA_COLS; ++c) {
    VGA[(VGA_ROWS - 1) * VGA_COLS + c] = vga_entry(' ', g_color);
  }
  g_row = VGA_ROWS - 1;
}
