/*

$Id: tsp_glu.h,v 1.16 2006/10/18 09:58:48 erk Exp $

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
Maintainer: tsp@astrium.eads.net
Component : Provider

-----------------------------------------------------------------------

Purpose   : Interface for the glue server : the data producer

-----------------------------------------------------------------------
 */

#ifndef _TSP_GLU_H
#define _TSP_GLU_H

#include <tsp_prjcfg.h>
#include <tsp_datastruct.h>  /* FIXME : just for TSP_sample_symbol_info_list_t, and shouldn't use generated rpc struct */
#if defined (WIN32)
#include <pthread.h>
#endif
/* forward declare datapool type */
struct TSP_datapool;


/**
 * @defgroup TSP_GLULib GLU Library
 * @ingroup TSP_ProviderLib
 * The GLU is the part of the TSP provider that should be
 * coded each time the underlying 'real' sampled system 
 * (hardware card, simulator, ...) change.
 * The GLU object interface @ref GLU_handle_t should be implemented in order to combine
 * the TSP provider lib in a real TSP provider.
 * @{
 */

/**
 * Asynchronous write "allowed" value.
 */
#define TSP_ASYNC_WRITE_ALLOWED   1
/**
 * Asynchronous write "forbidden" value.
 */
#define TSP_ASYNC_WRITE_FORBIDDEN 0


/** GLU server type */
typedef enum GLU_server_type_t
{
  /** 
   * GLU is active. Means that the data are continuously produced
   * and must be read at the same pace (or faster) by the provider.
   * When GLU is active their shouldn'tr be more that one 
   * GLU instance running by provider.
   * @see GLU_get_instance.
   */
  GLU_SERVER_TYPE_ACTIVE,

  /** 
   * GLU is passive. Means that the data are produced only when the
   * provider ask for them (typically File Based Glu/Provider)
   */
  GLU_SERVER_TYPE_PASSIVE
} GLU_server_type_t;


/**
 * The state of the GLU.
 */
typedef enum GLU_get_state_t
{
  /** new item available */
  GLU_GET_NEW_ITEM,

  /** no item available */
  GLU_GET_NO_ITEM,

  /** end of data stream */
  GLU_GET_EOF,

  /** end of data stream, and GLU list of symbols changed, all
      client will have to ask fo the new symbol list */
  GLU_GET_RECONF,

  /** Data were lost in GLU internals ; 
      The provider might be too slow and/or the GLU too fast, 
      and/or the CPU overloaded : all clients will be notified
      by the provider */
  GLU_GET_DATA_LOST

} GLU_get_state_t;

/** 
 * Item used to transmit a sample data
 * from the GLU to the datapool of the TSP provider lib.
 */
typedef struct glu_item_t
{
  /**
   * The GLU timestamp
   */
  time_stamp_t time;
  /**
   * The PGI of the concerned sample
   */
  int32_t      provider_global_index;
  /**
   * Pointer to the raw sample value.
   */
  void*        raw_value;
  /**
   * The size of the pointer to be copied.
   * The size may vary for different type
   * including array version.
   */
  uint32_t     size;

} glu_item_t;

/*
 * Define the GLU handle data type
 */
struct GLU_handle_t; /* necessary forward declaration */
/** 
 * GLU server name getter.
 * This string will be used by the consumers to get informations
 * about a provider. It must be meaningfull (ex : "Temperatures Sampler" ) 
 */
typedef char*              (* GLU_get_server_name_ft   )(struct GLU_handle_t* cthis);
/**
 * GLU server type getter.
 * Will get the GLU type i.e. ACTIVE or PASSIVE.
 * @return GLU server type.
 */
typedef GLU_server_type_t  (* GLU_get_server_type_ft   )(struct GLU_handle_t* cthis);
/**
 * Default GLU base frequency.
 * @return GLU base frequency (Hz)
 */
typedef double             (* GLU_get_base_frequency_ft)(struct GLU_handle_t* cthis);
/**
 * The maximum number of consumer connection
 * @return maximum number of consumer connection
 */
