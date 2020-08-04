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
    Viaccess is a registered trademark of Viaccess (r) in France and/or other
    countries. All other product and company names mentioned herein are the
    trademarks of their respective owners.
    Viaccess may hold patents, patent applications, trademarks, copyrights
    or other intellectual property rights over the code hereafter. Unless
    expressly specified otherwise in a Viaccess written license agreement, the
    delivery of this code does not imply the concession of any license over
    these patents, trademarks, copyrights or other intellectual property.

******************************************************************************/

/*
    $Revision: 15328 $
    $Date: 2010-06-04 17:46:59 +0200 (ven., 04 juin 2010) $
*/

#ifndef _VA_FS_H_
#define _VA_FS_H_

#include "va_types.h"
#include "va_errors.h"

/**
 @ingroup std_drivers
 */

/**
 @defgroup fsDriver VA_FS API
 @brief This section describes the interface with a generic STB manufacturer non-volatile File System driver.
 @{
*/

/**
* @defgroup fsConst VA_FS Constants
  @brief The VA_FS driver uses a list of constants.

    In the `va_errors.h` header file:
    - @ref kVA_OK
    - @ref kVA_ERROR
    - @ref kVA_INVALID_PARAMETER
    - @ref kVA_ILLEGAL_HANDLE
    - @ref kVA_EOF
    - @ref kVA_FILE_NOT_FOUND
    - @ref kVA_FILE_IS_READ_ONLY

    In the `va_setup.h` header file:
    - @ref kVA_SETUP_FS_SIZE_IN_BYTE

    In the `va_fs.h` header file:
    - @ref kVA_FS_MAX_FILE_NAME_SIZE
* @{
*/

/** Maximum length of a filename */
#define kVA_FS_MAX_FILE_NAME_SIZE 32

/**
* @}
* @defgroup fsEnum VA_FS Enumerations
  @brief The VA_FS driver uses the following enumerations.
* @{
*/

/** This enumeration gives the opening modes associated with a file: */
typedef enum
{

    eFILE_READ,     /**< The file is opened in read-only binary mode. 
                         The file pointer is positioned at the beginning of the file. */

    eFILE_WRITE,    /**< The The file is opened in read-write binary mode. 
                         Previous data is to be kept, if any. 
                         The file is created if it does not exist yet. 
                         The file pointer is positioned at the beginning of the file. */

    eFILE_PURGE,    /**< The file is opened in read-write binary mode. 
                         Any previous data is to be discarded. 
                         The file is created if it does not exist yet. 
                         The file pointer is positioned at the beginning of the file. */

    eFILE_ENUM_LAST /**< This does not specify an opening, it just gives the number of 
                         available values of the @ref tVA_FS_Mode enumeration. */

} tVA_FS_Mode ;


/** This enumeration gives the origin for the @ref VA_FS_Seek function: */
typedef enum
{
    eSEEK_FROM_BEGIN,   /**< The origin of the offset is the beginning of the file. 
                             The offset must then be positive or null. */

    eSEEK_FROM_CURRENT, /**< The origin of the offset is the current file pointer. 
                             The offset can be positive, negative or null. */

    eSEEK_FROM_END,     /**< The origin of the offset is the end of file. 
                             The offset must then be negative or null. */

    eSEEK_ENUM_LAST     /**< This does not specify an offset origin, it just gives the number 
                             of available values in the @ref tVA_FS_SeekOrigin enumeration. */

} tVA_FS_SeekOrigin ;


/**
* @}
* @defgroup fsStruct VA_FS Structures
  @brief The VA_FS driver uses the following structure.
* @{
*/

/** 
  In the current version, this structure contains a NULL-terminated string with the name of the file.
*/
typedef struct
{
    char szName[kVA_FS_MAX_FILE_NAME_SIZE + 1]; /**< A NULL-terminated string with the name
                                                     of the file. The file name cannot exceed 
                                                     kVA_FS_MAX_FILE_NAME_SIZE bytes, excluding
                                                     the terminating null character. */
} tVA_FS_FileInfo ;

/** @} */


/**
* @defgroup manFsFunc VA_FS STB Manufacturer Functions
  @brief The following section details the functions that are coded by the STB manufacturer.
  @{
*/

/** This function opens the file whose name is given as parameter. If the file is already open, 
    the result is not defined (STB manufacturer dependent).
@param pszFileName 
       A NULL-terminated string with the name of the file to be opened. The file name must not 
       exceed kVA_FS_MAX_FILE_NAME_SIZE bytes, excluding the terminating null character. 
       Note also that the file name is case sensitive and it cannot contain the following special 
       or reserved characters : '0x00'...'0x1F', 0x22, '"', '*', ':', '<', '>', '?', '\', '/', '|'.

@param eOpeningMode 
       The opening mode, as described by the @ref tVA_FS_Mode enumeration.

@return The file handle `dwFileHandle` if the opening operation is successful.
@retval kVA_ILLEGAL_HANDLE
        If the open request failed for any reason. 
        The typical situations where the file opening fails are:
        - The parameter `pszFileName` is NULL or it exceeds @ref kVA_FS_MAX_FILE_NAME_SIZE bytes
          (excluding the terminating null character).
        - Illegal character in the file name.
        - The opening mode is @ref eFILE_READ and no file named `pszFileName` is found.
*/

