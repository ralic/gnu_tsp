/*

$Id: gdisp_sampling.c,v 1.24 2007/11/14 21:53:20 esteban Exp $

-----------------------------------------------------------------------

GDISP+ - Graphic Tool based on GTK+,
         for being used upon the generic Transport Sampling Protocol.

Copyright (c) 2003 - Euskadi.

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
Component : Graphic Tool

-----------------------------------------------------------------------

Purpose   : Graphic Tool based on GTK+ that provide several kinds of
            plot. This tool is to be used with the generic TSP protocol.

File      : Graphic Tool Sampling Core Process.

-----------------------------------------------------------------------
*/


/*
 * System includes.
 */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <ctype.h>


/*
 * GDISP+ includes.
 */
#include "gdisp_sampling.h"

#undef GD_THREAD_DEBUG
#undef GD_SAMPLING_DEBUG

/*
 --------------------------------------------------------------------
                             STATIC ROUTINES
 --------------------------------------------------------------------
*/


/*
 * Get double value from TSP value.
 */
static gdouble
gdisp_getDoubleValue(TSP_datatype_t  symbolType,
		     TSP_sample_t   *sampleValue)
{

  gdouble doubleValue = 0.0;

  switch (symbolType) {

  case TSP_TYPE_DOUBLE :
    doubleValue = sampleValue->uvalue.double_value;
    break;

  case TSP_TYPE_FLOAT :
    doubleValue = (gdouble)sampleValue->uvalue.float_value;
    break;

  case TSP_TYPE_INT8 :
    doubleValue = (gdouble)sampleValue->uvalue.int8_value;
    break;

  case TSP_TYPE_INT16 :
    doubleValue = (gdouble)sampleValue->uvalue.int16_value;
    break;

  case TSP_TYPE_INT32 :
    doubleValue = (gdouble)sampleValue->uvalue.int32_value;
    break;

  case TSP_TYPE_INT64 :
    doubleValue = (gdouble)sampleValue->uvalue.int64_value;
    break;

  case TSP_TYPE_UINT8 :
    doubleValue = (gdouble)sampleValue->uvalue.uint8_value;
    break;

  case TSP_TYPE_UINT16 :
    doubleValue = (gdouble)sampleValue->uvalue.uint16_value;
    break;

  case TSP_TYPE_UINT32 :
    doubleValue = (gdouble)sampleValue->uvalue.uint32_value;
    break;

  case TSP_TYPE_UINT64 :
    doubleValue = (gdouble)sampleValue->uvalue.uint64_value;
    break;

  case TSP_TYPE_CHAR :
    doubleValue = (gdouble)sampleValue->uvalue.char_value;
    break;

  case TSP_TYPE_UCHAR :
    doubleValue = (gdouble)sampleValue->uvalue.uchar_value;
    break;

  case TSP_TYPE_RAW :
    doubleValue = (gdouble)sampleValue->uvalue.raw_value;
    break;

  case TSP_TYPE_UNKNOWN :
  case TSP_TYPE_LAST :
  default :
    doubleValue = 0.0;
    break;

  }
  
  return doubleValue;
	
}


/*
 * Encapsulation of thread creation.
 */
static gint
gdisp_createThread ( Kernel_T               *kernel,
		     gchar                  *threadName,
		     pthread_t              *threadIdentity,
		     const pthread_attr_t   *threadAttributes,
		     void                 *(*threadProcedure)(void*),
		     void                   *threadArgument,
		     gboolean                threadIsDetached,
		     gboolean                threadCreationIsVerbose )
{

  ThreadStatus_T  threadStatus  = GD_THREAD_STOPPED;
  GString        *messageString =    (GString*)NULL;
  gint            status        =                 0;


  /*
   * Really create the thread.
   */
  *threadIdentity = (pthread_t)NULL;

  status = pthread_create(threadIdentity,
			  threadAttributes,
			  threadProcedure,
			  threadArgument);

  if (status != 0) {

    *threadIdentity = (pthread_t)NULL;
    threadStatus    = GD_THREAD_ERROR;

    if (threadCreationIsVerbose == TRUE) {

      messageString = g_string_new((gchar*)NULL);
      g_string_sprintf(messageString,
		       "%s thread creation failed.",
		       threadName);
      (*kernel->outputFunc)(kernel,messageString,GD_ERROR);

      messageString = g_string_new((gchar*)NULL);
      g_string_sprintf(messageString,
		       "%s.",
		       strerror(errno));
      (*kernel->outputFunc)(kernel,messageString,GD_ERROR);

    } /* verbose */

  }
  else if (threadIsDetached == TRUE) {

    status = pthread_detach(*threadIdentity);

    if (status != 0 && threadCreationIsVerbose == TRUE) {

      threadStatus = GD_THREAD_WARNING;

      messageString = g_string_new((gchar*)NULL);
      g_string_sprintf(messageString,
		       "Cannot detach %s thread.",
		       threadName);
      (*kernel->outputFunc)(kernel,messageString,GD_ERROR);

      messageString = g_string_new((gchar*)NULL);
      g_string_sprintf(messageString,
		       "%s.",
		       strerror(errno));
      (*kernel->outputFunc)(kernel,messageString,GD_ERROR);

    }

  }

  return threadStatus;

}


