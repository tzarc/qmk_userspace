PROJECT:=rv32_runner
TARGETS:=$(PROJECT).elf $(PROJECT).bin $(PROJECT).debug.txt

all : $(TARGETS)

PREFIX:=riscv32-unknown-elf-
CFLAGS:=-fno-stack-protector
CFLAGS+=-static-libgcc -fdata-sections -ffunction-sections
CFLAGS+=-g -O3 -march=rv32ima_zicsr -mabi=ilp32 -static
LDFLAGS:= -T flatfile.lds -nostdlib -Wl,--gc-sections

$(PROJECT).elf : $(wildcard *.S) $(wildcard *.c)
	$(PREFIX)gcc -o $@ $^ $(CFLAGS) $(LDFLAGS)

$(PROJECT).debug.txt : $(PROJECT).elf
	$(PREFIX)objdump -t $^ > $@
	$(PREFIX)objdump -S $^ >> $@

$(PROJECT).bin : $(PROJECT).elf
	$(PREFIX)objcopy $^ -O binary $@

clean :
	rm -rf $(TARGETS)
