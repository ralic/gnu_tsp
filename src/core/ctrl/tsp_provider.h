/*

$Id: tsp_provider.h,v 1.24 2008/03/24 23:56:20 deweerdt Exp $

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

Project    : TSP
Maintainer : tsp@astrium.eads.net
Component  : Provider

-----------------------------------------------------------------------

Purpose   : Main interface for the producer module

-----------------------------------------------------------------------
 */

#ifndef _TSP_PROVIDER_H
#define _TSP_PROVIDER_H

#include <tsp_prjcfg.h>
#include <tsp_abs_types.h>
#include <tsp_datastruct.h>
#include <tsp_glu.h>

/**
 * @defgroup TSP_ProviderLib Provider Core Library
 * @ingroup  TSP_CoreLib
 * The Provider module is the set of all 
 * provider library interface.
 * @{
 */

BEGIN_C_DECLS

/**
 * Initialize TSP provider library.
 * One should call this before any other TSP provider lib call.
 * @param[in,out] theGLU the GLU to be registered into the TSP Provider Library.
 * @param[in,out] argc   the number of argument of the main
 * @param[in,out] argv   array of argument of size argc.
 */
int32_t TSP_provider_private_init(GLU_handle_t* theGLU, int* argc, char** argv[]);

int32_t TSP_provider_private_run();

const char* TSP_provider_get_name();


void TSP_provider_request_open(const TSP_request_open_t* req_open,
		      TSP_answer_open_t* ans_open);
		      
void TSP_provider_request_close(const TSP_request_close_t* req_close);

void  TSP_provider_request_information(TSP_request_information_t* req_info, 
				       TSP_answer_sample_t* ans_sample);

void  TSP_provider_request_filtered_information(TSP_request_information_t* req_info, 
						int filter_kind, char* filter_string,
						TSP_answer_sample_t* ans_sample);

void  TSP_provider_request_sample(TSP_request_sample_t* req_sample, 
				  TSP_answer_sample_t* ans_sample);

void  TSP_provider_request_sample_init(TSP_request_sample_init_t* req_sample_init, 
				       TSP_answer_sample_init_t* ans_sample);

void  TSP_provider_request_sample_destroy(TSP_request_sample_destroy_t* req_sample_destroy, 
					  TSP_answer_sample_destroy_t* ans_sample);

int TSP_provider_is_initialized();

int TSP_provider_get_server_number();
int TSP_provider_get_server_base_number( );

int32_t TSP_provider_request_async_sample_write(TSP_async_sample_t* async_sample_write);

int32_t TSP_provider_request_async_sample_read(TSP_async_sample_t* async_sample_read);

void  TSP_provider_request_extended_information(TSP_request_extended_information_t* req_extinfo, 
						TSP_answer_extended_information_t* ans_extinfo);

END_C_DECLS
/** @} end group Provider */ 

#endif /* _TSP_PROVIDER_H */
