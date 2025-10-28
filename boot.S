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

/* boot.S — Multiboot2 (32-bit) -> Long mode trampoline -> call kernel_main (64-bit) */

.set MAGIC,       0xE85250D6          /* Multiboot2 header magic */
.set ARCH,        0                    /* i386 */
.set HEADER_LEN,  multiboot2_end - multiboot2_header
.set CHECKSUM,    -(MAGIC + ARCH + HEADER_LEN)

/* Page flags */
.set P_PRESENT,   1
.set P_WRITE,     2
.set P_PS,        (1 << 7)             /* 2 MiB page */

/* MSRs */
.set IA32_EFER,   0xC0000080
.set EFER_LME,    (1 << 8)

/* GDT entries: null, 64-bit code, data */
    .p2align 3
/* Multiboot2 header must be in a loadable section and 8B-aligned */
    .section .multiboot2, "a", @progbits
    .balign 8
multiboot2_header:
    .long 0xE85250D6      /* MAGIC */
    .long 0                /* ARCH i386 */
    .long multiboot2_end - multiboot2_header
    .long -(0xE85250D6 + 0 + (multiboot2_end - multiboot2_header))

    /* End tag */
    .short 0
    .short 0
    .long 8
multiboot2_end:

    .extern kernel_main
    .extern __bss_start
    .extern __bss_end

/* ---------- 32-bit entry ---------- */
    .section .text
    .code32
    .globl _start
    .type _start, @function
_start:
    /* Load a minimal GDT with 64-bit code/data descriptors */
    lgdt gdt_descriptor

    /* Enable PAE (CR4.PAE = 1) */
    movl %cr4, %eax
    orl  $(1 << 5), %eax
    movl %eax, %cr4

    /* Build identity-mapped paging using 2MiB pages for the first 1 GiB */
    /* CR3 = PML4 base */
    movl $pml4, %eax
    movl %eax, %cr3

    /* Enable Long Mode (set EFER.LME) */
    movl $IA32_EFER, %ecx
    rdmsr
    orl  $EFER_LME, %eax
    wrmsr

    /* Enable paging (CR0.PG = 1) — PE is already on under GRUB */
    movl %cr0, %eax
    orl  $(1 << 31), %eax
    movl %eax, %cr0

    /* Far jump to 64-bit code segment */
    ljmp $0x08, $long_mode_entry

/* ---------- 64-bit code ---------- */
    .code64
long_mode_entry:
    /* Update data segments (harmless but tidy) */
    mov $0x10, %ax
    mov %ax, %ds
    mov %ax, %es
    mov %ax, %ss
    mov %ax, %fs
    mov %ax, %gs

    /* Zero .bss */
    leaq __bss_start(%rip), %rdi
    leaq __bss_end(%rip),   %rcx
    subq %rdi, %rcx         /* RCX = size */
    xorl %eax, %eax
    cld
    rep stosb

    /* Jump to C kernel */
    call kernel_main

.hang:
    cli
    hlt
    jmp .hang

/* ---------- Paging structures (aligned) ---------- */
    .p2align 12
pml4:
    .quad pdpt + (P_PRESENT|P_WRITE)
    .fill 511,8,0

    .p2align 12
pdpt:
    .quad pd0 + (P_PRESENT|P_WRITE)
    .fill 511,8,0

    .p2align 12
pd0:
    /* Map 0..1GiB using 2MiB pages */
    .set i, 0
    .rept 512
        .quad (i * 0x200000) | (P_PRESENT|P_WRITE|P_PS)
        .set i, i+1
    .endr

/* ---------- GDT ---------- */
    .p2align 3
gdt:
    /* 0x00: null */
    .quad 0x0000000000000000
    /* 0x08: 64-bit code: present, executable, long */
    /* Flags: 0xA09B = 1 0 1 0 | 1001 1011 (long=1, default=0) */
    .quad 0x00AF9A000000FFFF  /* canonical long code descriptor */
    /* 0x10: data segment: present, writable */
    .quad 0x00AF92000000FFFF

gdt_descriptor:
    .word (gdt_end - gdt - 1)
    .quad gdt
gdt_end:
