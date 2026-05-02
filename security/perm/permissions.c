#include <stdint.h>
#include <stddef.h>

#define PERM_READ   (1 << 0)
#define PERM_WRITE  (1 << 1)
#define PERM_EXEC   (1 << 2)

typedef struct {
    uint32_t uid;
    uint32_t gid;
    uint16_t mode;
} file_perms_t;

typedef struct {
    uint32_t uid;
    uint32_t gid;
    uint32_t groups[32];
    uint8_t group_count;
} user_cred_t;

int check_permission(file_perms_t* file, user_cred_t* user, int access_type) {
    if (user->uid == 0) {
        return 1;
    }
    
    uint16_t perm = 0;
    
    if (user->uid == file->uid) {
        perm = (file->mode >> 6) & 0x7;
    } else {
        int in_group = 0;
        for (int i = 0; i < user->group_count; i++) {
            if (user->groups[i] == file->gid) {
                in_group = 1;
                break;
            }
        }
        
        if (in_group) {
            perm = (file->mode >> 3) & 0x7;
        } else {
            perm = file->mode & 0x7;
        }
    }
    
    if (access_type == PERM_READ && (perm & 4)) return 1;
    if (access_type == PERM_WRITE && (perm & 2)) return 1;
    if (access_type == PERM_EXEC && (perm & 1)) return 1;
    
    return 0;
}

void set_permission(file_perms_t* file, uint32_t uid, uint32_t gid, uint16_t mode) {
    file->uid = uid;
    file->gid = gid;
    file->mode = mode;
}