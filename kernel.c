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

#include <stdint.h>
#include <stddef.h>

static volatile uint16_t* const VGA = (uint16_t*)0xB8000;
static const int VGA_COLS = 80;
static const int VGA_ROWS = 25;

static void vga_clear(uint8_t color) {
  for (int i = 0; i < VGA_COLS * VGA_ROWS; i++) {
    VGA[i] = ((uint16_t)color << 8) | ' ';
  }
}

static void vga_puts_at(const char* s, int row, int col, uint8_t color) {
  int idx = row * VGA_COLS + col;
  while (*s) {
    VGA[idx++] = ((uint16_t)color << 8) | (uint8_t)*s++;
  }
}

void kernel_main(void) {
  /* light gray on black: fg=7, bg=0: (bg<<4 | fg) */
  uint8_t color = (0 << 4) | 7;
  vga_clear(color);
  vga_puts_at("HypeOS is going to be lit! \x03", 12, 20, color);

  /* hang */
  for (;;)
    __asm__ __volatile__("hlt");
}
