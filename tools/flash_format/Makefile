BOARD := SPRESENSE:spresense:spresense:Core=Main
PORT ?= $(shell arduino-cli board list | grep SPRESENSE | awk '{print $$1}')
SKETCH := flash_format.ino

all:compile

compile:
	arduino-cli compile \
		--fqbn $(BOARD) \
		$(SKETCH)

upload:
	arduino-cli upload \
		--fqbn $(BOARD) \
		-p $(PORT) \
		$(SKETCH)

monitor:
	arduino-cli monitor -p $(PORT) -c baudrate=115200