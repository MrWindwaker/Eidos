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

rwildcard=$(foreach d,$(wildcard $(1:=/*)),$(call rwildcard,$d,$2) $(filter $(subst *,%,$2),$d))

SRCS_C := $(call rwildcard,kernel,*.c)
SRCS_S := $(call rwildcard,kernel,*.S)

OBJS    := $(patsubst %.c,$(BUILD)/%.o,$(SRCS_C)) \
           $(patsubst %.S,$(BUILD)/%.S.o,$(SRCS_S))

KERNEL  := $(BUILD)/eidos.elf
RAMDISK := build/ramdisk.img

.PHONY: all clean run userspace ramdisk

all: userspace ramdisk $(KERNEL)

userspace: 
	$(MAKE) -C userspace

$(KERNEL): $(OBJS)
	@mkdir -p $(BUILD)
	$(LD) $(LDFLAGS) -o $@ $^

$(BUILD)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD)/%.S.o: %.S
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

ramdisk: userspace
	@mkdir -p build
	python3 tools/mkeidar.py build/ramdisk.img build/userspace/hello.elf
	python3 tools/mkramdisk_h.py build/ramdisk.img build/ramdisk_bytes.h build/ramdisk_size.h

run: $(KERNEL)
	riscv64-unknown-elf-objcopy -O binary $(KERNEL) build/eidos.bin
	qemu-system-riscv64 \
		-machine virt \
		-cpu rv64 \
		-nographic \
		-bios build/eidos.bin \
		-m 128M

clean:
	rm -rf $(BUILD)
	$(MAKE) -C userspace clean