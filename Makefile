CROSS   := riscv64-unknown-elf
CC      := $(CROSS)-gcc
AS      := $(CROSS)-as
LD      := $(CROSS)-ld
OBJCOPY := $(CROSS)-objcopy

CFLAGS  := -march=rv64imac_zicsr -mabi=lp64 -mcmodel=medany \
           -ffreestanding -fno-builtin -nostdlib \
           -Wall -Wextra -O0 -g

LDFLAGS := -T tools/linker.ld -nostdlib

BUILD   := build

SRCS_C  := kernel/main.c \
           kernel/drivers/uart/uart.c

SRCS_S  := kernel/arch/riscv64/boot.S

OBJS    := $(patsubst %.c,$(BUILD)/%.o,$(SRCS_C)) \
           $(patsubst %.S,$(BUILD)/%.o,$(SRCS_S))

KERNEL  := $(BUILD)/eidos.elf

.PHONY: all clean run

all: $(KERNEL)

$(KERNEL): $(OBJS)
	@mkdir -p $(BUILD)
	$(LD) $(LDFLAGS) -o $@ $^

$(BUILD)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD)/%.o: %.S
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

run: $(KERNEL)
	qemu-system-riscv64 \
		-machine virt \
		-cpu rv64 \
		-nographic \
		-bios none \
		-kernel $(KERNEL)

clean:
	rm -rf $(BUILD)