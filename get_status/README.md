# get_status
This command line tool will print every (identified) information it got from the printer. It is intended for testing and as a base for further development of other tools/stuff. Use the code in this folder to check if your Brother printer is supported or if support can be added.

## Structure of code/API
The code is divided into several files. There are 2 main parts:
* `printercom.{c,h}` for getting the raw data from the printer (connected via USB, not Ethernet!)
* `parser.{c,h}` for crunching the raw data and providing a more convenient interface for the rest of the code
### Getting the raw data
Well, there is only one function... The raw-data is (almost entirely) printable ASCII and can be printf'ed for debugging.
### Parsing the raw data/Format of the data
Luckely the raw data is (almost) human-readable. The first line is some PJL-stuff, the code only checks if it matches to make sure we got the data we want. The bits we are interested in are encoded as `DATA=<hex_value>,<hex_value>,[...]\r\n`. At the end of the data is a linefeed character (ASCII 0x0C) as a kind of an end of record marker (cf PJL standard).  
The first hex_value (all without 0x by the way) is what i call an "index" and tells what kind of data follows. Then there are several unknown bytes (mostly 00) and finally one or more "data" bytes. The encoding is quite easy to figure out. For the percentages just concatenate the last two bytes, convert to decimal and divide by 100 (dez). I don't know if the decimal places are actually used or always 0. For the integer values it's the same except you don't divide by 100. Simple, isn't it?
### Accessing the crunched data/API of the parser
As i said at this point i identified 2 formats:
- Percentage as float to be divided by 100 (to get 2 decimal places)
- Unsigned integer
Because of this, the parser provides 2 functions for accessing the data:
```
float parser_get_value_float(const value_names_t value);
unsigned int parser_get_value_uint(const value_names_t value); 
```
The important part is the `value`-argument. It is an enum (inside `parser.h`) that contains a way to specify which information you want.  
You must make sure to use the correct function or the code will return an invalid value (`#define`ed in the same header), ie you can't ask for a page count (that is an integer) using the get_value_float function.

## Configuration and Compiling
### Prerequisites
This code needs `libusb-1.0`. On Debian 11 you have to install `libusb-1.0-0-dev`.
### Configuration
You can configure some stuff using `#define` inside the different headerfiles, more precisely:
#### Inside main.h
- You can configure the size of the internal buffer for the received data from the printer. I set this to 1kB, fine for my printer but you may need to increase this. Informations "too small for $model" welcome.
#### Inside printercom.h
- **IMPORTANT** You have to specify the USB ProductId of your printer! Just use `lsusb`. The VendorId ist hardcoded to the one from Brother (but could be changed if needed, this is FOSS after all).
- For debugging/testing you can configure the code to NOT communicate with the printer but use a hardcoded response.
- For debugging you can add some verbosity to the code.
- `TRIES_MAX` is the number of read retries the code will make while waiting for the data from the printer. After each failed read (0 bytes returned by the printer) the code will sleep for 5 seconds (hardcoded for now) and then try again. On my system usually the third try is successful.
#### Inside parser.h
- Verbose mode for debugging as above.
- 2 `#define` that specify what the parser will return if you make a request for data that is not known or not in the correct format (this float vs integer stuff i wrote about above).
### Compiling
Just run `./make` (yes it's a local script that calls GCC as i still don't know makefiles...).

## Example Output
```
$ ./get_status 
USING DUMMY DATA!
925 bytes received from printer
remaining toner: black 15.00% - cyan 62.00% - magenta 62.00% - yellow 62.00%
remaining drum lifetime: black 94.00% - cyan 94.00% - magenta 94.00% - yellow 94.00%
remaining belt lifetime: 96.00%
remaining PF Kit1 (whatever that is...) lifetime: 99.00%
remaining fuser lifetime: 99.00%
drum count: black 1037 - cyan 1037 - magenta 1037 - yellow 1037
```
