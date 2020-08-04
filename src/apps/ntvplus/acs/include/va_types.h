/******************************************************************************
    Copyright
    This code is strictly confidential and the receiver is obliged to use it
    exclusively for his or her own purposes. No part of Viaccess code may be
    reproduced or transmitted in any form or by any means, electronic or
    mechanical, including photocopying, recording, or by any information storage
    and retrieval system, without permission in writing from Viaccess.
    The information in this code is subject to change without notice. Viaccess
    does not warrant that this code is error free. If you find any problems
    with this code or wish to make comments, please report them to Viaccess.

    Trademarks
    Viaccess is a registered trademark of Viaccess ® in France and/or other
    countries. All other product and company names mentioned herein are the
    trademarks of their respective owners.
    Viaccess may hold patents, patent applications, trademarks, copyrights
    or other intellectual property rights over the code hereafter. Unless
    expressly specified otherwise in a Viaccess written license agreement, the
    delivery of this code does not imply the concession of any license over
    these patents, trademarks, copyrights or other intellectual property.

******************************************************************************/

#ifndef VA_TYPES_H
#define VA_TYPES_H

#include <stdlib.h>
#include <stddef.h>

/**
 @ingroup common
 */

/**
 @defgroup types va_types.h
 @brief Provides the definition of VO's basic types.
 @{
 */


#ifdef __BASETYPE_DEFINED__

#include <basetsd.h>

#else

/** 8-bits signed integer */
typedef char            INT8    ;

/** 32-bits unsigned integer */
typedef unsigned long   UINT32  ;

/** 32-bits signed integer */
typedef long            INT32   ;

#endif


/** Byte */
typedef unsigned char   BYTE    ;

/** 8-bits unsigned integer */
typedef unsigned char   UINT8   ; 

/** 16-bits word */
typedef unsigned short  WORD    ;

/** 16-bits unsigned integer */
typedef unsigned short  UINT16  ;

/** 16-bits signed integer */
typedef short           INT16   ;

/** unsigned integer */
typedef unsigned int    UINT    ;

/** signed integer */
typedef int             INT     ;

/** 32-bits double word */
typedef unsigned long   DWORD   ;

/** boolean */
typedef int             BOOL    ;

/** string */
typedef char *          CSTRING ;

/** FALSE constant */
#define FALSE           0

/** TRUE constant */
#define TRUE            1

/** The Unique Identifier Size */
#define kVA_UUID_SIZE                         16

/** Model Id maximum size */
#define kVA_DRM_MODEL_ID_SIZE_MAX             64

/** Directory maximum size */
#define kVA_DRM_DIRECTORY_SIZE_MAX           256

/**
    This type defines the Universal Unique Identifier.@n
    It must be formatted in network byte order (i.e. MSB first).@n
    For example, for a DRM system identifier string "00112233-4455-6677-8899-aabbccddeeff"  found in MPD, then
    tVA_UUID UUID = { 0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF };
  */
typedef BYTE tVA_UUID[kVA_UUID_SIZE];

/**
    This string gives model identifier type.@n
    It is a null-terminated UTF-8 string that is at most @ref kVA_DRM_MODEL_ID_SIZE_MAX -bytes long.
*/
typedef char tVA_DRM_ModelId[kVA_DRM_MODEL_ID_SIZE_MAX+1];

/**
    This string gives Agent directory type.@n
    It is a null-terminated string that is at most @ref kVA_DRM_DIRECTORY_SIZE_MAX -bytes long and contains the directory dedicated to the VO DRM Agent.
*/
typedef char tVA_DRM_AgentDirectory[kVA_DRM_DIRECTORY_SIZE_MAX+1];

/**
    This structure is used as a general purpose byte buffer. @n
    The memory management (allocation and free) of the `pBuffer` field depends on the context.
 */
typedef struct
{
    BYTE         *pBuffer;          /**< The pointer to the byte buffer. */
    UINT32        uiBufferLength;   /**< The length in bytes of the buffer. */

} tVA_BYTE_BUFFER;

/** @} */

#endif /* VA_TYPES_H */