/*
 * Get provider by symbol.
 */
static Provider_T*
gdisp_getProviderBySymbol ( Kernel_T *kernel,
			    Symbol_T *symbol )
{

  GList      *providerItem = (GList*)NULL;
  Provider_T *provider     = (Provider_T*)NULL;

  /*
   * Loop over all providers, and look for the provider
   * the given symbol belongs to.
   */
  providerItem = g_list_first(kernel->providerList);
  while (providerItem != (GList*)NULL) {

    provider = (Provider_T*)providerItem->data;

    /*
     * The symbol belongs to the current provider because its
     * address is in the good address interval.
     */
    if (provider->pSymbolList <= symbol &&
	symbol < provider->pSymbolList + provider->pSymbolNumber) {

      return provider;

    }

    providerItem = g_list_next(providerItem);

  }

  return (Provider_T*)NULL;

}


/*
 * Main callback routine to perform timer period computation.
 */
static void
gdisp_computeTimerPeriod ( Kernel_T         *kernel,
			   Page_T           *page,
			   PlotSystemData_T *plotSystemData,
			   void             *userData )
{

  guint *timerPeriod = (guint*)userData;
  guint  plotPeriod  = 0;

  /*
   * Get back the current period of the plot.
   */
  plotPeriod = (*plotSystemData->plotSystem->psGetPeriod)
                                      (kernel,plotSystemData->plotData);

  if (*timerPeriod == G_MAXINT) {
    *timerPeriod = plotPeriod;
  }
  else {
    *timerPeriod = gdisp_computePgcd(*timerPeriod,plotPeriod);
  }

}


/*
 * Main callback routine to perform plot cycle computation.
 */
static void
gdisp_computePlotCycles ( Kernel_T         *kernel,
			  Page_T           *page,
			  PlotSystemData_T *plotSystemData,
			  void             *userData )
{

  guint *timerPeriod = (guint*)userData;
  guint  plotPeriod  = 0;

  /*
   * Compute the plot cycle.
   * If GTK step timer has a period equal to 100 milli-seconds, and
   * our plot has a period of 1000 milli-seconds, so out plot cycle is 10.
   * It will be "stepped" 1 time out of 10.
   */
  plotPeriod = (*plotSystemData->plotSystem->psGetPeriod)
                                      (kernel,plotSystemData->plotData);

  plotSystemData->plotCycle = plotPeriod / *timerPeriod;

  if (plotSystemData->plotCycle == 0) {

    plotSystemData->plotCycle = 1;

  }

}


/*
 * Main callback routine to perform symbol treatment on the graphic plot.
 */
static void
gdisp_treatSymbolOnOneGraphicPlot ( Kernel_T         *kernel,
				    Page_T           *page,
				    PlotSystemData_T *plotSystemData,
				    void             *userData )
{

  (*plotSystemData->plotSystem->psTreatSymbolValues)(kernel,
						     plotSystemData->plotData);

}


/*
 * Main callback routine to perform start-step on the graphic plot.
 */
static void
gdisp_startStepOnOneGraphicPlot ( Kernel_T         *kernel,
				  Page_T           *page,
				  PlotSystemData_T *plotSystemData,
				  void             *userData )
{

  gboolean ready = FALSE;

  ready = (*plotSystemData->plotSystem->psStartStep)(kernel,
						     plotSystemData->plotData);

  if (ready == FALSE) {

    *((gboolean*)userData) = FALSE;

  }

}


/*
 * Main callback routine to perform stop-step on the graphic plot.
 */
static void
gdisp_stopStepOnOneGraphicPlot ( Kernel_T         *kernel,
				 Page_T           *page,
				 PlotSystemData_T *plotSystemData,
				 void             *userData )
{

  (*plotSystemData->plotSystem->psStopStep)(kernel,
					    plotSystemData->plotData);

}


/*
 * Main callback routine to perform steps on the graphic plot.
 */
static void
gdisp_stepOnOneGraphicPlot ( Kernel_T         *kernel,
			     Page_T           *page,
			     PlotSystemData_T *plotSystemData,
			     void             *userData )
{

  /*
   * Take into account the period of the step management, and the
   * period of the graphic plot.
   */
  if (kernel->stepGlobalCycle % plotSystemData->plotCycle == 0) {

    (*plotSystemData->plotSystem->psStep)(kernel,
					  plotSystemData->plotData);

  }

}


/*
 * Main routine to perform steps on the graphic pages.
 * The prototype of this routine ensures that it may be used
 * as a callback given to the 'gtk_timeout_add' action.
 */
#undef GD_BENCHMARK

