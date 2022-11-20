# check_toner_level
This tool will check the toner levels and pop up a notification if you are running out of toner.

## Example notifications:
![example status message (everything ok)](notification_ok.png)  
![example status message (low on toner)](notification_low.png)  
![example status message (communication error)](notification_error.png)  

## Overview
For an overview of the code/API please check [the other README](https://github.com/kittennbfive/Brother-status-tool/blob/main/get_status/README.md).

## How to use?
### Prerequisites
This code needs `libusb-1.0` and `libnotify`. On Debian 11 the packets you need are called `libusb-1.0-0-dev` and `libnotify-dev`. 
### Configuration
You can configure some stuff using `#define` inside the different headerfiles, more precisely:
#### Inside main.h
- You can configure the warning-thresholds (% of toner remaining, default 25%)
- You can configure if you want a notification even if the levels are fine. This is enabled by default at this point to make sure the code is actually run when the printer is plugged in/switched on. You can comment/remove the `#define` to change this behaviour.
- There is a switch for debug mode / increased verbosity.
- You can configure the size of the internal buffer for the received data from the printer. I set this to 1kB, fine for my printer but you may need to increase this. Informations "too small for $model" welcome.
#### Inside printercom.h
- **IMPORTANT** You have to specify the USB ProductId of your printer! Just use `lsusb`. The VendorId ist hardcoded to the one from Brother (but could be changed if needed, this is FOSS after all).
- For debugging you can add some verbosity to the code.
- Also for debugging/testing you can configure the code to NOT communicate with the printer but use a hardcoded response.
- `TRIES_MAX` is the number of read retries the code will make while waiting for the data from the printer. After each failed read (0 bytes returned by the printer) the code will sleep for 5 seconds (hardcoded for now) and then try again. On my system usually the third try is successful. This means some delay but as this tool runs automatically in the background this is not a problem.
#### Inside parser.h
- Verbose mode for debugging as above.
- 2 `#define` that specify what the parser will return if you make a request for data that is not known or not in the correct format (cf the API description linked above).
### Compiling
Just run `./make` (yes it's a local script that calls GCC as i still don't know makefiles...). You might notice there are some `-I` arguments passed to `gcc` because on my system `gcc` had some trouble with `glib` that is needed for `libnotify`. YMMV - adjust or remove these as needed.
### Installation
You will need superuser-privileges.
#### The udev-stuff
Adjust `99-printer-status.rules`:
* Replace <USB_PRODUCT_ID> with the correct ProductId *without* "0x".
* Replace \<YOURUSERNAME> with your user name. This `su`-stuff is a bit of an ugly hack (or is it?) that is needed because by default the tool would run as root and this will prevent the notifications from showing up. If anybody knows a better way please share.  

Then copy the file to `/etc/udev/rules.d/`. `udev` should automatically "discover" and parse the new file.
#### The other stuff
Just copy `printer_check_toner` and the "wrapper" `printer_check_toner.sh` to `/usr/local/bin`. Make sure both are executable!
