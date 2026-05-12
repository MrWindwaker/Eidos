TINYGO  := tinygo
TARGET  := $(shell pwd)/targets/eidos.json
BUILD   := ../build/userspace

.PHONY: all clean

all: $(BUILD)/hello.elf

$(BUILD)/hello.elf: $(wildcard *.go)
	@mkdir -p $(BUILD)
	$(TINYGO) build -target=$(TARGET) -o $@ .

$(BUILD)/hello.bin: $(BUILD)/hello.elf
	riscv64-unknown-elf-objcopy -O binary $< $@

clean:
	rm -rf $(BUILD)