static gint
gdisp_stepsOnGraphicPlots ( void *data )
{

  Kernel_T        *kernel    = (Kernel_T*)data;

#if defined(GD_BENCHMARK)

  HRTime_T         startMark = (HRTime_T)0;
  HRTime_T         stopMark  = (HRTime_T)0;
  HRTime_T         deltaMark = (HRTime_T)0;
  static HRTime_T  lastMark  = (HRTime_T)0;

#endif

  /*
   * Check GTK timer.
   */
#if defined(GD_BENCHMARK)

  if (lastMark > 0) {

    deltaMark = gdisp_getHRTime() - lastMark;

    fprintf(stdout,
	    "GTK Timer : %d nanoseconds\n",
	    (int)(deltaMark & 0xFFFFFFFF));

  }

#endif

  /*
   * Do the plot step.
   */
#if defined(GD_BENCHMARK)

  startMark = gdisp_getHRTime();

#endif

  /* ************************************************************
   *
   * BEGIN : Loop over all graphic plots of all pages, do steps...
   *
   * ************************************************************/

  kernel->stepGlobalCycle++;

  gdisp_loopOnGraphicPlots (kernel,
			    gdisp_stepOnOneGraphicPlot,
			    (void*)NULL);

  /* ************************************************************
   *
   * END.
   *
   * ************************************************************/

#if defined(GD_BENCHMARK)

  stopMark = gdisp_getHRTime();

  deltaMark = stopMark - startMark;

  fprintf(stdout,
	  "Step on all plots : %d nanoseconds\n",
          (int)(deltaMark & 0xFFFFFFFF));

  lastMark = gdisp_getHRTime();

#endif

  return TRUE; /* keep on running */

}


/*
 * Memory allocation for sampling symbols.
 */
static void
gdisp_freeSymbolsForSampling (Kernel_T *kernel)
{

  GList    *symbolList =    (GList*)NULL;
  Symbol_T *symbol     = (Symbol_T*)NULL;

  /*
   * Loop on every symbol that must be sampled.
   * Free memory that has been used for storing the evolution of the symbol.
   */
  symbolList = gdisp_getSymbolsInPages(kernel);
  symbolList = g_list_first(symbolList);

  while (symbolList != (GList*)NULL) {

    symbol = (Symbol_T*)symbolList->data;

    /*
     * Release everything necessary here.
     */

    /* nothing to be done by now */

    symbolList = g_list_next(symbolList);

  }

  g_list_free(symbolList);

}


/*
 * Memory allocation for sampling symbols.
 */
static gboolean
gdisp_allocateSymbolsForSampling (Kernel_T *kernel)
{

  GList    *symbolList =    (GList*)NULL;
  Symbol_T *symbol     = (Symbol_T*)NULL;

  /*
   * Loop on every symbol that must be sampled.
   * Allocate memory for storing the evolution of the symbol.
   */
  symbolList = gdisp_getSymbolsInPages(kernel);
  symbolList = g_list_first(symbolList);

  while (symbolList != (GList*)NULL) {

    symbol = (Symbol_T*)symbolList->data;

    /*
     * Do allocation here if necessary.
     */

    /* nothing to be done by now */

    symbolList = g_list_next(symbolList);

  }

  g_list_free(symbolList);

  return TRUE; /* no error */

}


/*
 * Garbage collector on symbols.
 * Loop over all symbols and release memory if necessary.
 */
static void*
gdisp_garbageCollectorThread ( void *data )
{

  Kernel_T   *kernel       =   (Kernel_T*)data;
  GList      *providerItem =      (GList*)NULL;
  Symbol_T   *symbolPtr    =   (Symbol_T*)NULL;
  Provider_T *provider     = (Provider_T*)NULL;
  gint        symbolCpt    =                 0;

#if defined(GD_THREAD_DEBUG)
  fprintf(stdout,"Beginning of garbage collector thread.\n");
  fflush (stdout);
#endif

  /*
   * Wait for the thread creation to finalise...
   */
  while (kernel->garbageCollectorThread != pthread_self()) {

    gdisp_uSleep(_100_MILLISECONDS_IN_MICROSECONDS_);

  }

  /*
   * Garbage collector on symbols.
   * Loop over all symbols and release memory if necessary.
   */
  providerItem = g_list_first(kernel->providerList);
  while (providerItem != (GList*)NULL) {

    provider  = (Provider_T*)providerItem->data;
    symbolPtr = provider->pSymbolList;

    for (symbolCpt=0; symbolCpt<provider->pSymbolNumber; symbolCpt++) {

      /*
       * Release memory if no more graphic plots make use of this symbol.
       */
      if (symbolPtr->sReference == 0) {

	/*
	 * Nothing by now.
	 */

      }

      /*
       * Every 100 symbols, wait for a while by leaving CPU.
       * This enables other threads to keep on running...
       * FIXME : confirm it is useful...
       */
      if (symbolCpt % 100 == 0) {

	gdisp_uSleep(0);

      }

      symbolPtr++;

    } /* symbolCpt */

    providerItem = g_list_next(providerItem);

  }

  /*
   * Tell the kernel the garbage thread is about to finish.
   */
  kernel->garbageCollectorThread = (pthread_t)NULL;

#if defined(GD_THREAD_DEBUG)
  fprintf(stdout,"End of garbage collector thread.\n");
  fflush (stdout);
#endif

  pthread_exit((void*)TRUE /* OK */);

}


/*
 * Request init and sample.
 */
