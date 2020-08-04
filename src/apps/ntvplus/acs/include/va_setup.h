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

#ifndef _VA_SETUP_H_
#define _VA_SETUP_H_

/**
 @ingroup common
 */

/**
 @defgroup setup va_setup.h
 @brief This is the setup file associated with the STB. The values of the following
        constants are given here for example, they must be adapted to each project.
 @{
 */

/*
 * General
 * ------- */
/**
 * Maximum number of ACS instances (source or tuner).
 */
#define kVA_SETUP_NBMAX_ACS                         3

/*
 * General
 * ------- */
/**
 * Maximum number of SmartCard reader (slot).
 * @note For Adaptive Sentinel solution this value MUST be 0 !!!
 */
#define kVA_SETUP_NBMAX_SC                          1

/*
 * Platform
 * -------- */
/**
 * Semaphore size (in DWORD) required on the OS platform.
 */
#define kVA_SETUP_SEMAPHORE_SIZE_IN_DWORD           6

/*
 * Platform
 * -------- */
/**
 * Mutex size (in DWORD) required on the OS platform.
 */
#define kVA_SETUP_MUTEX_SIZE_IN_DWORD               6

/*
 * Non volatile memory
 * ------------------- */
/**
 * NVM data segment size:
 *  - For Adaptive Sentinel, 32 Ko are required.
 *  - If the VOD option is used, additional 16 Ko are required.
 *  - If the Secure Hybrid option is used (for Broadband), additional 10 Ko are required.
 */
#define kVA_SETUP_ACS_DATA_SEGMENT_SIZE             (64 * 1024)

/**
 * Backup size:
 *  - For Adaptive Sentinel, 8 Ko are required.
 */
#define kVA_SETUP_ACS_DATA_BACKUP_SIZE              (8  * 1024)

/*
 * File system
 * ----------- */
/**
 * Minimal size required by Viaccess-Orca for the file system.
 */
#define kVA_SETUP_FS_SIZE_IN_BYTE                   (50 * 1024 * 1024)

/*
 * Demux setup
 * ----------- */
/**
 * Number of EMM PIDs the ACS has to filter per instance.
 */
#define _kVA_SETUP_NBPID_EMM_PER_INSTANCE 3
/**
 * Number of ECM PIDs the ACS has to filter per instance.
 */
#define _kVA_SETUP_NBPID_ECM_PER_INSTANCE 5
/**
 * Number of section filters per EMM channel  = GA + DU + GH + FT + (U*NbCard) + (S*NbCard)
 *                                            = 4 + (2*NbCard)
 *                                            = 6 (for PurpleBox or Cardless solutions).
 */
#define _kVA_SETUP_NBSECTIONFILTERS_PER_EMM_CHANNEL  8
/**
 * Number of section filters per ECM channel.
 */
#define _kVA_SETUP_NBSECTIONFILTERS_PER_ECM_CHANNEL  1
/**
 * Maximum number of active filters per ACS instance.
 */
#define kVA_SETUP_NBMAX_SECTIONFILTERS                                                   \
    ((_kVA_SETUP_NBSECTIONFILTERS_PER_EMM_CHANNEL * _kVA_SETUP_NBPID_EMM_PER_INSTANCE) + \
     (_kVA_SETUP_NBSECTIONFILTERS_PER_ECM_CHANNEL * _kVA_SETUP_NBPID_ECM_PER_INSTANCE))
/**
 * Maximum number of active filters per PID.
 */
#define kVA_SETUP_NBMAX_FILTERS_PER_CHANNEL         _kVA_SETUP_NBSECTIONFILTERS_PER_EMM_CHANNEL
/**
 * Recommended maximum number of PIDs per ACS instance.
 */
#define kVA_SETUP_NBMAX_PID_CHANNELS               (_kVA_SETUP_NBPID_EMM_PER_INSTANCE + _kVA_SETUP_NBPID_ECM_PER_INSTANCE)

/*
 * Network setup
 * ------------- */
/**
 * Maximum size of multicast datagram.
 */
#define kVA_SETUP_MULTICAST_DATAGRAM_MAX_SIZE       1536
/**
 * Minimum buffer size allocated for every multicast connection.
 */
#define kVA_SETUP_MULTICAST_MINIMUM_BUFFER_SIZE     (4 * 1024)
/**
 * Maximum number of multicast sockets for the STB.
 */
#define kVA_SETUP_MAX_MULTICAST_SOCKETS             10
/**
 * Maximum number of unicast sockets for the STB.
 */
#define kVA_SETUP_MAX_UNICAST_SOCKETS               10

/**
 * Control Word processing type. */
/* ----------------------------- */
/** @n Ask Viaccess-Orca for the value of your chipset (0 or 1).
 *  @n If you are using an old VA_SCHIP driver, please only set :@n
 *     `define kVA_SETUP_CHIPSET_TYPE               2`
 */
#define kVA_SETUP_CWPT                              0


/** @} */

#endif /* _VA_SETUP_H_ */
