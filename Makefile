BOARD := SPRESENSE:spresense:spresense:Core=Main
PORT ?= $(shell arduino-cli board list | grep SPRESENSE | awk '{print $$1}')
SKETCH := 100kinsat_neo.ino

all:compile

compile:
	arduino-cli compile \
		--fqbn $(BOARD) \
		--build-property build.extra_flags="-Isrc" \
		$(SKETCH)

upload:
	arduino-cli upload \
		--fqbn $(BOARD) \
		-p $(PORT) \
		$(SKETCH)