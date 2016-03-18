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
	pid_t pid;
	int curr_child = 0;
	int stored_child = 0;
	int err;

	// check for pointer validity
	if (!list && limit > 0) {
		return -EFAULT;
	}

	if (!num_children) {
		return -EFAULT;
	}

	read_lock(&tasklist_lock);

	// traverse children, if still below limit add to buffer
	list_for_each(p, &current->children) {
		if (curr_child < limit) {			
			child = list_entry(p, struct task_struct, sibling);
			pid = (pid_t) child->pid;
			read_unlock(&tasklist_lock);
			err = put_user(pid, list + curr_child);
			if (err != 0) {
				return -EFAULT;
			}
			stored_child++;
			read_lock(&tasklist_lock);
		}
		curr_child++;
	}

	read_unlock(&tasklist_lock);

	// set num_children
	err = put_user(curr_child, num_children);

	// let user know not all pids were stored
	if (err == 0 && stored_child < curr_child && limit != 0) {
		return -ENOBUFS;
	}

	return err;
}