/**
*****************************************************************************
**
**  File        : tiny_printf.c
**
**  Modified	: 13/10/2013
**
**  Author		: Marcel Tresanchez
**
**  Abstract    : Atollic TrueSTUDIO Minimal iprintf/siprintf/fiprintf
**                and puts/fputs.
**                Provides aliased declarations for printf/sprintf/fprintf
**                pointing to *iprintf variants.
**
**                The following conversion specifiers are supported:
**
**
**            %[flags][width][.precision][length]specifier
**
**            Specifier:
**                c			character
**                d,i		Signed decimal integer
**                s			String of characters
**                u			Unsigned decimal integer
**                f,F		Decimal floating point
**                x,X		Unsigned hexadecimal integer (uppercase letter)
**                %			% is written (conversion specification is '%%')
**
**            flags:
**                -			Left-justify within the given field width; Right justification is the default.
**                +		 	Forces to preceed the result with a plus or minus sign (+ or -) even for positive numbers.
**                (space)   If no sign is going to be written, a blank space is inserted before the value.
**                #			Used with x or X specifier, the value is preceeded with 0x.
**                0			Left-pads the number with zeroes (0) instead of spaces when padding is specified.
**
**            width:
**                (number)	Minimum number of characters to be printed. If the value to be printed is shorter than this number.
**                *			The width is not specified in the format string, but as an additional integer value argument.
**
**            .presicion:
**                .number	For f and F specifiers: this is the number of digits to be printed after the decimal point.
**                .*		The precision is not specified in the format string, but as an additional integer value argument.
**
**            length:
**            	  l			Modifies the length of the data type. See data types below.
**				  L			Modifies the length of the data type. See data types below.
**
**
**
**	Data types:
**				uint8_t		"%u"	(unsigned char: 		0 to 255)
**				uint16_t	"%u"	(unsigned short: 		0 to 65535)
**				uint32_t 	"%lu"	(unsigned long: 		0 to 4294967295)
**				uint64_t	"%Lu"	(unsigned long long: 	0 to 18446744073709551615)
**
**				int8_t		"%d"	(signed char: 			-127 to 127)
**				int16_t		"%d"	(signed short:			-32767 to 32767)
**			 	int32_t		"%ld"	(signed long: 			-2147483647 to 2147483647)
**			 	int64_t		"%Ld"	(signed long long: 		-9223372036854775807 to 9223372036854775807)
**
**				float32_t	"%f"	(float: 				1.175494351e-38 to -1.175494351e-38)
**				float64_t	"%f"	(double: 				2.2250738585072014e-308 to -2.2250738585072014e-308)
**
**
**
**
**  Environment : Atollic TrueSTUDIO
**
**  Distribution: The file is distributed “as is,” without any warranty
**                of any kind.
**
** 	Reference:	http://www.cplusplus.com/reference/cstdio/printf/
** 				http://www.opensource.apple.com/source/xnu/xnu-1456.1.26/osfmk/kern/printf.c
** 				http://stackoverflow.com/questions/589575/size-of-int-long-etc
**
*****************************************************************************
*/

/* Includes */
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include "stm32f4xx.h"
#include ".\USART\usart.h"

#define SEND_BYTE_FUNCTION(X)	USARTx_Put(X); // External redirecting write byte function

// Macro de valor absolut
#ifndef ABS
#define ABS(X)  ((X) > 0 ? (X) : -(X))
#endif

#define isdigit(d) ((d) >= '0' && (d) <= '9')
#define car2dec(c) ((c) - '0')

#define MAX_PRINTF_SIZE		300

/* Create aliases for *printf to integer variants *iprintf */
__attribute__ ((alias("iprintf"))) int printf(const char *fmt, ...);
__attribute__ ((alias("fiprintf"))) int fprintf(FILE* fp, const char *fmt, ...);
__attribute__ ((alias("siprintf"))) int sprintf(char* str, const char *fmt, ...);

///* External function prototypes (defined in syscalls.c) */
//extern int _write(int fd, char *str, int len);
int _write(int32_t file, char *ptr, int32_t len);

