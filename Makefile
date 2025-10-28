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

TARGET = kernel.elf
ISO = kernel.iso
BUILD = build

CC = gcc
LD = ld
AS = gcc
CFLAGS = -O2 -Wall -Wextra -ffreestanding -fno-stack-protector -fno-pic -mno-red-zone -m64
LDFLAGS = -nostdlib -z max-page-size=0x1000
ASFLAGS = -m64 -ffreestanding -fno-pic

OBJS = $(BUILD)/boot.o $(BUILD)/kernel.o

all: $(ISO)

$(BUILD):
	mkdir -p $(BUILD)

$(BUILD)/boot.o: boot.S | $(BUILD)
	$(AS) $(ASFLAGS) -c $< -o $@

$(BUILD)/kernel.o: kernel.c | $(BUILD)
	$(CC) $(CFLAGS) -std=c11 -c $< -o $@

$(TARGET): $(OBJS) linker.ld
	$(LD) $(LDFLAGS) -T linker.ld -o $@ $(OBJS)

$(ISO): $(TARGET) grub/grub.cfg
	mkdir -p iso/boot/grub
	cp $(TARGET) iso/boot/kernel.elf
	cp grub/grub.cfg iso/boot/grub/grub.cfg
	grub2-mkrescue -o $(ISO) iso

run: $(ISO)
	qemu-system-x86_64 -cdrom $(ISO) -m 256 -serial stdio

clean:
	rm -rf $(BUILD) $(TARGET) $(ISO) iso

.PHONY: all run clean
