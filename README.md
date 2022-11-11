# Brother-status-tool
A free and open source tool for reading status data from some Brother printers via USB.

## What is this exactly?
This tool allows you to get some status data like remaining percentage of toner from Brother HL-3150CDW (connected via USB) and possibly other models (information "works/does not work with $model" welcome!).

## Why did you make this?
Well, Brother makes fine printers but it seems there is no way of getting the remaining percentage of toner from my printer on Linux. The printer has an LCD and i can get the informations i want from the menu but this is not very convenient... Also i wanted some automatic warning if i run low on toner to be able to buy a new cartridge soon enough. There is some proprietary software for Windows but even this one will only show you 4 bargraphs for the toner levels without any numbers (and maybe warn you if you are running out of toner, i did not check). My tool is free and open source (so you can modify it to suit your specific needs) and will report more data than the Windows software.

## Licence and Disclaimer
This work is provided under AGPLv3+ and WITHOUT ANY WARRANTY! It is HIGHLY EXPERIMENTAL! I have no link with Brother and i hope i won't get in trouble for providing this code.

## How does it work?
My tool will send some "magic" command to the printer that will return a lot of (encoded) informations that are then parsed. The "magic" command is a vendor-specific PJL (printer job language) command. Not all returned data has been decoded, help welcome.

## Where did you get this command and the meaning of the returned bytes?
I ran the Windows-tool inside a VM, sniffed the USB-traffic with Wireshark and correlated the resultats against what is displayed on the LCD of the printer. I didn't disassemble any software or driver. I am totally **not** sure my guesses are correct, that's why this tool is classified HIGHLY EXPERIMENTAL and provided WITHOUT ANY WARRANTY.

## What is inside this repo?
Actually there a two pieces:
* A basic "get status"-tool written in ANSI-C and using only `libusb`. As `libusb` is platform-independant as far as i know this code *might* compile and work on Windows. I did not test compiling for Windows and cannot provide any support for this.
* A Linux-only tool, written in C and using `libusb` and `libnotify`, that will check the toner levels of the printer and show a warning (notification) on screen if toner is running out (thresholds are configurable at compile-time). Currently this tool is run using `udev` every time the printer is plugged in. Tested on Debian 11.
  
For details please check the Readme inside each folder.
