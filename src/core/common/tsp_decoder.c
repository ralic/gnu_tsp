/*

$Header: /sources/tsp/tsp/src/core/common/tsp_decoder.c,v 1.1 2006/03/31 12:59:07 erk Exp $

-----------------------------------------------------------------------

TSP Library - core components for a generic Transport Sampling Protocol.

Copyright (c) 2002 Eric Noulard et Arnaud MORVAN 

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
Component : Provider

-----------------------------------------------------------------------

Purpose   :  Implementation for the functions used to decode the data received

-----------------------------------------------------------------------
 */


#include <rpc/types.h>
#include <rpc/xdr.h>
 
#include <tsp_const_def.h>
#include <tsp_datastruct.h>
#include <tsp_simple_trace.h>
#include <tsp_decoder.h>


int32_t TSP_data_channel_double_decoder(void* out_double, uint32_t dimension,  char* in_buf)
{
  uint32_t i;


#ifndef TSP_NO_XDR_ENCODE

  XDR xhandle;
  xdrmem_create(&xhandle, in_buf, TSP_SIZEOF_ENCODED_DOUBLE, XDR_DECODE);

  for(i=0;i<dimension;++i)
  {
    if( xdr_double(&xhandle, (double*)out_double+i) != TRUE)
    {
      STRACE_ERROR(("Function xdr_double failed"));
      return FALSE;
    }
    else
    {
      return TRUE;
    }
  }

#else
  
  for(i=0;i<dimension;++i)
  {
    ((uint64_t*)out_double)[i] = TSP_DECODE_DOUBLE_TO_UINT64(in_buf+(i*TSP_SIZEOF_ENCODED_DOUBLE));   
    STRACE_DEBUG(("decoded DOUBLE = %f, (received) encoded DOUBLE=0x%08x",((double*)out_double)[i],((uint64_t*)in_buf)[i]));
  }
  
  return TRUE;

#endif

}

int32_t TSP_data_channel_float_decoder(void* out_float, uint32_t dimension,  char* in_buf)
{
  uint32_t i;


#ifndef TSP_NO_XDR_ENCODE

  XDR xhandle;
  xdrmem_create(&xhandle, in_buf, TSP_SIZEOF_ENCODED_FLOAT, XDR_DECODE);

  for(i=0;i<dimension;++i)
  {
    if( xdr_float(&xhandle, (float*)out_float+i) != TRUE)
    {
      STRACE_ERROR(("Function xdr_float failed"));
      return FALSE;
    }
    else
    {
      return TRUE;
    }
  }

#else
  
  for(i=0;i<dimension;++i)
  {

    ((uint32_t*)out_float)[i] = TSP_DECODE_FLOAT_TO_UINT32(in_buf+(i* TSP_SIZEOF_ENCODED_FLOAT));   
  }
  
  return TRUE;

#endif
}

int32_t TSP_data_channel_int8_decoder(void* out_int8, uint32_t dimension,  char* in_buf)
{

#ifndef TSP_NO_XDR_ENCODE

  uint32_t i;
  XDR xhandle;
  xdrmem_create(&xhandle, in_buf, TSP_SIZEOF_ENCODED_INT8, XDR_DECODE);

  for(i=0;i<dimension;++i)
  {
    if( xdr_opaque(&xhandle, (char *)out_int8+i,1) != TRUE)
    {
      STRACE_ERROR(("Function xdr_int failed"));
      return FALSE;
    }
    else
    {
      return TRUE;
    }
  }

#else
  memcpy(in_buf,out_int8,TSP_SIZEOF_ENCODED_INT8 * dimension);
   
  return TRUE;

#endif
}

int32_t TSP_data_channel_int16_decoder(void* out_int16, uint32_t dimension,  char* in_buf)
{
  uint32_t i;


#ifndef TSP_NO_XDR_ENCODE

  XDR xhandle;
  xdrmem_create(&xhandle, in_buf, TSP_SIZEOF_ENCODED_INT16, XDR_DECODE);

  for(i=0;i<dimension;++i)
  {
    if( xdr_opaque(&xhandle, (char *)out_int16+i,2) != TRUE)
    {
      STRACE_ERROR(("Function xdr_float failed"));
      return FALSE;
    }
    else
    {
      return TRUE;
    }
  }

#else
  
  for(i=0;i<dimension;++i)
  {

    ((int16_t*)out_int16)[i] = TSP_DECODE_INT16(in_buf+(i * TSP_SIZEOF_ENCODED_INT16));   
  }
  
  return TRUE;

#endif
}


int32_t TSP_data_channel_int32_decoder(void* out_int32, uint32_t dimension,  char* in_buf)
{
  uint32_t i;


#ifndef TSP_NO_XDR_ENCODE

  XDR xhandle;
  xdrmem_create(&xhandle, in_buf, TSP_SIZEOF_ENCODED_INT32, XDR_DECODE);

  for(i=0;i<dimension;++i)
  {
    if( xdr_int(&xhandle, (int32_t*)out_int32+i) != TRUE)
    {
      STRACE_ERROR(("Function xdr_float failed"));
      return FALSE;
    }
    else
    {
      return TRUE;
    }
  }

#else
  
  for(i=0;i<dimension;++i)
  {

    ((int32_t*)out_int32)[i] = TSP_DECODE_INT32(in_buf+(i * TSP_SIZEOF_ENCODED_INT32));   
  }
  
  return TRUE;

#endif
}

