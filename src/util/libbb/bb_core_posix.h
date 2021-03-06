/*

$Header: /sources/tsp/tsp/src/util/libbb/bb_core_posix.h,v 1.1 2008/03/16 20:55:00 deweerdt Exp $

-----------------------------------------------------------------------

TSP Library - core components for a generic Transport Sampling Protocol.

Copyright (c) 2006 Eric NOULARD, Frederik DEWEERDT

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
 */
#ifndef _BB_CORE_POSIX_H_
#define _BB_CORE_POSIX_H_

#include <tsp_abs_types.h>
#include "bb_core.h"

/*
 *	The maximum number of BlackBoard's
 */
#define MAX_NUM_OF_BB	5

struct bb_used_t {
	char name[BB_NAME_MAX_SIZE+1];
	S_BB_T *fd;
};




#endif /* _BB_CORE_POSIX_H_ */
