// File_Utils.ino
//
// Functions related to the SD card (if present) including listing the directory,
// and reading an interpreting a .HEX file.
//
// Author: Nick Gammon

/* ----------------------------------------------------------------------------
NOTE: This file should only be modified in the Atmega_Hex_Uploader directory.
Copies in other directories are hard-linked to this one.
After modifying it run the shell script:
  fixup_links.sh
This script needs to be run in the directories:
  Atmega_Board_Programmer and Atmega_Board_Detector
That will ensure that those directories now are using the same file.
------------------------------------------------------------------------------ */
#include "main.h"
#include <stdio.h>
#include <string.h>
#include <stdarg.h>


#include "usart.h"
#include "fatfs.h"

#define MAX_FILENAME 13
  void * LAST_FILENAME_LOCATION_IN_EEPROM = 0;
#define true 1
#define false 0

void SerPrint(const char *fmt, ...) {
  	char buffer[200];
  	va_list args;
  	va_start(args, fmt);
  	vsnprintf(buffer, sizeof(buffer), fmt, args);
  	va_end(args);

  	int len = strlen(buffer);
  	HAL_UART_Transmit(&huart4, (uint8_t*)buffer, len, 1000);
  }

  enum {
      checkFile,
      verifyFlash,
      writeToFlash,
  };


// types of record in .hex file
enum {
    hexDataRecord,  // 00
    hexEndOfFile,   // 01
    hexExtendedSegmentAddressRecord, // 02
    hexStartSegmentAddressRecord,  // 03
    hexExtendedLinearAddressRecord, // 04
    hexStartLinearAddressRecord // 05
};

BYTE gotEndOfFile;
unsigned long extendedAddress;
unsigned int lineCount;

/*
Line format:

  :nnaaaatt(data)ss

  Where:
  :      = a colon

  (All of below in hex format)

  nn     = length of data part
  aaaa   = address (eg. where to write data)
  tt     = transaction type
           00 = data
           01 = end of file
           02 = extended segment address (changes high-order byte of the address)
           03 = start segment address *
           04 = linear address *
           05 = start linear address *
  (data) = variable length data
  ss     = sumcheck

            * We don't use these

*/

uint8_t name[MAX_FILENAME] = { 0 };  // current file name

BYTE processLine (const uint8_t * pLine, const uint8_t action)
  {
  if (*pLine++ != ':')
     {
     SerPrint("Line does not start with ':' character.\r\n");
     return true;  // error
     }

  const int maxHexData = 40;
  uint8_t hexBuffer [maxHexData];
  int bytesInLine = 0;

  if (action == checkFile)
    if (lineCount++ % 40 == 0)
      showProgress ();

  // convert entire line from ASCII into binary
  while (isxdigit (*pLine))
    {
    // can't fit?
    if (bytesInLine >= maxHexData)
      {
      SerPrint("Line too long to process.\r\n");
      return true;
      } // end if too long

    if (hexConv (pLine, hexBuffer [bytesInLine++]))
      return true;
    }  // end of while

  if (bytesInLine < 5)
    {
    SerPrint("Line too short.\r\n");
    return true;
    }

  // sumcheck it

  uint8_t sumCheck = 0;
  for (int i = 0; i < (bytesInLine - 1); i++)
    sumCheck += hexBuffer [i];

  // 2's complement
  sumCheck = ~sumCheck + 1;

  // check sumcheck
  if (sumCheck != hexBuffer [bytesInLine - 1])
    {
    SerPrint("Sumcheck error. Expected: %02X, got: %02X\r\n",sumCheck,hexBuffer [bytesInLine - 1]);
    return true;
    }

  // length of data (eg. how much to write to memory)
  uint8_t len = hexBuffer [0];

  // the data length should be the number of bytes, less
  //   length / address (2) / transaction type / sumcheck
  if (len != (bytesInLine - 5))
    {
    SerPrint("Line not expected length. Expected %d bytes, got %d bytes.\r\n",len,bytesInLine-5);
    return true;
    }

  // two bytes of address
  unsigned long addrH = hexBuffer [1];
  unsigned long addrL = hexBuffer [2];

  unsigned long addr = addrL | (addrH << 8);

  uint8_t recType = hexBuffer [3];

  switch (recType)
    {
    // stuff to be written to memory
    case hexDataRecord:
      lowestAddress  = min (lowestAddress, addr + extendedAddress);
      highestAddress = max (lowestAddress, addr + extendedAddress + len - 1);
      bytesWritten += len;

      switch (action)
        {
        case checkFile:  // nothing much to do, we do the checks anyway
          break;

        case verifyFlash:
          verifyData (addr + extendedAddress, &hexBuffer [4], len);
          break;

        case writeToFlash:
          writeData (addr + extendedAddress, &hexBuffer [4], len);
          break;
        } // end of switch on action
      break;

    // end of data
    case hexEndOfFile:
      gotEndOfFile = true;
      break;

    // we are setting the high-order byte of the address
    case hexExtendedSegmentAddressRecord:
      extendedAddress = ((((unsigned long) hexBuffer [4]) << 8) + hexBuffer [5]) << 4;
      break;

    case hexExtendedLinearAddressRecord:
      extendedAddress = ((((uint32_t) hexBuffer [4]) << 8) + hexBuffer [5]) << 16;
      break;


    // ignore these, who cares?
    case hexStartSegmentAddressRecord:
    case hexStartLinearAddressRecord:
      break;

    default:
      SerPrint("Cannot handle record type: %d\r\n",recType);
      return true;
    }  // end of switch on recType

  return false;
  } // end of processLine

