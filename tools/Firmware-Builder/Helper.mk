firmware-info:

	@# For architecture "AVR"
	@echo "TARGET_HEX: $(TARGET_HEX)"
	@echo "TARGET_ELF: $(TARGET_ELF)"
	@echo "TARGET_BOARD_TAG: $(BOARD_TAG)"
	@echo "TARGET_BOARD_SUB: $(BOARD_SUB)"

	@# For architecture "ESP"
	@echo "TARGET_BIN: $(MAIN_EXE)"
	@echo "TARGET_CHIP: $(CHIP)"

all-plus-firmware-info: all firmware-info

