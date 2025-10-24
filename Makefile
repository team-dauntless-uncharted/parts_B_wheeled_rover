BOARD := SPRESENSE:spresense:spresense:Core=Main,Memory=1536
PORT ?= $(shell arduino-cli board list | grep SPRESENSE | awk '{print $$1}')
SKETCH := parts_B_wheeled_rover.ino

# FEATURE_FLAG="-D USE_FLASH"
FEATURE_FLAG ?=

all:compile

compile:
	arduino-cli compile \
		--fqbn $(BOARD) \
		--libraries libraries \
		--build-property build.extra_flags="-Isrc" \
		--build-property compiler.cpp.extra_flags="-std=gnu++14 $(FEATURE_FLAG)" \
		$(SKETCH)

upload:
	arduino-cli upload \
		--fqbn $(BOARD) \
		-p $(PORT) \
		$(SKETCH)

monitor:
	arduino-cli monitor -p $(PORT) -c baudrate=115200