static gboolean
gdisp_requestSample ( Kernel_T   *kernel,
		      Provider_T *provider )
{

  const SampleList_T *sampleList        = (const SampleList_T*)NULL;
  Symbol_T           *symbol            = (Symbol_T*)NULL;
  gint                requestStatus     = 0;
  guint               sampleCpt         = 0;
  guint               symbolLoad        = 0;
  TSP_sample_symbol_info_t *symbolInfo  = (TSP_sample_symbol_info_t*)NULL;
  gboolean            errorCode         = FALSE;


  /*
   * Give requested symbols to TSP core.
   */
  requestStatus = TSP_consumer_request_sample(provider->pHandle,
					      &provider->pSampleList);

  if (TSP_STATUS_OK != requestStatus) {

    provider->pSamplingThreadStatus = GD_THREAD_REQUEST_SAMPLE_ERROR;
    return FALSE;

  }


  /*
   * Manage output PGI of each symbol.
   */
  sampleList = TSP_consumer_get_requested_sample(provider->pHandle);

  if (sampleList == (SampleList_T*)NULL) {

    provider->pSamplingThreadStatus = GD_THREAD_REQUEST_SAMPLE_ERROR;
    return FALSE;

  }


  /*
   * Build PGI to symbol hash table.
   */
  errorCode = gdisp_buildProviderPgiHashTable(provider,
					      sampleList);

  if (errorCode == FALSE) {

    provider->pSamplingThreadStatus = GD_THREAD_REQUEST_SAMPLE_ERROR;
    return FALSE;

  }


  /*
   * Get back all extended information.
   */
  gdisp_getSymbolExtendedInformation(provider,
				     FALSE /* do not forget any previous info */);


  /*
   * Compute maximum load.
   */
  provider->pLoad    = 0;
  provider->pMaxLoad = 0;
  symbolInfo         = sampleList->TSP_sample_symbol_info_list_t_val;

  for (sampleCpt=0;
       sampleCpt<sampleList->TSP_sample_symbol_info_list_t_len;
       sampleCpt++, symbolInfo++) {

    symbol = (Symbol_T*)hash_get(provider->pSymbolHashTable,
				 symbolInfo->name);

    if (symbol->sInfo.type >= TSP_TYPE_LAST) {
      symbol->sInfo.type = TSP_TYPE_LAST;
    }
    symbolLoad  = tsp_type_size[symbol->sInfo.type];
    symbolLoad *= provider->pBaseFrequency;
    symbolLoad *= symbolInfo->nelem;
    symbolLoad /= symbolInfo->period;

    provider->pMaxLoad += symbolLoad;

  }

  return TRUE;

}


/*
 * Main Sampling Thread.
 */
