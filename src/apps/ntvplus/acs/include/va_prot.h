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

#ifndef _VA_PROT_H_
#define _VA_PROT_H_

#include "va_types.h"
#include "va_errors.h"


/**
 @ingroup std_drivers
 */

/**
 @defgroup protDriver VA_PROT API
 @brief This section describes the interface required to manage the content protection functionalities of the Viaccess-Orca ACS.
 
 @{
 */

/**
@defgroup protConst VA_PROT Constants
@brief The VA_PROT constants
@{
  
*/

/* Bitmap for Copy Protections */
#define  kVA_PROT_BIT_CP_MACROVISION                0x01    /**< Bit cp macrovision               */
#define  kVA_PROT_BIT_CP_CGMSA_LSB                  0x02    /**< Bit cp cgmsa lsb                 */
#define  kVA_PROT_BIT_CP_CGMSA_MSB                  0x04    /**< Bit cp cgmsa msb                 */
#define  kVA_PROT_BIT_CP_DTCP_IP_REDISTRIBUTION     0x08    /**< Bit cp dtcp ip redistribution    */
#define  kVA_PROT_BIT_CP_DTCP_IP_CONFIGURATION_LSB  0x10    /**< Bit cp dtcp ip configuration lsb */
#define  kVA_PROT_BIT_CP_DTCP_IP_CONFIGURATION_MSB  0x20    /**< Bit cp dtcp ip configuration msb */

/* Bitmap for Digital Ouptuts */
#define  kVA_PROT_BIT_SPDIF                         0x0001  /**< Bit spdif           */
#define  kVA_PROT_BIT_ETHERNET                      0x0002  /**< Bit ethernet        */
#define  kVA_PROT_BIT_USB                           0x0004  /**< Bit usb             */
#define  kVA_PROT_BIT_ILINK                         0x0008  /**< Bit ilink           */
#define  kVA_PROT_BIT_DVBCI                         0x0010  /**< Bit dvbci           */
#define  kVA_PROT_BIT_CLEAR_RECORDING               0x0020  /**< Bit clear recording */
#define  kVA_PROT_BIT_DVD                           0x0040  /**< Bit dvd             */
#define  kVA_PROT_BIT_CI_PLUS_1_2                   0x0080  /**< Bit ci plus 1.2     */
#define  kVA_PROT_BIT_CI_PLUS_1_3                   0x0100  /**< Bit ci plus 1.3     */

/* Bitmap for Copy Protection Mechanisms */
#define  kVA_PROT_BIT_CP_MECHANISM_MACROVISION      0x0001  /**< Bit cp mechanism macrovision       */
#define  kVA_PROT_BIT_CP_MECHANISM_FP               0x0002  /**< Bit cp mechanism finger printing   */
#define  kVA_PROT_BIT_CP_MECHANISM_CGMSA            0x0004  /**< Bit cp mechanism cgmsa             */
#define  kVA_PROT_BIT_CP_MECHANISM_DTCP_IP          0x0008  /**< Bit cp mechanism dtcp ip           */


/** Fingerprinting text string size max */
#define kVA_PROT_MAX_FINGERPRINTING_STRING_SIZE     20      /**< Max fingerprinting string size */

/** @} */

/**
 @defgroup protEnum VA_PROT Enumerations
 @brief The VA_PROT enumerations
 @{
*/

/**
    The DTCP-IP and CI+ interfaces require support to manage the CRL and CWL. According to the type of the
    certificate list to be set via function @ref VA_PROT_SetCertificateList (CI + CRL, CI + CWL or DTCP-IP CRL),
    the certificate list must be one of the enumerate values defined in the va_prot.h header file:
*/
typedef enum
{
    eCiPlusCertificateRevocationList,   /**< CI + Certificate Revocation List    */
    eCiPlusCertificateWhiteList,        /**< CI + Certificate White List         */
    eCiPlusRevocationSignallingData,    /**< CI + RevocationSignallingData       */
    eDtcpIpCertificateRevocationList,   /**< DTCP-IP Certificate Revocation List */

    eNbCertificateLists                 /**< The number of available types */

} tVA_PROT_CertificateListType;

/** @} */