typedef int32_t             (* GLU_get_nb_max_consumer_ft)(struct GLU_handle_t* cthis);
/**
 * GLU instance creation.
 * This function will be called by the provider for each consumer
 * connection.
 * This function is the place where the GLU must be instanciated.
 * 
 * - When the GLU is type ACTIVE, it must be a singleton and must
 * be instanciated once ( i.e. the first time this function is called).
 * When the instanciation of an ACTIVE GLU succed this function always
 * returns the value GLU_GLOBAL_HANDLE (and for all the subsequent calls
 * that should not create a new GLU, and always should always succed).
 * 
 * - When the GLU is type PASSIVE, for each consumer connection a new
 * GLU must be instanciated ; use the GLU_handle_t type to reference your
 * GLU object and return this handle when the instanciation succed
 * 
 * @param custom_argc Number of consumer initialisation elements
 * (when the consumer do not provide any parameters
 * this parameter is equal to fallback_argc of GLU_init)
 * @param custom_argv Initialisation elements
 * The first usefull parameter is always
 * fallback_argv[1], so as the getopt* functions may be used to
 * parse those parameters (when the consumer do not provide any parameters
 * this parameter is equal to fallback_argv of GLU_init)
 * @param error_info When the function fail the GLU may use this parameter
 * to return an error string that will be transmited to the consumer
 * to get detailed information about the error. 
 * @return The created GLU handle. Returns 0 when the function fail
 */
typedef struct GLU_handle_t*      (* GLU_get_instance_ft      )(struct GLU_handle_t* cthis, int32_t custom_argc, char* custom_argv[], char** error_info);
/**
 * GLU initialization function.
 *
 * The code in this function will be called once,
 * and may do any usefull global check and initialization. 
 * Usually the data stream should not be started in this function
 * as there is no connected consumer when this function is
 * called. To start your data stream, wait for the first GLU_get_instance
 * function that will be called for each consumer connection
 *
 * The parameters for this function come from the parameters
 * that may have been provided thrue the command line that was used
 * to launch the provider. In this function the GLU should not use these
 * parameters, but it should check their value, as these value
 * will be used as fallback parameters for the GLU_get_instance function
 * each time a consumer connect to the provider and does not provide
 * its own parameters
 *
 * the GLU may then for this function return :
 * - FALSE if it does not want to be started ( ex : when the
 * parameters are incorrect, or when there is no parameters
 * at all and this GLU exepects parameters). The provider
 * will stop.
 * - TRUE if everything is OK
 *
 * NOTE1 : Sometimes, the the only way to check the provided
 * parameters is to start the data stream. If so,
 * the data stream must not be started a second time 
 * in the get_instance function.
 * 
 * NOTE2 : This string is provided in this function for fallback
 * test only and should not be memorized by the GLU server
 *
 * NOTE3 : When there is not fallback stream at all, the value
 * of fallback_argv  is 0. It is up to the GLU server
 * to decide if it is acceptable for the provider to be 
 * started with no fallback stream at all (it usually is
 * acceptable)
 *
 * @param this, the GLU handle
 * @param fallback_argc Number of fallback initialisation elements as used in main(argc, argv)
 * @param fallback_argv Fallback initialisation elements as used in main(argc, argv)
 * The first usefull parameter is always
 * fallback_argv[1], so as the getopt* functions may be used to
 * parse those parameters
 * @return Return FALSE if a fatal error occur during initialisation
 * or if there is a probleme with the provided parameters
 */
typedef int32_t                (* GLU_init_ft              )(struct GLU_handle_t* cthis,int32_t fallback_argc, char* fallback_argv[]);
/**
 * Start the loop that will push data to datapool with push_next_item
 * 
 * @return status 
 */
typedef void*              (* GLU_run_ft               )(void* cthis);
/**
 * Start push datapool GLU activity AS A SEPARATE thread.
 * The start function should NOT block, the defaut implementation
 * is to start a new thread running the 'GLU_run_ft' of the GLU. 
 * @return 0 on success and other value on error.
 */
typedef int32_t                (* GLU_start_ft             )(struct GLU_handle_t* cthis);
/** 
 * GLU_get_provider_global_indexes provider global indexes corresponding
 * to given symbol list. Used to validate client provided symbol list
 * @param[in] symbol_list the symbol list to validate
 * @param[out] pg_indexes array containing corresponding provider global indexes or -1 if not found 
 * @return TRUE if all symbol found, else return FALSE 
 */
