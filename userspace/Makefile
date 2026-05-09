TINYGO  := tinygo
TARGET  := $(shell pwd)/targets/eidos.json
BUILD   := ../build/userspace

.PHONY: all clean

all: $(BUILD)/hello.h

$(BUILD)/hello.elf: $(wildcard *.go)
	@mkdir -p $(BUILD)
	$(TINYGO) build -target=$(TARGET) -o $@ .

$(BUILD)/hello.bin: $(BUILD)/hello.elf
	riscv64-unknown-elf-objcopy -O binary $< $@

$(BUILD)/hello.h: $(BUILD)/hello.bin
	@printf '#pragma once\n#include <stdint.h>\nstatic const uint8_t hello_bin[] = {\n' > $@
	@xxd -i < $< >> $@
	@printf '};\nstatic const uint32_t hello_bin_size = sizeof(hello_bin);\n' >> $@

clean:
	rm -rf $(BUILD)