/*

$Header: /sources/tsp/tsp/src/core/driver/tsp_data_receiver.h,v 1.10 2006/03/31 12:55:19 erk Exp $

-----------------------------------------------------------------------

TSP Library - core components for a generic Transport Sampling Protocol.

Copyright (c) 2002 Yves DUFRENNE, Stephane GALLES, Eric NOULARD and Robert PAGNOT 

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
Component : Consumer

-----------------------------------------------------------------------

Purpose   : Interface for the functions used to receive and decode the data
stream  for the requested  symbols
-----------------------------------------------------------------------
 */

#ifndef _TSP_DATA_RECEIVER_H
#define _TSP_DATA_RECEIVER_H

#include "tsp_prjcfg.h"

#include "tsp_sample_ringbuf.h"
#include "tsp_group.h"

typedef  void* TSP_data_receiver_t;



/**
 * Receive the data coming from the provider.
 * @param _receiver the data reciever object handle
 * @param _groups The group object handle
 * @param sample_fifo The ringbuf in which the incoming data must be put
 * @param fifo_full (*fifo_full) is set to TRUE when the ringbuf is full
 */
int TSP_data_receiver_receive(TSP_data_receiver_t _receiver,
                              TSP_groups_t _groups,
                              TSP_sample_ringbuf_t* sample_fifo,
			      int* fifo_full) ;

/**
 * Creation of the data receiver data object.
 * @param data_address The data address string (ex : 'hostname:port' for TCP/IP based protocol)
 * @param callback The callback function that must be called with the incoming data (set to 0 if not used)
 * @param user_data pointer on any custom user data ( set to 0 if not used)
 */
TSP_data_receiver_t TSP_data_receiver_create(const char* data_address, TSP_sample_callback_t callback, void* user_data);

/**
 * Prepare the receiver to stop.
 * This function set the receiver in a state, where the it does not
 * complain if the data stream is broken by the provider stream sender.
 * @param _receiver handle
 */
void TSP_data_receiver_prepare_stop(TSP_data_receiver_t _receiver);

/**
 * Stop receiving data.
 * @param _receiver handle
 */
void TSP_data_receiver_stop(TSP_data_receiver_t _receiver);

/**
 * Destroy a data receiver object.
 * @param _receiver handle that must be destroyed
 */
void TSP_data_receiver_destroy(TSP_data_receiver_t _receiver);


/**
 * Set the callback function that must be called for each
 * incoming sample
 * @param _receiver The receiver object handle
 * @param callback The callback function address
 * @return TRUE or FALSE. TRUE = OK
 */
int TSP_data_receiver_set_read_callback(TSP_data_receiver_t _receiver,
					TSP_sample_callback_t callback);


#endif /* _TSP_DATA_RECEIVER_H */