static void*
gdisp_samplingThread (void *data )
{

#define GD_SAMPLE_PGI_AS_STRING_LENGTH 10

  Kernel_T                 *kernel            = (Kernel_T*)data;
  Provider_T               *provider          = (Provider_T*)NULL;
  Symbol_T                 *symbol            = (Symbol_T*)NULL;
  GList                    *providerItem      = (GList*)NULL;
  gboolean                  providerIsFound   = FALSE;
  guchar                    watchDog          = 10;
  gint                      requestStatus     = 0;
  gboolean                  sampleHasArrived  = FALSE;
  guint                     sampleRefTimeTag  = 0;
  TSP_sample_t              sampleValue;
  gchar                 samplePGIasStringBuffer[GD_SAMPLE_PGI_AS_STRING_LENGTH];
  gchar                    *samplePGIasString = (gchar*)NULL;

#if defined(GD_THREAD_DEBUG)
  fprintf(stdout,"Beginning of provider sampling thread.\n");
  fflush (stdout);
#endif

  /*
   * Wait for the thread creation to finalise...
   * Let's search for the provider we are talking about...
   * because only the kernel has been given to the thread.
   */
  do {

    providerItem = g_list_first(kernel->providerList);
    while (providerItem != (GList*)NULL && providerIsFound == FALSE) {

      provider = (Provider_T*)providerItem->data;

      if (provider->pSamplingThread == pthread_self()) {

	providerIsFound = TRUE;

      }

      providerItem = g_list_next(providerItem);

    } /* loop on providers */

    if (providerIsFound == FALSE) {

      watchDog--;
      gdisp_uSleep(_100_MILLISECONDS_IN_MICROSECONDS_);

    }

  } while (providerIsFound == FALSE && watchDog > 0);

  /*
   * Do I know who I am ?
   */
  if (providerIsFound == FALSE) {

    pthread_exit((void*)FALSE);

  }

  /*
   * Tell TSP core to start sampling process.
   * Wait for a while so that TSP core has time to fill in FIFOs.
   */
  requestStatus =
    TSP_consumer_request_sample_init(provider->pHandle,
				     (TSP_sample_callback_t)NULL,
				     (void*)NULL);

  if (TSP_STATUS_OK != requestStatus) {

    provider->pSamplingThreadStatus = GD_THREAD_SAMPLE_INIT_ERROR;
    pthread_exit((void*)FALSE);

  }
  else {

    gdisp_uSleep(_ONE_SECOND_IN_MICROSECONDS_ / 2);

  }

  /*
   * Sample... Do it...
   */
  provider->pSamplingThreadStatus = GD_THREAD_RUNNING;

  while (kernel->samplingThreadMustExit == FALSE) {

    requestStatus = TSP_consumer_read_sample(provider->pHandle,
					     &sampleValue,
					     &sampleHasArrived);

    if (TSP_STATUS_OK != requestStatus) {

      /*
       * Out of "while" loop.
       */
      break;

    }

    if (sampleHasArrived == TRUE) {

      /*
       * Check out new incoming frame.
       */
      if (sampleRefTimeTag != 0 &&
	  sampleRefTimeTag != (guint)sampleValue.time) {

#if defined(GD_SAMPLING_DEBUG)

	printf("------------------------ FRAME ------------------------\n");

#endif

	gdisp_loopOnGraphicPlots (kernel,
				  gdisp_treatSymbolOnOneGraphicPlot,
				  (void*)NULL);

      }
      
      sampleRefTimeTag = (guint)sampleValue.time;

      /*
       * Treat symbol.
       */
#if defined(GD_LOAD_CONFIGURATION_WITH_ALL_SYMBOLS)

      symbol = &provider->pSymbolList[sampleValue.provider_global_index];

#else

      /*
       * Convert PGI as an unsigned integer to a string.
       */
      samplePGIasStringBuffer[GD_SAMPLE_PGI_AS_STRING_LENGTH-1] = '\0';
      samplePGIasString =
	gdisp_uIntToStr(sampleValue.provider_global_index,
               &samplePGIasStringBuffer[GD_SAMPLE_PGI_AS_STRING_LENGTH-1]);

      /*
       * Retreive target symbol.
       */
      symbol =
	(Symbol_T*)hash_get(provider->pSymbolHashTablePGI,samplePGIasString);

#endif

      /*
       * Count the number of incoming values in order to deduce
       * the provider load (bytes per seconds).
       */
      provider->pLoad   += tsp_type_size[symbol->sInfo.type];

      symbol->sTimeTag   = (guint)sampleValue.time;

      symbol->sLastValue = gdisp_getDoubleValue(symbol->sInfo.type,
						&sampleValue);

#if defined(GD_SAMPLING_DEBUG)

      printf("Time [%d] - Index [%d] - Name [%s] - Value [%f]\n",
	 sampleValue.time,
	 sampleValue.provider_global_index,
	 symbol->sInfo.name,
	 (float)symbol->sLastValue);

#endif

    } /* sampleHasArrived == TRUE */

    else {

      /*
       * Used to give time to other TSP threads for filling internal
       * FIFOs with received samples.
       */
      gdisp_uSleep(_ONE_SECOND_IN_MICROSECONDS_ / provider->pBaseFrequency);

    }

  }

  provider->pSamplingThreadStatus = GD_THREAD_STOPPED;

  /*
   * Tell TSP core to stop sampling process.
   */
  if (requestStatus == TSP_STATUS_OK) {

    requestStatus = TSP_consumer_request_sample_destroy(provider->pHandle);

  }

  if (TSP_STATUS_OK != requestStatus) {

    provider->pSamplingThreadStatus = GD_THREAD_SAMPLE_DESTROY_ERROR;

  }

  /*
   * Bye bye.
   */
  provider->pSamplingThread = (pthread_t)NULL;

#if defined(GD_THREAD_DEBUG)
  fprintf(stdout,"End of provider sampling thread.\n");
  fflush (stdout);
#endif

  pthread_exit((void*)TRUE);

}


/*
 * Pre-Sampling Thread.
 */
