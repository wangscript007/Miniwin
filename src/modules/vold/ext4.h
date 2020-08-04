#ifndef __EXT4_H__
#define __EXT4_H__

#include <unistd.h>

class Ext4 {
public:
    static int doMount(const char *fsPath, const char *mountPoint, bool ro, bool remount,
            bool executable);
    static int format(const char *fsPath, unsigned int numSectors, const char *mountpoint);
    static int resize(const char *fsPath, unsigned int numSectors);
};

#endif