/**
 @defgroup protStruct VA_PROT Structures
 @brief The VA_PROT structures
 @{
 */

/**
    This structure contains information related to a digital content, as required by the CI+ interface:
    i.e. the URI and the scrambling profile for controlled content. This information is forwarded
    to the STB using function @ref VA_PROT_SetCiPlusContentInfos.
*/
typedef struct
{
    BYTE aURI[8];                      /**< CI+ Usage Rules Information. See @ref URI details. */
    BOOL bAesMandatoryForContent;      /**< TRUE if the DES algorithm is not authorized for scrambling the controlled content.
                                            See @ref bAesMandatoryForContent details. */
} tVA_PROT_CiPlusInfos;

/**
    This structure permits to define the font and background colors of the character string. It is used by the
    @ref VA_PROT_StartFingerprintingSession function.
    The structure fields represent the 4 components of the RGBA color model
    (the byAlpha value is used as an opacity channel: 0 means a fully-transparent pixel and 255 means a fully-opaque pixel).
*/
typedef struct
{
    BYTE byAlpha; /**< Alpha component of the RGBA color model */
    BYTE byRed;   /**< Red component of the RGBA color model */
    BYTE byGreen; /**< Green component of the RGBA color model */
    BYTE byBlue;  /**< Blue component of the RGBA color model */
} tVA_PROT_Color;

/** @} */

/**
 @defgroup stbProtFunc VA_PROT STB Manufacturer Functions
 @brief The following section gives detailed information on the different protection mechanisms.

 All of these functions are coded by the STB manufacturer.@n 
 The functions are defined in:@n `va_prot.h`
 @{

*/

/**
    The VA_PROT_GetCapabilities function enables to obtain the available digital outputs and the supported
    copy protection mechanisms of the terminal.

    @param [out] pOutputProtection Describes the terminal's supported digital outputs as a bit field.
                                   The digital outputs range from o0 to o15.
                                   The output is defined as supported by the hardware
                                   if the corresponding bit is set to 1. See @ref pOutputProtection details

    @param [out] pCopyProtection   This parameter returns the copy protection mechanisms supported by this terminal and formatted as a bit field.
                                   The copy protection mechanisms range from m0 to m15. A copy protection mechanism is interpreted as
                                   supported by the hardware if the corresponding bit is set to 1. See @ref pCopyProtection details.

    @retval kVA_OK                 If the request is correctly accepted.
    @retval kVA_INVALID_PARAMETER  If the function is called with a NULL pointer.
    @retval kVA_DATA_NOT_AVAILABLE If the data is not yet available.
*/
INT VA_PROT_GetCapabilities(WORD *pOutputProtection, WORD *pCopyProtection);


/**
    The VA_PROT_SetCopyProtection function enables Viaccess-Orca to manage the activation and deactivation of
    the STB's copy protection mechanisms.

    @param [in] dwAcsId            The value of this argument ranges between 0 and uiNbAcs-1, with uiNbAcs being the number of ACS
                                   required by the STB at the initialization stage. For more information refer to @ref ctrlAPI. The
                                   dwAcsId parameter specifies the ACS instance to which the management of the copy protection mechanisms applies.

   @param [in] wCopyProtection     Describes the copy protection schemes as a bit field. The unused bits are reserved for future development and
                                   should be set to 0. See @ref wCopyProtection details.

   @retval kVA_OK                  If the request is correctly accepted.
   @retval kVA_INVALID_PARAMETER   If the dwAcsId parameter is out of range.
*/
INT VA_PROT_SetCopyProtection(DWORD dwAcsId, WORD wCopyProtection);