typedef int32_t                (* GLU_get_pgi_ft           )(struct GLU_handle_t* cthis, TSP_sample_symbol_info_list_t* symbol_list, int* pg_indexes);
/**
 * List of symbols managed by the GLU.
 * @param this Handle for the GLU (when the GLU is ACTIVE, it is always equal to GLU_GLOBAL_HANDLE)
 * @param symbol_list List of symbols
 * @return TRUE of FALSE. TRUE = OK;
 */
typedef int32_t                (* GLU_get_ssi_list_ft  )(struct GLU_handle_t* cthis, TSP_sample_symbol_info_list_t* symbol_list);

/**
 * Filtered list of symbols managed by the GLU.
 * @param[in] cthis Handle for the GLU (when the GLU is ACTIVE, it is always equal to GLU_GLOBAL_HANDLE)
 * @param[in] filter_kind the filter kind
 * @param[in] filter_string the filter string
 * @param[out] ans_sample the answer_sample to be filled-in by the GLU.
 * @return TRUE of FALSE. TRUE = OK;
 */
typedef int32_t                (* GLU_get_filtered_ssi_list_ft)(struct GLU_handle_t* cthis, int32_t filter_kind, char* filter_string, TSP_answer_sample_t* symbol_list);

/**
 * Get Sample Symbols Info list from PGI lists.
 * @param[in] cthis Handle for the GLU (when the GLU is ACTIVE, it is always equal to GLU_GLOBAL_HANDLE)
 * @param[in] pgis the array of requested PGI
 * @param[in] pgis_len the length of the pgis array
 * @param[in,out] the preallocated SSI list. 
 * @return TRUE of FALSE. TRUE = OK;
 */
typedef int32_t                (* GLU_get_ssi_list_fromPGI_ft)(struct GLU_handle_t* cthis, int* pgis, int32_t pgis_len, TSP_sample_symbol_info_list_t* SSI_list);

/**
 * Get Sample Symbols Extended Info list from PGI lists.
 * @param[in] cthis Handle for the GLU (when the GLU is ACTIVE, it is always equal to GLU_GLOBAL_HANDLE)
 * @param[in] pgis the array of requested PGI
 * @param[in] pgis_len the length of the pgis array
 * @param[in,out] on entry the preallocated SSEI list, on return the updated SSEI list
 * @return TRUE of FALSE. TRUE = OK;
 */
typedef int32_t                (* GLU_get_ssei_list_fromPGI_ft)(struct GLU_handle_t* cthis, int* pgis, int32_t pgis_len, TSP_sample_symbol_extended_info_list_t* SSEI_list);

/**
 * Get the number of symbols managed by the GLU.
 * @param this Handle for the GLU (when the GLU is ACTIVE, it is always equal to GLU_GLOBAL_HANDLE)
 * @return the number of symbols;
 */
typedef int32_t                (* GLU_get_nb_symbols_ft  )(struct GLU_handle_t* cthis);

/**
 * GLU asynchronous sample write.
 * This function will be called by the provider for TSP_async_sample_write.
 * The convention fixed is : if we can write with this function the sample
 * represented by its provider_global_index (correct pgi and authorization by
 * the GLU), the number returned is >0; else the number returned is <= 0
 *
 * @param[in] pgi        the index of the symbol concerned by the writing
 * @param[in] value_ptr  the new value of the symbol
 * @param[in] value_size the size of the value.
 * @return >0 on success <=0 on failure
 */
typedef int32_t                (* GLU_async_sample_write_ft)(struct GLU_handle_t* cthis, int32_t pgi, void* value_ptr, uint32_t value_size);

/**
 * GLU asynchronous sample read.
 * This function will be called by the provider for each TSP_async_sample_read.
 * The convention fixed is : if we can write with this function the sample
 * represented by its provider_global_index (correct pgi and authorization by
 * the GLU), the number returned is >0; else the number returned is <= 0
 *
 * @param[in] pgi        the index of the symbol concerned by the writing
 * @param[in] value_ptr  the new value of the symbol
 * @param[in,out] value_size the size of the value.
 * @return >0 on success <=0 on failure
 */