//------------------------------------------------------------------------------
BYTE readHexFile (const uint8_t * fName, const uint8_t action)
  {
  const int maxLine = 80;
  uint8_t buffer[maxLine];
  std::ifstream sdin (fName);
  int lineNumber = 0;
  gotEndOfFile = false;
  extendedAddress = 0;
  errors = 0;
  lowestAddress = 0xFFFFFFFF;
  highestAddress = 0;
  bytesWritten = 0;
  progressBarCount = 0;

  pagesize = currentSignature.pageSize;
  pagemask = ~(pagesize - 1);
  oldPage = NO_PAGE;

  SerPrint("Processing file: %s\r\n",fName);

  // check for open error
  if (!sdin.is_open())
    {
    SerPrint("Could not open file.\r\n");
    return true;
    }

  switch (action)
    {
    case checkFile:
      SerPrint("Checking file ...\r\n");
      break;

    case verifyFlash:
      SerPrint("Verifying flash ...\r\n");
      break;

    case writeToFlash:
      SerPrint("Erasing chip ...\r\n");
      eraseMemory ();
      SerPrint("Writing flash ...\r\n");
      break;
    } // end of switch

  while (sdin.getline (buffer, maxLine))
    {
    lineNumber++;
    int count = sdin.gcount();
    if (sdin.fail())
      {
      SerPrint("Line %d too long.\r\n",lineNumber);
      return true;
      }  // end of fail (line too long?)

    // ignore empty lines
    if (count > 1)
      {
      if (processLine (buffer, action))
        {
        SerPrint("Error in line %d.\r\n",lineNumber);
        return true;  // error
        }
      }
    }    // end of while each line

  if (!gotEndOfFile)
    {
    SerPrint("Did not get 'end of file' record.\r\n");
    return true;
    }

  switch (action)
    {
    case writeToFlash:
      // commit final page
      if (oldPage != NO_PAGE)
        commitPage (oldPage);
      SerPrint("\r\n");   // finish line of dots
      SerPrint("Written.\r\n");
      break;

    case verifyFlash:
       SerPrint("\r\n");   // finish line of dots
       if (errors == 0)
          SerPrint("No errors found.\r\n");
        else
          {
          SerPrint("%d verification error(s).\r\n",errors);
          if (errors > 100)
            SerPrint("First 100 shown.\r\n");
          }  // end if
       break;

    case checkFile:
      SerPrint("\r\n");   // finish line of dots
      SerPrint("Lowest address  = 0x%x\r\n",lowestAddress);
      SerPrint("Highest address  = 0x%x\r\n",highestAddress);
      SerPrint("Bytes to write  = %d\r\n",bytesWritten);
      break;

    }  // end of switch

  return false;
}  // end of readHexFile


