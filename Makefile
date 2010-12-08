CCS_COMPILER			= ccsc
CCS_SOURCE				= main.c
CCS_FLAGS_NBL			= +FH +Y9 -L -A -E -M -P -J -D
CCS_FLAGS_WBLHID		= $(CCS_FLAGS_NBL) +GWBOOTLOADERHID="true"
CCS_FLAGS_WBLMCHP		= $(CCS_FLAGS_NBL) +GWBOOTLOADERMCHP="true"
CCS_FLAGS_LEDS			= +GLEDR1="PIN_B4" +GLEDR2="PIN_B1" +GLEDR3="PIN_C0" +GLEDG1="PIN_B5" +GLEDG2="PIN_C1"
ZIP						= zip -r
BUILD_DIR				= build
CLEAN_FILES				= *.err *.esym *.cod *.sym *.hex *.zip $(BUILD_DIR)

BOOTLOADER_BUILDS	=	nBTL \
						wBTL_HID \
						wBTL_MCHP

VERSION = $(shell git rev-parse HEAD)

all:
		#HEX with HID Bootloader.
		$(CCS_COMPILER) $(CCS_FLAGS_WBLHID) $(CCS_FLAGS_LEDS) +GFW$(fw_pic)="true" +GPAYLOAD="$(pl_pic)" +GPAYLOAD_DIR=$(PAYLOAD_DIR) $(CCS_SOURCE)

		#HEX with MCHP Bootloader.
		$(CCS_COMPILER) $(CCS_FLAGS_WBLMCHP) $(CCS_FLAGS_LEDS) +GFW$(fw_pic)="true" +GPAYLOAD="$(pl_pic)" +GPAYLOAD_DIR=$(PAYLOAD_DIR) $(CCS_SOURCE)

		#HEX without Bootloader.
		$(CCS_COMPILER) $(CCS_FLAGS_NBL) $(CCS_FLAGS_LEDS) +GFW$(fw_pic)="true" +GPAYLOAD="$(pl_pic)" +GPAYLOAD_DIR=$(PAYLOAD_DIR) $(CCS_SOURCE)

		#Create build structure.
		mkdir $(BUILD_DIR);
		$(foreach bl_pic, $(BOOTLOADER_BUILDS), mkdir $(BUILD_DIR)/$(bl_pic); )

		#Fix MCHP Bootloader
		sed -i '1i :020000040000FA..' PSGradePIC*_wBTL_MCHP.hex

		#Move each payload to its directory.
		$(foreach bl_pic, $(BOOTLOADER_BUILDS), mv *_$(bl_pic).hex $(BUILD_DIR)/$(bl_pic); )

		#Zip all HEX.
		cd $(BUILD_DIR) && $(ZIP) "PSGradePIC_$(VERSION)" *
		mv build/PSGradePIC_$(VERSION).zip ./

clean: 
		#Clean files.
		rm -f -r $(CLEAN_FILES)