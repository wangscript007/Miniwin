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

#include <linux/kdev_t.h>

#include <ngl_types.h>
#include <ngl_log.h>
#include "ext4.h"

NGL_MODULE(EXT4)

#define MKEXT4FS_PATH "/system/bin/make_ext4fs"
#define RESIZE2FS_PATH "/system/bin/resize2fs"

int Ext4::doMount(const char *fsPath, const char *mountPoint, bool ro, bool remount,
        bool executable) {
    int rc;
    unsigned long flags;

    flags = MS_NOATIME | MS_NODEV | MS_NOSUID | MS_DIRSYNC;

    flags |= (executable ? 0 : MS_NOEXEC);
    flags |= (ro ? MS_RDONLY : 0);
    flags |= (remount ? MS_REMOUNT : 0);

    rc = mount(fsPath, mountPoint, "ext4", flags, NULL);

    if (rc && errno == EROFS) {
        NGLOG_ERROR("%s appears to be a read only filesystem - retrying mount RO", fsPath);
        flags |= MS_RDONLY;
        rc = mount(fsPath, mountPoint, "ext4", flags, NULL);
    }

    return rc;
}

int Ext4::resize(const char *fspath, unsigned int numSectors) {
    const char *args[4];
    char* size_str;
    int rc;
    int status;

    args[0] = "-f";
    args[1] = fspath;
    args[2] = NULL;
    if (asprintf(&size_str, "%ds", numSectors) < 0)
    {
      NGLOG_ERROR("Filesystem (ext4) resize failed to set size");
      return -1;
    }
    args[3] = size_str;
    rc = execvp(RESIZE2FS_PATH,(char **)args);
    free(size_str);
    if (rc != 0) {
        NGLOG_ERROR("Filesystem (ext4) resize failed due to logwrap error");
        errno = EIO;
        return -1;
    }

    if (!WIFEXITED(status)) {
        NGLOG_ERROR("Filesystem (ext4) resize did not exit properly");
        errno = EIO;
        return -1;
    }

    status = WEXITSTATUS(status);

    if (status == 0) {
        NGLOG_INFO("Filesystem (ext4) resized OK");
        return 0;
    } else {
        NGLOG_ERROR("Resize (ext4) failed (unknown exit code %d)", status);
        errno = EIO;
        return -1;
    }
    return 0;
}

int Ext4::format(const char *fsPath, unsigned int numSectors, const char *mountpoint) {
    int fd;
    const char *args[7];
    int rc;
    int status;

    args[0] = "-J";
    args[1] = "-a";
    args[2] = mountpoint;
    if (numSectors) {
        char tmp[32];
        snprintf(tmp, sizeof(tmp), "%u", numSectors * 512);
        const char *size = tmp;
        args[3] = "-l";
        args[4] = size;
        args[5] = fsPath;
        args[6] = NULL;
        rc = execvp(MKEXT4FS_PATH, (char **)args);
    } else {
        args[3] = fsPath;
        args[4] = NULL;
        rc = execvp(MKEXT4FS_PATH, (char **)args);
    }
    rc = execvp(MKEXT4FS_PATH, (char **)args);
    if (rc != 0) {
        NGLOG_ERROR("Filesystem (ext4) format failed due to logwrap error");
        errno = EIO;
        return -1;
    }

    if (!WIFEXITED(status)) {
        NGLOG_ERROR("Filesystem (ext4) format did not exit properly");
        errno = EIO;
        return -1;
    }

    status = WEXITSTATUS(status);

    if (status == 0) {
        NGLOG_INFO("Filesystem (ext4) formatted OK");
        return 0;
    } else {
        NGLOG_ERROR("Format (ext4) failed (unknown exit code %d)", status);
        errno = EIO;
        return -1;
    }
    return 0;
}
