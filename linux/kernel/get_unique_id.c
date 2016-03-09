#include <linux/linkage.h>
#include <linux/uaccess.h>
#include <linux/errno.h>
#include <linux/spinlock.h>

asmlinkage long sys_get_unique_id(int *uiid)
{
    static int i_uuid = 0;
    static spinlock_t lock;

    if (!uiid) {
        return -EFAULT;
    }

    spin_lock_init(&lock);

    //take the lock and disable interrupts
    spin_lock_irq(&lock);

    i_uuid++;

    //release the lock and enable interrupts
    spin_unlock_irq(&lock);
    
	return put_user(i_uuid, uiid);
}