
#include <sys/msg.h>

#include <bb_utils.h>
#include <bb_core.h>
#include <bb_simple.h>

static int bb_simple_synchro_type = BB_SIMPLE_SYNCHRO_PROCESS;

pthread_cond_t  bb_simple_go_condvar       = PTHREAD_COND_INITIALIZER;
pthread_mutex_t bb_simple_go_mutex         = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t  bb_simple_stockage_condvar = PTHREAD_COND_INITIALIZER;
pthread_mutex_t bb_simple_stcokage_mutex   = PTHREAD_MUTEX_INITIALIZER;

void* bb_simple_publish(S_BB_T* bb_simple,
			const char* var_name, 
			const char* module_name,
			const int module_instance,
			E_BB_TYPE_T type,
			int type_size,
			int dimension) {
  
  void* retval;
  S_BB_DATADESC_T s_data_desc;

  memset(s_data_desc.name,0,VARNAME_MAX_SIZE);
  if (-1!=module_instance) {
    snprintf(s_data_desc.name,VARNAME_MAX_SIZE,
	     "%s_%d_%s",
	     module_name,module_instance,var_name);
  } else {
    snprintf(s_data_desc.name,VARNAME_MAX_SIZE,
	     "%s_%s",
	     module_name,var_name);
  }
  s_data_desc.type        = type;
  s_data_desc.type_size   = type_size;
  s_data_desc.dimension   = dimension;
  retval = bb_publish(bb_simple,&s_data_desc);
  if (retval == NULL) {
    bb_logMsg(BB_LOG_SEVERE,"BlackBoard::bb_simple_publish", 
		"Cannot publish data <%s> instance <%d> module <%s>",
		var_name,module_instance,module_name);
  }  
  
  return retval;
} /* end of bb_simple_publish */


void* bb_simple_subscribe(S_BB_T* bb_simple,
			  const char* var_name,
			  const char* module_name,
			  const int module_instance,
			  E_BB_TYPE_T  type,
			  int* type_size,
			  int dimension) {
  
  void* retval;
  S_BB_DATADESC_T s_data_desc;

  memset(s_data_desc.name,0,VARNAME_MAX_SIZE);
  if (0!=module_instance) {
    snprintf(s_data_desc.name,VARNAME_MAX_SIZE,
	     "%s_%d_%s",
	     module_name,module_instance,var_name);
  } else {
    snprintf(s_data_desc.name,VARNAME_MAX_SIZE,
	     "%s_%s",
	     module_name,var_name);
  }
  
  retval = bb_subscribe(bb_simple,&s_data_desc);  
  
  return retval;
} /* end of bb_simple_subscribe */



int32_t 
bb_simple_synchro_config(int type_synchro) {
  
  int32_t retcode=E_OK;
  
  switch (type_synchro) {
  case BB_SIMPLE_SYNCHRO_PROCESS:
  case BB_SIMPLE_SYNCHRO_THREAD:
    bb_simple_synchro_type = type_synchro;
    break;
  default:
    bb_logMsg(BB_LOG_SEVERE,"BlackBoard::bb_simple_synchro_config",
		"Invalid synchro type <%d>, BB_SIMPLE_SYNCHRO_PROCESS <%d> used",
		type_synchro,BB_SIMPLE_SYNCHRO_PROCESS);
    bb_simple_synchro_type = BB_SIMPLE_SYNCHRO_PROCESS;
  }
  
  return retcode;
} /* end of bb_simple_synchro_config */

int32_t 
bb_simple_synchro_go(S_BB_T* bb_simple,int type_msg) {
  
  int32_t retcode;
  S_BB_MSG_T s_bb_msg;
  
  switch (bb_simple_synchro_type) {
  case BB_SIMPLE_SYNCHRO_PROCESS:
    s_bb_msg.mtype = type_msg;
    retcode = bb_snd_msg(bb_simple,&s_bb_msg);
    break;
  case BB_SIMPLE_SYNCHRO_THREAD:
    retcode=bb_simple_thread_synchro_go(type_msg);
    break;
  default:
    retcode=bb_simple_thread_synchro_go(type_msg); 
  }
  
  return retcode;
} /* end of bb_simple_synchro_go */

int32_t 
bb_simple_synchro_wait(S_BB_T* bb_simple,int type_msg) {
  
  int32_t retcode;
  S_BB_MSG_T s_bb_msg;
  
  switch (bb_simple_synchro_type) {
  case BB_SIMPLE_SYNCHRO_PROCESS:
    s_bb_msg.mtype = type_msg;
    retcode = bb_rcv_msg(bb_simple,&s_bb_msg);      
    break;
  case BB_SIMPLE_SYNCHRO_THREAD:
    retcode=bb_simple_thread_synchro_wait(type_msg);
    break;
  default:
    retcode=bb_simple_thread_synchro_wait(type_msg);    
  }
  
  return retcode;
} /* end of bb_simple_synchro_wait */

int32_t 
bb_simple_synchro_verify(S_BB_T* bb_simple) {
  
  int32_t retcode;
  struct msqid_ds s_msgq_stat;

  msgctl(bb_msg_id(bb_simple),
	 IPC_STAT,
	 &s_msgq_stat);
  
  if (s_msgq_stat.msg_qnum > 0) {
    retcode = E_NOK;
    
  } else {
    retcode = E_OK;
  }  
  
  return retcode;
} /* end of bb_simple_synchro_verify */

int32_t 
bb_simple_thread_synchro_go(int type_msg) {
  
  int32_t retcode;
  
  switch (type_msg) {
  case BB_SIMPLE_MSGID_SYNCHRO_COPY:
    pthread_mutex_lock(&bb_simple_go_mutex);
    pthread_cond_broadcast(&bb_simple_go_condvar);
    pthread_mutex_unlock(&bb_simple_go_mutex);
    break;
  case BB_SIMPLE_MSGID_SYNCHRO_COPY_ACK:
    pthread_mutex_lock(&bb_simple_stockage_mutex);
    pthread_cond_broadcast(&bb_simple_stockage_condvar);
    pthread_mutex_unlock(&bb_simple_stockage_mutex);
    break;
  default:
    bb_logMsg(BB_LOG_SEVERE,"BlackBoard::bb_simple_thread_synchro_go", 
	      "Invalid thread synchro ID <%d>",type_msg);
  }
  /* pthread_cond_broadcast always succeed */
  retcode = E_OK;
  
  return retcode;
} /* end of bb_simple_synchro_go */

int32_t 
bb_simple_thread_synchro_wait(int type_msg) {
  
  int32_t retcode;
  
  switch (type_msg) {
  case BB_SIMPLE_MSGID_SYNCHRO_COPY:
    pthread_mutex_lock(&bb_simple_go_mutex);
    pthread_cond_wait(&bb_simple_go_condvar,&bb_simple_go_mutex);
    pthread_mutex_unlock(&bb_simple_go_mutex);
    break;
  case BB_SIMPLE_MSGID_SYNCHRO_COPY_ACK:
    pthread_mutex_lock(&bb_simple_stockage_mutex);
    pthread_cond_wait(&bb_simple_stockage_condvar,&bb_simple_stockage_mutex);
    pthread_mutex_unlock(&bb_simple_stockage_mutex);
    break;
  default:
    bb_logMsg(BB_LOG_SEVERE,"BlackBoard::bb_simple_thread_synchro_wait", 
	      "Invalid thread synchro ID <%d>",type_msg);
  }
  /* pthread_cond_wait always succeed (or never return)*/
  retcode = E_OK;
  
  return retcode;
} /* end of bb_simple_synchro_wait */