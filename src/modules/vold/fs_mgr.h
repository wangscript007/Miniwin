#ifndef __FS_MGR_H__
#define __FS_MGR_H__

struct fstab {
    int num_entries;
    struct fstab_rec *recs;
    char *fstab_filename;
};

struct fstab_rec {
    char *blk_device;
    char *mount_point;
    char *fs_type;
    unsigned long flags;
    char *fs_options;
    int fs_mgr_flags;
    char *key_loc;
    char *verity_loc;
    long long length;
    char *label;
    int partnum;
    int swap_prio;
    unsigned int zram_size;
};

struct fstab *fs_mgr_read_fstab(const char *fstab_path);
void fs_mgr_free_fstab(struct fstab *fstab);

#define FS_MGR_MNTALL_DEV_NEEDS_RECOVERY 3
#define FS_MGR_MNTALL_DEV_NEEDS_ENCRYPTION 2
#define FS_MGR_MNTALL_DEV_MIGHT_BE_ENCRYPTED 1
#define FS_MGR_MNTALL_DEV_NOT_ENCRYPTED 0
int fs_mgr_mount_all(struct fstab *fstab);

#define FS_MGR_DOMNT_FAILED -1
#define FS_MGR_DOMNT_BUSY -2
int fs_mgr_do_mount(struct fstab *fstab, char *n_name, char *n_blk_device, char *tmp_mount_point);
int fs_mgr_do_tmpfs_mount(char *n_name);
int fs_mgr_unmount_all(struct fstab *fstab);
int fs_mgr_get_crypt_info(struct fstab *fstab, char *key_loc, char *real_blk_device, int size);
int fs_mgr_add_entry(struct fstab *fstab,const char *mount_point, const char *fs_type, const char *blk_device);
struct fstab_rec *fs_mgr_get_entry_for_mount_point(struct fstab *fstab, const char *path);
int fs_mgr_is_voldmanaged(struct fstab_rec *fstab);
int fs_mgr_is_nonremovable(struct fstab_rec *fstab);
int fs_mgr_is_encryptable(struct fstab_rec *fstab);
int fs_mgr_is_noemulatedsd(struct fstab_rec *fstab);
int fs_mgr_swapon_all(struct fstab *fstab);
#endif