/**
    This function starts the Fingerprinting copy protection system. It is used to create the text area which contains
    the string to display. This function is new from ACS 5.0 on. 

    @param [in] dwAcsId            The value of this argument ranges between 0 and uiNbAcs-1, with uiNbAcs being the number of ACS
                                   required by the STB at the initialization stage. For more information refer to @ref ctrlAPI. The
                                   dwAcsId parameter specifies the ACS instance for which the fingerprinting is started.

   @param [in] pszFingerprintStr   Address of the string to display. The string has a terminating null character ("\0"). The maximum string size is
                                   @ref kVA_PROT_MAX_FINGERPRINTING_STRING_SIZE excluding the terminating null character. The memory
                                   allocated for the string is freed when the function returns.

   @param [in] uiFontSize          Height of the string to display (in number of pixels).

   @param [in] stForegroundColor   Value of @ref tVA_PROT_Color type. Font color of the string to display.

   @param [in] stBackgroundColor   Value of @ref tVA_PROT_Color type. Background color of the text area.

   @retval kVA_OK                  If the request is correctly accepted.
   @retval kVA_ALREADY_DONE        If the Fingerprinting copy protection system is already started.
   @retval kVA_INVALID_PARAMETER   If the dwAcsId parameter is out of range or if pszFingerprintStr is NULL or points to a null-terminated
                                   string the length of which is superior to @ref kVA_PROT_MAX_FINGERPRINTING_STRING_SIZE.
*/
INT VA_PROT_StartFingerprintingSession(DWORD dwAcsId,
                                       const char *pszFingerprintStr,
                                       UINT8 uiFontSize,
                                       tVA_PROT_Color stForegroundColor,
                                       tVA_PROT_Color stBackgroundColor);

/**
    This function indicates that the text area must be displayed. The text area displayed in the previous call to this
    function must be removed. This function is new from ACS 5.0 on.

    @param [in] dwAcsId            The value of this argument ranges between 0 and uiNbAcs-1, with uiNbAcs being the number of ACS
                                   required by the STB at the initialization stage. For more information refer to @ref ctrlAPI. The
                                   dwAcsId parameter specifies the ACS instance for which the fingerprinting is displayed.

   @param [in] uiHorizontalPercent Coordinates of the upper/left corner of the text area in terms of percentage of the screen from its upper/left
                                   corner. The text area must always be fully visible. Therefore, when calculating the actual location of the text
                                   area, its size must also be taken into account.
                                   For example, if the screen is 1080 pixel high and the text area is 110 pixel high, when uiVerticalPercent =
                                   90, the upper border of the text area should be at 90% x (1080 - 110) = 873 pixels.

   @param [in] uiVerticalPercent   See above.

   @param [in] uiDuration          Number of milliseconds during which the text area must be displayed before removal. The STB manufacturers
                                   can adjust the actual duration according to the frame duration. If the value is 0xFFFF, the text area must not be
                                   removed from display until @ref VA_PROT_DisplayFingerprint or @ref VA_PROT_StopFingerprintingSession is called.


   @retval kVA_OK                  If the request is correctly accepted.
   @retval kVA_INVALID_OPERATION  If the Fingerprinting copy protection system is not started.
   @retval kVA_INVALID_PARAMETER   If the dwAcsId parameter is out of range or if uiHorizontalPercent or uiVerticalPercent values are
                                   greater than 100.
*/
INT VA_PROT_DisplayFingerprint(DWORD dwAcsId,
                               UINT8 uiHorizontalPercent,
                               UINT8 uiVerticalPercent,
                               UINT16 uiDuration);

/**
    This function stops the Fingerprinting copy protection system. If a text area is currently displayed, it must be
    removed. The text area will no longer be used. This function is new from ACS 5.0 on.

    @param [in] dwAcsId            The value of this argument ranges between 0 and uiNbAcs-1, with uiNbAcs being the number of ACS
                                   required by the STB at the initialization stage. For more information refer to @ref ctrlAPI. The dwAcsId
                                   parameter specifies the ACS instance for which the fingerprinting is stopped.

   @retval kVA_OK                  If the request is correctly accepted.
   @retval kVA_ALREADY_DONE        If the Fingerprinting copy protection system is already stopped.
   @retval kVA_INVALID_PARAMETER   If the dwAcsId parameter is out of range.

   @note If the STB is set to inform the end user that a protection mechanism is activated, it must also inform the end user when
    the protection mechanism is deactivated.
*/
INT VA_PROT_StopFingerprintingSession(DWORD dwAcsId);

