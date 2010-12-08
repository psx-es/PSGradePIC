#include <18F4550.h>
#include <stdint.h>
#include <stdlib.h>
#fuses HSPLL,NOWDT,NOPROTECT,NODEBUG,NOBROWNOUT,USBDIV,PLL2,CPUDIV1,VREGEN,PUT,NOMCLR,NOLVP
#use delay(clock=48000000)

#use rs232(baud=115200, xmit=pin_c6, rcv=pin_c7)

#define JIG_DATA_HEADER_LEN	7

/////////////////////////
// Bootloader Memory Space
// USB HID Bootloader
#if defined (WBOOTLOADERHID)
	#define CODE_START   0x1000
	#build(reset=CODE_START, interrupt=CODE_START+0x08)
	#org 0, CODE_START-1 {}
#elif defined (WBOOTLOADERMCHP)
	#define CODE_START   0x800
	#build(reset=CODE_START, interrupt=CODE_START+0x08)
	#org 0, CODE_START-1 {}
#endif
/////////////////////////

#define USB_MAX_NUM_INTERFACES 1

#define USB_HID_DEVICE     FALSE
#define USB_EP1_TX_ENABLE  USB_ENABLE_INTERRUPT
#define USB_EP1_TX_SIZE    8
#define USB_EP2_RX_ENABLE  USB_ENABLE_INTERRUPT
#define USB_EP2_RX_SIZE    8

#include "usb.h"
#include "usb_desc.h"

#define __USB_DESCRIPTORS__
unsigned char USB_NUM_INTERFACES[1] = {1};
char const USB_STRING_DESC[] = {5, USB_DESC_STRING_TYPE, 'H', 0, 'O', 0, 'L', 0, 'A', 0};

#include "pic18_usb.h"
#include "pic18_usb.c"
#include "usb.c"

#include "sha1.c"

#define PORT_EMPTY   0x0100 
#define PORT_FULL    0x0103 
#define C_PORT_CONN  0x0001 
#define C_PORT_RESET 0x0010 
#define C_PORT_NONE  0x0000

unsigned char BlinkMode = 0;
unsigned char cnt = 0;
unsigned char DelayCount = 0;

char Connect = 0;
char Reset = 0;
char Disconnect = 0;
char Disconnecting = 0;
char Address = -1;
char DevicePort = 0;
char WaitJig = 0;
char nJigs = 0;
char Force0DTS = 0;
unsigned char HubAddress;
unsigned char TxBuf[8];

unsigned int16 port_status[6] = { PORT_EMPTY, PORT_EMPTY, PORT_EMPTY, PORT_EMPTY, PORT_EMPTY, PORT_EMPTY };
unsigned int16 port_change[6] = { C_PORT_NONE, C_PORT_NONE, C_PORT_NONE, C_PORT_NONE, C_PORT_NONE, C_PORT_NONE };

void Chirp() {
	cnt = 2;
	output_high(LEDR1);
	output_high(LEDR2);
	output_high(LEDR3);
}

void Delay10ms(unsigned char delay) {
	DelayCount = delay;
}

void Hub_SetFeature(unsigned char feature,unsigned char port) {
	Chirp();

	switch(feature) {
		case 4:
			port_change[port - 1] |= C_PORT_RESET;
			Reset = port;
			Delay10ms(2);
			break;
		case 8:
			HubAddress = UADDR;
			if(port == 6) {
				Connect = 5;
			}
			break;
	}
}

void Hub_ClearFeature(unsigned char feature,unsigned char port) {
	Chirp();

	if(feature == 0x10) {
		port_change[port - 1] &= ~C_PORT_CONN;
		if(Disconnecting) {
			switch(Disconnecting) {
				case 5:
					OnDongleOK();
					Delay10ms(20);
					break;
			}

			Disconnecting = 0;
		}
	}
	else if(feature == 0x14) {
		port_change[port - 1] &= ~C_PORT_RESET;
		Address = 0;
	}
}

void Hub_GetStatus(unsigned char port,unsigned char *buf) {
	Chirp();

	buf[0] = port_status[port - 1] & 0xFF;
	buf[1] = port_status[port - 1] >> 8;
	buf[2] = port_change[port - 1] & 0xFF;
	buf[3] = port_change[port - 1] >> 8;
}

int16 GetDevicePointer() {
	Chirp();

	switch(DevicePort) {
		case 0:
			return HUB_DEVICE_OFFSET;
		case 5:
			return PORT5_DEVICE_OFFSET;
	}
}

int16 GetDeviceLength() {
	return 0x12;
}

int16 GetConfigPointer(unsigned char nConfig,char shortConfig) {
	Chirp();

	switch(DevicePort) {
		case 0:
			BlinkMode = 2;

			return HUB_CONFIG_OFFSET;
		case 5:
			WaitJIG = 1;
			nJigs = 0;

			return PORT5_CONFIG_OFFSET;
	}
}

int16 GetConfigLength(unsigned char nConfig,char shortConfig) {
	switch(DevicePort) {
		case 0:
			return HUB_CONFIG_SIZE;
		case 5:
			return PORT5_CONFIG_SIZE;
	}
}

int16 GetStringPointer(unsigned char nString) {
	return 0;
}

int16 GetStringLength(unsigned char nString) {
	return 0x3;
}

int16 GetHubPointer() {
	return HUB_HUB_DEVICE_OFFSET;
}