static void*
gdisp_preSamplingThread (void *data )
{

  Kernel_T       *kernel            =   (Kernel_T*)data;
  Provider_T     *provider          = (Provider_T*)NULL;
  GList          *providerItem      =      (GList*)NULL;
  gboolean        memoryIsAllocated =             FALSE;
  ThreadStatus_T  threadStatus      =   GD_THREAD_ERROR;


#if defined(GD_THREAD_DEBUG)
  fprintf(stdout,"Beginning of pre-sampling thread.\n");
  fflush (stdout);
#endif

  /*
   * Memory allocation for sampling symbols.
   */
  memoryIsAllocated = gdisp_allocateSymbolsForSampling(kernel);
  if (memoryIsAllocated == FALSE) {

    /*
     * No matter the returned value, because 'preSamplingThread' is detached.
     */
    pthread_exit((void*)FALSE);

  }


  /*
   * All provider threads must keep on running...
   */
  kernel->samplingThreadMustExit = FALSE;


  /*
   * Assign symbols to providers.
   * This is done because few symbols may have been removed from graphic plots.
   */
  gdisp_affectRequestedSymbolsToProvider(kernel);


  /*
   * Loop on each provider, and launch the sampling thread.
   */
  providerItem = g_list_first(kernel->providerList);
  while (providerItem != (GList*)NULL) {

    provider = (Provider_T*)providerItem->data;

    /*
     * Create the thread if only there something to be sampled...
     */
    if (provider->pSampleList.TSP_sample_symbol_info_list_t_len != 0) {

      threadStatus = gdisp_createThread(kernel,
					provider->pUrl->str,
					&provider->pSamplingThread,
					(const pthread_attr_t*)NULL,
					gdisp_samplingThread,
					(void*)kernel,
					TRUE  /* thread is detached */,
					FALSE /* no verbose         */);

      provider->pSamplingThreadStatus =
	threadStatus == GD_THREAD_ERROR ? threadStatus : GD_THREAD_STARTING;

    } /* something to be sampled... */

    providerItem = g_list_next(providerItem);

  } /* loop on providers */


  /*
   * Launch a timer.
   * The period must be defined in milli-seconds.
   */
  kernel->stepTimerIdentity  = gtk_timeout_add(kernel->stepTimerPeriod,
					       gdisp_stepsOnGraphicPlots,
					       (gpointer)kernel);


  /*
   * No matter the returned value, because 'preSamplingThread' is detached.
   */
#if defined(GD_THREAD_DEBUG)
  fprintf(stdout,"End of pre-sampling thread.\n");
  fflush (stdout);
#endif

  /*
   * ========================== WARNING ======================
   * From esteban.
   * I disagree with the calling of the procedure here.
   * It is meant for being called when the sampling process is over, whereas
   * it is not the case at the end of this thread.
   * As this procedure does nothing by now, it is not a hurry to find the correct
   * place (should be at the end of the 'stopSamplingProcess' procedure according to me).
   */
  gdisp_freeSymbolsForSampling(kernel);
  /*
   * esteban.
   */

  pthread_exit((void*)TRUE);

}


/*
 --------------------------------------------------------------------
                             PUBLIC ROUTINES
 --------------------------------------------------------------------
*/


/*
 * Start everything that deals with sampling process.
 */
gboolean
gdisp_startSamplingProcess (Kernel_T *kernel)
{

  pthread_t       preSamplingThread = (pthread_t)NULL;
  ThreadStatus_T  threadStatus      = GD_THREAD_ERROR;
  gboolean        allPlotsReady     = TRUE;
  GString        *messageString     = (GString*)NULL;


  /*
   * Compute now the period (expressed in milli-seconds that
   * must be given to the GTK timer that performs steps on plots.
   * Each graphic plot is asked its own period, then the minimum
   * period is given to the GTK timer.
   */
  kernel->stepTimerPeriod   = G_MAXINT; /* will be overwritten */
  kernel->stepGlobalCycle   = 0;
  kernel->startSamplingTime = (time_t)NULL;
  kernel->stopSamplingTime  = (time_t)NULL;

  gdisp_loopOnGraphicPlots(kernel,
			   gdisp_computeTimerPeriod,
			   (void*)&kernel->stepTimerPeriod);

  if (kernel->stepTimerPeriod == G_MAXINT /* no plot created */) {

    /*
     * Error message.
     */
    messageString =
      g_string_new("You should create a graphic plot before sampling...");
    (*kernel->outputFunc)(kernel,messageString,GD_ERROR);

    return FALSE;

  }
  
  if (kernel->stepTimerPeriod < GD_TIMER_MIN_PERIOD) {

    /*
     * GTK is not precise enough... it is not real time...
     * 100 milli-seconds is a minimum minimorum.
     */
    kernel->stepTimerPeriod = GD_TIMER_MIN_PERIOD;

  }
  
  gdisp_loopOnGraphicPlots(kernel,
			   gdisp_computePlotCycles,
			   (void*)&kernel->stepTimerPeriod);

  messageString = g_string_new((gchar*)NULL);
  g_string_sprintf(messageString,
		   "Step process period is %d milli-seconds.",
		   kernel->stepTimerPeriod);
  (*kernel->outputFunc)(kernel,messageString,GD_MESSAGE);


  /*
   * Before starting anything, ask all graphic plots if they
   * are ready to perform real-time steps.
   * If one answer is NO (ie FALSE), abort operation.
   */
  gdisp_loopOnGraphicPlots(kernel,
			   gdisp_startStepOnOneGraphicPlot,
			   (void*)&allPlotsReady);

  if (allPlotsReady == FALSE) {

    /*
     * Cancel any start-step side effects.
     */
    gdisp_loopOnGraphicPlots(kernel,
			     gdisp_stopStepOnOneGraphicPlot,
			     (void*)NULL);


    /*
     * Error message.
     */
    messageString =
      g_string_new("Cannot start dynamic plot process");
    (*kernel->outputFunc)(kernel,messageString,GD_ERROR);

    messageString =
      g_string_new("because one or more plots are unabled to proceed.");
    (*kernel->outputFunc)(kernel,messageString,GD_ERROR);

    return FALSE;

  }


  /*
   * Create the thread that will handle TSP symbol pre-sampling,
   * according to symbols that have been dropped on plots.
   * The pre-sampling phase consists in allocating memory for symbols
   * that must be sampled, assigning symbols to providers, creating
   * a sampling thread for each provider, and launching a GTK timer
   * for managing steps on graphic plots.
   */
  threadStatus = gdisp_createThread(kernel,
				    "Pre-sampling",
				    &preSamplingThread,
				    (const pthread_attr_t*)NULL,
				    gdisp_preSamplingThread,
				    (void*)kernel,
				    TRUE /* thread is detached */,
				    TRUE /* verbose            */);

  if (threadStatus == GD_THREAD_ERROR) {

    return FALSE;

  }


  /*
   * Create the garbage collector thread that will release
   * memory of symbols that are no longer in use.
   * CAUTION : this thread must be detached in order to release
   * ressouces, because no other thread will wait for it (pthread_join).
   */
  if (kernel->garbageCollectorThread == (pthread_t)NULL) {

    threadStatus = gdisp_createThread(kernel,
				      "Garbage collector",
				      &kernel->garbageCollectorThread,
				      (const pthread_attr_t*)NULL,
				      gdisp_garbageCollectorThread,
				      (void*)kernel,
				      TRUE /* thread is detached */,
				      TRUE /* verbose            */);

  } /* test whether garbage collector thread already exists */


  /*
   * Record the time when the sampling process begins.
   */
  kernel->startSamplingTime = time((time_t*)NULL);
  kernel->stopSamplingTime  = kernel->startSamplingTime;

  return TRUE;

}


