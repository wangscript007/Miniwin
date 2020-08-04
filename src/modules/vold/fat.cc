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

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/mount.h>
#include <sys/wait.h>
#include <linux/fs.h>
#include <sys/ioctl.h>

#include <linux/kdev_t.h>

#include "fat.h"
#include <ngl_types.h>
#include <ngl_log.h>
NGL_MODULE(FAT)

#define ARRAY_SIZE(x) (sizeof(x)/sizeof(x[0]))

static char FSCK_MSDOS_PATH[] = "/system/bin/fsck_msdos";
static char MKDOSFS_PATH[] = "/system/bin/newfs_msdos";
extern "C" int mount(const char *, const char *, const char *, unsigned long, const void *);

int Fat::check(const char *fsPath) {
    bool rw = true;
    if (access(FSCK_MSDOS_PATH, X_OK)) {
        NGLOG_WARN("Skipping fs checks\n");
        return 0;
    }

    int pass = 1;
    int rc = 0;
    do {
        const char *args[4];
        int status;
        args[0] = "-p";
        args[1] = "-f";
        args[2] = fsPath;
        args[3]=NULL;
        rc = execvp(FSCK_MSDOS_PATH, (char **)args);
        if (rc != 0) {
            NGLOG_ERROR("Filesystem check failed due to logwrap error");
            errno = EIO;
            return -1;
        }

        if (!WIFEXITED(status)) {
            NGLOG_ERROR("Filesystem check did not exit properly");
            errno = EIO;
            return -1;
        }

        status = WEXITSTATUS(status);

        switch(status) {
        case 0:
            NGLOG_INFO("Filesystem check completed OK");
            return 0;

        case 2:
            NGLOG_ERROR("Filesystem check failed (not a FAT filesystem)");
            errno = ENODATA;
            return -1;

        case 4:
            if (pass++ <= 3) {
                NGLOG_WARN("Filesystem modified - rechecking (pass %d)",
                        pass);
                continue;
            }
            NGLOG_ERROR("Failing check after too many rechecks");
            errno = EIO;
            return -1;

        default:
            NGLOG_ERROR("Filesystem check failed (unknown exit code %d)", status);
            errno = EIO;
            return -1;
        }
    } while (0);

    return 0;
}

int Fat::doMount(const char *fsPath, const char *mountPoint,
                 bool ro, bool remount, bool executable,
                 int ownerUid, int ownerGid, int permMask, bool createLost) {
    int rc;
    unsigned long flags;
    char mountData[255];

    flags = MS_NODEV | MS_NOSUID | MS_DIRSYNC;

    flags |= (executable ? 0 : MS_NOEXEC);
    flags |= (ro ? MS_RDONLY : 0);
    flags |= (remount ? MS_REMOUNT : 0);

    /*
     * Note: This is a temporary hack. If the sampling profiler is enabled,
     * we make the SD card world-writable so any process can write snapshots.
     *
     * TODO: Remove this code once we have a drop box in system_server.
     */
    /*char value[PROPERTY_VALUE_MAX];
    property_get("persist.sampling_profiler", value, "");
    if (value[0] == '1') {
        NGLOG_WARN("The SD card is world-writable because the"
            " 'persist.sampling_profiler' system property is set to '1'.");
        permMask = 0;
    }*/

    sprintf(mountData,"utf8,fmask=%o,dmask=%o,shortname=mixed",permMask, permMask);

    rc = mount(fsPath, mountPoint, "vfat", flags, mountData);

    if (rc && errno == EROFS) {
        NGLOG_ERROR("%s appears to be a read only filesystem - retrying mount RO", fsPath);
        flags |= MS_RDONLY;
        rc = mount(fsPath, mountPoint, "vfat", flags, mountData);
    }

    if (rc == 0 && createLost) {
        char *lost_path;
        asprintf(&lost_path, "%s/LOST.DIR", mountPoint);
        if (access(lost_path, F_OK)) {
            /*
             * Create a LOST.DIR in the root so we have somewhere to put
             * lost cluster chains (fsck_msdos doesn't currently do this)
             */
            if (mkdir(lost_path, 0755)) {
                NGLOG_ERROR("Unable to create LOST.DIR (%s)", strerror(errno));
            }
        }
        free(lost_path);
    }

    return rc;
}

int Fat::format(const char *fsPath, unsigned int numSectors, bool wipe) {
    int fd;
    const char *args[11];
    int rc;
    int status;

    if (wipe) {
        Fat::wipe(fsPath, numSectors);
    }

    args[0] = "-F";
    args[1] = "32";
    args[2] = "-O";
    args[3] = "android";
    args[4] = "-c";
    args[5] = "64";
    args[6] = "-A";

    if (numSectors) {
        char tmp[32];
        snprintf(tmp, sizeof(tmp), "%u", numSectors);
        const char *size = tmp;
        args[7] = "-s";
        args[8] = size;
        args[9] = fsPath;
        args[10]=NULL;
        rc = execvp(MKDOSFS_PATH ,(char**)args);
    } else {
        args[7] = fsPath;
        args[8]=NULL;
        rc = execvp(MKDOSFS_PATH,(char **)args);
    }

    if (rc != 0) {
        NGLOG_ERROR("Filesystem format failed due to logwrap error");
        errno = EIO;
        return -1;
    }

    if (!WIFEXITED(status)) {
        NGLOG_ERROR("Filesystem format did not exit properly");
        errno = EIO;
        return -1;
    }

    status = WEXITSTATUS(status);

    if (status == 0) {
        NGLOG_INFO("Filesystem formatted OK");
        return 0;
    } else {
        NGLOG_ERROR("Format failed (unknown exit code %d)", status);
        errno = EIO;
        return -1;
    }
    return 0;
}

unsigned int get_blkdev_size(int fd)
{
  unsigned int nr_sec;

  if ( (ioctl(fd, BLKGETSIZE, &nr_sec)) == -1) {
    nr_sec = 0;
  }
  return nr_sec;
}

void Fat::wipe(const char *fsPath, unsigned int numSectors) {
    int fd;
    unsigned long long range[2];

    fd = open(fsPath, O_RDWR);
    if (fd >= 0) {
        if (numSectors == 0) {
            numSectors = get_blkdev_size(fd);
        }
        if (numSectors == 0) {
            NGLOG_ERROR("Fat wipe failed to determine size of %s", fsPath);
            close(fd);
            return;
        }
        range[0] = 0;
        range[1] = (unsigned long long)numSectors * 512;
        if (ioctl(fd, BLKDISCARD, &range) < 0) {
            NGLOG_ERROR("Fat wipe failed to discard blocks on %s", fsPath);
        } else {
            NGLOG_INFO("Fat wipe %d sectors on %s succeeded", numSectors, fsPath);
        }
        close(fd);
    } else {
        NGLOG_ERROR("Fat wipe failed to open device %s", fsPath);
    }
}
