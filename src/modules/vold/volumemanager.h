#ifndef __VOLUME_MANAGER_H__
#define __VOLUME_MANAGER_H__

#include <pthread.h>

#ifdef __cplusplus
#include <vector>

#include "volume.h"

/* The length of an MD5 hash when encoded into ASCII hex characters */
#define MD5_ASCII_LENGTH_PLUS_NULL ((MD5_DIGEST_LENGTH*2)+1)

class VolumeManager {
private:
    static VolumeManager *sInstance;

private:
    //SocketListener        *mBroadcaster;

    VolumeCollection      *mVolumes;
    bool                   mDebug;

    // for adjusting /proc/sys/vm/dirty_ratio when UMS is active
    int                    mUmsSharingCount;
    int                    mSavedDirtyRatio;
    int                    mUmsDirtyRatio;
    int                    mVolManagerDisabled;

public:
    virtual ~VolumeManager();

    int start();
    int stop();
    int process_config();
    void handleBlockEvent(NetlinkEvent *evt);

    int addVolume(Volume *v);

    int listVolumes(/*SocketClient *cli, bool broadcast*/);
    int mountVolume(const char *label);
    int unmountVolume(const char *label, bool force, bool revert);
    int formatVolume(const char *label, bool wipe);
    void disableVolumeManager(void) { mVolManagerDisabled = 1; }

    Volume* getVolumeForFile(const char *fileName);

    //void setBroadcaster(SocketListener *sl) { mBroadcaster = sl; }
    //SocketListener *getBroadcaster() { return mBroadcaster; }

    static VolumeManager *Instance();


    Volume *lookupVolume(const char *label);
    int getNumDirectVolumes(void);
    int getDirectVolumeList(struct volume_info *vol_list);
    int unmountAllAsecsInDir(const char *directory);

    /*
     * Ensure that all directories along given path exist, creating parent
     * directories as needed.  Validates that given path is absolute and that
     * it contains no relative "." or ".." paths or symlinks.  Last path segment
     * is treated as filename and ignored, unless the path ends with "/".  Also
     * ensures that path belongs to a volume managed by vold.
     */
    int mkdirs(char* path);

private:
    VolumeManager();
    void readInitialState();
    bool isMountpointMounted(const char *mp);
};

extern "C" {
#endif /* __cplusplus */
#define UNMOUNT_NOT_MOUNTED_ERR -2
    int vold_disableVol(const char *label);
    int vold_getNumDirectVolumes(void);
    int vold_getDirectVolumeList(struct volume_info *v);
    int vold_unmountAllAsecs(void);
#ifdef __cplusplus
}
#endif

#endif
