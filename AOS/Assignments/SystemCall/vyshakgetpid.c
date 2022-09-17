#include <linux/kernel.h>
#include <linux/syscalls.h>
#include <linux/sched.h>
#include <linux/cred.h>

asmlinkage long vyshakgetpid(void){
	return task_tgid_vnr(current);
}
