CCS_COMPILER			= ccsc
CCS_SOURCE				= main.c
CCS_FLAGS_NBL			= +FH +Y9 -T -L -E -M -P -J -D -A
CCS_FLAGS_WBLHID		= $(CCS_FLAGS_NBL) +GWBOOTLOADERHID="true"
CCS_FLAGS_WBLMCHP		= $(CCS_FLAGS_NBL) +GWBOOTLOADERMCHP="true"
CCS_FLAGS_LEDS			= +GLEDR1="PIN_B4" +GLEDR2="PIN_B1" +GLEDR3="PIN_C0" +GLEDG1="PIN_B5" +GLEDG2="PIN_C1"
ZIP						= zip -r
BUILD_DIR				= build
CLEAN_FILES				= *.err *.esym *.cod *.sym *.hex *.lst *.zip $(BUILD_DIR)

VERSION = $(shell git rev-parse HEAD)

all:
		#HEX with HID Bootloader.
		$(CCS_COMPILER) $(CCS_FLAGS_WBLHID) $(CCS_FLAGS_LEDS) $(CCS_SOURCE)

		#HEX with MCHP Bootloader.
		$(CCS_COMPILER) $(CCS_FLAGS_WBLMCHP) $(CCS_FLAGS_LEDS) $(CCS_SOURCE)

		#HEX without Bootloader.
		$(CCS_COMPILER) $(CCS_FLAGS_NBL) $(CCS_FLAGS_LEDS) $(CCS_SOURCE)

		#Create build structure.
		rm -f -r $(BUILD_DIR)
		mkdir $(BUILD_DIR);

		#Fix MCHP Bootloader
		sed -i '1i :020000040000FA..' PSGradePIC*_wBTL_MCHP.hex

		#Move each payload to its directory.
		mv PSGradePIC*.hex $(BUILD_DIR);

		#Zip all HEX.
		cd $(BUILD_DIR) && $(ZIP) "PSGradePIC_$(VERSION)" *
		mv build/PSGradePIC_$(VERSION).zip ./
		rm -f -r $(BUILD_DIR)

clean: 
		#Clean files.
		rm -f -r $(CLEAN_FILES)