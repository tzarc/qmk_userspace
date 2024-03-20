// Copyright 2022-2024 Nick Brassel (@tzarc)
// SPDX-License-Identifier: GPL-2.0-or-later
#include <string.h>
#include "filesystem.h"
#include "lfs.h"

// Can't use fd of 0, or 1/2/3 (stdin/stdout/stderr)
#define FIRST_VALID_FD 4

static fs_fd_t current_fd = FIRST_VALID_FD;

typedef enum fs_lfs_fd_type_t { FD_TYPE_EMPTY, FD_TYPE_DIR, FD_TYPE_FILE } fs_lfs_fd_type_t;

typedef struct fs_lfs_handle_t {
    fs_fd_t          fd;
    fs_lfs_fd_type_t type;
    union {
        struct {
            lfs_dir_t       dir_handle;
            struct lfs_info entry_info;
            fs_dirent_t     dirent; // data replicated from entry_info
        } dir;
        struct {
            lfs_file_t             file_handle;
            struct lfs_file_config cfg;
        } file;
    };
} fs_lfs_handle_t;

extern const struct lfs_config lfs_cfg; // provided by underlying driver

static lfs_t           lfs;
static fs_lfs_handle_t fs_handles[MAX_NUM_OPEN_FDS];

#define FIND_FD_GET_HANDLE(search_fd, fd_type, block)                           \
    do {                                                                        \
        for (int __find_idx = 0; __find_idx < MAX_NUM_OPEN_FDS; ++__find_idx) { \
            fs_lfs_handle_t *handle = &fs_handles[__find_idx];                  \
            if (handle->fd == (search_fd) && handle->type == (fd_type)) {       \
                {                                                               \
                    block                                                       \
                }                                                               \
            }                                                                   \
        }                                                                       \
    } while (0)

static inline bool fd_can_be_used(fs_fd_t fd) {
    if (fd < FIRST_VALID_FD) {
        return false;
    }
    for (int i = 0; i < MAX_NUM_OPEN_FDS; ++i) {
        if (fs_handles[i].fd == fd) {
            return false;
        }
    }
    return true;
}

static inline fs_fd_t allocate_fd(void) {
    fs_fd_t first = ++current_fd;
    while (!fd_can_be_used(current_fd)) {
        ++current_fd;
        if (current_fd == first) {
            // If we've looped back around to the first, everything is already allocated (yikes!). Need to exit with a failure.
            return INVALID_FILESYSTEM_FD;
        }
    }
    return current_fd;
}

bool fs_lock(void) {
    // Placeholder for later. Must be recursive!
    return true;
}

bool fs_unlock(void) {
    // Placeholder for later. Must be recursive!
    return true;
}

static void fs_unlock_helper(bool *locked) {
    if (*locked) {
        fs_unlock();
    }
}

#define FS_AUTO_LOCK_UNLOCK(ret_on_fail)                                       \
    bool __fs_lock __attribute__((__cleanup__(fs_unlock_helper))) = fs_lock(); \
    do {                                                                       \
        if (!__fs_lock) {                                                      \
            return ret_on_fail;                                                \
        }                                                                      \
    } while (0)

#define FS_SKIP_AUTO_UNLOCK() \
    do {                      \
        __fs_lock = false;    \
    } while (0)

bool fs_init(void) {
    FS_AUTO_LOCK_UNLOCK(false);
    memset(fs_handles, 0, sizeof(fs_handles));

    extern bool fs_device_init(void);
    return fs_device_init() && fs_mount();
}

static int mount_count = 0;

bool is_mounted(void) {
    FS_AUTO_LOCK_UNLOCK(false);
    return mount_count > 0;
}

bool fs_mount(void) {
    FS_AUTO_LOCK_UNLOCK(false);
    if (!is_mounted()) {
        int err = lfs_mount(&lfs, &lfs_cfg);

        // reformat if we can't mount the filesystem
        // this should only happen on the first boot
        if (err < 0) {
            err = lfs_format(&lfs, &lfs_cfg);
            if (err < 0) {
                return false;
            }
            err = lfs_mount(&lfs, &lfs_cfg);
            if (err < 0) {
                return false;
            }
        }
    }
    ++mount_count;
    fs_dprintf("mount_count=%d (mount)\n", mount_count);
    return true;
}

void fs_unmount(void) {
    FS_AUTO_LOCK_UNLOCK();
    if (mount_count > 0) {
        --mount_count;
        if (mount_count == 0) {
            lfs_unmount(&lfs);
        }
    }
    fs_dprintf("mount_count=%d (unmount)\n", mount_count);
}

