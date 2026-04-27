CROSS   := riscv64-unknown-elf
AS      := $(CROSS)-gcc
OBJCOPY := $(CROSS)-objcopy

USER_CFLAGS := -march=rv64ima_zicsr -mabi=lp64 -mcmodel=medany \
               -ffreestanding -nostdlib -static -Wl,-Ttext=0x1000,-e,_start

BUILD := ../build/userspace

.PHONY: all clean

all: $(BUILD)/hello.h

$(BUILD)/hello.elf: hello.S
	@mkdir -p $(BUILD)
	$(AS) $(USER_CFLAGS) -o $@ $<

$(BUILD)/hello.bin: $(BUILD)/hello.elf
	$(OBJCOPY) -O binary $< $@

$(BUILD)/hello.h: $(BUILD)/hello.bin
	@mkdir -p $(BUILD)
	@echo "generating hello.h..."
	@printf '#pragma once\n#include <stdint.h>\nstatic const uint8_t hello_bin[] = {\n' > $@
	@xxd -i < $< >> $@
	@printf '};\nstatic const uint32_t hello_bin_size = sizeof(hello_bin);\n' >> $@