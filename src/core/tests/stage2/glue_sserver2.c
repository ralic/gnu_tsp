/*!  \file 

$Header: /sources/tsp/tsp/src/core/tests/stage2/Attic/glue_sserver2.c,v 1.2 2002/09/30 12:06:53 galles Exp $

-----------------------------------------------------------------------

Project   : TSP

Component : Provider

-----------------------------------------------------------------------

Purpose   : Implementation for the glue_server

-----------------------------------------------------------------------
 */

#include "tsp_sys_headers.h"
#include <pthread.h>
#include <math.h>
#include "glue_sserver.h"
#include "tsp_ringbuf.h"
#include "tsp_time.h"



/*�S*/
#define TSP_USLEEP_PERIOD_US 10*1000



#define GLU_MAX_SYMBOLS 1000



/*RINGBUF_DEFINE_DYNAMIC(glu_ringbuf, glu_ring,  RINGBUF_SZ(GLU_RING_BUFSIZE));*/



static TSP_sample_symbol_info_t *X_sample_symbol_info_list_val;

static tsp_hrtime_t X_lasttime;

static glu_ringbuf* glu_ring = 0;

/* =
{

{"Symbol1", 0, 0, 0, {0,0,0,0}, 0, 3, 0},
{"Symbol2", 1, 0, 0, {0,0,0,0}, 0, 2, 0},
{"Symbol3", 2, 0, 0, {0,0,0,0}, 0, 2, 0},
{0, 0, 0, 0, 0, 0 , 0,0, 0}
 
};*/

static char* X_server_name = "ServeurPetitScarabe";

static time_stamp_t my_time = 0;

glu_ringbuf* GLU_active_get_ringbuf(GLU_handle_t h_glu)
{
  return glu_ring;
}

char* GLU_get_server_name(void)
{
  return X_server_name;
}

static int  GLU_get_symbol_number(void)

{
  SFUNC_NAME(GLU_get_symbol_number);

	
  int i = 0;
  TSP_sample_symbol_info_t* p;
	
  STRACE_IO(("-->IN"));
	
  for(p = X_sample_symbol_info_list_val;
      p->name != 0 ; 
      p++)
    {
      i++;
    }

  STRACE_IO(("-->OUT : -Nombre symboles = %u", i));
    
  return i;
}

long count_add = 0;

static void* GLU_thread(void* arg)
{
  
  SFUNC_NAME(GLU_thread);

  static int last_missed = 0;
  int symbols_nb;
  int i;
  tsp_hrtime_t current_time;
  
  glu_item_t item;

  symbols_nb  = GLU_get_symbol_number();
    
  STRACE_IO(("-->IN"));
    
  current_time = X_lasttime = tsp_gethrtime();  
  while(1)
    {
      for(i = 0 ; i <  symbols_nb ; i++)
	{
	  if(my_time%X_sample_symbol_info_list_val[i].period == 0)
	    {

	      item.time = my_time;
	      item.provider_global_index = i;
	      item.value = sin((double)(my_time + i));
                
	      /* STRACE_DEBUG(("IN->T=%d, Id=%d, V=%f",
		 item.time,
		 item.provider_global_index,
		 item.value))*/
                
	      RINGBUF_PTR_PUT(glu_ring, item);
	      count_add ++;
                     
	    }
	}
      
      if( current_time <= X_lasttime )
	{ 
	  tsp_usleep(TSP_USLEEP_PERIOD_US);
	}
      X_lasttime += TSP_USLEEP_PERIOD_US*1000;
      current_time = tsp_gethrtime();
      
      if(last_missed!=RINGBUF_PTR_MISSED(glu_ring))
	{
	  STRACE_ERROR(("RingBuffer full : %d missed data", RINGBUF_PTR_MISSED(glu_ring)));
	  last_missed = RINGBUF_PTR_MISSED(glu_ring);
	  
	}
      
      my_time++;    
      
      if(! (my_time%100))  STRACE_INFO(("TOP !"));
      
			     }
  

  
  STRACE_IO(("-->OUT"));
    
  return arg;

}

int GLU_init(char* fallback_stream_init)
{
  SFUNC_NAME(TSP_glue_sserver_init);

  
  pthread_t thread_id;	
  int i;
  char symbol_buf[50];
  
  STRACE_IO(("-->IN"));
  
  X_sample_symbol_info_list_val = calloc (GLU_MAX_SYMBOLS+1, sizeof (TSP_sample_symbol_info_t)) ;
  for (i=0; i<GLU_MAX_SYMBOLS; i++)
    {      
      sprintf(symbol_buf, "Symbol%d",i);
      X_sample_symbol_info_list_val[i].name = strdup(symbol_buf);
      X_sample_symbol_info_list_val[i].provider_global_index = i;
      X_sample_symbol_info_list_val[i].period = 1;
    }
  
  RINGBUF_PTR_INIT(glu_ringbuf, glu_ring, glu_item_t,  RINGBUF_SZ(GLU_RING_BUFSIZE));
  RINGBUF_PTR_RESET (glu_ring);
  TSP_CHECK_THREAD( (pthread_create(&thread_id, NULL, GLU_thread, NULL)), FALSE);
  
  STRACE_IO(("-->OUT"));
  
  return TRUE;
  
}

int  GLU_get_sample_symbol_info_list(GLU_handle_t h_glu,TSP_sample_symbol_info_list_t* symbol_list)
{
  SFUNC_NAME(TSP_DTP_get_sample_symbol_info_list);

	
  gint32 i = 0;
  TSP_sample_symbol_info_t* p;
	
  STRACE_IO(("-->IN"));
	
  for(p = X_sample_symbol_info_list_val;
      p->name != 0 ; 
      p++)
    {
      i++;
    }

  symbol_list->TSP_sample_symbol_info_list_t_len = i;
  symbol_list->TSP_sample_symbol_info_list_t_val = X_sample_symbol_info_list_val;
	    
  STRACE_IO(("-->OUT : -Nombre symboles = %u", symbol_list->TSP_sample_symbol_info_list_t_len));

  return TRUE;
}

int GLU_add_block(GLU_handle_t h_glu,int provider_global_index, xdr_and_sync_type_t type)
{

  SFUNC_NAME(GLU_add_block);
  STRACE_IO(("-->IN index=%d type=%X", provider_global_index, type));
  STRACE_IO(("FIXME : JE NE FAIS RIEN POUR LE MOMENT !"));
  /* FIXME : g�rer ca ! */
  STRACE_IO(("-->OUT"));
	
  return TRUE;
}

int GLU_commit_add_block(GLU_handle_t h_glu)
{
  SFUNC_NAME(GLU_commit_add_block);
  STRACE_IO(("-->IN"));
  STRACE_IO(("FIXME : JE NE FAIS RIEN POUR LE MOMENT !"));
  STRACE_IO(("-->OUT"));
	
  return TRUE;
	
}

GLU_server_type_t GLU_get_server_type(void)
{
  return GLU_SERVER_TYPE_ACTIVE;
}

/* not used */
int GLU_pasive_get_next_item(GLU_handle_t h_glu, glu_item_t* item)
{
  return FALSE;
}

GLU_handle_t GLU_get_instance(char *stream_init, char** error_info)
{
  if(error_info)
    *error_info = "";
  return GLU_GLOBAL_HANDLE;
}