#include <stdint.h>
#include <stddef.h>

typedef struct file_operations {
    int (*open)(const char* path, int flags);
    int (*read)(void* buf, uint32_t size, uint32_t offset);
    int (*write)(const void* buf, uint32_t size, uint32_t offset);
    int (*close)(void);
    int (*readdir)(void* buf, uint32_t index);
} file_ops_t;

typedef struct inode {
    uint32_t ino;
    uint32_t mode;
    uint32_t size;
    void* private_data;
    file_ops_t* ops;
} inode_t;

typedef struct filesystem {
    const char* name;
    int (*mount)(const char* source, const char* target);
    int (*unmount)(const char* target);
    inode_t* (*get_inode)(const char* path);
} filesystem_t;

#define MAX_MOUNTS 16
#define MAX_FDS 256

typedef struct mount_point {
    const char* path;
    filesystem_t* fs;
    void* data;
} mount_point_t;

static mount_point_t mounts[MAX_MOUNTS];
static int mount_count = 0;
static inode_t* fds[MAX_FDS];
static int fd_count = 0;

int vfs_mount(const char* source, const char* target, filesystem_t* fs) {
    if (mount_count >= MAX_MOUNTS) return -1;
    mounts[mount_count].path = target;
    mounts[mount_count].fs = fs;
    mounts[mount_count].data = 0;
    if (fs->mount) fs->mount(source, target);
    mount_count++;
    return 0;
}

inode_t* vfs_resolve(const char* path) {
    for (int i = mount_count - 1; i >= 0; i--) {
        const char* mpath = mounts[i].path;
        int len = 0;
        while (mpath[len]) len++;
        
        int match = 1;
        for (int j = 0; j < len; j++) {
            if (path[j] != mpath[j]) {
                match = 0;
                break;
            }
        }
        
        if (match) {
            if (mounts[i].fs->get_inode) {
                return mounts[i].fs->get_inode(path + len);
            }
        }
    }
    return 0;
}

int vfs_open(const char* path, int flags) {
    inode_t* inode = vfs_resolve(path);
    if (!inode) return -1;
    if (fd_count >= MAX_FDS) return -1;
    fds[fd_count] = inode;
    if (inode->ops && inode->ops->open) {
        inode->ops->open(path, flags);
    }
    return fd_count++;
}

int vfs_read(int fd, void* buf, uint32_t size) {
    if (fd < 0 || fd >= fd_count || !fds[fd]) return -1;
    if (fds[fd]->ops && fds[fd]->ops->read) {
        return fds[fd]->ops->read(buf, size, 0);
    }
    return -1;
}