int16 GetHubLength() {
	return 0x9;
}

void OnDongleOK() {
	BlinkMode = 1;
	output_high(LEDR1);
	output_high(LEDR2);
	output_high(LEDR3);
	output_high(LEDG1);
	output_high(LEDG2);
}

void main() {
	output_high(LEDR1);
	output_high(LEDR2);
	output_high(LEDR3);
	output_low(LEDG1);
	output_low(LEDG2);
   
	usb_init();
   
	setup_timer_0(RTCC_INTERNAL|RTCC_DIV_4);
	set_timer0(0x8ad0);

	enable_interrupts(GLOBAL);
	enable_interrupts(INT_TIMER0);

	while(1) {
		usb_task();
		usb_isr();

		if(DelayCount) continue;

		if(Connect) {
			if(UADDR != HubAddress) {
				usb_set_address(HubAddress);
			}

			DevicePort = Connect;
			port_status[Connect - 1] = PORT_FULL;
			port_change[Connect - 1] = C_PORT_CONN;

			TxBuf[0] = 1 << Connect;
			if(Force0DTS)
				usb_put_packet(1, TxBuf, 1, 0);
			else
				usb_put_packet(1, TxBuf, 1, USB_DTS_TOGGLE);
			Connect = 0;
			Force0DTS = 0;
		}

		if(Reset) {
			TxBuf[0] = 1 << Reset;
			usb_put_packet(1, TxBuf, 1, USB_DTS_TOGGLE);
			Reset = 0;
		}

		if(Disconnect) {
			if(UADDR != HubAddress)
				usb_set_address(HubAddress);
			DevicePort = Disconnect;
			port_status[Disconnect - 1] = PORT_EMPTY;
			port_change[Disconnect - 1] = C_PORT_CONN;
			TxBuf[0] = 1 << Disconnect;
			usb_put_packet(1, TxBuf, 1, USB_DTS_TOGGLE);
			Disconnecting = Disconnect;
			Disconnect = 0;
		}

		if(WaitJig) {
			if(WaitJig == 1) {
				if(usb_kbhit(2)) {
					int i;
					unsigned char c;
					Chirp();
					c = usb_get_packet(2, TxBuf, 8);

					for(i = 0; i < 8; i++) {
						jig_response[8 * nJigs + i] = TxBuf[i];
					}

					nJigs++;
					EP_BDxST_I(1) = 0x40;   //Clear IN endpoint
					if(nJigs == 8) {
						//prepare the response
						uint16_t dongle_id [2] = {rand(), rand()};
						uint8_t dongle_key[SHA1_MAC_LEN] = {0};

						jig_response[0] = 0x00;
						jig_response[1] = 0x00;
						jig_response[2] = 0xFF;
						jig_response[3] = 0x00;
						jig_response[4] = 0x2E;
						jig_response[5] = 0x02;
						jig_response[6] = 0x02;

						for(i = 0; usb_dongle_revoke_list[i] != 0xFFFF; i++) {
							if (dongle_id == usb_dongle_revoke_list[i]) {
								dongle_id[0] = random();
								dongle_id[1] = random();
								i = -1;
								continue;
							}
						}

						hmac_sha1 (usb_dongle_master_key, sizeof(usb_dongle_master_key), (uint8_t *)&dongle_id, sizeof(uint16_t), usb_dongle_key);
						hmac_sha1 (usb_dongle_key, SHA1_MAC_LEN, jig_challenge + JIG_DATA_HEADER_LEN, SHA1_MAC_LEN, jig_response + JIG_DATA_HEADER_LEN + sizeof(dongle_id));

						/*
						hmac_sha1 (usb_dongle_master_key, SHA1_MAC_LEN, (uint8_t *)&dongle_id, sizeof(uint16_t), dongle_key);
						hmac_sha1 (dongle_key, SHA1_MAC_LEN, jig_challenge + JIG_DATA_HEADER_LEN, SHA1_MAC_LEN, jig_response + JIG_DATA_HEADER_LEN + sizeof(dongle_id));
						*/

						nJigs = 0;
						WaitJig = 2;
						Delay10ms(50);
					}
				}
			}
			else {
				int n = 0;
				for(n = 0; n < 8; ++n) {
					TxBuf[n] = jig_challenge[8 * nJigs + n];
				}
				if(usb_put_packet(1, TxBuf, 8, nJigs == 0 ? 0 : USB_DTS_TOGGLE)) {
					Delay10ms(1);
					nJigs++;
					Chirp();
					if(nJigs == 8) {
						nJigs = 0;
						WaitJig = 0;
						Delay10ms(50);
						Disconnect = 5;
					}
				}
			}
		}

		if(Address != -1) {
			delay_ms(1);
			usb_set_address(Address);
			Address = -1;
		}
	}
}

#int_timer0
void timer() {
	set_timer0(0x8ad0);

	if(DelayCount) {
		--DelayCount;
	}

	//Blink
	if(BlinkMode == 0) {
		if(cnt == 20) {
			output_toggle(LEDR1);
			output_toggle(LEDR2);
			output_toggle(LEDR3);
			cnt = 0;
		}
		else {
			cnt++;
		}
	}

	//Chirp
	if(BlinkMode == 2) {
		if(!cnt) {
			output_low(LEDR1);
			output_low(LEDR2);
			output_low(LEDR3);
		}
		else {
			cnt--;
		}
	}
}
