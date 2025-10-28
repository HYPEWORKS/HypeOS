# Copyright (C) 2025 HYPEWORKS Research and Development
# This file is part of HypeOS.
#
# HypeOS is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 2 of the License, or
# (at your option) any later version.
#
# HypeOS is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with HypeOS. If not, see <http://www.gnu.org/licenses/>.

OUTPUT_FORMAT(elf64-x86-64)
OUTPUT_ARCH(i386:x86-64)
ENTRY(_start)

SECTIONS
{
  . = 1M;

  /* Must be early in the file (and loadable) so GRUB can see it */
  .multiboot2 : { KEEP(*(.multiboot2)) }

  .text : ALIGN(4K) { *(.text*) }
  .rodata : ALIGN(4K) { *(.rodata*) }
  .data : ALIGN(4K)  { *(.data*) }
  .bss  : ALIGN(4K)  {
    __bss_start = .;
    *(.bss*)
    *(COMMON)
    __bss_end = .;
  }
}
