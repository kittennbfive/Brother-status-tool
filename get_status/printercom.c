/*
This file is part of Brother-status-tool

(c) 2022 by kittennbfive

AGPLv3+ and NO WARRANTY! EXPERIMENTAL STUFF!
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <libusb-1.0/libusb.h>

#include "printercom.h"
#include "error.h"

//no notifications here as the code for this is Linux-only
//ugly hack to remove calls...
#define show_notification(a,b) (void)0

unsigned int get_status_data_from_printer(char * const status_data, const unsigned int sz_buffer)
{
#ifndef USE_DUMMY_DATA
	libusb_context *ctx;
	int ret;
	
	ret=libusb_init(&ctx);
	if(ret)
	{
		show_notification(EV_USB_ERROR, "libusb_init failed");
		ERR_MSG("libusb_init failed");
	}

#ifdef DEBUG_MODE_VERBOSE
	ret=libusb_set_option(ctx, LIBUSB_OPTION_LOG_LEVEL, LIBUSB_LOG_LEVEL_WARNING);
	if(ret) //no notification here as this is for debugging only
		ERR_MSG("libusb_set_option failed");
#endif

	libusb_device_handle *dev_handle;
	dev_handle=libusb_open_device_with_vid_pid(ctx, 0x04f9, USB_PID); //This function has limitations/drawbacks like it will only return the first matching device. However generally there is only one printer of the same model attached via USB i assume, so this should be fine. See https://libusb.sourceforge.io/api-1.0/group__libusb__dev.html#ga10d67e6f1e32d17c33d93ae04617392e
	if(dev_handle==NULL)
	{
		show_notification(EV_PRINTER_NOT_FOUND, NULL);
		ERR_MSG("printer not found");
	}
	
	const char magic_string[]="\033%-12345X@PJL SET NOTIFYBRMAINTENANCE=ON\r\n";
	
	int bytes_transfered;
	ret=libusb_bulk_transfer(dev_handle, 0x01, (unsigned char*)magic_string, strlen(magic_string), &bytes_transfered, 100);
	if(ret)
	{
		show_notification(EV_USB_ERROR, "libusb_bulk_transfer TX command failed");
		ERR_MSG("libusb_bulk_transfer TX command failed");
	}

#ifdef DEBUG_MODE_VERBOSE
	printf("%d bytes command transfered\n", bytes_transfered);
#endif

	unsigned int tries=1;
	int bytes_received;
	
	do
	{
#ifdef DEBUG_MODE_VERBOSE
		printf("RX try %u...\n", tries);
#endif
		
		ret=libusb_bulk_transfer(dev_handle, 0x82, (unsigned char*)status_data, sz_buffer-1, &bytes_received, 100);
		if(ret)
		{
			show_notification(EV_USB_ERROR, "libusb_bulk_transfer RX failed");
			ERR_MSG("libusb_bulk_transfer RX failed");
		}

#ifdef DEBUG_MODE_VERBOSE
		printf("%d bytes data received\n", bytes_received);
#endif
		
		if(bytes_received==0)
			sleep(5); //decrease this?

	} while(bytes_received==0 && tries++<TRIES_MAX);
	
	if(bytes_received==0)
	{
		show_notification(EV_USB_ERROR, "did not receive any data");
		ERR_MSG("did not receive any data from printer");
	}
	
	status_data[bytes_received]='\0';
	
	libusb_close(dev_handle);
	libusb_exit(ctx);

#else
#define min(a,b) (a<b?a:b)
	printf("USING DUMMY DATA!\n");
	const char dummydata[]="@PJL INFO BRMAINTENANCE\r\nDATA=\"31,01,04,00,00,00,01\"\r\nDATA=\"32,01,04,00,00,00,01\"\r\nDATA=\"33,01,04,00,00,00,01\"\r\nDATA=\"34,01,04,00,00,00,01\"\r\nDATA=\"35,01,04,00,00,00,01\"\r\nDATA=\"54,01,04,00,00,00,01\"\r\nDATA=\"55,01,04,00,00,00,01\"\r\nDATA=\"68,01,04,00,00,00,01\"\r\nDATA=\"69,01,04,00,00,25,80\"\r\nDATA=\"6A,01,04,00,00,26,AC\"\r\nDATA=\"6D,01,04,00,00,26,AC\"\r\nDATA=\"6F,01,04,00,00,05,DC\"\r\nDATA=\"70,01,04,00,00,18,38\"\r\nDATA=\"71,01,04,00,00,18,38\"\r\nDATA=\"72,01,04,00,00,18,38\"\r\nDATA=\"73,01,04,00,00,04,0D\"\r\nDATA=\"74,01,04,00,00,04,0D\"\r\nDATA=\"75,01,04,00,00,04,0D\"\r\nDATA=\"76,01,04,00,00,00,01\"\r\nDATA=\"77,01,04,00,00,00,01\"\r\nDATA=\"78,01,04,00,00,00,01\"\r\nDATA=\"79,01,04,00,00,24,B8\"\r\nDATA=\"7A,01,04,00,00,24,B8\"\r\nDATA=\"7B,01,04,00,00,24,B8\"\r\nDATA=\"7E,01,04,00,00,04,0D\"\r\nDATA=\"7F,01,04,00,00,00,01\"\r\nDATA=\"80,01,04,00,00,24,B8\"\r\nDATA=\"81,01,04,00,00,00,64\"\r\nDATA=\"82,01,04,00,00,00,46\"\r\nDATA=\"83,01,04,00,00,00,46\"\r\nDATA=\"84,01,04,00,00,00,46\"\r\n\x0c";
	strncpy(status_data, dummydata, sz_buffer);
	int bytes_received=min(strlen(dummydata), sz_buffer);
#endif

	return bytes_received;
}
