#include <stdint.h>
#include <stddef.h>
#include <stdarg.h>
#include <stdbool.h>

#include "string.h"

#define FL_ZFILL        0x01
#define FL_PLUS         0x02
#define FL_SPACE        0x04
#define FL_LPAD         0x08
#define FL_ALT          0x10
#define FL_WIDTH        0x20
#define FL_PREC         0x40
#define FL_LONG         0x80

#define FL_NEGATIVE     FL_LONG

#define FL_ALTUPP       FL_PLUS
#define FL_ALTHEX       FL_SPACE

#define FL_FLTUPP       FL_ALT
#define FL_FLTEXP       FL_PREC
#define FL_FLTFIX       FL_LONG

int row = 2;
int col = 0;
extern uint16_t ekran[25][80];
#if DEBUG
#include <stdio.h>
void puts_raw(const char *p) {
    puts(p);
}
#else
void puts_raw(const char *p) {
	while (*p) {
		char c = *p++;
		if (c == '\n')
			row++, col = 0;
		else
			ekran[row][col++] = c | 0x0e00;
	}
}
#endif

size_t strnlen(const char *s, size_t maxlen) {
    const char *start = s;
    size_t ret;
    for(;*s != '\0' && s < start + maxlen; s++)
        ;
    return ret;
}

int printf(const char *fmt, ...) {
    char buf[1024] = {0}; //meh

    va_list ap;
    va_start(ap, fmt);
    int ret = vsnprintf(buf, sizeof buf, fmt, ap);
    va_end(ap);
    puts_raw(buf);
    return ret;
}

int snprintf(char *s, size_t n, const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    int ret = vsnprintf(s, n, fmt, ap);
    va_end(ap);
    return ret;
}


#define XTOA_PREFIX     0x0100  /* put prefix for octal or hex  */
#define XTOA_UPPER      0x0200  /* use upper case letters       */
#define PRINTF_MIN 1
#define PRINTF_STD 2
#define PRINTF_FLT 3
# define PRINTF_LEVEL PRINTF_STD

char * __ultoa_invert (unsigned long val, char * str, int base) {
    char hex_a = (base & XTOA_UPPER) ? 'A' : 'a';
    base &= 0x1f;
    while(val > 0) {
        int dig = val % base;
        val /= base;
        char chr;
        if(dig < 10)
            chr = '0' + dig;
        else
            chr = hex_a + (dig - 10);
        *str++ = chr;
    }
    return str;
}

//the following is a (modified) part of avr-libc

