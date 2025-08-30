SKETCHES = AHTX0 SCAN SSD1306
CORES ?= arduino:avr ATTinyCore:avr MicroCore:avr SparkFun:avr rp2040:rp2040 esp32:esp32
LIBS ?= "MicroSerial"

BUILD_CONFIG ?= ./arduino-cli.yaml
BUILD_DIR ?= ./build

DEPLOY_CMD ?= cp -r
DEPLOY_DIR ?= ~/Documents/Arduino/libraries/MicroWire

TEMP_DIR ?= ./tmp

define build-sketches
	$(foreach SKETCH,$(SKETCHES), \
		$(call build-sketch,$(1),$(SKETCH)))
endef

define build-sketch
	arduino-cli compile \
		--library ./src \
		--fqbn $(1) \
		--export-binaries \
		$(if $(filter-out undefined,$(origin DEBUG)),--build-property "build.extra_flags=-DDEBUG") \
		./examples/$(2)/$(2).ino;
endef

.PHONY: all
all: clean install build

.PHONY: clean
clean:
	rm -rf $(BUILD_DIR) $(TEMP_DIR)
	find . -type f -exec chmod -x {} +
	$(foreach TARGET,$(SKETCHES),rm -fr ./examples/$(TARGET)/build/;)
	$(foreach TARGET,$(SKETCHES),rm -f ./examples/$(TARGET)/*.lst;)

.PHONY: build
build: build/rpipico build/esp32c3 build/uno build/nano build/leonardo build/promicro build/attiny85

.PHONY: build/rpipico
build/rpipico:
	$(call build-sketches,rp2040:rp2040:rpipico)

.PHONY: build/esp32c3
build/esp32c3:
	$(call build-sketches,esp32:esp32:esp32c3)

.PHONY: build/uno
build/uno:
	$(call build-sketches,arduino:avr:uno)

.PHONY: build/nano
build/nano:
	$(call build-sketches,arduino:avr:nano)

.PHONY: build/leonardo
build/leonardo:
	$(call build-sketches,arduino:avr:leonardo)

.PHONY: build/promicro
build/promicro:
	$(call build-sketches,SparkFun:avr:promicro)

.PHONY: build/attiny85
build/attiny85:
	$(call build-sketches,ATTinyCore:avr:attinyx5:chip=85,clock=8internal)

.PHONY: build/attiny13
build/attiny13:
	$(call build-sketches,MicroCore:avr:13)

.PHONY: deploy
deploy:
	mkdir -p $(DEPLOY_DIR)
	$(DEPLOY_CMD) -r README.md keywords.txt library.properties ./src $(DEPLOY_DIR)

.PHONY: install
install: install/core install/lib

.PHONY: install/core
install/core:
	@if [ ! -f ~/.arduino15/arduino-cli.yaml ]; then arduino-cli config init; fi
	arduino-cli --config-file $(BUILD_CONFIG) core update-index
	arduino-cli --config-file $(BUILD_CONFIG) core install $(CORES)

.PHONY: install/lib
install/lib:
	@if [ ! -f ~/.arduino15/arduino-cli.yaml ]; then arduino-cli config init; fi
	arduino-cli --config-file $(BUILD_CONFIG) lib update-index
	arduino-cli --config-file $(BUILD_CONFIG) lib install $(LIBS)

.PHONY: uninstall/lib
uninstall/lib:
	@if [ ! -f ~/.arduino15/arduino-cli.yaml ]; then arduino-cli config init; fi
	arduino-cli --config-file $(BUILD_CONFIG) lib update-index
	arduino-cli --config-file $(BUILD_CONFIG) lib uninstall $(LIBS)