/**
    The VA_PROT_SetDigitalOutputProtection function enables Viaccess-Orca to manage the activation and
    deactivation of the STB's digital outputs.

    @param [in] wOutputProtection  Describes the protected digital outputs as a bit field. See @ref wOutputProtection details.

    @retval kVA_OK                 If the request is correctly accepted.

    @note Individual or multiple digital outputs can be managed simultaneously using the bit field.
          The STB ignores the bits associated with a non-supported digital output.
*/
INT VA_PROT_SetDigitalOutputProtection(WORD wOutputProtection);

/**
    This function suspends the display of all free2air channels.

    @retval kVA_OK                 If the request is correctly accepted.
    @retval kVA_ALREADY_DONE       If the free channels are already suspended.

    @note Though not mandatory, the STB can be set to inform the end user that a protection mechanism is activated by
    displaying a message.
*/
INT VA_PROT_SuspendFreeChannels(void);

/**
    This function resumes the display of all the free2air channels.

    @retval kVA_OK                 If the request is correctly accepted.
    @retval kVA_ALREADY_DONE       If the free channels are already resumed.

    @note If the STB is set to inform the end user that a protection mechanism is activated, it must also inform the end user when
    the protection mechanism is deactivated.
*/
INT VA_PROT_ResumeFreeChannels(void);

/**
    This function is used to forward to the STB the CRL and CWL as required by the DTCP-IP and CI+ interfaces.

    @param [in] eCertificateListType  Value of the @ref tVA_PROT_CertificateListType enumeration. This parameter specifies the type of the
                                      certificate list to be set (CI + CRL, CI + CWL, CI + RSD or DTCP-IP CRL).

    @param [in] uiLength              Length in bytes of the buffer (pCertificateList) containing the certificate file.

    @param [in] pCertificateList      Pointer to the buffer containing the certificate list file; its length is specified by the uiLength parameter.
                                      The memory buffer is allocated by Viaccess-Orca and is freed once the return code is received (which means
                                      that the called driver must use an internal copy of the input buffer).

    @retval kVA_OK                    If the request is correctly accepted.
    @retval kVA_INVALID_PARAMETER     Either if the function is called with a NULL pCertificateList pointer or if eCertificateListType is
                                      not a value of the tVA_PROT_CertificateListType enumeration.
*/
INT VA_PROT_SetCertificateList(tVA_PROT_CertificateListType eCertificateListType, 
                               UINT32 uiLength,
                               const BYTE *pCertificateList);

/**
    This function is used to transfer to the STB specific information related to digital content, as required by the
    CI+ interface : the URI and the scrambling profile for the controlled content (AES mandatory or not).

    @param [in] dwAcsId               The value of this argument ranges between 0 and uiNbAcs-1, with uiNbAcs being the number of ACS
                                      required by the STB at the initialization stage. For more information refer to @ref ctrlAPI. The dwAcsId
                                      parameter specifies the ACS instance to which the management of the CI+ interface applies.

    @param [in] pstCiPlusInfos        This parameter contains information related to the content and is a pointer to the tVA_PROT_CiPlusInfos
                                      structure. The memory is allocated by Viaccess-Orca and is freed once the return code is received (which means that
                                      the called driver must use an internal copy of the input data). This parameter cannot be NULL.

    @retval kVA_OK                    If the request is correctly accepted.
    @retval kVA_INVALID_PARAMETER     If the dwAcsId parameter is out of range, or if pstCiPlusInfos is NULL.
*/
INT VA_PROT_SetCiPlusContentInfos(DWORD dwAcsId,
                                  const tVA_PROT_CiPlusInfos *pstCiPlusInfos);

/**
    This function is used to set the Content Key Lifetime Period for the CI+ interface. The Content Key is the DES
    or AES key used to protect the controlled content within the CI+ interface. This key should be changed and recalculated
    periodically by the CAM. If this function is not used to set the key lifetime period, the default value
    is 3600 (number of seconds, corresponding to 1 hour).

    @param [in] uiKeyLifetime         This parameter specifies the content key lifetime period in seconds.
    
    @retval kVA_OK                    If the request is correctly accepted.
    @retval kVA_INVALID_PARAMETER     If the uiKeyLifetime parameter is set to 0.
*/
INT VA_PROT_SetCiPlusKeyLifetime(UINT32 uiKeyLifetime);

/** @} */

/** @} */

#endif /* _VA_PROT_H_ */

