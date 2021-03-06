/*
 * Five Event interface
 *
 * Copyright (C) 2018 Samsung Electronics, Inc.
 * Ivan Vorobiov, <i.vorobiov@samsung.com>
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef _FIVE_HOOKS_H
#define _FIVE_HOOKS_H

#include <linux/file.h>
#include <linux/rculist.h>
#include <linux/sched.h>

void five_hook_file_processed(struct task_struct *task,
				struct file *file, void *xattr,
				size_t xattr_size, int result);

void five_hook_task_forked(struct task_struct *parent,
				struct task_struct *child);

void five_hook_file_skipped(struct task_struct *task, struct file *file);

void five_hook_file_signed(struct task_struct *task,
				struct file *file, void *xattr,
				size_t xattr_size, int result);

union five_list_options {
	void (*file_processed)(struct task_struct *task,
				struct file *file,
				void *xattr,
				size_t xattr_size,
				int result);

	void (*file_skipped)(struct task_struct *task,
				struct file *file);

	void (*task_forked)(struct task_struct *parent,
				struct task_struct *child);

	void (*file_signed)(struct task_struct *task,
				struct file *file, void *xattr,
				size_t xattr_size, int result);
};

struct five_hook_heads {
	struct list_head file_processed;
	struct list_head file_skipped;
	struct list_head file_signed;
	struct list_head task_forked;
};

/*
 * FIVE module hook list structure.
 * For use with generic list macros for common operations.
 */
struct five_hook_list {
	struct list_head list;
	struct list_head *head;
	union five_list_options hook;
};

/*
 * Initializing a five_hook_list structure takes
 * up a lot of space in a source file. This macro takes
 * care of the common case and reduces the amount of
 * text involved.
 */
#define FIVE_HOOK_INIT(HEAD, HOOK) \
	{ .head = &five_hook_heads.HEAD, .hook = { .HEAD = HOOK } }

extern struct five_hook_heads five_hook_heads;

static inline void five_add_hooks(struct five_hook_list *hooks,
				      int count)
{
	int i;

	for (i = 0; i < count; i++)
		list_add_tail_rcu(&hooks[i].list, hooks[i].head);
}

int five_hook_wq_init(void);

#endif /* _FIVE_HOOKS_H */
