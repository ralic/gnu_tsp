/*!  \file 

$Id: gdisp_kernel.c,v 1.1 2004/02/04 20:32:09 esteban Exp $

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
Maintainer: tsp@astrium-space.com
Component : Graphic Tool

-----------------------------------------------------------------------

Purpose   : Graphic Tool based on GTK+ that provide several kinds of
            plot. This tool is to be used with the generic TSP protocol.

File      : Graphic Tool Kernel Implementation.
            Kernel allocation.

-----------------------------------------------------------------------
*/


/*
 * System includes.
 */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <pthread.h>
#include <errno.h>


/*
 * GDISP+ includes.
 */
#include "gdisp_kernel.h"
#include "gdisp_prototypes.h"


/*
 --------------------------------------------------------------------
                             STATIC ROUTINES
 --------------------------------------------------------------------
*/

static void*
gdisp_mutexNew (void)
{

  guint            status = 0;
  pthread_mutex_t *mutex  = (pthread_mutex_t*)NULL;

  mutex = (pthread_mutex_t*)g_malloc0(sizeof(pthread_mutex_t));
  assert(mutex);

  status = pthread_mutex_init(mutex,(pthread_mutexattr_t*)NULL);

  return (void*)mutex;

}


static void
gdisp_mutexLock (Kernel_T *kernel,
		 void     *mutex)
{

  guint    status = 0;
  GString *error  = (GString*)NULL;

  status = pthread_mutex_lock((pthread_mutex_t*)mutex);
  switch (status) {

  case EINVAL :
    error = g_string_new("The mutex has not been properly initialized.");
    kernel->outputFunc(kernel,error,GD_ERROR);
    break;

  case EDEADLK :
    error = g_string_new("The mutex is already locked by the calling thread.");
    kernel->outputFunc(kernel,error,GD_ERROR);
    break;

  default :
    break;

  }

}


static gboolean
gdisp_mutexTrylock (Kernel_T *kernel,
		    void     *mutex)
{

  guint     status  = 0;
  gboolean  gStatus = TRUE;
  GString  *error   = (GString*)NULL;

  status = pthread_mutex_trylock((pthread_mutex_t*)mutex);
  switch (status) {

  case EINVAL :
    error = g_string_new("The mutex has not been properly initialized.");
    kernel->outputFunc(kernel,error,GD_ERROR);
    gStatus = FALSE;
    break;

  case EBUSY  :
    error = g_string_new("The mutex could not be acquired"
			 "because it was currently locked.");
    kernel->outputFunc(kernel,error,GD_ERROR);
    gStatus = FALSE;
    break;

  default :
    gStatus = TRUE;
    break;

  }

  return gStatus;

}


static void
gdisp_mutexUnlock (Kernel_T *kernel,
		   void     *mutex)
{

  guint    status = 0;
  GString *error  = (GString*)NULL;

  status = pthread_mutex_unlock((pthread_mutex_t*)mutex);
  switch (status) {

  case EINVAL :
    error = g_string_new("The mutex has not been properly initialized.");
    kernel->outputFunc(kernel,error,GD_ERROR);
    break;

  case EPERM :
    error = g_string_new("The calling thread does not own the mutex");
    kernel->outputFunc(kernel,error,GD_ERROR);
    break;

  default :
    break;

  }

}


static void
gdisp_mutexFree (Kernel_T *kernel,
		 void     *mutex)
{

  guint    status = 0;
  GString *error  = (GString*)NULL;

  status = pthread_mutex_destroy((pthread_mutex_t*)mutex);
  switch (status) {

  case EBUSY :
    error = g_string_new("The mutex is currently locked.");
    kernel->outputFunc(kernel,error,GD_ERROR);
    return;
    break;

  default :
    break;

  }

  memset(mutex,0,sizeof(pthread_mutex_t));
  g_free(mutex);

}


/*
 --------------------------------------------------------------------
                             PUBLIC ROUTINES
 --------------------------------------------------------------------
*/


/*
 * Allocate memory for GDISP+ Kernel.
 */
Kernel_T*
gdisp_createKernel (gint    argc,
		   gchar **argv)
{

  Kernel_T        *kernel          =       (Kernel_T*)NULL;
  PlotSystem_T    *plotSystem      =   (PlotSystem_T*)NULL;
  PlotType_T       plotType        =       GD_PLOT_DEFAULT;
  guint            functionSetSize =                     0;
  guint            functionCpt     =                     0;
  guint            functionNb      =                     0;
  FunctionTable_T  functionTable   = (FunctionTable_T)NULL;

  /*
   * Memory allocation.
   */
  kernel = (Kernel_T*)g_malloc0(sizeof(Kernel_T));
  assert(kernel);

  /*
   * Defaults.
   */
  kernel->isThreadSafe  = FALSE;
  kernel->sortingMethod = GD_SORT_BY_PROVIDER;
  kernel->dndScope      = GD_DND_UNICAST;
  kernel->argCounter    = argc;
  kernel->argTable      = argv;
  kernel->timerPeriod   = 100; /* milli-seconds */

  /*
   * Try to know whether a multi-threaded environment is available ?
   */

  /*
   * We must provide our own thread-safe system.
   */
  kernel->mutexNew     = gdisp_mutexNew;
  kernel->mutexLock    = gdisp_mutexLock;
  kernel->mutexTrylock = gdisp_mutexTrylock;
  kernel->mutexUnlock  = gdisp_mutexUnlock;
  kernel->mutexFree    = gdisp_mutexFree;

  /*
   * Use this system to create a mutex.
   */
  kernel->isThreadSafe = TRUE;

  /*
   * Initialise all fonts.
   */
  gdisp_loadFonts(kernel->fonts);


  /*
   * Initialise all plot systems.
   * Each plot system that is supported may provide several functions.
   */

  /* Remove size of 'psIsSupported' */
  functionSetSize = sizeof(PlotSystem_T) - sizeof(gboolean);
  functionNb      = functionSetSize / sizeof(aFunction_T);

  for (plotType=GD_PLOT_DEFAULT; plotType<GD_MAX_PLOT; plotType++) {

    plotSystem = &kernel->plotSystems[plotType];
    switch (plotType) {

    case GD_PLOT_DEFAULT :
      gdisp_initDefaultPlotSystem(kernel,plotSystem);
      break;

    case GD_PLOT_2D :
      gdisp_initPlot2DSystem(kernel,plotSystem);
      break;

    default :
      break;

    }

    /*
     * We must check out, at kernel level, that all functions have been
     * initialised by each plot system.
     */

    /* By default, the plot system is supported */
    plotSystem->psIsSupported = TRUE;
    functionTable             = (FunctionTable_T)plotSystem;

    for (functionCpt=0; functionCpt<functionNb; functionCpt++) {

      if (functionTable[functionCpt] == (aFunction_T)NULL) {

	plotSystem->psIsSupported = FALSE;

      } /* if */

    } /* for 'functionCpt' */

  } /* for 'plotType' */

  /*
   * Return the kernel itself.
   */
  return kernel;

}


/*
 * Free memory used by GDISP+ Kernel.
 */
void
gdisp_destroyKernel (Kernel_T *kernel)
{

  assert(kernel);

  /*
   * Initialise all fonts.
   */
  gdisp_destroyFonts(kernel->fonts);

  /*
   * Destroy our colormap.
   */
  gdisp_destroyColormap(kernel);

  /*
   * Free Kernel.
   */
  memset(kernel,0,sizeof(Kernel_T));
  g_free(kernel);

}

