/*!  \file 

$Header: /sources/tsp/tsp/src/jcore/tsp/core/rpc/Attic/TSP_rpc.x,v 1.1 2003/02/07 16:09:48 SyntDev1 Exp $

-----------------------------------------------------------------------

Project   : TSP

Component : Consumer / Provider

-----------------------------------------------------------------------

Purpose   : 

-----------------------------------------------------------------------
 */

/** Status for all functions */
enum TSP_status_t 
{
	/** OK */
	TSP_STATUS_OK,

	/** Fatal error */
	TSP_STATUS_ERROR_UNKNOWN,

	/** 
	 * Means that a detailed error string is provided by the function,
	 * and the details of the error are in this string
         */
	TSP_STATUS_ERROR_SEE_STRING,	

	/** 
         * The requested version for the protocol does not match
         */
	TSP_STATUS_ERROR_VERSION,

	/**
         * Error with the symbols (asked or returned )
         */
	TSP_STATUS_ERROR_SYMBOLS	
	
};


struct TSP_answer_open_t
{
	int version_id;
	unsigned int channel_id;
	TSP_status_t status;
	string status_str<>;

};

typedef string   TSP_argv_item_t<>;
typedef TSP_argv_item_t TSP_argv_t<>;	

struct TSP_request_open_t
{
	int version_id;
/*	string stream_init<>;
	int use_stream_init;*/

	TSP_argv_t argv;	

};

struct TSP_request_close_t
{
	int version_id;
	unsigned int channel_id;
};

struct TSP_request_information_t
{
	int version_id;
	unsigned int channel_id;
};

struct TSP_request_feature_t
{
	int version_id;
	unsigned int channel_id;
	unsigned int feature_words[4];
};


struct TSP_answer_feature_t
{
	int version_id;
	unsigned int channel_id;
	unsigned int feature_words[4];
	int int_value;
	double double_value;
	string string_value<>;
};

struct TSP_exec_feature_t
{
	int version_id;
	unsigned int channel_id;
	unsigned int feature_words[4];
	int int_value;
	double double_value;
	string string_value<>;
};

struct TSP_sample_symbol_info_t
{
	string name<>;
	int provider_global_index;
	int provider_group_index;
	int provider_group_rank;
	opaque xdr_tsp_t[4];
	unsigned int dimension;
	int period;
	int phase; 
};


/*struct TSP_sample_symbol_info_list_t
{
	TSP_sample_symbol_info_t TSP_sample_symbol_info_list_t<>;
};
*/

typedef TSP_sample_symbol_info_t TSP_sample_symbol_info_list_t<>;

/*
FIXME : Use it when the consumer will be able
to ask for a given endianity
enum TSP_endianity_t 
{
	TSP_PSEUDO_XDR_LITTLE_ENDIAN,
	TSP_XDR_BIG_ENDIAN
};*/


struct TSP_request_sample_t
{
	int version_id;
	unsigned int channel_id;
	unsigned int feature_words[4];
	int consumer_timeout;
	TSP_sample_symbol_info_list_t symbols;

	/* TSP_endianity_t data_endianity; FIXME : implementer */
};

struct TSP_answer_sample_t
{
	int version_id;
	unsigned int channel_id;

	int provider_timeout;
	int provider_group_number;
	TSP_sample_symbol_info_list_t symbols;
	double base_frequency;
	int max_period; 
	int max_client_number; 
	int current_client_number; 
	TSP_status_t status;		

	/*unsigned int feature_words[4]; FIXME*/
	/* TSP_endianity_t data_endianity; FIXME : implementer */


};

struct TSP_request_sample_init_t
{
	int version_id;
	unsigned int channel_id;
};

struct TSP_answer_sample_init_t
{
	int version_id;
	unsigned int channel_id;
	string data_address<>;		
	TSP_status_t status;			
};


/* FIXME : utiliser ca...*/
struct TSP_asynchronous_sample_symbol_t
{
	int provider_index;
};


/* FIXME : implementer */
struct TSP_request_sample_destroy_t
{
	int version_id;
	unsigned int channel_id;	
};

/* FIXME : implementer */
struct TSP_answer_sample_destroy_t
{
	int version_id;
	unsigned int channel_id;	
	int status;
};



/* This structure is not part of TSP Protocol*/
struct TSP_provider_info_t{
	string info<>;
};


/*------------------------------------------------------*/
/*		RPC Functions for TSP Provider		*/
/*------------------------------------------------------*/
program TSP_RPC {

	version TSP_RPC_VERSION_INITIAL {

	

	/* This function is not part of TSP Protocol */	
	TSP_provider_info_t 	TSP_PROVIDER_INFORMATION(void) = 101;

	
	/* TSP Protocol */	

	TSP_answer_open_t 		TSP_REQUEST_OPEN(TSP_request_open_t req_open) = 102;
	
	void 				TSP_REQUEST_CLOSE(TSP_request_close_t req_close) = 103;
	
	TSP_answer_sample_t		TSP_REQUEST_INFORMATION(TSP_request_information_t req_info) = 104;
	
	TSP_answer_feature_t		TSP_REQUEST_FEATURE(TSP_request_feature_t req_feature) = 105;
	
	TSP_answer_sample_t		TSP_REQUEST_SAMPLE(TSP_request_sample_t req_sample) = 106;
	
	TSP_answer_sample_init_t	TSP_REQUEST_SAMPLE_INIT(TSP_request_sample_init_t req_sample) = 107;
	
	TSP_answer_sample_destroy_t	TSP_REQUEST_SAMPLE_DESTROY(TSP_request_sample_destroy_t req_destroy) = 108;
	
	void				TSP_EXEC_FEATURE(TSP_exec_feature_t exec_feature) = 109;
	

	} = 1;
} = 0 ;