void showDirectory ()
  {
  if (!haveSDcard)
    {
    SerPrint("*** No SD card detected.\r\n");
    return;
    }

  // list files in root directory

  FIL file;
  uint8_t name[MAX_FILENAME];

  SerPrint("\r\n");
  SerPrint("HEX files in root directory:\r\n");
  SerPrint("\r\n");

  // back to start of directory
  sd.vwd()->rewind ();

  // open next file in root.  The volume working directory, vwd, is root
  while (file.openNext(sd.vwd(), O_READ)) {
    file.getName(name,13);
    uint8_t len = strlen (name);
    if (len > 4 && strcmp (&name [len - 4], ".HEX") == 0)
      {
      SerPrint("%s",name);
      for (uint8_t i = strlen (name); i < 13; i++)
        SerPrint(" ");  // space out so dates line up
      SerPrint(" : ");
      uint8_t buf [12];
      sprintf (buf, "%10lu", file.fileSize ());

      SerPrint("%s bytes",buf);
      
      dir_t d;
      if (!file.dirEntry(&d))
        SerPrint("Failed to find file date/time.\r\n");
      else if (d.creationDate != FAT_DEFAULT_DATE)
        {
        Serial.print(F("  Created: "));
        file.printFatDate(&Serial, d.creationDate);
        Serial.print(F(" "));
        file.printFatTime(&Serial, d.creationTime);
        Serial.print(F(".  Modified: "));
        file.printFatDate(&Serial, d.lastWriteDate);
        Serial.print(F(" "));
        file.printFatTime(&Serial, d.lastWriteTime);
        }  // end of got date/time from directory
      SerPrint("\r\n");
      }
    file.close();
    }  // end of listing files

  }  // end of showDirectory

uint8_t lastFileName [MAX_FILENAME] = { 0 };


BYTE chooseInputFile ()
  {
  SerPrint("\r\n");
  SerPrint("Choose disk file [ %s ] ...\r\n",lastFileName);

  getline (name, sizeof name);

  // no name? use last one
  if (name [0] == 0)
    memcpy (name, lastFileName, sizeof name);

  if (readHexFile(name, checkFile))
    {
    SerPrint("***********************************\r\n");
    return true;  // error, don't attempt to write
    }

  // remember name for next time
  memcpy (lastFileName, name, sizeof lastFileName);

  uint8_t fileNameInEEPROM [MAX_FILENAME];
  eeprom_read_block (&fileNameInEEPROM, LAST_FILENAME_LOCATION_IN_EEPROM, MAX_FILENAME);
  fileNameInEEPROM [MAX_FILENAME - 1] = 0;  // ensure terminating null

  // save new file name if it changed from what we have saved
  if (strcmp (fileNameInEEPROM, lastFileName) != 0)
    eeprom_write_block ((const void *) &lastFileName, LAST_FILENAME_LOCATION_IN_EEPROM, MAX_FILENAME);

  // check file would fit into device memory
  if (highestAddress > currentSignature.flashSize)
    {
    SerPrint("Highest address of 0x%08X exceeds available flash memory top 0x%08X\r\n",highestAddress,currentSignature.flashSize);
    SerPrint("***********************************\r\n");
    return true;
    }

  // check start address makes sense
  if (updateFuses (false))
    {
    SerPrint("***********************************\r\n");
    return true;
    }

   return false;
  }  // end of chooseInputFile

