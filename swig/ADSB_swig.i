/* -*- c++ -*- */

#define ADSB_API

%include "gnuradio.i"			// the common stuff

//load generated python docstrings
%include "ADSB_swig_doc.i"

%{
#include "ADSB/adsbSingleMsgGen_b.h"
%}


%include "ADSB/adsbSingleMsgGen_b.h"
GR_SWIG_BLOCK_MAGIC2(ADSB, adsbSingleMsgGen_b);
