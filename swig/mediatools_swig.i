/* -*- c++ -*- */

#define MEDIATOOLS_API

%include "gnuradio.i"			// the common stuff
/*
%include "typemaps.i"
%include "std_string.i"
%include "std_vector.i"

namespace std
{
    %template(strvec) vector<string>;
}
*/


//load generated python docstrings
%include "mediatools_swig_doc.i"

%{
#include "mediatools_audiosource_s.h"
%}

GR_SWIG_BLOCK_MAGIC(mediatools,audiosource_s);
%include "mediatools_audiosource_s.h"

#if SWIGGUILE
%scheme %{
(load-extension-global "libguile-gnuradio-mediatools_swig" "scm_init_gnuradio_mediatools_swig_module")
%}

%goops %{
(use-modules (gnuradio gnuradio_core_runtime))
%}
#endif