static void fs_unmount_helper(bool *mounted) {
    if (*mounted) {
        fs_unmount();
    }
}

#define FS_AUTO_MOUNT_UNMOUNT(ret_on_fail)                                        \
    bool __fs_mount __attribute__((__cleanup__(fs_unmount_helper))) = fs_mount(); \
    do {                                                                          \
        if (!__fs_mount) {                                                        \
            return ret_on_fail;                                                   \
        }                                                                         \
    } while (0)

#define FS_SKIP_AUTO_UNMOUNT() \
    do {                       \
        __fs_mount = false;    \
    } while (0)

fs_fd_t fs_opendir(const char *path) {
    FS_AUTO_LOCK_UNLOCK(INVALID_FILESYSTEM_FD);
    FIND_FD_GET_HANDLE(INVALID_FILESYSTEM_FD, FD_TYPE_EMPTY, {
        FS_AUTO_MOUNT_UNMOUNT(INVALID_FILESYSTEM_FD);

        if (lfs_dir_open(&lfs, &handle->dir.dir_handle, path) < 0) {
            return INVALID_FILESYSTEM_FD;
        }

        fs_fd_t fd   = allocate_fd();
        handle->fd   = fd;
        handle->type = FD_TYPE_DIR;

        // Intentionally don't unmount while we have an open directory
        FS_SKIP_AUTO_UNMOUNT();
        return fd;
    });
    return INVALID_FILESYSTEM_FD;
}

fs_dirent_t *fs_readdir(fs_fd_t fd) {
    FS_AUTO_LOCK_UNLOCK(NULL);
    FIND_FD_GET_HANDLE(fd, FD_TYPE_DIR, {
        if (!is_mounted()) {
            return NULL;
        }

        int err = lfs_dir_read(&lfs, &handle->dir.dir_handle, &handle->dir.entry_info);
        if (err < 0) {
            return NULL;
        }

        handle->dir.dirent.is_dir = handle->dir.entry_info.type == LFS_TYPE_DIR;
        handle->dir.dirent.size   = handle->dir.entry_info.size;
        handle->dir.dirent.name   = handle->dir.entry_info.name;
        return &handle->dir.dirent;
    });
    return NULL;
}
void fs_closedir(fs_fd_t fd) {
    FS_AUTO_LOCK_UNLOCK();
    FIND_FD_GET_HANDLE(fd, FD_TYPE_DIR, {
        lfs_dir_close(&lfs, &handle->dir.dir_handle);

        handle->fd   = INVALID_FILESYSTEM_FD;
        handle->type = FD_TYPE_EMPTY;

        fs_unmount(); // we can unmount here, mirrors the open in fs_opendir()
    });
}

bool fs_mkdir(const char *path) {
    FS_AUTO_LOCK_UNLOCK(false);
    FS_AUTO_MOUNT_UNMOUNT(false);

    int err = lfs_mkdir(&lfs, path);
    if (err != LFS_ERR_EXIST && err < 0) {
        return false;
    }

    return true;
}

bool fs_rmdir(const char *path, bool recursive) {
    FS_AUTO_LOCK_UNLOCK(false);
    FS_AUTO_MOUNT_UNMOUNT(false);

    if (recursive) {
        // TBD
    }

    int err = lfs_remove(&lfs, path);
    if (err < 0) {
        return false;
    }

    return true;
}

fs_fd_t fs_open(const char *filename, const char *mode) {
    FS_AUTO_LOCK_UNLOCK(INVALID_FILESYSTEM_FD);
    FIND_FD_GET_HANDLE(INVALID_FILESYSTEM_FD, FD_TYPE_EMPTY, {
        FS_AUTO_MOUNT_UNMOUNT(INVALID_FILESYSTEM_FD);

        int flags = LFS_O_CREAT;
        while (*mode) {
            switch (*mode) {
                case 'r':
                    flags |= LFS_O_RDONLY;
                    break;
                case 'w':
                    flags |= LFS_O_WRONLY;
                    break;
                case 't':
                    flags |= LFS_O_TRUNC;
                    break;
            }
            ++mode;
        }

        extern void *fs_device_filebuf(int file_idx);
        memset(&handle->file.cfg, 0, sizeof(handle->file.cfg));
        handle->file.cfg.buffer = fs_device_filebuf(__find_idx);

        int err = lfs_file_opencfg(&lfs, &handle->file.file_handle, filename, flags, &handle->file.cfg);
        if (err < 0) {
            return INVALID_FILESYSTEM_FD;
        }

        fs_fd_t fd   = allocate_fd();
        handle->fd   = fd;
        handle->type = FD_TYPE_FILE;

        // Intentionally don't unmount while we have an open file
        FS_SKIP_AUTO_UNMOUNT();
        return fd;
    });
    return INVALID_FILESYSTEM_FD;
}

