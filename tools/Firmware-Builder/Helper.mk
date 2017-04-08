firmware-info:

	@# For architecture "AVR"
	@echo "TARGET_HEX: $(TARGET_HEX)"
	@echo "TARGET_ELF: $(TARGET_ELF)"

	@# For architecture "ESP"
	@echo "TARGET_BIN: $(MAIN_EXE)"

all-plus-firmware-info: all firmware-info

