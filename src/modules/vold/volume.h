/*
 * Copyright (C) 2008 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef __VOLUME_H__
#define __VOLUME_H__

#include <vector>

class NetlinkEvent;
class VolumeManager;

#define VOL_NONREMOVABLE  1
#define VOL_ENCRYPTABLE   2
#define VOL_PROVIDES_ASEC 4
#define AID_MEDIA_RW  0x1000

struct volume_info{
    int flags;
    char blk_dev[256];
    char mnt_point[256];
    char label[256];
};

class Volume {
private:
    int mState;
    int mFlags;

public:
    static const int State_Init       = -1;
    static const int State_NoMedia    = 0;
    static const int State_Idle       = 1;
    static const int State_Pending    = 2;
    static const int State_Checking   = 3;
    static const int State_Mounted    = 4;
    static const int State_Unmounting = 5;
    static const int State_Formatting = 6;
    static const int State_Shared     = 7;
    static const int State_SharedMnt  = 8;

    static const char *MEDIA_DIR;
    static const char *FUSE_DIR;
    static const char *SEC_ASECDIR_EXT;
    static const char *SEC_ASECDIR_INT;
    static const char *ASECDIR;
    static const char *LOOPDIR;
    static const char *BLKID_PATH;

protected:
    char* mLabel;
    char* mUuid;
    char* mUserLabel;
    VolumeManager *mVm;
    bool mDebug;
    int mPartIdx;
    int mOrigPartIdx;
    bool mRetryMount;

    /*
     * The major/minor tuple of the currently mounted filesystem.
     */
    dev_t mCurrentlyMountedKdev;

public:
    Volume(VolumeManager *vm,const struct fstab_rec*rec, int flags);
    virtual ~Volume();

    int mountVol();
    int unmountVol(bool force, bool revert);
    int formatVol(bool wipe);

    const char* getLabel() { return mLabel; }
    const char* getUuid() { return mUuid; }
    const char* getUserLabel() { return mUserLabel; }
    int getState() { return mState; }
    int getFlags() { return mFlags; };

    /* Mountpoint of the raw volume */
    virtual const char *getMountpoint() = 0;
    virtual const char *getFuseMountpoint() = 0;

    virtual int handleBlockEvent(NetlinkEvent *evt);
    virtual dev_t getDiskDevice();
    virtual dev_t getShareDevice();
    virtual void handleVolumeShared();
    virtual void handleVolumeUnshared();

    void setDebug(bool enable);
    virtual int getVolInfo(struct volume_info *v) = 0;

protected:
    void setUuid(const char* uuid);
    void setUserLabel(const char* userLabel);
    void setState(int state);

    virtual int getDeviceNodes(dev_t *devs, int max) = 0;
    virtual int updateDeviceInfo(char *new_path, int new_major, int new_minor) = 0;
    virtual void revertDeviceInfo(void) = 0;

    int createDeviceNode(const char *path, int major, int minor);

private:
    int initializeMbr(const char *deviceNode);
    bool isMountpointMounted(const char *path);
    int mountAsecExternal();
    int doUnmount(const char *path, bool force);
    int extractMetadata(const char* devicePath);
};

typedef std::vector<Volume *> VolumeCollection;

#endif