void fs_close(fs_fd_t fd) {
    FS_AUTO_LOCK_UNLOCK();
    FIND_FD_GET_HANDLE(fd, FD_TYPE_FILE, {
        int ret = lfs_file_close(&lfs, &handle->file.file_handle);
        fs_dprintf("Closing fd %d, ret=%d\n", (int)fd, ret);

        handle->fd   = INVALID_FILESYSTEM_FD;
        handle->type = FD_TYPE_EMPTY;

        fs_unmount(); // we can unmount here, mirrors the open in fs_open()
        return;
    });
    fs_dprintf("No fd %d found\n", (int)fd);
}

bool fs_delete(const char *filename) {
    FS_AUTO_LOCK_UNLOCK(false);
    FS_AUTO_MOUNT_UNMOUNT(false);
    return lfs_remove(&lfs, filename) >= 0;
}

fs_offset_t fs_seek(fs_fd_t fd, fs_offset_t offset, fs_whence_t whence) {
    FS_AUTO_LOCK_UNLOCK(-1);
    FIND_FD_GET_HANDLE(fd, FD_TYPE_FILE, {
        if (!is_mounted()) {
            return -1;
        }

        int lfs_whence = 0;
        switch (whence) {
            case FS_SEEK_SET:
                lfs_whence = LFS_SEEK_SET;
                break;
            case FS_SEEK_CUR:
                lfs_whence = LFS_SEEK_CUR;
                break;
            case FS_SEEK_END:
                lfs_whence = LFS_SEEK_END;
                break;
        }

        int err = lfs_file_seek(&lfs, &handle->file.file_handle, (lfs_soff_t)offset, lfs_whence);
        if (err < 0) {
            return -1;
        }

        fs_offset_t offset = (fs_offset_t)lfs_file_tell(&lfs, &handle->file.file_handle);
        return (offset < 0) ? -1 : offset;
    });
    return -1;
}

fs_offset_t fs_tell(fs_fd_t fd) {
    FS_AUTO_LOCK_UNLOCK(-1);
    FIND_FD_GET_HANDLE(fd, FD_TYPE_FILE, {
        if (!is_mounted()) {
            return -1;
        }

        fs_offset_t offset = (fs_offset_t)lfs_file_tell(&lfs, &handle->file.file_handle);
        return (offset < 0) ? -1 : offset;
    });
    return -1;
}

fs_size_t fs_read(fs_fd_t fd, void *buffer, fs_size_t length) {
    FS_AUTO_LOCK_UNLOCK(-1);
    FIND_FD_GET_HANDLE(fd, FD_TYPE_FILE, {
        if (!is_mounted()) {
            return -1;
        }

        fs_size_t ret = (fs_size_t)lfs_file_read(&lfs, &handle->file.file_handle, buffer, (lfs_size_t)length);
        return (ret < 0) ? -1 : ret;
    });
    return -1;
}

fs_size_t fs_write(fs_fd_t fd, const void *buffer, fs_size_t length) {
    FS_AUTO_LOCK_UNLOCK(-1);
    FIND_FD_GET_HANDLE(fd, FD_TYPE_FILE, {
        if (!is_mounted()) {
            return -1;
        }

        fs_size_t ret = (fs_size_t)lfs_file_write(&lfs, &handle->file.file_handle, buffer, (lfs_size_t)length);
        return (ret < 0) ? -1 : ret;
    });
    return -1;
}

bool fs_is_eof(fs_fd_t fd) {
    FS_AUTO_LOCK_UNLOCK(true);
    FIND_FD_GET_HANDLE(fd, FD_TYPE_FILE, {
        if (!is_mounted()) {
            return true;
        }

        lfs_soff_t orig_offset = lfs_file_tell(&lfs, &handle->file.file_handle);
        if (orig_offset < 0) {
            return true;
        }

        lfs_soff_t end_offset = lfs_file_seek(&lfs, &handle->file.file_handle, 0, LFS_SEEK_END);
        if (end_offset < 0) {
            return true;
        }

        if (orig_offset != end_offset) {
            lfs_file_seek(&lfs, &handle->file.file_handle, orig_offset, LFS_SEEK_SET);
            return true;
        }

        return false;
    });
    return true;
}