/*
 * Stop everything that deals with sampling process.
 */
void
gdisp_stopSamplingProcess (Kernel_T *kernel)
{

  /*
   * Stop the timer that performs steps on graphic plots.
   */
  if (kernel->stepTimerIdentity > 0) {

    gtk_timeout_remove(kernel->stepTimerIdentity);
    kernel->stepTimerIdentity = 0;

  }


  /*
   * Record the time when the sampling process stops.
   */
  if (kernel->startSamplingTime != (time_t)NULL) {
    kernel->stopSamplingTime = time((time_t)NULL);
  }


  /*
   * Stop sampling threads.
   * Do not wait (pthread_join) for those provider threads,
   * because they are detached.
   */
  kernel->samplingThreadMustExit = TRUE;
  gdisp_uSleep(_ONE_SECOND_IN_MICROSECONDS_ / 2);


  /*
   * Tell all plots that step-process has stopped.
   */
  gdisp_loopOnGraphicPlots(kernel,
			   gdisp_stopStepOnOneGraphicPlot,
			   (void*)NULL);

}


/*
 * Build PGI hash table.
 */
gboolean
gdisp_buildProviderPgiHashTable ( Provider_T         *provider,
				  const SampleList_T *sampleList )
{

  hash_t                   *tmpHashTable = (hash_t*)NULL;
  Symbol_T                 *symbol       = (Symbol_T*)NULL;
  guint                     sampleCpt    = 0;
  TSP_sample_symbol_info_t *symbolInfo   = (TSP_sample_symbol_info_t*)NULL;
  gchar            samplePGIasStringBuffer[GD_SAMPLE_PGI_AS_STRING_LENGTH];


#if defined(GD_PRINT_SAMPLE_LIST)
 {

   symbolInfo = sampleList->TSP_sample_symbol_info_list_t_val;

   printf("--------------- REQUEST INIT AND SAMPLE  -------------------\n");

   for (sampleCpt=0;
	sampleCpt<sampleList->TSP_sample_symbol_info_list_t_len;
	sampleCpt++, symbolInfo++) {

     printf("%s\n",symbolInfo->name);
     printf("  pGlobalIndex %d, pGroupIndex %d, pGroupRank %d\n",
	    symbolInfo->provider_global_index,
	    symbolInfo->provider_group_index,
	    symbolInfo->provider_group_rank);
     printf("  Type %d, Dimension %d, Offset %d, nElem %d\n",
	    symbolInfo->type,
	    symbolInfo->dimension,
	    symbolInfo->offset,
	    symbolInfo->nelem);
     printf("  Period %d, Phase %d\n",
	    symbolInfo->period,
	    symbolInfo->phase);

   }

 }
#endif


  /*
   * Destroy previous hash table.
   */
  if (provider->pSymbolHashTablePGI != (hash_t*)NULL) {

    tmpHashTable = provider->pSymbolHashTablePGI;
    provider->pSymbolHashTablePGI = (hash_t*)NULL;
    hash_close(tmpHashTable);

  }


  /*
   * And create new one.
   */
  tmpHashTable = hash_open('.','z');
  symbolInfo   = sampleList->TSP_sample_symbol_info_list_t_val;


  /*
   * Insert symbol into the hash table.
   */
  for (sampleCpt=0;
       sampleCpt<sampleList->TSP_sample_symbol_info_list_t_len;
       sampleCpt++, symbolInfo++) {

    symbol = (Symbol_T*)hash_get(provider->pSymbolHashTable,
				 symbolInfo->name);

    symbol->sPgi = symbolInfo->provider_global_index;

    /* I can use 'sprintf' because sampling has not started yet */
    sprintf(samplePGIasStringBuffer,
	    "%d",
	    symbol->sPgi);
    
    hash_append(tmpHashTable,
		samplePGIasStringBuffer,
		(void*)symbol);

  }

  provider->pSymbolHashTablePGI = tmpHashTable;

  return TRUE;

}


