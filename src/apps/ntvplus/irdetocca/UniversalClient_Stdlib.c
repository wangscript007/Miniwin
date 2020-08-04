/**
 * @file UniversalClient_Stdlib.c
 */

//////////////////////////////////////////////////////////////////////////////////////////
//	include header file
//////////////////////////////////////////////////////////////////////////////////////////
#if 0 // for ecos
#include <string.h>
#else // for linux
#include <linux/string.h> //yenjen.0427
#endif
#include "stdio.h"
#include "UniversalClient_API.h"


//////////////////////////////////////////////////////////////////////////////////////////
//	global variable
//////////////////////////////////////////////////////////////////////////////////////////



//////////////////////////////////////////////////////////////////////////////////////////
//	external function
//////////////////////////////////////////////////////////////////////////////////////////
#ifdef UC_STD_DBG
externC int  diag_printf( const char *fmt, ... );  /* Formatted print */
externC int  diag_sprintf(char *buf, const char *fmt, ...);
#endif



/** @defgroup stdlib Cloaked CA Agent Standard Library
 *  Cloaked CA Agent Standard Library Functions
 *
 * These functions are a direct mapping of the corresponding ANSI C runtime libraries.
 * In cases where these functions are already available on the platform, they can be used directly.
 * On platforms where full ANSI C runtime support is not provided, they need to be implemented manually.
 *  @{
 */

/**
 * CRT strlen function
 */
uc_uint32
UniversalClient_Stdlib_strlen(
    const uc_char *pString)
{
	if( pString == NULL )
    {
		return 0;
    }

	return strlen(pString);
}

/**
 * CRT strcpy function
 */
uc_char *
UniversalClient_Stdlib_strcpy(
    uc_char *pDest,
    const uc_char *pSource)
{
	if( pDest == NULL || pSource == NULL)
    {
		return NULL;
    }

	return strcpy( pDest, pSource);
}

/**
 * CRT memcpy function
 */
void *
UniversalClient_Stdlib_memcpy(
    void *pDest,
    const void *pSource,
    uc_uint32 count)
{
	if( pDest == NULL || pSource == NULL)
    {
		return NULL;
    }

	return memcpy( pDest, pSource, count);
}

/**
 * CRT memset function
 */
void *
UniversalClient_Stdlib_memset(
    void *dest,
    uc_uint8 c,
    uc_uint32 count)
{
	if( dest == NULL)
    {
        return NULL;
    }

	return memset(dest, c, count);
}

/**
 * CRT memcmp function
 */
uc_sint32
UniversalClient_Stdlib_memcmp(
    const uc_uint8 *pBuf1,
    const uc_uint8 *pBuf2,
    uc_uint32 len)
{
	if( pBuf1 == NULL || pBuf2 == NULL)
    {
		return 0;
    }

	return memcmp(pBuf1, pBuf2, len);
}

/**
 * CRT srand function
 */
void
UniversalClient_Stdlib_srand(uc_uint32 seed)
{
	return srand(seed);
}

/**
 * CRT rand function
 */
uc_uint32
UniversalClient_Stdlib_rand(void)
{
	return rand();
}


/**
 * CRT printf function
 *
 * @param[in] pFormat
 *
 * \note Implementation of this requires access to varargs implementation of the core CRT method.
 */
uc_sint32
UniversalClient_Stdlib_printf(
    const uc_char *pFormat,
    ...)
{
	#ifdef UC_STD_DBG
	diag_printf(pFormat);
	#endif

	unsigned int retVal;
	va_list args;

	va_start( args, pFormat );
	retVal = vprintf( pFormat,args );
	va_end( args );
	return retVal;

}

/**
 * CRT sprintf function
 *
 * @param[in] buffer
 *
 * @param[in] format
 *
 * \note Implementation of this requires access to varargs implementation of the core CRT method.
 */
uc_uint32
UniversalClient_Stdlib_sprintf(
    uc_char *buffer,
    const uc_char *format,
    ...)
{
	#ifdef UC_STD_DBG
	diag_sprintf(buffer, format);
	#endif

	unsigned int retVal;
	va_list args;

	va_start( args, format );
	retVal = vsprintf( buffer, format, args );
	va_end( args );
	return retVal;

}

/** @} */ /* End of stdlib */