DWORD VA_FS_Open ( const char *pszFileName , tVA_FS_Mode eOpeningMode ) ;


/** This function closes a previously opened file.

@param  dwFileHandle          The file handle returned by the @ref VA_FS_Open function.

@retval kVA_OK                If the close request is successful.
@retval kVA_INVALID_PARAMETER If the dwFileHandle parameter does not correspond to an opened file.
@retval kVA_ERROR             If the close request fails for any other reason.
*/
INT VA_FS_Close( DWORD dwFileHandle );


/** This function reads n bytes from the current file pointer into the given buffer.
@param dwFileHandle 
       The file handle returned by the @ref VA_FS_Open function.

@param pNbReadBytes 
       This parameter points to the number of bytes to be read. After the function's return, 
       this value contains the number of the actual read bytes and it must be less than or equal 
       to the initial requested value. If @ref VA_FS_Read is called with `*pNbReadBytes` equal to 0,
       the function has just to return `kVA_OK` , the file pointer remains unchanged and the `pBuffer`
       buffer contents are also unchanged.
@param pBuffer
       This is a pointer to a contiguous block of memory with the length indicated by the initial 
       value of the `*pNbReadBytes`. This memory block is allocated and freed by Viaccess-Orca.

@retval kVA_OK
        If the read request is successful. The memory pointed by `pBuffer` is therefore filled with 
        `*pNbReadBytes` bytes coming from the file. The file pointer moves `*pNbReadBytes` bytes 
        forward. Note that the number of bytes actually read can be less than the requested value. 
        Indeed, if for implementation reasons the STB manufacturer is not able to read at once the 
        whole required bytes amount, it is acceptable that a lesser number of bytes be read. 
        The parameter `*pNbReadBytes` must be set accordingly and Viaccess-Orca can renew the read request
        for the remaining data.
@retval kVA_EOF
        If the end of the file has been reached and there is a read operation that attempts to read
        past the end of the file. The file pointer is consequently set at the end of the file. The 
        number of bytes actually read is less than the initially requested value.
@retval kVA_INVALID_PARAMETER
        If the dwFileHandle parameter does not correspond to a valid opened file or if the function
        is called with null parameters (`pNbReadBytes` or `pBuffer`). The file pointer is kept 
        unchanged and `*pNbReadBytes` is set to 0 (if `pNbReadBytes` is not NULL).
@retval kVA_ERROR
        If the read request fails for any other reason. The file pointer is kept unchanged and 
        `*pNbReadBytes` is set to 0.
*/
INT VA_FS_Read( DWORD dwFileHandle,
                UINT32 *pNbReadBytes,
                BYTE *pBuffer );

/** This function writes the given byte buffer into an opened file, starting from the current file 
    pointer position.
    The write operation is synchronous. If a cache mechanism is implemented, the STB manufacturer 
    driver must immediately flush the modified memory into the Non-Volatile memory, before the 
    function return.

@param dwFileHandle
       The file handle returned by the @ref VA_FS_Open function.
@param pNbWriteBytes
       This parameter points to the number of bytes to be written. After the function return, 
       this value contains the number of bytes actually written, which must be less than or equal 
       to the initial requested value. If @ref VA_FS_Write is called with `*pNbWriteBytes` equal to 
       0, the function has just to return kVA_OK and the file pointer remains unchanged.
@param pBuffer
       This is a pointer to a contiguous block of memory with its length indicated by the initial 
       value of the `*pNbWriteBytes`. This memory block is allocated and freed by Viaccess-Orca.

@retval kVA_OK
        If the write request is successful. The file pointer moves the actually written 
        `*pNbWriteBytes` bytes forward.Note that the actually written bytes count can be inferior to 
        the required amount. Indeed, if for implementation reasons the STB manufacturer is not able
        to write at once the whole required bytes amount, it is acceptable that an inferior number 
        of bytes be written. The parameter `*pNbWriteBytes` must be set accordingly and Viaccess-Orca can 
        renew the write request for the remaining data. Anyway, if the actual space remaining in 
        the file system is less than the size of the buffer the function is trying to write, 
        the function fails and returns kVA_ERROR, as defined below.
@retval kVA_INVALID_PARAMETER
        If the dwFileHandle parameter does not correspond to a valid open file or if the function 
        is called with null parameters (`pNbWriteBytes` or `pBuffer`). The file pointer is kept 
        unchanged.
@retval kVA_FILE_IS_READ_ONLY
        If the file was opened in read only mode. The file pointer is kept unchanged.
@retval kVA_ERROR
        If the write request fails for any other reason, including when there is no more space 
        left in the file system. The file pointer is kept unchanged and `*pNbWriteBytes` is set to 0.
*/
INT VA_FS_Write( DWORD dwFileHandle,
                 UINT32 *pNbWriteBytes,
                 BYTE *pBuffer );

