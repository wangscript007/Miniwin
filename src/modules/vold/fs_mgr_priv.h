#ifndef __CORE_FS_MGR_PRIV_H__
#define __CORE_FS_MGR_PRIV_H__
 
#define MF_WAIT         0x1
#define MF_CHECK        0x2
#define MF_CRYPT        0x4
#define MF_NONREMOVABLE 0x8
#define MF_VOLDMANAGED  0x10
#define MF_LENGTH       0x20
#define MF_RECOVERYONLY 0x40
#define MF_SWAPPRIO     0x80
#define MF_ZRAMSIZE     0x100
#define MF_VERIFY       0x200
#define MF_FORCECRYPT   0x400
#define MF_NOEMULATEDSD 0x800 /* no emulated sdcard daemon, sd card is the only
                                 external storage */

#define DM_BUF_SIZE 4096

#endif __CORE_FS_MGR_PRIV_H__
