/*
 * Dummy scheduling class, mapped to range a of 5 levels of SCHED_NORMAL policy
 */

#include "sched.h"

/*
 * Timeslice and age threshold are repsented in jiffies. Default timeslice
 * is 100ms. Both parameters can be tuned from /proc/sys/kernel.
 */

#define DUMMY_TIMESLICE		(100 * HZ / 1000)
#define DUMMY_AGE_THRESHOLD	(3 * DUMMY_TIMESLICE)

unsigned int sysctl_sched_dummy_timeslice = DUMMY_TIMESLICE;
static inline unsigned int get_timeslice(void)
{
	return sysctl_sched_dummy_timeslice;
}

unsigned int sysctl_sched_dummy_age_threshold = DUMMY_AGE_THRESHOLD;
static inline unsigned int get_age_threshold(void)
{
	return sysctl_sched_dummy_age_threshold;
}

/*
 * Init
 */

void init_dummy_rq(struct dummy_rq *dummy_rq, struct rq *rq)
{
	int i;
	for (i = 0; i < 5; i++) {
		INIT_LIST_HEAD(&dummy_rq->queue[i]);
	}
}

/*
 * Helper functions
 */

static inline struct task_struct *dummy_task_of(struct sched_dummy_entity *dummy_se)
{
	return container_of(dummy_se, struct task_struct, dummy_se);
}

static inline void _enqueue_task_dummy(struct rq *rq, struct task_struct *p)
{
	int n_prio = p->prio - 120;
	struct sched_dummy_entity *dummy_se = &p->dummy_se;
	struct list_head *queue;

	// set times that will be handled in the ticks
	dummy_se->timeslice = get_timeslice();
	dummy_se->age = 0;
	
	queue = &rq->dummy.queue[n_prio - 11];

	// add to queue relative to niceness value
	list_add_tail(&dummy_se->run_list, queue);
}

static inline void _dequeue_task_dummy(struct task_struct *p)
{
	struct sched_dummy_entity *dummy_se = &p->dummy_se;
	list_del_init(&dummy_se->run_list);
}

/*
 * Scheduling class functions to implement
 */

static void enqueue_task_dummy(struct rq *rq, struct task_struct *p, int flags)
{
	_enqueue_task_dummy(rq, p);
	add_nr_running(rq,1);
}

static void dequeue_task_dummy(struct rq *rq, struct task_struct *p, int flags)
{
	_dequeue_task_dummy(p);
	sub_nr_running(rq,1);
}

static void yield_task_dummy(struct rq *rq)
{
	struct task_struct *curr = rq->curr;
	// set priority back to original and put at the end of queue
	curr->prio = curr->static_prio;
	_dequeue_task_dummy(curr);
	_enqueue_task_dummy(rq, curr);
}

static void check_preempt_curr_dummy(struct rq *rq, struct task_struct *p, int flags)
{
	struct task_struct *curr = rq->curr;
	int prio = p->prio;

	if (curr->prio > prio) {
		_dequeue_task_dummy(curr);
		_enqueue_task_dummy(rq, curr);
		resched_curr(rq);
	}
}

static struct task_struct *pick_next_task_dummy(struct rq *rq, struct task_struct* prev)
{
	struct dummy_rq *dummy_rq = &rq->dummy;
	struct sched_dummy_entity *next;
	int i;

	// try to pick from least nice to nicest
	for (i = 0; i < 5; i++) {
		if (!list_empty(&dummy_rq->queue[i])) {
			next = list_first_entry(&dummy_rq->queue[i], struct sched_dummy_entity, run_list);
			put_prev_task(rq, prev);
			return dummy_task_of(next);
		}
	}

	return NULL;
}

static void put_prev_task_dummy(struct rq *rq, struct task_struct *prev)
{
	// restore priority;
	prev->prio = prev->static_prio;
}

static void set_curr_task_dummy(struct rq *rq)
{
}

static void task_tick_dummy(struct rq *rq, struct task_struct *curr, int queued)
{
	struct sched_dummy_entity *dummy_se, *l_de;
	struct task_struct *task;
	struct dummy_rq * dummy_rq = &rq->dummy;
 	unsigned int age_threshold = get_age_threshold();
 	int i;

 	
 	// age tasks with prio 12 - 15
 	for (i = 1; i < 5; i++) {
 		if (list_empty(&dummy_rq->queue[i])) {
 			continue;
 		}

 		list_for_each_entry_safe(dummy_se, l_de, &dummy_rq->queue[i], run_list) {
 			task = dummy_task_of(dummy_se);
 			if (task != curr && ++dummy_se->age == age_threshold) {
 				task->prio--;
				_dequeue_task_dummy(task);
				_enqueue_task_dummy(rq, task);
 			}
 		}
 	}

 	dummy_se = &curr->dummy_se;

 	// if current timeslice didn't run out keep going
 	if (--dummy_se->timeslice) {
 		return;
 	}

	// preempt if we are not the only ones on the queue
 	if (dummy_se->run_list.prev != dummy_se->run_list.next) {
 		curr->prio = curr->static_prio;
		_dequeue_task_dummy(curr);
		_enqueue_task_dummy(rq, curr);
 		resched_curr(rq);
 	} else {
 		// another round	
 		dummy_se->timeslice = get_timeslice();
 	}
}

static void switched_from_dummy(struct rq *rq, struct task_struct *p)
{
}

static void switched_to_dummy(struct rq *rq, struct task_struct *p)
{
}

static void prio_changed_dummy(struct rq*rq, struct task_struct *p, int oldprio)
{
	// requeue task in the appropriate queue
	_dequeue_task_dummy(p);
	_enqueue_task_dummy(rq, p);
}

static unsigned int get_rr_interval_dummy(struct rq* rq, struct task_struct *p)
{
	return get_timeslice();
}
#ifdef CONFIG_SMP
/*
 * SMP related functions	
 */

static inline int select_task_rq_dummy(struct task_struct *p, int cpu, int sd_flags, int wake_flags)
{
	int new_cpu = smp_processor_id();
	
	return new_cpu; //set assigned CPU to zero
}


static void set_cpus_allowed_dummy(struct task_struct *p,  const struct cpumask *new_mask)
{
}
#endif
/*
 * Scheduling class
 */
static void update_curr_dummy(struct rq*rq)
{
}
const struct sched_class dummy_sched_class = {
	.next				= &idle_sched_class,
	.enqueue_task		= enqueue_task_dummy,
	.dequeue_task		= dequeue_task_dummy,
	.yield_task			= yield_task_dummy,

	.check_preempt_curr	= check_preempt_curr_dummy,
	
	.pick_next_task		= pick_next_task_dummy,
	.put_prev_task		= put_prev_task_dummy,

#ifdef CONFIG_SMP
	.select_task_rq		= select_task_rq_dummy,
	.set_cpus_allowed	= set_cpus_allowed_dummy,
#endif

	.set_curr_task		= set_curr_task_dummy,
	.task_tick			= task_tick_dummy,

	.switched_from		= switched_from_dummy,
	.switched_to		= switched_to_dummy,
	.prio_changed		= prio_changed_dummy,

	.get_rr_interval	= get_rr_interval_dummy,
	.update_curr		= update_curr_dummy,
};
