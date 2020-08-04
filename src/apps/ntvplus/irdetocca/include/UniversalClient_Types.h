/*
 * Copyright 2009-2016 Irdeto B.V.
 *
 * This file and the information contained herein are the subject of copyright
 * and intellectual property rights under international convention. All rights
 * reserved. No part of this file may be reproduced, stored in a retrieval
 * system or transmitted in any form by any means, electronic, mechanical or
 * optical, in whole or in part, without the prior written permission of Irdeto
 * B.V.
 */

/**
 * @file UniversalClient_Types.h
 *
 * Cloaked CA Agent types.  This file contains common types that are platform specific.
 */
#ifndef UNIVERSALCLIENT_TYPES_H__INCLUDED__
#define UNIVERSALCLIENT_TYPES_H__INCLUDED__

#ifdef __cplusplus
extern "C" {
#endif

typedef unsigned int    uc_uint32;  /**< Unsigned 32-bit integer */
typedef unsigned short  uc_uint16;  /**< Unsigned 16-bit integer */
typedef short           uc_int16;  /**< Signed 16-bit integer */
typedef unsigned char   uc_byte;    /**< Unsigned 8-bit byte */
typedef char *          uc_string;  /**< Null-terminated string */
typedef char            uc_char;    /**< Single character */
typedef int             uc_sint32;  /**< Signed 32-bit integer */
typedef unsigned char   uc_uint8;   /**< Unsigned 8-bit integer */
typedef signed char     uc_int8;    /**< Signed 8-bit integer */

typedef int             uc_intptr;  /**< Signed int of the same size as a pointer */
typedef unsigned int    uc_uintptr; /**< Unsigned int of the same size as a pointer */

/**
 * Invalid handle value.  This is equivalent to a void* with all bits set (i.e., 0xFFFFFFFF)
 */
#define UC_INVALID_HANDLE_VALUE ((uc_uintptr)((uc_intptr)-1))


#ifdef __cplusplus
}
#endif

#endif /* !UNIVERSALCLIENT_TYPES_H__INCLUDED__ */