/*
 * All symbols that must be plotted do not belong to the same
 * provider. So give back requested symbols to the provider each
 * of them belongs to.
 */
void
gdisp_affectRequestedSymbolsToProvider ( Kernel_T *kernel )
{

  GArray       *requestedSymbolArray = (GArray*)NULL;
  GList        *providerItem         = (GList*)NULL;
  Provider_T   *provider             = (Provider_T*)NULL;
  GList        *symbolList           = (GList*)NULL;
  GList        *symbolItem           = (GList*)NULL;
  Symbol_T     *symbol               = (Symbol_T*)NULL;
  guint         elementSize          = 0;
  SampleList_T *pSampleListPtr       = (SampleList_T*)NULL;
  gboolean      errorCode            = FALSE;

  /*
   * Get back requested symbols from graphic pages.
   */
  symbolList = gdisp_getSymbolsInPages(kernel);


  /*
   * Loop over all providers, cancel any previous sampling
   * configuration, attach new symbols if any.
   */
  providerItem = g_list_first(kernel->providerList);
  while (providerItem != (GList*)NULL) {

    provider       = (Provider_T*)providerItem->data;
    pSampleListPtr = &provider->pSampleList;

    if (provider->pStatus == GD_SESSION_OPENED) {

      /*
       * Cancel any previous sampling configuration.
       */
      pSampleListPtr->TSP_sample_symbol_info_list_t_len = 0;
      if (pSampleListPtr->TSP_sample_symbol_info_list_t_val != (TSP_sample_symbol_info_t*)NULL)
	g_free(pSampleListPtr->TSP_sample_symbol_info_list_t_val);
      pSampleListPtr->TSP_sample_symbol_info_list_t_val = (TSP_sample_symbol_info_t*)NULL;


      /*
       * Temporary ressource.
       */
      elementSize = sizeof(TSP_sample_symbol_info_t);
      requestedSymbolArray = g_array_new(FALSE, /* zero_terminated */
					 TRUE,  /* clear           */
					 (guint)elementSize);


      /*
       * Loop on every symbol that must be sampled.
       */
      symbolItem = g_list_first(symbolList);
      while (symbolItem != (GList*)NULL) {

	symbol = (Symbol_T*)symbolItem->data;

	/*
	 * The symbol belongs to the current provider because its
	 * address is in the good address interval.
	 */
	if (provider->pSymbolList <= symbol &&
	    symbol < provider->pSymbolList + provider->pSymbolNumber) {

	  g_array_append_val(requestedSymbolArray,symbol->sInfo);

	  /* CAUTION
	   * The 'sInfo' structure is going to be passed to the
	   * 'TSP_consummer_request_sample' function.
	   * Thus, the name will probably be freed by TSP.
	   * Forget the current reference by duplicating the string.
	   */
	  symbol->sInfo.name = gdisp_strDup(symbol->sInfo.name);

	}

	symbolItem = g_list_next(symbolItem);

      }


      /*
       * Transfer information to provider.
       */
      pSampleListPtr->TSP_sample_symbol_info_list_t_val =
                         (TSP_sample_symbol_info_t*)requestedSymbolArray->data;
      pSampleListPtr->TSP_sample_symbol_info_list_t_len =
                                                     requestedSymbolArray->len;


      /*
       * Free temporary ressource.
       * Free the table, not the content.
       */
      g_array_free(requestedSymbolArray,FALSE);


      /*
       * Request sample.
       */
      errorCode = gdisp_requestSample(kernel,
				      provider);


    } /* provider->pStatus == GD_SESSION_OPENED */

    providerItem = g_list_next(providerItem);

  }


  /*
   * Release ressource.
   */
  g_list_free(symbolList);

}


/*
 * Async Write Method.
 * Returns TRUE if success, FALSE otherwise.
 */
gboolean
gdisp_asyncWriteSymbol ( Kernel_T *kernel,
			 Symbol_T *symbol,
			 gchar    *valueAsString )

{

  TSP_consumer_async_sample_t  asyncSample;
  Provider_T                  *provider    = (Provider_T*)NULL;
  gint                         errorCode   = 0;
  gdouble                      doubleValue = 0.0;

  /*
   * Check if an async-write operation is allowed.
   */
  if (kernel->asyncWriteIsAllowed == FALSE) {
    return FALSE;
  }

  /*
   * Try to get back the provider the symbol belongs to.
   */
  provider = gdisp_getProviderBySymbol(kernel,
				       symbol);

  if (provider == (Provider_T*)NULL) {
    return FALSE;
  }

  /*
   * Async Write.
   */
  doubleValue = atof(valueAsString);

  asyncSample.provider_global_index = symbol->sPgi;
  asyncSample.value_ptr             = &doubleValue;
  asyncSample.value_size            = sizeof(doubleValue);

  errorCode =
    TSP_consumer_request_async_sample_write(provider->pHandle,
					    &asyncSample);

  return (errorCode == TSP_STATUS_OK ? TRUE : FALSE);

}
