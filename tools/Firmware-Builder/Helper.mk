firmware-info:
	@echo "TARGET_HEX: $(TARGET_HEX)"
	@echo "TARGET_ELF: $(TARGET_ELF)"

all-plus-firmware-info: all firmware-info