/* Private function prototypes */
void ts_doubleFormat(char **buf, double d, long nDigits, char padChar, signed long nprecision, char plusSign);
void ts_sintFormat(char **buf, signed int d, int base, int altFormat, int leftAdjust, long nDigits, char padChar, char plusSign);
void ts_slongFormat(char **buf, signed long long d, int base, int altFormat, int leftAdjust, long nDigits, char padChar, char plusSign);
void ts_uintFormat(char **buf, unsigned int d, int base, int altFormat, int leftAdjust, long nDigits, char padChar);
void ts_ulongFormat(char **buf, unsigned long long d, int base, int altFormat, int leftAdjust, long nDigits, char padChar);

int ts_formatstring(char *buf, const char *fmt, va_list va);


/******* Private functions ************/

// Transform float32_t and float64_t to string
void ts_doubleFormat(char **buf, double d, long nDigits, char padChar, signed long nprecision, char plusSign) {
	long long i;
	unsigned long long part_entera, part_decimal;
	signed long long mult10_entera, mult10_decimal;
	long digits_entera;;
	long digits_add_entera;


	// Si es valor negatiu
	if (d < 0) {
		plusSign = '-';
	}

	// Si no especifica presicio, posem 6 digits decimals per defecte
	if (nprecision == -1) nprecision =  6;


	// Limitem la presicio a 18 digits (maxim digits de long long)
	if (nprecision > 18) nprecision = 18;


	// Obtenim el numero de digits part entera i el mult10_entera
	part_entera = ABS(d);
	digits_entera = 1;
	mult10_entera = 1;
	while (part_entera/mult10_entera >= 10) {
		mult10_entera *= 10;
		digits_entera++;
	}
	digits_add_entera = nDigits-digits_entera;

	// Obtenim els digits de la part decimal i el mult10_dec
	mult10_decimal = 0;
	if (nprecision > 0) {
		mult10_decimal = 1;
		for(i=0;i<nprecision;i++) mult10_decimal*=10;
	}
	part_decimal = ABS((d-(signed long long)d)*mult10_decimal);
	mult10_decimal /= 10; // Corregim el pas de part decimal a part entera


	// ARA MONTEM EL STRING:
	///////////////////////////////

	// Si es positiu i indica "-" "+" o " " => ho posa al davant
	if (plusSign != 0) {
		*((*buf)++) =  plusSign;
	}

	// Posem els digitsde mes de la part entera al davant
	while (digits_add_entera > 0) {
		digits_add_entera--;
		*((*buf)++) = padChar;
	}

	// posem part entera
	while (mult10_entera != 0) {
		long long num = part_entera/mult10_entera;
		*((*buf)++) = num + '0';

		part_entera = part_entera%mult10_entera;
		mult10_entera /= 10;
	}

	// Posem el punt si especifica part decimal
	if (nprecision > 0) {
		*((*buf)++) = '.';
	}

	// Posem part decimal
	while (mult10_decimal != 0) {
		long long num = part_decimal/mult10_decimal;
		*((*buf)++) = num + '0';

		part_decimal = part_decimal%mult10_decimal;
		mult10_decimal /= 10;
	}

}


// Transform int8_t, int16_t and int32_t to string
void ts_sintFormat(char **buf, int d, int base, int altFormat, int leftAdjust, long nDigits, char padChar, char plusSign) {
	int div = 1;
	int nDigitsToAdd;
	int digitsnum;

	// Si es valor negatiu
	if (d < 0) {
		d = -d;
		plusSign = '-';
	}

	// Si es positiu i indica "-" "+" o " " => ho posa al davant
	if (base == 10 && plusSign != 0) {
		*((*buf)++) =  plusSign;
	}

	// si '#' i base 16 => 0x...
	if (base == 16 && altFormat == 1) {
		*((*buf)++) = '0';
		*((*buf)++) = 'x';
	}

	// Obtenim el numero de digits i la base
	digitsnum = 1;
	while (d/div >= base) {
		div *= base;
		digitsnum++;
	}

	// Calcular els digits de mes a ficar
	nDigitsToAdd = nDigits - digitsnum;


	// Posem els digits de mes al davant
	if (leftAdjust == 0) {
		while (nDigitsToAdd > 0) {
			nDigitsToAdd--;
			*((*buf)++) = padChar;
		}
	}

	while (div != 0) {
		int num = d/div;
		d = d%div;
		div /= base;
		if (num > 9)
			*((*buf)++) = (num-10) + 'A';
		else
			*((*buf)++) = num + '0';
	}

	// Posem els digits de mes al final
	if (leftAdjust == 1) {
		while (nDigitsToAdd > 0) {
			nDigitsToAdd--;
			*((*buf)++) = ' '; // si selecciona padChar='0' es ignorat, sempre es posa ' '
		}
	}


}

