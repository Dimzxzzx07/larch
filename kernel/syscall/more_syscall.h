#ifndef MORE_SYSCALLS_H
#define MORE_SYSCALLS_H

#define SYS_FORK 8
#define SYS_EXEC 9
#define SYS_GETPID 10
#define SYS_SLEEP 11
#define SYS_WAITPID 12
#define SYS_GETTICKS 13

uint32_t sys_fork(void);
uint32_t sys_exec(const char* path, char* const argv[]);
uint32_t sys_getpid(void);
void sys_sleep(uint32_t ticks);
uint32_t sys_waitpid(uint32_t pid);
uint32_t sys_getticks(void);

#endif