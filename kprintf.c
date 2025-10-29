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

#include "kprintf.h"
#include "vga.h"

#include <stdarg.h>
#include <stdint.h>
#include <stddef.h>

void kputc(char c) {
  if (c == '\n') { g_col = 0; g_row++; vga_scroll_if_needed(); return; }
  if (c == '\r') { g_col = 0; return; }
  if (c == '\t') { int next = (g_col + 4) & ~3; while (g_col < next) kputc(' '); return; }
  if (c == '\b') {
    if (g_col > 0) { g_col--; VGA[g_row * VGA_COLS + g_col] = vga_entry(' ', g_color); }
    return;
  }
  VGA[g_row * VGA_COLS + g_col] = vga_entry(c, g_color);
  if (++g_col >= VGA_COLS) { g_col = 0; g_row++; vga_scroll_if_needed(); }
}

void kputs(const char* s) { while (*s) kputc(*s++); }

/* --- Integer formatting --- */
static void utoa_base(uint64_t val, char* buf, int base, int upper) {
  static const char* dl = "0123456789abcdef";
  static const char* du = "0123456789ABCDEF";
  const char* d = upper ? du : dl;
  char tmp[32]; int i = 0;
  if (val == 0) tmp[i++] = '0';
  while (val && i < (int)sizeof(tmp)) { tmp[i++] = d[val % (unsigned)base]; val /= (unsigned)base; }
  int j = 0; while (i--) buf[j++] = tmp[i]; buf[j] = '\0';
}

static void itoa_dec(int64_t v, char* buf) {
  if (v < 0) { *buf++ = '-'; uint64_t u = (uint64_t)(-(v + 1)) + 1; utoa_base(u, buf, 10, 0); }
  else { utoa_base((uint64_t)v, buf, 10, 0); }
}

/* public */
void kprintf(const char* fmt, ...) {
  va_list ap; va_start(ap, fmt);
  for (const char* p = fmt; *p; ++p) {
    if (*p != '%') { kputc(*p); continue; }
    ++p;
    if (*p == '%') { kputc('%'); continue; }
    char buf[64];
    switch (*p) {
      case 'c': kputc((char)va_arg(ap, int)); break;
      case 's': { const char* s = va_arg(ap, const char*); kputs(s ? s : "(null)"); break; }
      case 'd': { int64_t v = (int64_t)va_arg(ap, int); itoa_dec(v, buf); kputs(buf); break; }
      case 'u': { uint64_t v = (uint64_t)va_arg(ap, unsigned); utoa_base(v, buf, 10, 0); kputs(buf); break; }
      case 'x': { uint64_t v = (uint64_t)va_arg(ap, unsigned); utoa_base(v, buf, 16, 0); kputs(buf); break; }
      case 'X': { uint64_t v = (uint64_t)va_arg(ap, unsigned); utoa_base(v, buf, 16, 1); kputs(buf); break; }
      case 'p': { uint64_t v = (uint64_t)va_arg(ap, void*); kputs("0x"); utoa_base(v, buf, 16, 0); kputs(buf); break; }
      default: kputc('%'); kputc(*p); break;
    }
  }
  va_end(ap);
}

