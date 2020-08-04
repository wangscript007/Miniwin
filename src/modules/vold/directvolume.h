#ifndef __DEVICEVOLUME_H__
#define __DEVICEVOLUME_H__

#include "volume.h"

class PathInfo {
public:
	PathInfo(const char *pattern);
	~PathInfo();
	bool match(const char *path);
private:
	bool warned;
	char *pattern;
	enum PatternType { prefix, wildcard };
	PatternType patternType;
};

typedef std::vector<PathInfo *> PathCollection;

class DirectVolume : public Volume {
public:
    static const int MAX_PARTITIONS = 32;
protected:
    const char* mMountpoint;
    const char* mFuseMountpoint;

    PathCollection *mPaths;
    int            mDiskMajor;
    int            mDiskMinor;
    int            mPartMinors[MAX_PARTITIONS];
    int            mOrigDiskMajor;
    int            mOrigDiskMinor;
    int            mOrigPartMinors[MAX_PARTITIONS];
    int            mDiskNumParts;
    int            mPendingPartCount;
    int            mIsDecrypted;

public:
    DirectVolume(VolumeManager *vm,const fstab_rec*rec, int flags);
    virtual ~DirectVolume();

    int addPath(const char *path);

    const char *getMountpoint() { return mMountpoint; }
    const char *getFuseMountpoint() { return mFuseMountpoint; }

    int handleBlockEvent(NetlinkEvent *evt);
    dev_t getDiskDevice();
    dev_t getShareDevice();
    void handleVolumeShared();
    void handleVolumeUnshared();
    int getVolInfo(struct volume_info *v);

protected:
    int getDeviceNodes(dev_t *devs, int max);
    int updateDeviceInfo(char *new_path, int new_major, int new_minor);
    virtual void revertDeviceInfo(void);
    int isDecrypted() { return mIsDecrypted; }

private:
    void handleDiskAdded(const char *devpath, NetlinkEvent *evt);
    void handleDiskRemoved(const char *devpath, NetlinkEvent *evt);
    void handleDiskChanged(const char *devpath, NetlinkEvent *evt);
    void handlePartitionAdded(const char *devpath, NetlinkEvent *evt);
    void handlePartitionRemoved(const char *devpath, NetlinkEvent *evt);
    void handlePartitionChanged(const char *devpath, NetlinkEvent *evt);

    int doMountVfat(const char *deviceNode, const char *mountPoint);

};

typedef std::vector<DirectVolume *> DirectVolumeCollection;

#endif
