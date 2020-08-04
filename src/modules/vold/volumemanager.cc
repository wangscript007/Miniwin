#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/mount.h>
#include <sys/ioctl.h>
#include <dirent.h>
#include <directvolume.h>
#include <volumemanager.h>
#include <netlinkevent.h>
#include <fs_mgr.h>
#include <linux/kdev_t.h>
#include <ngl_types.h>
#include <ngl_log.h>

NGL_MODULE(VOLD)

VolumeManager *VolumeManager::sInstance = NULL;

VolumeManager *VolumeManager::Instance() {
    if (!sInstance){
        sInstance = new VolumeManager();
        sInstance->process_config();
    }
    return sInstance;
}

VolumeManager::VolumeManager() {
    mDebug = false;
    mVolumes = new VolumeCollection();
    //mBroadcaster = NULL;
    mUmsSharingCount = 0;
    mSavedDirtyRatio = -1;
    // set dirty ratio to 0 when UMS is active
    mUmsDirtyRatio = 0;
    mVolManagerDisabled = 0;
    NGLOG_DEBUG("");
}

VolumeManager::~VolumeManager() {
    delete mVolumes;
    //delete mActiveContainers;
}

int VolumeManager::process_config(){
    char fstab_filename[PATH_MAX];
    int i;
    int ret = -1;
    int flags;

    sprintf(fstab_filename,"./fstab");

    struct fstab*fstab = fs_mgr_read_fstab(fstab_filename);
    if (!fstab) {
        NGLOG_ERROR("failed to open %s", fstab_filename);
        return -1;
    }
    NGLOG_DEBUG("fstab->num_entries=%d",fstab->num_entries);
    /* Loop through entries looking for ones that vold manages */
    for (i = 0; i < fstab->num_entries; i++) {
        if (fs_mgr_is_voldmanaged(&fstab->recs[i])) {
            DirectVolume *dv = NULL;
            flags = 0;
NGLOG_DEBUG("");
            /* Set any flags that might be set for this volume */
            if (fs_mgr_is_nonremovable(&fstab->recs[i])) {
                flags |= VOL_NONREMOVABLE;
            }
            if (fs_mgr_is_encryptable(&fstab->recs[i])) {
                flags |= VOL_ENCRYPTABLE;
            }
            /* Only set this flag if there is not an emulated sd card */
            if (fs_mgr_is_noemulatedsd(&fstab->recs[i]) &&
                !strcmp(fstab->recs[i].fs_type, "vfat")) {
                flags |= VOL_PROVIDES_ASEC;
            }
NGLOG_DEBUG("");
            dv = new DirectVolume(this, &(fstab->recs[i]), flags);

            if (dv->addPath(fstab->recs[i].blk_device)) {
                NGLOG_ERROR("Failed to add devpath %s to volume %s",fstab->recs[i].blk_device, fstab->recs[i].label);
                goto out_fail;
            }
            addVolume(dv);
        }
    }
    ret = 0;
out_fail:
    return ret;
}

int VolumeManager::addVolume(Volume *v) {
    mVolumes->push_back(v);
    NGLOG_DEBUG("Label:%s point:%s",v->getLabel(),v->getMountpoint());
    return 0;
}

void VolumeManager::handleBlockEvent(NetlinkEvent *evt) {
    const char *devpath = evt->findParam("DEVPATH");

    /* Lookup a volume to handle this device */
    VolumeCollection::iterator it;
    bool hit = false;
    for (it = mVolumes->begin(); it != mVolumes->end(); ++it) {
        if (!(*it)->handleBlockEvent(evt)) {
            NGLOG_DEBUG("Device '%s' event handled by volume %s\n", devpath, (*it)->getLabel());
            hit = true;
            break;
        }
    }

    if (!hit) {
        NGLOG_WARN("No volumes handled block event for '%s'", devpath);
    }
}

int VolumeManager::formatVolume(const char *label, bool wipe) {
    Volume *v = lookupVolume(label);

    if (!v) {
        errno = ENOENT;
        return -1;
    }

    if (mVolManagerDisabled) {
        errno = EBUSY;
        return -1;
    }

    return v->formatVol(wipe);
}
int VolumeManager::mountVolume(const char *label) {
    Volume *v = lookupVolume(label);

    if (!v) {
        errno = ENOENT;
        return -1;
    }

    return v->mountVol();
}


int VolumeManager::getNumDirectVolumes(void) {
    VolumeCollection::iterator i;
    int n=0;

    for (i = mVolumes->begin(); i != mVolumes->end(); ++i) {
        if ((*i)->getShareDevice() != (dev_t)0) {
            n++;
        }
    }
    return n;
}


int VolumeManager::getDirectVolumeList(struct volume_info *vol_list) {
    VolumeCollection::iterator i;
    int n=0;
    dev_t d;

    for (i = mVolumes->begin(); i != mVolumes->end(); ++i) {
        if ((d=(*i)->getShareDevice()) != (dev_t)0) {
            (*i)->getVolInfo(&vol_list[n]);
            snprintf(vol_list[n].blk_dev, sizeof(vol_list[n].blk_dev),
                     "/dev/block/vold/%d:%d", major(d), minor(d));
            n++;
        }
    }

    return 0;
}


int VolumeManager::unmountVolume(const char *label, bool force, bool revert) {
    Volume *v = lookupVolume(label);

    if (!v) {
        errno = ENOENT;
        return -1;
    }

    if (v->getState() == Volume::State_NoMedia) {
        errno = ENODEV;
        return -1;
    }

    if (v->getState() != Volume::State_Mounted) {
        NGLOG_WARN("Attempt to unmount volume which isn't mounted (%d)\n",
             v->getState());
        errno = EBUSY;
        return UNMOUNT_NOT_MOUNTED_ERR;
    }

    //cleanupAsec(v, force);

    return v->unmountVol(force, revert);
}

Volume *VolumeManager::lookupVolume(const char *label) {
    VolumeCollection::iterator i;

    for (i = mVolumes->begin(); i != mVolumes->end(); ++i) {
        if (label[0] == '/') {
            if (!strcmp(label, (*i)->getFuseMountpoint()))
                return (*i);
        } else {
            if (!strcmp(label, (*i)->getLabel()))
                return (*i);
        }
    }
    return NULL;
}

bool VolumeManager::isMountpointMounted(const char *mp)
{
    char device[256];
    char mount_path[256];
    char rest[256];
    FILE *fp;
    char line[1024];

    if (!(fp = fopen("/proc/mounts", "r"))) {
        NGLOG_ERROR("Error opening /proc/mounts (%s)", strerror(errno));
        return false;
    }

    while(fgets(line, sizeof(line), fp)) {
        line[strlen(line)-1] = '\0';
        sscanf(line, "%255s %255s %255s\n", device, mount_path, rest);
        if (!strcmp(mount_path, mp)) {
            fclose(fp);
            return true;
        }
    }

    fclose(fp);
    return false;
}