int32_t TSP_data_channel_int64_decoder(void* out_int64, uint32_t dimension,  char* in_buf)
{
  uint32_t i;


#ifndef TSP_NO_XDR_ENCODE

  XDR xhandle;
  xdrmem_create(&xhandle, in_buf, TSP_SIZEOF_ENCODED_INT64, XDR_DECODE);

  for(i=0;i<dimension;++i)
  {
    if( xdr_long(&xhandle, (int64_t*)out_int64+i) != TRUE)
    {
      STRACE_ERROR(("Function xdr_float failed"));
      return FALSE;
    }
    else
    {
      return TRUE;
    }
  }

#else
  
  for(i=0;i<dimension;++i)
  {

    ((int64_t*)out_int64)[i] = TSP_DECODE_INT64(in_buf+(i * TSP_SIZEOF_ENCODED_INT64));   
  }
  
  return TRUE;

#endif
}

int32_t TSP_data_channel_uint8_decoder(void* out_uint8, uint32_t dimension,  char* in_buf)
{

#ifndef TSP_NO_XDR_ENCODE

  uint32_t i;
  XDR xhandle;
  xdrmem_create(&xhandle, in_buf, TSP_SIZEOF_ENCODED_UINT8, XDR_DECODE);

  for(i=0;i<dimension;++i)
  {
    if( xdr_opaque(&xhandle, (char*)out_uint8+i,1) != TRUE)
    {
      STRACE_ERROR(("Function xdr_u_int failed"));
      return FALSE;
    }
    else
    {
      return TRUE;
    }
  }

#else
  memcpy(in_buf,out_uint8,TSP_SIZEOF_ENCODED_UINT8 * dimension);
   
  return TRUE;

#endif
}

int32_t TSP_data_channel_uint16_decoder(void* out_uint16, uint32_t dimension,  char* in_buf)
{
  uint32_t i;


#ifndef TSP_NO_XDR_ENCODE

  XDR xhandle;
  xdrmem_create(&xhandle, in_buf, TSP_SIZEOF_ENCODED_UINT16, XDR_DECODE);

  for(i=0;i<dimension;++i)
  {
    if( xdr_opaque(&xhandle, (char*)out_uint16+i,2) != TRUE)
    {
      STRACE_ERROR(("Function xdr_float failed"));
      return FALSE;
    }
    else
    {
      return TRUE;
    }
  }

#else
  
  for(i=0;i<dimension;++i)
  {

    ((uint16_t*)out_uint16)[i] = TSP_DECODE_UINT16(in_buf+(i * TSP_SIZEOF_ENCODED_UINT16));   
  }
  
  return TRUE;

#endif
}

int32_t TSP_data_channel_uint32_decoder(void* out_uint32, uint32_t dimension,  char* in_buf)
{
  uint32_t i;


#ifndef TSP_NO_XDR_ENCODE

  XDR xhandle;
  xdrmem_create(&xhandle, in_buf, TSP_SIZEOF_ENCODED_UINT32, XDR_DECODE);

  for(i=0;i<dimension;++i)
  {
    if( xdr_int(&xhandle, (uint32_t*)out_uint32+i) != TRUE)
    {
      STRACE_ERROR(("Function xdr_float failed"));
      return FALSE;
    }
    else
    {
      return TRUE;
    }
  }

#else
  
  for(i=0;i<dimension;++i)
  {

    ((uint32_t*)out_uint32)[i] = TSP_DECODE_UINT32(in_buf+(i * TSP_SIZEOF_ENCODED_UINT32));   
  }
  
  return TRUE;

#endif
}

int32_t TSP_data_channel_uint64_decoder(void* out_uint64, uint32_t dimension,  char* in_buf)
{
  uint32_t i;


#ifndef TSP_NO_XDR_ENCODE

  XDR xhandle;
  xdrmem_create(&xhandle, in_buf, TSP_SIZEOF_ENCODED_UINT64, XDR_DECODE);

  for(i=0;i<dimension;++i)
  {
    if( xdr_long(&xhandle, (uint64_t*)out_uint64+i) != TRUE)
    {
      STRACE_ERROR(("Function xdr_float failed"));
      return FALSE;
    }
    else
    {
      return TRUE;
    }
  }

#else
  
  for(i=0;i<dimension;++i)
  {

    ((uint64_t*)out_uint64)[i] = TSP_DECODE_UINT64(in_buf+(i * TSP_SIZEOF_ENCODED_UINT64));   
  }
  
  return TRUE;

#endif
}

