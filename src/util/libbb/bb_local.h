/*

$Header: /sources/tsp/tsp/src/util/libbb/bb_local.h,v 1.5 2008/07/23 15:18:05 jaggy Exp $

-----------------------------------------------------------------------

TSP Library - A way to attach local context to a Blackboard.

Copyright (c) 2008 Jerome Arbez-Gindre

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

-----------------------------------------------------------------------

Project   : TSP
Maintainer : tsp@astrium-space.com
Component : BlackBoard

-----------------------------------------------------------------------

Purpose   : BlackBoard Local Data

-----------------------------------------------------------------------
 */

#ifndef _BB_LOCAL_H_
#define _BB_LOCAL_H_

#include "bb_core.h"

struct S_BB_SUBSCRIBE;

/**
 * BlackBoard local data descriptor.
 * Any field declared in S_BB_T is shared between every users which
 * are attach to this BB.
 * This structure contains every field which need toi be loacl to a
 * particular BB user.
 */
typedef struct S_BB_LOCAL {
#ifdef __KERNEL__
	/** The actual pointer to the allocated memory, this is needed
	    as the BB must be aligned on a PAGE_SIZE boundary,
	    kmalloc_ptr is the _real_ start of the allocate memory */
	void *kmalloc_ptr;
	/** in-kernel structure, points to the char device */
	struct bb_device *dev;
#endif /* __KERNEL__ */
	/** single chained list to keep track to message
	    subscribtions */
	struct S_BB_SUBSCRIBE *subscribed;
} S_BB_LOCAL_T;


/**
 * A function to create and initiliaze a local structure
 *
 * @return a valid pointer if the operation succeeded, NULL otherwise.
 */
struct S_BB_LOCAL *bb_local_new(void);

/**
 * A function to delete a local structure
 *
 * @param[in] local a BB local pointer
 */
void bb_local_delete(struct S_BB_LOCAL *local);

/**
 * A function to attach a local structure to a BlackBoard
 * @param[in] bb a BlackBoard pointer
 * @return BB_OK is the operation succeeded, BB_NOK otherwise.
 */
int32_t bb_attach_local(struct S_BB *bb, struct S_BB_LOCAL *local);

/**
 * A function to clear local structure attached to a BlackBoard
 * @param[in] bb a BlackBoard pointer
 * @return BB_OK is the operation succeeded, BB_NOK otherwise.
 */
int32_t bb_detach_local(struct S_BB *bb);

/**
 * A function to get local structure attached to a BlackBoard
 * @param[in] bb a BlackBoard pointer
 * @return BB_OK is the operation succeeded, BB_NOK otherwise.
 */
struct S_BB_LOCAL *bb_get_local(const S_BB_T *bb);

#endif /* _BB_LOCAL_H_ */
