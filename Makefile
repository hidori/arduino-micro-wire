TARGETS = AHTX0 SCAN SSD1306
BUILD_CONFIG ?= ./arduino-cli.yaml
CORES ?= arduino:avr ATTinyCore:avr MicroCore:avr
LIBS ?= "MicroSerial"

BUILD_DIR ?= ./build
DEPLOY_DIR ?= ~/Documents/Arduino/libraries/MicroWire
TEMP_DIR ?= ./tmp

define build-targets
	$(foreach TARGET,$(TARGETS), \
		$(call build-target,$(1),$(TARGET)))
endef

define build-target
	arduino-cli compile \
		$(if $(filter-out undefined,$(origin DEBUG)),--build-property "build.extra_flags=-DDEBUG") \
		--fqbn $(1) \
		--library ./src \
		--export-binaries \
		./examples/$(2)/$(2).ino;
endef

.PHONY: clean
clean:
	rm -rf $(BUILD_DIR) $(TEMP_DIR)
	find . -type f -exec chmod -x {} +
	$(foreach TARGET,$(TARGETS),rm -fr ./examples/$(TARGET)/build/;)
	$(foreach TARGET,$(TARGETS),rm -f ./examples/$(TARGET)/*.lst;)

.PHONY: build
build: build/uno build/leonardo build/digispark build/attiny85 build/attiny13

.PHONY: build/uno
build/uno:
	$(call build-targets,arduino:avr:uno)

.PHONY: build/leonardo
build/leonardo:
	$(call build-targets,arduino:avr:leonardo)

.PHONY: build/digispark
build/digispark:
	$(call build-targets,ATTinyCore:avr:attinyx5micr)

.PHONY: build/attiny85
build/attiny85:
	$(call build-targets,ATTinyCore:avr:attinyx5:chip=85,clock=8internal)

.PHONY: build/attiny13
build/attiny13:
	$(call build-targets,MicroCore:avr:13)

.PHONY: deploy
deploy: build
	mkdir -p $(DEPLOY_DIR)
	cp -r ./src/* $(DEPLOY_DIR)

.PHONY: install
install: core/install lib/install

.PHONY: core/update-index
core/update-index:
	arduino-cli core update-index

.PHONY: core/install
core/install: core/update-index
	@if [ ! -f ~/.arduino15/arduino-cli.yaml ]; then arduino-cli config init; fi
	arduino-cli --config-file $(BUILD_CONFIG) core update-index
	arduino-cli --config-file $(BUILD_CONFIG) core install $(CORES)

.PHONY: lib/update-index
lib/update-index:
	arduino-cli lib update-index

.PHONY: lib/install
lib/install: lib/update-index
	arduino-cli --config-file $(BUILD_CONFIG) lib install $(LIBS)
