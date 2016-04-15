/* -*- c++ -*- */

#define SYNTHESIS_API

%include "gnuradio.i"			// the common stuff

//load generated python docstrings
%include "synthesis_swig_doc.i"

%{
#include "synthesis/synth_selector.h"
#include "synthesis/synth_pfb.h"
#include "synthesis/synthesizer_pfb_ccf.h"
%}


%include "synthesis/synth_selector.h"
GR_SWIG_BLOCK_MAGIC2(synthesis, synth_selector);
%include "synthesis/synth_pfb.h"
GR_SWIG_BLOCK_MAGIC2(synthesis, synth_pfb);
%include "synthesis/synthesizer_pfb_ccf.h"
GR_SWIG_BLOCK_MAGIC2(synthesis, synthesizer_pfb_ccf);