int32_t TSP_data_channel_char_decoder(void* out_char, uint32_t dimension,  char* in_buf)
{

#ifndef TSP_NO_XDR_ENCODE

  uint32_t i;
  XDR xhandle;
  xdrmem_create(&xhandle, in_buf, TSP_SIZEOF_ENCODED_UINT8, XDR_DECODE);

  for(i=0;i<dimension;++i)
  {
    if( xdr_bytes(&xhandle, &((char*)out_char+i),dimension,dimension) != TRUE)
    {
      STRACE_ERROR(("Function xdr_u_int failed"));
      return FALSE;
    }
    else
    {
      return TRUE;
    }
  }

#else
  memcpy(in_buf,out_char,TSP_SIZEOF_ENCODED_UINT8 * dimension);
   
  return TRUE;

#endif
}

int32_t TSP_data_channel_uchar_decoder(void* out_uchar, uint32_t dimension,  char* in_buf)
{

#ifndef TSP_NO_XDR_ENCODE

  uint32_t i;
  XDR xhandle;
  xdrmem_create(&xhandle, in_buf, TSP_SIZEOF_ENCODED_UINT8, XDR_DECODE);

  for(i=0;i<dimension;++i)
  {
    if( xdr_bytes(&xhandle,&( (char*)out_uchar+i),dimension,dimension) != TRUE)
    {
      STRACE_ERROR(("Function xdr_u_int failed"));
      return FALSE;
    }
    else
    {
      return TRUE;
    }
  }

#else
  memcpy(in_buf,out_uchar,TSP_SIZEOF_ENCODED_UINT8 * dimension);
   
  return TRUE;

#endif
}

int32_t TSP_data_channel_user_decoder(void* out_user, uint32_t dimension,  char* in_buf)
{

#ifndef TSP_NO_XDR_ENCODE

  uint32_t i;
  XDR xhandle;
  xdrmem_create(&xhandle, in_buf, TSP_SIZEOF_ENCODED_UINT8, XDR_DECODE);

  for(i=0;i<dimension;++i)
  {
    if( xdr_opaque(&xhandle,(char*)out_user+i,dimension) != TRUE)
    {
      STRACE_ERROR(("Function xdr_u_int failed"));
      return FALSE;
    }
    else
    {
      return TRUE;
    }
  }

#else
  memcpy(in_buf,out_user,TSP_SIZEOF_ENCODED_UINT8 * dimension);
   
  return TRUE;

#endif
}



TSP_data_decoder_t TSP_data_channel_get_decoder(TSP_datatype_t type)
{

  switch(type)
  {
    case TSP_TYPE_DOUBLE :
      return TSP_data_channel_double_decoder;
    
    case TSP_TYPE_FLOAT :
      return TSP_data_channel_float_decoder;

    case TSP_TYPE_INT8 :
      return  TSP_data_channel_int8_decoder;

    case TSP_TYPE_INT16:
      return TSP_data_channel_int16_decoder;

    case TSP_TYPE_INT32 :
      return TSP_data_channel_int32_decoder;

    case TSP_TYPE_INT64 :
      return TSP_data_channel_int64_decoder;

    case TSP_TYPE_UINT8:
      return TSP_data_channel_uint8_decoder;

    case TSP_TYPE_UINT16:
      return TSP_data_channel_uint16_decoder;

    case TSP_TYPE_UINT32:
      return TSP_data_channel_uint32_decoder;

    case TSP_TYPE_UINT64:
      return  TSP_data_channel_uint64_decoder;

    case TSP_TYPE_CHAR:
      return TSP_data_channel_char_decoder;

    case TSP_TYPE_UCHAR:
      return TSP_data_channel_uchar_decoder;

    case TSP_TYPE_RAW:
      return TSP_data_channel_user_decoder;

    default:
      return NULL;

  }
}

int32_t TSP_data_channel_get_encoded_size(TSP_datatype_t type)
{
  
  switch(type)
  {

     case TSP_TYPE_DOUBLE :
       return TSP_SIZEOF_ENCODED_DOUBLE;
    
     case TSP_TYPE_FLOAT :
       return TSP_SIZEOF_ENCODED_FLOAT;

     case TSP_TYPE_INT8 :
       return TSP_SIZEOF_ENCODED_INT8;

     case TSP_TYPE_INT16:
       return TSP_SIZEOF_ENCODED_INT16;

     case TSP_TYPE_INT32 :
       return TSP_SIZEOF_ENCODED_INT32;

     case TSP_TYPE_INT64 :
       return TSP_SIZEOF_ENCODED_INT64;

     case TSP_TYPE_UINT8:
       return TSP_SIZEOF_ENCODED_UINT8;

     case TSP_TYPE_UINT16:
       return TSP_SIZEOF_ENCODED_UINT16;

     case TSP_TYPE_UINT32:
       return TSP_SIZEOF_ENCODED_UINT32;

     case TSP_TYPE_UINT64:
       return TSP_SIZEOF_ENCODED_UINT64;

     case TSP_TYPE_CHAR:
       return TSP_SIZEOF_ENCODED_CHAR;

     case TSP_TYPE_UCHAR:
       return TSP_SIZEOF_ENCODED_UCHAR;

     case TSP_TYPE_RAW:
       return TSP_SIZEOF_ENCODED_USER;

     default:
       return NULL;

  }
}

 