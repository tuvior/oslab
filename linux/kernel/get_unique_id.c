#include <linux/linkage.h>
#include <linux/uaccess.h>
#include <linux/errno.h>
#include <linux/spinlock.h>

asmlinkage long sys_get_unique_id(int *uiid)
{
	static int i_uuid = 0;
	static DEFINE_SPINLOCK(lock);
	int id;
	int ret;

	if (!uiid) {
		return -EFAULT;
	}

	//take the lock
	spin_lock(&lock);

	id = ++i_uuid;

	//release the lock
	spin_unlock(&lock);

	ret = put_user(id, uiid);

	return ret;
}