#if ALLOW_FILE_SAVING
void readFlashContents ()
  {
  if (!haveSDcard)
    {
    SerPrint("*** No SD card detected.\r\n");
    return;
    }

  progressBarCount = 0;
  pagesize = currentSignature.pageSize;
  pagemask = ~(pagesize - 1);
  oldPage = NO_PAGE;
  uint8_t lastMSBwritten = 0;

  while (true)
    {

    SerPrint("\r\n");
    SerPrint("Choose file to save as: \r\n");

    getline (name, sizeof name);
    int len = strlen (name);

    if (len < 5 || strcmp (&name [len - 4], ".HEX") != 0)
      {
      SerPrint("File name must end in .HEX\r\n");
      return;
      }

    // if file doesn't exist, proceed
    if (!sd.vwd()->exists (name))
      break;

    SerPrint("File %s exists. Overwrite? Type 'YES' to confirm ...\r\n",name);

    if (getYesNo ())
      break;

    }  // end of checking if file exists

  // ensure back in programming mode
  if (!startProgramming ())
    return;

  SdFile myFile;

  // open the file for writing
  if (!myFile.open(name, O_WRITE | O_CREAT | O_TRUNC))
    {
    SerPrint("Could not open file %s for writing.\r\n",name);
    return;
    }

  uint8_t memBuf [16];
  unsigned int i;
  uint8_t linebuf [50];
  uint8_t sumCheck;

  SerPrint("Copying flash memory to SD card (disk) ...\r\n");

  for (unsigned long address = 0; address < currentSignature.flashSize; address += sizeof memBuf)
    {
    bool allFF;

    unsigned long thisPage = address & pagemask;
    // page changed? show progress
    if (thisPage != oldPage && oldPage != NO_PAGE)
      showProgress ();
    // now this is the current page
    oldPage = thisPage;

    // don't write lines that are all 0xFF
    allFF = true;

    for (i = 0; i < sizeof memBuf; i++)
      {
      memBuf [i] = readFlash (address + i);
      if (memBuf [i] != 0xFF)
        allFF = false;
      }  // end of reading 16 bytes
    if (allFF)
      continue;

    uint8_t MSB = address >> 16;
    if (MSB != lastMSBwritten)
      {
      sumCheck = 2 + 2 + (MSB << 4);
      sumCheck = ~sumCheck + 1;
      // hexExtendedSegmentAddressRecord (02)
      sprintf (linebuf, ":02000002%02X00%02X\r\n", MSB << 4, sumCheck);
      myFile.print (linebuf);
      lastMSBwritten = MSB;
      }  // end if different MSB

    sumCheck = 16 + lowByte (address) + highByte (address);
    sprintf (linebuf, ":10%04X00", (unsigned int) address & 0xFFFF);
    for (i = 0; i < sizeof memBuf; i++)
      {
      sprintf (&linebuf [(i * 2) + 9] , "%02X",  memBuf [i]);
      sumCheck += memBuf [i];
      }  // end of reading 16 bytes

    // 2's complement
    sumCheck = ~sumCheck + 1;
    // append sumcheck
    sprintf (&linebuf [(sizeof memBuf * 2) + 9] , "%02X\r\n",  sumCheck);

    myFile.clearWriteError ();
    myFile.print (linebuf);
    if (myFile.getWriteError ())
       {
       SerPrint("\r\n");  // finish off progress bar
       SerPrint("Error writing file.\r\n");
       myFile.close ();
       return;
       }   // end of an error

    }  // end of reading flash

  SerPrint("\r\n");  // finish off progress bar
  myFile.print (":00000001FF\r\n");    // end of file record
  myFile.close ();
  // ensure written to disk
  sd.vwd()->sync ();
  SerPrint("File %s saved.\r\n",name);
  }  // end of readFlashContents
#endif

void writeFlashContents ()
  {
  if (!haveSDcard)
    {
    SerPrint("*** No SD card detected.\r\n");
    return;
    }

  if (chooseInputFile ())
    return;

  // ensure back in programming mode
  if (!startProgramming ())
    return;

  // now commit to flash
  readHexFile(name, writeToFlash);

  // verify
  readHexFile(name, verifyFlash);

  // now fix up fuses so we can boot
  updateFuses (true);

  }  // end of writeFlashContents

void verifyFlashContents ()
  {
  if (!haveSDcard)
    {
    SerPrint("*** No SD card detected.\r\n");
    return;
    }

  if (chooseInputFile ())
    return;

  // ensure back in programming mode
  if (!startProgramming ())
    return;

  // verify it
  readHexFile(name, verifyFlash);
  }  // end of verifyFlashContents

void initFile ()
  {
  SerPrint("Reading SD card ...\r\n");

  // initialize the SD card at SPI_HALF_SPEED to avoid bus errors with
  // breadboards.  use SPI_FULL_SPEED for better performance.
  if (!sd.begin (chipSelect, SPI_HALF_SPEED))
    {
    sd.initErrorPrint();
    haveSDcard = false;
    }
  else
    {
    haveSDcard = true;
    showDirectory ();
    }

//  SerPrint("Free memory = %d\r\n",getFreeMemory ());


  // find what filename they used last
  eeprom_read_block (&lastFileName, LAST_FILENAME_LOCATION_IN_EEPROM, MAX_FILENAME);
  lastFileName [MAX_FILENAME - 1] = 0;  // ensure terminating null

  // ensure file name valid
  for (uint8_t i = 0; i < strlen (lastFileName); i++)
    {
    if (!isprint (lastFileName [i]))
      {
      lastFileName [0] = 0;
      break;
      }
    }
  }  // end of initFile

