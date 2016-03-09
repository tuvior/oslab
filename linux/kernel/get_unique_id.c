#include <linux/time.h>
#include <linux/linkage.h>
#include <linux/uaccess.h>
#include <linux/errno.h>
#include <linux/spinlock.h>

asmlinkage long sys_get_unique_id(int *uiid)
{
    if (!uiid) {
        return -EFAULT;
    }

    struct timespec t_call;
    int gen_id;
    spinlock_t lock;
    spin_lock_init(&lock);

    //take the lock and disable interrupts
    spin_lock_irq(&lock);

    //get current timens
    getnstimeofday(&t_call);

    gen_id = (int) t_call.tv_nsec/1000;

    //release the lock and enable interrupts
    spin_unlock_irq(&lock
    
	return put_user(gen_id, uiid);
}