// Transform int64_t to string
void ts_slongFormat(char **buf, signed long long d, int base, int altFormat, int leftAdjust, long nDigits, char padChar, char plusSign) {
	signed long long div = 1;
	int nDigitsToAdd;
	int digitsnum;

	// Si es valor negatiu
	if (d < 0) {
		d = -d;
		plusSign = '-';
	}

	// Si es positiu i indica "-" "+" o " " => ho posa al davant
	if (base == 10 && plusSign != 0) {
		*((*buf)++) =  plusSign;
	}

	// si '#' i base 16 => 0x...
	if (base == 16 && altFormat == 1) {
		*((*buf)++) = '0';
		*((*buf)++) = 'x';
	}

	// Obtenim el numero de digits i la base
	digitsnum = 1;
	while (d/div >= base) {
		div *= base;
		digitsnum++;
	}

	// Calcular els digits de mes a ficar
	nDigitsToAdd = nDigits - digitsnum;


	// Posem els digits de mes al davant
	if (leftAdjust == 0) {
		while (nDigitsToAdd > 0) {
			nDigitsToAdd--;
			*((*buf)++) = padChar;
		}
	}

	while (div != 0) {
		signed long long num = d/div;
		d = d%div;
		div /= base;
		if (num > 9)
			*((*buf)++) = (num-10) + 'A';
		else
			*((*buf)++) = num + '0';
	}

	// Posem els digits de mes al final
	if (leftAdjust == 1) {
		while (nDigitsToAdd > 0) {
			nDigitsToAdd--;
			*((*buf)++) = ' '; // si selecciona padChar='0' es ignorat, sempre es posa ' '
		}
	}


}


// Transform uint8_t, uint16_t and uint32_t to string
void ts_uintFormat(char **buf, unsigned int d, int base, int altFormat, int leftAdjust, long nDigits, char padChar) {
	int div = 1;
	int nDigitsToAdd;
	int digitsnum;

	// si '#' i base 16 => 0x...
	if (base == 16 && altFormat == 1) {
		*((*buf)++) = '0';
		*((*buf)++) = 'x';
	}

	// Obtenim el numero de digits i la base
	digitsnum = 1;
	while (d/div >= base) {
		div *= base;
		digitsnum++;
	}

	// Calcular els digits de mes a ficar
	nDigitsToAdd = nDigits - digitsnum;

	// Posem els digits de mes al davant
	if (leftAdjust == 0) {
		while (nDigitsToAdd > 0) {
			nDigitsToAdd--;
			*((*buf)++) = padChar;
		}
	}

	while (div != 0) {
		int num = d/div;
		d = d%div;
		div /= base;
		if (num > 9)
			*((*buf)++) = (num-10) + 'A';
		else
			*((*buf)++) = num + '0';
	}

	// Posem els digits de mes al final
	if (leftAdjust == 1) {
		while (nDigitsToAdd > 0) {
			nDigitsToAdd--;
			*((*buf)++) = ' '; // si selecciona padChar='0' es ignorat, sempre es posa ' '
		}
	}

}


// Transform unt64_t to string
void ts_ulongFormat(char **buf, unsigned long long d, int base, int altFormat, int leftAdjust, long nDigits, char padChar) {
	unsigned long long div = 1;
	int nDigitsToAdd;
	int digitsnum;

	// si '#' i base 16 => 0x...
	if (base == 16 && altFormat == 1) {
		*((*buf)++) = '0';
		*((*buf)++) = 'x';
	}

	// Obtenim el numero de digits i la base
	digitsnum = 1;
	while (d/div >= base) {
		div *= base;
		digitsnum++;
	}

	// Calcular els digits de mes a ficar
	nDigitsToAdd = nDigits - digitsnum;

	// Posem els digits de mes al davant
	if (leftAdjust == 0) {
		while (nDigitsToAdd > 0) {
			nDigitsToAdd--;
			*((*buf)++) = padChar;
		}
	}

	while (div != 0) {
		unsigned long long num = d/div;
		d = d%div;
		div /= base;
		if (num > 9)
			*((*buf)++) = (num-10) + 'A';
		else
			*((*buf)++) = num + '0';
	}

	// Posem els digits de mes al final
	if (leftAdjust == 1) {
		while (nDigitsToAdd > 0) {
			nDigitsToAdd--;
			*((*buf)++) = ' '; // si selecciona padChar='0' es ignorat, sempre es posa ' '
		}
	}

}


