

#include "integer.h"
#include "fattime.h"
//#include "rtc.h" //RPi

DWORD get_fattime (void)
{
  return ((2006UL-1980) << 25)	      // Year = 2006
		  | (2UL << 21)	      // Month = Feb
		  | (9UL << 16)	      // Day = 9
		  | (22U << 11)	      // Hour = 22
		  | (30U << 5)	      // Min = 30
		  | (0U >> 1)	      // Sec = 0
		  ;
}

