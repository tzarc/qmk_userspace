# vim: set ft=asm ts=8 sw=8 noexpandtab
# Copyright 2024 Nick Brassel (@tzarc)
# SPDX-License-Identifier: GPL-2.0-or-later
.section .initial_jump
.global _start

.align 4
_start:
	la t3, _api_table_end 		# end of table
	la t2, _api_table		# start of table
	li t1, 4			# index multiplier
	mul t0, t0, t1			# multiply supplied index (t0) by 4 to get memory offset
	add t0, t0, t2			# add memory offset to start of table to get function to invoke
	bge t0, t3, _shutdown		# exit if we've calculated past the end of the table
	lw t0, 0(t0)			# load the address from the table
	beq t0, x0, _shutdown		# exit if the resulting location is null (weak, undefined in code)
	la sp, _sstack			# set up stack
	addi sp,sp,-16			# make stack space
	sw ra,12(sp)			# save return address (not that it's in use...)
	jalr t0				# invoke the function from the table
_shutdown:
	li t4, 0x5555			# load 0x5555 for value
	la t5, SYSCON			# load SYSCON for address
	sw t4, 0(t5)			# write to SYSCON to exit

_hard_loop:
	j _hard_loop			# Hard loop if writing to SYSCON didn't terminate the VM for whatever reason

.global _api_table

.weak effect_init
.weak effect_led

.global _vcall0
.global _vcall1
.global _vcall2
.global _vcall3
.global _vcall4
.global _vcall5
.global _vcall6
.global _vcall7

# "vcall" -- simple invocation of host by writing 0x9999 to SYSCON
# 	- saves messing about with traps and the like as this is only used by baremetal execution
_vcall0:				# 0 arguments set up by compiler, as per vcall.h
_vcall1:				# 1 argument set up by compiler, as per vcall.h
_vcall2:				# 2 arguments set up by compiler, as per vcall.h
_vcall3:				# 3 arguments set up by compiler, as per vcall.h
_vcall4:				# 4 arguments set up by compiler, as per vcall.h
_vcall5:				# 5 arguments set up by compiler, as per vcall.h
_vcall6:				# 6 arguments set up by compiler, as per vcall.h
_vcall7:				# 7 arguments set up by compiler, as per vcall.h
	li t4, 0x9999			# load 0x9999 for value
	la t5, SYSCON			# load SYSCON for address
	sw t4, 0(t5)			# write to SYSCON to invoke vcall handler
	ret				# back to caller

_api_table:				# API table for functions to invoke -- kicking off the core with t0 set to an index will invoke the corresponding API
	.long effect_init
	.long effect_led

_api_table_end:
	.long 0
