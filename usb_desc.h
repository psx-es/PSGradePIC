#if defined (WBOOTLOADERHID)
	#define BOOTLOADER_EXPORT	wBTL_HID
#elif defined (WBOOTLOADERMCHP)
	#define BOOTLOADER_EXPORT	wBTL_MCHP
#else
	#define BOOTLOADER_EXPORT	nBTL
#endif

#define FILE_EXPORT(btl) FILE=PSGradePIC_##btl.hex
#export (HEX, FILE_EXPORT(BOOTLOADER_EXPORT))

#define JIG_DATA_HEADER_LEN		7

const unsigned int8 USB_DEVICE_DESC[] = {
	//HUB_DEVICE
		0x12, 0x01, 0x00, 0x02, 0x09, 0x00, 0x01, 0x08,
		0xAA, 0xAA, 0xCC, 0xCC, 0x00, 0x01, 0x00, 0x00,
		0x00, 0x01,
	//HUB_HUB_DEVICE
		0x09, 0x29, 0x06, 0xa9, 0x00, 0x32, 0x64, 0x00,
		0xff,
	//PORT5_DEVICE
		0x12, 0x01, 0x00, 0x02, 0x00, 0x00, 0x00, 0x08,
		0x4c, 0x05, 0xeb, 0x02, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x01
};

const unsigned int8 USB_CONFIG_DESC[] = {
	//HUB_CONFIG
		// Config
		0x09, 0x02, 0x19, 0x00, 0x01, 0x01, 0x00, 0xe0,
		0x32,
		// Interface
		0x09, 0x04, 0x00, 0x00, 0x01, 0x09, 0x00, 0x00,
		0x00,
		// Endpoint (interrupt in)
		0x07, 0x05, 0x81, 0x03, 0x01, 0x00, 0x0c,
	//PORT5_CONFIG
		// config
		0x09, 0x02, 0x20, 0x00, 0x01, 0x00, 0x00, 0x80,
		0x01,
		// interface
		0x09, 0x04, 0x00, 0x00, 0x02, 0xff, 0x00, 0x00,
		0x00,
		// endpoint
		0x07, 0x05, 0x02, 0x02, 0x08, 0x00, 0x00,
		// endpoint
		0x07, 0x05, 0x81, 0x02, 0x08, 0x00, 0x00
};

const unsigned int16 usb_dongle_revoke_list[] = {0, 2, 13, 32, 34, 176, 241};
static unsigned int8 usb_dongle_key[20];
static unsigned int8 usb_dongle_master_key[20] = {0x46, 0xDC, 0xEA, 0xD3, 0x17, 0xFE, 0x45, 0xD8, 0x09, 0x23, 0xEB, 0x97, 0xE4, 0x95, 0x64, 0x10, 0xD4, 0xCD, 0xB2, 0xC2};

static unsigned int8 dongle_id[2];

static unsigned int8 jig_challenge[64];
static unsigned int8 jig_response[64];	

#define HUB_DEVICE_SIZE				0x0012
#define HUB_HUB_DEVICE_SIZE			0x0009
#define PORT5_DEVICE_SIZE			0x0012

#define HUB_DEVICE_OFFSET			0x0000
#define HUB_HUB_DEVICE_OFFSET		HUB_DEVICE_SIZE
#define PORT5_DEVICE_OFFSET			HUB_HUB_DEVICE_OFFSET + HUB_HUB_DEVICE_SIZE

#define HUB_CONFIG_SIZE				0x0019
#define PORT5_CONFIG_SIZE			0x0020

#define HUB_CONFIG_OFFSET			0x0000
#define PORT5_CONFIG_OFFSET			HUB_CONFIG_SIZE