/**
**---------------------------------------------------------------------------
**  Abstract: Writes arguments va to buffer buf according to format fmt
**  Returns:  Length of string
**---------------------------------------------------------------------------
*/
int ts_formatstring(char *buf, const char *fmt, va_list va) {
	int leftAdjust, altFormat, isLong;
	char plusSign, padChar;
	long nDigits;
	signed long nprecision;
	char *start_buf = buf;
	char c;

	while(*fmt) {
		/* Character no needs formating? */
		if (*fmt != '%') {
			// Just copy and increment
			*buf = *fmt;
			buf++;
			fmt++;
			continue;
		}

		// Mirem si te el #, -, +, ' '
		leftAdjust = 0;
		altFormat  = 0;
		plusSign   = 0;
		fmt++;
		while (1) {
			c = *fmt;
			if (c == '#') {
				altFormat = 1;

			} else if (c == '-') {
				leftAdjust = 1;

			} else if (c == '+') {
				plusSign = '+';

			} else if (c == ' ') {
				if (plusSign == 0) {
					plusSign = ' ';
				}

			} else {
				break;

			}
			fmt++;
		}


		// Mirem si te 0 (indica s'omplira de 0's el que falta per fer el num de digits tambe indicats)
		padChar = ' ';
		if (c == '0') {
			padChar = '0';
			c = *(++fmt);
		}

		// Agafem el num de digits pel numero
		nDigits = 0;
		if (isdigit(c)) {
			while(isdigit(c)) {
				nDigits = 10 * nDigits + car2dec(c);
				c = *(++fmt);
			}
		} else if (c == '*') {
			nDigits = va_arg(va, int);
			if (nDigits < 0) {
				leftAdjust = !leftAdjust;
				nDigits = -nDigits;
			}
			c = *(++fmt);
		}

		// Mirem si te num de presicio
		nprecision = -1;
		if (c == '.') {
			c = *(++fmt);
			if (isdigit(c)) {
				// Agafem la presicio
				nprecision = 0;
				while(isdigit(c)) {
					nprecision = 10 * nprecision + car2dec(c);
					c = *(++fmt);
				}
			} else if (c == '*') {
				nprecision = va_arg(va, int);
				if (nDigits < 0) {
					nprecision = -nprecision;
				}
				c = *(++fmt);
			}
		}

		// Mirem si te el l, ll o L
		isLong = 0;
	    if (c == 'l') {
	    	c = *(++fmt);	/* need it if sizeof(int) < sizeof(long) */
	    	if (sizeof(int) < sizeof(long)) {
	    		isLong = 1;
	    	}
	    	if (c == 'l') {
	    		isLong = 1;
	    		c = *(++fmt);
	    	}
	    } else if (c == 'L') {
	    	isLong = 1;
	    	c = *++fmt;
	    }


	    // Fiquem el format de la dada
	    switch (c) {

	    	// Si es un char
	    	case 'c':
	    		*buf++ = va_arg(va, int);
	    		break;

	    	// si es un string
			case 's':
				{
					char * arg = va_arg(va, char *);
					while (*arg) {
						*buf++ = *arg++;
					}
				}
				break;

	    	// Si es un int8_t/int16_t amb "d", int32_t amb "ld", int64_t amd "Ld")
	    	case 'd':
			case 'i':
				{
					if (isLong == 0) {
						signed int val = va_arg(va, signed int);
						ts_sintFormat(&buf, val, 10, altFormat, leftAdjust, nDigits, padChar, plusSign);

					} else { // int64_t = long long
						signed long long val = va_arg(va, signed long long);
						ts_slongFormat(&buf, val, 10, altFormat, leftAdjust, nDigits, padChar, plusSign);
					}
				}
				break;


		    // Si es un uint8_t/uint16_t amb "u", uint32_t amb "lu", uint64_t amd "Ld")
			case 'u':
				{
					if (isLong == 0) {
						unsigned int val = va_arg(va, unsigned int);
						ts_uintFormat(&buf, val, 10, altFormat, leftAdjust, nDigits, padChar);

					} else { // uint64_t = long long
						unsigned long long val = va_arg(va, unsigned long long);
						ts_ulongFormat(&buf, val, 10, altFormat, leftAdjust, nDigits, padChar);
					}
				}

				break;

			// Mostra el numero enter a Hexadecimal.
			// CUIDADO!!: Los negativos de int8/int16 los convierte a 32 bits.
			// CUIDADO!!: Los float32_t y float64_t no contemplados
			case 'x':
			case 'X':
				{
					if (isLong == 0) {
						unsigned int val = va_arg(va, unsigned int); // de 0 a 0xFFFFFFFF (32 bits)
						ts_uintFormat(&buf, val, 16, altFormat, leftAdjust, nDigits, padChar);
					} else { // uint64_t = long long
						long long val = va_arg(va, long long);
						ts_ulongFormat(&buf, val, 16, altFormat, leftAdjust, nDigits, padChar);
					}
				}
				break;


			// Si es un float32_t amb "f", float64_t amd "f" o "lf")
			case 'f':
			case 'F':
				{
					if (isLong == 0) {
						//float val = va_arg(va, float); // El float no el accepta el va_arg
						double val = va_arg(va, double);
						ts_doubleFormat(&buf, val, nDigits, padChar, nprecision, plusSign);
					} else { // float64_t = double
						double val = va_arg(va, double);
						ts_doubleFormat(&buf, val, nDigits, padChar, nprecision, plusSign);
					}
				}
				break;

			case '%':
				*buf++ = '%';
				break;

			}
			fmt++;


	}
	*buf = 0;

	return (int)(buf - start_buf);
}