/** This function sets the file pointer to the position determined by an origin and an offset 
    relative to this origin.
@param dwFileHandle
       The file handle returned by the @ref VA_FS_Open function.
@param eOrigin
       The origin of the offset, as described by the enumeration @ref tVA_FS_SeekOrigin.
@param iOffset
       The offset related to the above eOrigin value. The iOffset parameter satisfies the 
       constraints as provided in the following table:
       Value of eOrigin   | Range for iOffset
       -------------------|--------------------------
       eSEEK_FROM_BEGIN   |  0 <= iOffset <= FileSize
       eSEEK_FROM_CURRENT |  -CurrentPos <= iOffset <= FileSize - CurrentPos
       eSEEK_FROM_END     |  -FileSize <= iOffset <= 0
@param pNewPositionFromBegin
       Pointer to the new position of the file pointer in relation to the beginning of the file. 
       This value is set by the STB driver and ranges from 0 to the size of the file.

@retval kVA_OK
        If the seek request is successful. The address pointed by `pNewPositionFromBegin` is filled 
        with the new file pointer position.
@retval kVA_INVALID_PARAMETER
        If the dwFileHandle parameter does not correspond to a valid open file or if `eOrigin` is 
        not one of the enumerations given by the @ref tVA_FS_SeekOrigin type. This error code is also 
        returned if `iOffset` does not satisfy the constraints described above or 
        if `pNewPositionFromBegin` is NULL. The file pointer is kept unchanged.
@retval kVA_ERROR
        If the seek request fails for any other reason.
*/
INT VA_FS_Seek( DWORD dwFileHandle,
                tVA_FS_SeekOrigin eOrigin,
                INT32 iOffset,
                UINT32 *pNewPositionFromBegin );


/** This function returns the size in bytes of the specified file. If the file is currently open, 
    then the result is not defined (STB manufacturer dependent).

@param pszFileName
       A NULL-terminated string with the name of the file whose size is to be returned.
@param pFileSize
       The address of the UINT32 integer where the returning file size (in bytes) is to be stored.

@retval kVA_OK
        If the get size request is successful. The address pointed by `pFileSize` is filled with 
        the size in bytes of the file.
@retval kVA_INVALID_PARAMETER
        If the function is called with null parameters (`pszFileName` or `pFileSize`) or if the file 
        name exceeds @ref kVA_FS_MAX_FILE_NAME_SIZE bytes (excluding the terminating null character).
@retval kVA_FILE_NOT_FOUND
        If no file named `pszFileName` is found.
@retval kVA_ERROR
        If the get size request fails for any other reason.
*/
INT VA_FS_GetSize(const char *pszFileName, UINT32 *pFileSize );

/** This function removes the specified file from the file system. 
    If the file is currently open, then the result is not defined (STB manufacturer dependent).

@param pszFileName
       A NULL-terminated string with the name of the file to be removed.

@retval kVA_OK
        If the remove request is successful.
@retval kVA_INVALID_PARAMETER
        If the function is called with null parameters (`pszFileName`) or if the file name exceeds
        @ref kVA_FS_MAX_FILE_NAME_SIZE bytes (excluding the terminating null character).
@retval kVA_FILE_NOT_FOUND
        If no file named `pszFileName` is found.
@retval kVA_ERROR
        If the remove request fails for any other reason.
*/
INT VA_FS_Remove( const char *pszFileName );

/** This function returns the number of the files from the file system.
@return The function returns the number of the files from the reserved Viaccess-Orca directory. 
        Note that only the files are counted (the current directory is excluded) and the 
        returned value ranges from 0 (no file in the file system) to n.
*/
UINT32 VA_FS_GetFilesCount( void );

/** This function returns the list of the files (their names) from the file system.

@param pFilesCount
       The initial value of this parameter gives the number of elements of the `aFiles` array. 
       After the function return, this value contains the actual number of elements of the aFiles 
       array that were fulfilled by the VA_FS driver. The returned value must be less than 
       or equal to the initial value. The elements of the array are fulfilled in order, starting 
       from element 0.
@param aFiles
       This parameter is an array with `*pFilesCount` elements of type @ref tVA_FS_FileInfo. The 
       VA_FS driver fulfills the first `*pFilesCount` elements of the array with the 
       appropriate files information (the files names). The selection criteria for the returned 
       files and their order are not defined in this document and they are driver-manufacturer 
       specific.

@retval kVA_OK
        If the request is successful. The function returns kVA_OK even if the returned files list 
        is not complete, i.e. when the initial `*pFilesCount` is inferior to the actually existing 
        files number (value returned by the function @ref VA_FS_GetFilesCount). In this case, the 
        algorithm for choosing the first `*pFilesCount` files among the @ref VA_FS_GetFilesCount() 
        files is not defined in this document and it is driver-manufacturer specific.
@retval kVA_INVALID_PARAMETER
        If the function is called with null parameters (`pFilesCount`).
@retval kVA_ERROR
        If the request fails for any other reason. In this case, the parameter `*pFilesCount` is 
        set to 0 at the function return.
*/
INT VA_FS_GetFiles( UINT32 *pFilesCount, tVA_FS_FileInfo aFiles[] );

/**@}*/

/**@}*/


#endif /* _VA_FS_H_ */