typedef int32_t                (* GLU_async_sample_read_ft)(struct GLU_handle_t* cthis, int32_t pgi, void* value_ptr, uint32_t* value_size);

                
/** 
 * GLU handle object.
 * This object represents a GLU instance.
 * It contains some minimal data and 
 * function pointer which are 'methods' 
 * for this object.
 * GLU creation function provides a pre-initialize GLU_handle_t 
 * structure with default implementation for some methods.
 * Specific GLU implementation may overrides thoses methods
 * with more efficient ones.
 */
typedef struct GLU_handle_t {

  pthread_t                 tid;             /**< The GLU thread Id */
  char*                     name;            /**< The GLU name */
  GLU_server_type_t         type;            /**< The GLU type */  
  double                    base_frequency;  /**< The provider base frequency */
  int32_t                   nb_max_consumer; /**< The provider base frequency */
  int32_t                   nb_connected_consumer; /**< The number of currently connected consumers using this GLU instance */
  void*                     private_data;    /**< The opaque pointer extensibility point */
  struct TSP_datapool*      datapool;

  /**
   * Name getter
   */
  GLU_get_server_name_ft     get_name; 
  /**
   * Type getter.
   */
  GLU_get_server_type_ft     get_type; 
  /**
   * Base frequency getter
   */ 
  GLU_get_base_frequency_ft  get_base_frequency;
  /**
   * max consumer getter
   */ 
  GLU_get_nb_max_consumer_ft get_nb_max_consumer;
  /**
   * instance getter 
   */
  GLU_get_instance_ft        get_instance;
  /**
   * Initializer member function.
   */
  GLU_init_ft                initialize;
  /**
   * Run member function.
   */
  GLU_run_ft                 run;
  /**
   * Start member function.
   */
  GLU_start_ft               start;
  /**
   * Get valid PGI list from sample symbol list
   * member function.
   */
  GLU_get_pgi_ft                 get_pgi;
  /**
   * Get complete sample symbol info list
   * member function.
   */
  GLU_get_ssi_list_ft            get_ssi_list;
  /**
   * Get filetered sample symbol info list
   * member function.
   */  
  GLU_get_filtered_ssi_list_ft   get_filtered_ssi_list;
  /** 
   * Get sample symbol info list from PGI list.
   */
  GLU_get_ssi_list_fromPGI_ft    get_ssi_list_fromPGI;
  /** 
   * Get sample symbol extended info list from PGI list.
   */
  GLU_get_ssei_list_fromPGI_ft   get_ssei_list_fromPGI;
  /**
   * Get total number of available symbols
   * member function.
   */ 
  GLU_get_nb_symbols_ft          get_nb_symbols; 
  /**
   * Asynchronous read
   * member function.
   */
  GLU_async_sample_read_ft       async_read;
  /**
   * Asynchronous write
   * member function.
   */
  GLU_async_sample_write_ft      async_write;
  
} GLU_handle_t;

BEGIN_C_DECLS

/**
 * Create a GLU_handle. 
 * This will provide some default implementation for member function.
 * @param[out] glu pointer to a GLU_handle pointer that will be allocated
 * @param[in] name the name of the GLU that will be advertised by the TSP provider.
 * @param[in] type the type of the GLU @see GLU_server_type_t
 * @param[in] base_frequency the base frequency that will be advertised by the TSP provider
 * @return TRUE if ok FALSE otherwise
 */
_EXPORT_TSP_PROVIDER int32_t GLU_handle_create(GLU_handle_t** glu, const char* name, const GLU_server_type_t type, const double base_frequency);

/**
 * Destroy a GLU_handle.
 * @param[out] glu pointer to a GLU_handle pointer that will be allocated
 * @return TRUE if ok FALSE otherwise
 */
int32_t GLU_handle_destroy(GLU_handle_t** glu);

/**
 * Check a GLU_handle for consistency.
 * This function may be called after specific GLU initialisation 
 * in order to verify member function pointer consistency and values:
 * <ul>
 *     <li> mandatory member functions must not be NULL </li>
 *     <li> try to detect unwanted circular reference between 
 *          provided and default member function(s) </li> 
 * </ul>
 * @param[in] glu the GLU_handle object to be checked.
 * @return TRUE if ok FALSE otherwise
 */
int32_t GLU_handle_check(GLU_handle_t* glu);


/** @} */

END_C_DECLS

#endif /* _TSP_GLU_H */
