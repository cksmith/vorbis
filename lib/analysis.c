/********************************************************************
 *                                                                  *
 * THIS FILE IS PART OF THE Ogg Vorbis SOFTWARE CODEC SOURCE CODE.  *
 * USE, DISTRIBUTION AND REPRODUCTION OF THIS SOURCE IS GOVERNED BY *
 * THE GNU PUBLIC LICENSE 2, WHICH IS INCLUDED WITH THIS SOURCE.    *
 * PLEASE READ THESE TERMS DISTRIBUTING.                            *
 *                                                                  *
 * THE OggSQUISH SOURCE CODE IS (C) COPYRIGHT 1994-2000             *
 * by Monty <monty@xiph.org> and The XIPHOPHORUS Company            *
 * http://www.xiph.org/                                             *
 *                                                                  *
 ********************************************************************

 function: single-block PCM analysis mode dispatch
 last mod: $Id: analysis.c,v 1.24.4.1 2000/04/01 12:51:32 xiphmont Exp $

 ********************************************************************/

#include <stdio.h>
#include <string.h>
#include <math.h>
#include "vorbis/codec.h"
#include "bitwise.h"
#include "registry.h"

/* decides between modes, dispatches to the appropriate mapping. */
int vorbis_analysis(vorbis_block *vb,ogg_packet *op){
  vorbis_dsp_state *vd=vb->vd;
  vorbis_info      *vi=vd->vi;
  int              type;
  int              mode=0;

  vb->glue_bits=0;
  vb->time_bits=0;
  vb->floor_bits=0;
  vb->res_bits=0;

  /* first things first.  Make sure encode is ready */
  _oggpack_reset(&vb->opb);
  /* Encode the packet type */
  _oggpack_write(&vb->opb,0,1);

  /* currently lazy.  Short block dispatches to 0, long to 1. */

  if(vb->W &&vi->modes>1)mode=1;
  type=vi->map_type[vi->mode_param[mode]->mapping];
  vb->mode=mode;

  /* Encode frame mode, pre,post windowsize, then dispatch */
  _oggpack_write(&vb->opb,mode,vd->modebits);
  if(vb->W){
    _oggpack_write(&vb->opb,vb->lW,1);
    _oggpack_write(&vb->opb,vb->nW,1);
  }

  if(_mapping_P[type]->forward(vb,vd->mode[mode]))
    return(-1);

  /* set up the packet wrapper */

  op->packet=_oggpack_buffer(&vb->opb);
  op->bytes=_oggpack_bytes(&vb->opb);
  op->b_o_s=0;
  op->e_o_s=vb->eofflag;
  op->frameno=vb->frameno;
  op->packetno=vb->sequence; /* for sake of completeness */

  return(0);
}
