// USB connect and disconnect functions
// These functions interface directly with the libusb-win32 driver

#include <stdio.h>
#include "usb.h"
#include "usb_io.h"
#include "connect.h"


// Connects to the device via USB using the LIBUSB-WIN32 drivers
// Returns
// - dh : if successfully connected returns the USB device handle used by all other functions in this program.
//      : if failed, just returns NULL
usb_dev_handle* connect_device()
{
	usb_dev_handle* dh;
	int success;
	struct usb_bus * bus;
	struct usb_device *dev, *device;
	int i;

	// Initialise USB API
	usb_init();

	// Loop for a maximum number of attempts (currently 1).
	// Note: this was set at 3 but attempting to force the Humax to do something it doesn't want to
	// do can crash the poor thing! So currently set to 1 just to make it's life a bit easier.
	dh = NULL;
	device = NULL;
	success = 0;
	for (i = 0; !success && i < 1; ++i)
	{
		// If second or third pass then reset the device and try again (currently disabled - see above)
		if (i > 0 && dh != NULL)
		{
			if (usb_reset(dh) != 0)
			{
				fprintf(stderr, "connect: Device reset failed\n");
				return NULL;
			}
			//Sleep(1000);
		}

		// Enumerate busses and devices
		usb_find_busses();
		usb_find_devices();

		// Find the device
		for (bus = usb_get_busses(); !device && bus; bus = bus->next)
		{
			for (dev = bus->devices; !device && dev; dev = dev->next)
			{
				if (dev->descriptor.idVendor == 0x11db && dev->descriptor.idProduct == 0x1000)
					device = dev;
			}
		}
		if (!device)
		{
			fprintf(stderr, "connect: No Topfield devices found\n");
			continue;
		}

		// Open the device
		dh = usb_open(device);
		if (dh == NULL)
		{
			fprintf(stderr, "connect: Open Topfield device failed\n");
			continue;
		}

		// Select configuration 0x01
		if (usb_set_configuration(dh, 0x01))
		{
			fprintf(stderr, "connect: Select configuration failed\n");
			continue;
		}

		// Claim interface 0x00
		if (usb_claim_interface(dh, 0x00))
		{
			fprintf(stderr, "connect: Claim interface failed\n");
			continue;
		}

		// Clear any halt conditions on the bulk transfer endpoint 0x01 (Topfield -> Host)
		/*if (usb_clear_halt(dh, 0x01))
		{
			fprintf(stderr, "connect: Clear halt condition on endpoint 0x01 failed\n");
			usb_release_interface(dh, 0x00);
			continue;
		}*/

		// Force-set connect success flag to ensure connect loop only occurs once
		success = 1;
	}

	// If success and device is open then close it
	if (!success && dh)
	{
		usb_close(dh);
		dh = NULL;
	}

	// Return the device handle as success
	return dh;
}

// Disconnects from the device via USB
// Inputs
// - dh    : USB device handle
// Returns
// - 1  : operation succeeded
// - 0 : operation failed
int disconnect_device(usb_dev_handle * dh)
{
	int success = 1;

	// Release interface 0x00
	if (usb_release_interface(dh, 0x00))
	{
		success = 0;
		fprintf(stderr, "disconnect: Release interface failed 0x00\n");
	}

	// Close the device
	if (usb_close(dh))
	{
		success = 0;
		fprintf(stderr, "disconnect: Close device failed");
	}

	// Return disconnect status
	return success;
}