/**
**===========================================================================
**  Abstract: Loads data from the given locations and writes them to the
**            given character string according to the format parameter.
**  Returns:  Number of bytes written
**===========================================================================
*/
int siprintf(char *buf, const char *fmt, ...) {
	int length;
	va_list va;
	va_start(va, fmt);
	length = ts_formatstring(buf, fmt, va);
	va_end(va);
	return length;
}

/**
**===========================================================================
**  Abstract: Loads data from the given locations and writes them to the
**            given file stream according to the format parameter.
**  Returns:  Number of bytes written
**===========================================================================
*/
int fiprintf(FILE * stream, const char *fmt, ...) {
	int length = 0;
	va_list va;
	char buf[MAX_PRINTF_SIZE];

	va_start(va, fmt);
	length = ts_formatstring(buf, fmt, va);
	length = _write(stream->_file, buf, length);
	va_end(va);

	return length;
}

/**
**===========================================================================
**  Abstract: Loads data from the given locations and writes them to the
**            standard output according to the format parameter.
**  Returns:  Number of bytes written
**
**===========================================================================
*/
int iprintf(const char *fmt, ...) {
	int length = 0;
	va_list va;
	char buf[MAX_PRINTF_SIZE];

	va_start(va, fmt);
	length = ts_formatstring(buf, fmt, va);
	length = _write(1, buf, length); // 1 = Sortida estandard
	va_end(va);

	return length;
}

/**
**===========================================================================
**  Abstract: fputs writes the string at s (but without the trailing null) to
**  the file or stream identified by fp.
**  Returns:  If successful, the result is 0; otherwise, the result is EOF.
**
**===========================================================================
*/
int fputs(const char *s, FILE *fp) {
	int length = strlen(s);
	int wlen = 0;
	int res;

	wlen = _write((fp->_file), (char*)s, length);
	wlen += _write((fp->_file), "\n", 1);

	if (wlen == (length+1)) {
		res = 0;
	}
	else {
		res = EOF;
	}

	return res;
}

/**
**===========================================================================
**  Abstract: puts writes the string at s (followed by a newline, instead of
**  the trailing null) to the standard output stream.
**  Returns:  If successful, the result is a nonnegative integer; otherwise,
**  the result is EOF.
**
**===========================================================================
*/
int puts(const char *s) {
	int length = strlen(s);
	int numbytes = 0;
	int res;

	numbytes = _write(1, (char*)s, length);
	numbytes += _write(1, "\n", 1);

	if (numbytes == (length+1)) {
		res = 0;
	}
	else {
		res = EOF;
	}

	return res;
}


/**
**===========================================================================
**  Abstract: Imported external _write function from syscalls.c.
**
**===========================================================================
*/
int _write(int32_t file, char *ptr, int32_t len) {
	int i=0;
	for(i=0 ; i<len ; i++)
	SEND_BYTE_FUNCTION((*ptr++));

	return len;
}