int vsnprintf (char *stream, size_t size, const char *fmt, va_list ap)
{
    unsigned char c;		/* holds a char from the format string */
    unsigned char flags;
    unsigned char width;
    unsigned char prec;
    unsigned char buf[32];	/* size for -1 in octal, without '\0'	*/

    const char *stream_end = stream + size;
    const char *stream_start = stream;
#define putc(__chr, __stream) do { if(__stream >= stream_end) return stream_end - stream_start; else *__stream++ = __chr; } while(0);
    for (;;) {

	for (;;) {
	    c = *fmt++;
	    if (!c) goto ret;
	    if (c == '%') {
		c = *fmt++;
		if (c != '%') break;
	    }
	    putc (c, stream);
	}

	flags = 0;
	width = 0;
	prec = 0;
	
	do {
	    if (flags < FL_WIDTH) {
		switch (c) {
		  case '0':
		    flags |= FL_ZFILL;
		    continue;
		  case '+':
		    flags |= FL_PLUS;
		    /* FALLTHROUGH */
		  case ' ':
		    flags |= FL_SPACE;
		    continue;
		  case '-':
		    flags |= FL_LPAD;
		    continue;
		  case '#':
		    flags |= FL_ALT;
		    continue;
		}
	    }

	    if (flags < FL_LONG) {
		if (c >= '0' && c <= '9') {
		    c -= '0';
		    if (flags & FL_PREC) {
			prec = 10*prec + c;
			continue;
		    }
		    width = 10*width + c;
		    flags |= FL_WIDTH;
		    continue;
		}
		if (c == '.') {
		    if (flags & FL_PREC)
			goto ret;
		    flags |= FL_PREC;
		    continue;
		}
		if (c == 'l') {
		    flags |= FL_LONG;
		    continue;
		}
		if (c == 'h')
		    continue;
	    }
	    
	    break;
	} while ( (c = *fmt++) != 0);

	/* Only a format character is valid.	*/

#if	'F' != 'E'+1  ||  'G' != 'F'+1  ||  'f' != 'e'+1  ||  'g' != 'f'+1
# error
#endif

#if PRINTF_LEVEL >= PRINTF_FLT
	if (c >= 'E' && c <= 'G') {
	    flags |= FL_FLTUPP;
	    c += 'e' - 'E';
	    goto flt_oper;

	} else if (c >= 'e' && c <= 'g') {

	    int exp;		/* exponent of master decimal digit	*/
	    int n;
	    unsigned char vtype;	/* result of float value parse	*/
	    unsigned char sign;		/* sign character (or 0)	*/
# define ndigs	c		/* only for this block, undef is below	*/

	    flags &= ~FL_FLTUPP;

	  flt_oper:
	    if (!(flags & FL_PREC))
		prec = 6;
	    flags &= ~(FL_FLTEXP | FL_FLTFIX);
	    if (c == 'e')
		flags |= FL_FLTEXP;
	    else if (c == 'f')
		flags |= FL_FLTFIX;
	    else if (prec > 0)
		prec -= 1;

	    if (flags & FL_FLTFIX) {
		vtype = 7;		/* 'prec' arg for 'ftoa_engine'	*/
		ndigs = prec < 60 ? prec + 1 : 60;
	    } else {
		if (prec > 7) prec = 7;
		vtype = prec;
		ndigs = 0;
	    }
	    exp = __ftoa_engine (va_arg(ap,double), (char *)buf, vtype, ndigs);
	    vtype = buf[0];
    
	    sign = 0;
	    if ((vtype & FTOA_MINUS) && !(vtype & FTOA_NAN))
		sign = '-';
	    else if (flags & FL_PLUS)
		sign = '+';
	    else if (flags & FL_SPACE)
		sign = ' ';

	    if (vtype & (FTOA_NAN | FTOA_INF)) {
		const char *p;
		ndigs = sign ? 4 : 3;
		if (width > ndigs) {
		    width -= ndigs;
		    if (!(flags & FL_LPAD)) {
			do {
			    putc (' ', stream);
			} while (--width);
		    }
		} else {
		    width = 0;
		}
		if (sign)
		    putc (sign, stream);
		p = PSTR("inf");
		if (vtype & FTOA_NAN)
		    p = PSTR("nan");
# if ('I'-'i' != 'N'-'n') || ('I'-'i' != 'F'-'f') || ('I'-'i' != 'A'-'a')
#  error
# endif
		while ( (ndigs = pgm_read_byte(p)) != 0) {
		    if (flags & FL_FLTUPP)
			ndigs += 'I' - 'i';
		    putc (ndigs, stream);
		    p++;
		}
		goto tail;
	    }

	    /* Output format adjustment, number of decimal digits in buf[] */
	    if (flags & FL_FLTFIX) {
		ndigs += exp;
		if ((vtype & FTOA_CARRY) && buf[1] == '1')
		    ndigs -= 1;
		if ((signed char)ndigs < 1)
		    ndigs = 1;
		else if (ndigs > 8)
		    ndigs = 8;
	    } else if (!(flags & FL_FLTEXP)) {		/* 'g(G)' format */
		if (exp <= prec && exp >= -4)
		    flags |= FL_FLTFIX;
		while (prec && buf[1+prec] == '0')
		    prec--;
		if (flags & FL_FLTFIX) {
		    ndigs = prec + 1;		/* number of digits in buf */
		    prec = prec > exp
			   ? prec - exp : 0;	/* fractional part length  */
		}
	    }
    
	    /* Conversion result length, width := free space length	*/
	    if (flags & FL_FLTFIX)
		n = (exp>0 ? exp+1 : 1);
	    else
		n = 5;		/* 1e+00 */
	    if (sign) n += 1;
	    if (prec) n += prec + 1;
	    width = width > n ? width - n : 0;
    
	    /* Output before first digit	*/
	    if (!(flags & (FL_LPAD | FL_ZFILL))) {
		while (width) {
		    putc (' ', stream);
		    width--;
		}
	    }
	    if (sign) putc (sign, stream);
	    if (!(flags & FL_LPAD)) {
		while (width) {
		    putc ('0', stream);
		    width--;
		}
	    }
    
	    if (flags & FL_FLTFIX) {		/* 'f' format		*/

		n = exp > 0 ? exp : 0;		/* exponent of left digit */
		do {
		    if (n == -1)
			putc ('.', stream);
		    flags = (n <= exp && n > exp - ndigs)
			    ? buf[exp - n + 1] : '0';
		    if (--n < -prec)
			break;
		    putc (flags, stream);
		} while (1);
		if (n == exp
		    && (buf[1] > '5'
		        || (buf[1] == '5' && !(vtype & FTOA_CARRY))) )
		{
		    flags = '1';
		}
		putc (flags, stream);
	
	    } else {				/* 'e(E)' format	*/

		/* mantissa	*/
		if (buf[1] != '1')
		    vtype &= ~FTOA_CARRY;
		putc (buf[1], stream);
		if (prec) {
		    putc ('.', stream);
		    sign = 2;
		    do {
			putc (buf[sign++], stream);
		    } while (--prec);
		}

		/* exponent	*/
		putc (flags & FL_FLTUPP ? 'E' : 'e', stream);
		ndigs = '+';
		if (exp < 0 || (exp == 0 && (vtype & FTOA_CARRY) != 0)) {
		    exp = -exp;
		    ndigs = '-';
		}
		putc (ndigs, stream);
		for (ndigs = '0'; exp >= 10; exp -= 10)
		    ndigs += 1;
		putc (ndigs, stream);
		putc ('0' + exp, stream);
	    }

	    goto tail;
# undef ndigs
	}

#else		/* to: PRINTF_LEVEL >= PRINTF_FLT */
	if ((c >= 'E' && c <= 'G') || (c >= 'e' && c <= 'g')) {
	    (void) va_arg (ap, double);
	    buf[0] = '?';
	    goto buf_addr;
	}

#endif

	{
	    const char * pnt;
	    size_t size;

	    switch (c) {

	      case 'c':
		buf[0] = va_arg (ap, int);
#if  PRINTF_LEVEL < PRINTF_FLT
	      buf_addr:
#endif
		pnt = (char *)buf;
		size = 1;
		goto no_pgmstring;

	      case 's':
		pnt = va_arg (ap, char *);
		size = strnlen (pnt, (flags & FL_PREC) ? prec : ~0);
	      no_pgmstring:
		goto str_lpad;

	      str_lpad:
		if (!(flags & FL_LPAD)) {
		    while (size < width) {
			putc (' ', stream);
			width--;
		    }
		}
		while (size) {
		    putc (*pnt++, stream);
		    if (width) width -= 1;
		    size -= 1;
		}
		goto tail;
	    }
	}

	if (c == 'd' || c == 'i') {
	    long x = (flags & FL_LONG) ? va_arg(ap,long) : va_arg(ap,int);
	    flags &= ~(FL_NEGATIVE | FL_ALT);
	    if (x < 0) {
		x = -x;
		flags |= FL_NEGATIVE;
	    }
	    c = __ultoa_invert (x, (char *)buf, 10) - (char *)buf;

	} else {
	    int base;

	    if (c == 'u') {
		flags &= ~FL_ALT;
		base = 10;
		goto ultoa;
	    }

	    flags &= ~(FL_PLUS | FL_SPACE);

	    switch (c) {
	      case 'o':
	        base = 8;
		goto ultoa;
	      case 'p':
	        flags |= FL_ALT;
                flags |= FL_LONG;
		/* no break */
	      case 'x':
		if (flags & FL_ALT)
		    flags |= FL_ALTHEX;
	        base = 16;
		goto ultoa;
	      case 'X':
		if (flags & FL_ALT)
		    flags |= (FL_ALTHEX | FL_ALTUPP);
	        base = 16 | XTOA_UPPER;
	      ultoa:
		c = __ultoa_invert ((flags & FL_LONG)
				    ? va_arg(ap, unsigned long)
				    : va_arg(ap, unsigned int),
				    (char *)buf, base)  -  (char *)buf;
		flags &= ~FL_NEGATIVE;
		break;

	      default:
	        goto ret;
	    }
	}

	{
	    unsigned char len;

	    len = c;
	    if (flags & FL_PREC) {
		flags &= ~FL_ZFILL;
		if (len < prec) {
		    len = prec;
		    if ((flags & FL_ALT) && !(flags & FL_ALTHEX))
			flags &= ~FL_ALT;
		}
	    }
	    if (flags & FL_ALT) {
		if (buf[c-1] == '0') {
		    flags &= ~(FL_ALT | FL_ALTHEX | FL_ALTUPP);
		} else {
		    len += 1;
		    if (flags & FL_ALTHEX)
		    	len += 1;
		}
	    } else if (flags & (FL_NEGATIVE | FL_PLUS | FL_SPACE)) {
		len += 1;
	    }

	    if (!(flags & FL_LPAD)) {
		if (flags & FL_ZFILL) {
		    prec = c;
		    if (len < width) {
			prec += width - len;
			len = width;
		    }
		}
		while (len < width) {
		    putc (' ', stream);
		    len++;
		}
	    }
	
	    width =  (len < width) ? width - len : 0;

	    if (flags & FL_ALT) {
		putc ('0', stream);
		if (flags & FL_ALTHEX)
		    putc (flags & FL_ALTUPP ? 'X' : 'x', stream);
	    } else if (flags & (FL_NEGATIVE | FL_PLUS | FL_SPACE)) {
		unsigned char z = ' ';
		if (flags & FL_PLUS) z = '+';
		if (flags & FL_NEGATIVE) z = '-';
		putc (z, stream);
	    }
		
	    while (prec > c) {
		putc ('0', stream);
		prec--;
	    }
	
	    do {
		putc (buf[--c], stream);
	    } while (c);
	}
	
      tail:
	/* Tail is possible.	*/
	while (width) {
	    putc (' ', stream);
	    width--;
	}
    } /* for (;;) */

  ret:
    return stream_end - stream;
}

