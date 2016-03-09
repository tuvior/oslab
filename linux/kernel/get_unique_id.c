#include <linux/linkage.h>
#include <linux/uaccess.h>
#include <linux/errno.h>
#include <linux/spinlock.h>

asmlinkage long sys_get_unique_id(int *uiid)
{
    static int i_uuid = 0;
    static spinlock_t lock = SPIN_LOCK_UNLOCKED;
    int ret;

    if (!uiid) {
        return -EFAULT;
    }

    //take the lock and disable interrupts
    spin_lock_irq(&lock);

    i_uuid++;
    ret = put_user(i_uuid, uiid);

    //release the lock and enable interrupts
    spin_unlock_irq(&lock);
    
	return ret;
}