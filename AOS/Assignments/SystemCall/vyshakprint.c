#include <linux/kernel.h>
#include <linux/syscalls.h>
#include <linux/sched.h>
#include <linux/cred.h>
SYSCALL_DEFINE1(vyshakprint, char __user *, data){
	char buffer[1500];
	if(copy_from_user(buffer,data, sizeof(buffer))){
		return -EFAULT;
	}
	printk ("Recieved input = %s. \n",buffer);
	return 0;
}
