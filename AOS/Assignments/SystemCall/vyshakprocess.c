#include <linux/kernel.h>
#include <linux/syscalls.h>
#include <linux/sched.h>
#include <linux/cred.h>
asmlinkage long vyshakprocess(void){

	struct task_struct *parents=current->parent;
	printk ("Current id= %d. \n", current->pid);
	printk ("Parent id= %d. \n", parents->pid);

	return 0;
}
