#include <linux/linkage.h>
#include <linux/sched.h>
#include <linux/types.h>
#include <linux/uaccess.h>
#include <linux/errno.h>
#include <linux/list.h>

asmlinkage long sys_get_child_pids(pid_t* list, size_t limit, size_t* num_children)
{
	struct task_struct *child;
	struct list_head *p;
	int curr_child = 0;
	int stored_child = 0;
	pid_t pid;
	int res;

	// check for pointer validity
	if (!list) {
		if (limit > 0) {
			return -EFAULT;
		}
	} else if (!access_ok(VERIFY_READ, list, sizeof(pid_t) * limit)) {
		return -EFAULT;
	}

	if (!access_ok(VERIFY_READ, num_children, sizeof(size_t))) {
		return -EFAULT;
	}

	read_lock(&tasklist_lock);

	// traverse children, if still below limit add to buffer
	list_for_each(p, &current->children) {
		if (curr_child < limit) {
			child = list_entry(p, struct task_struct, sibling);
			pid = (pid_t) child->pid;
			res = put_user(pid, list + curr_child);
			if (res != 0) {
				read_unlock(&tasklist_lock);
				return -EFAULT;
			}
			stored_child++;
		}
		curr_child++;
	}

	read_unlock(&tasklist_lock);

	// set num_children
	res = put_user(curr_child, num_children);

	// let user know not all pids were stored
	if (res == 0 && stored_child < curr_child && limit != 0) {
		return -ENOBUFS;
	}

	return res;
}