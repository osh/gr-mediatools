/* -*- c++ -*- */
/* 
 * Copyright 2012 <+YOU OR YOUR COMPANY+>.
 * 
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 * 
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this software; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gnuradio/io_signature.h>
#include <mediatools_audiosource_s.h>


mediatools_audiosource_s_sptr
mediatools_make_audiosource_s (std::vector<std::string> args)
{
	return mediatools_audiosource_s_sptr (new mediatools_audiosource_s (args));
}


mediatools_audiosource_s::mediatools_audiosource_s (std::vector<std::string> args)
	: gr::sync_block ("audiosource_s",
		gr::io_signature::make (0, 0, 0),
		gr::io_signature::make (1,1, sizeof (int16_t))),
    d_list(args)
{
    d_impl = new mediatools_audiosource_impl();
}


mediatools_audiosource_s::~mediatools_audiosource_s ()
{
}


int
mediatools_audiosource_s::work (int noutput_items,
			gr_vector_const_void_star &input_items,
			gr_vector_void_star &output_items)
{
	int16_t *out = (int16_t *) output_items[0];

    // make sure we have enough data to output
    while(d_data.size() < noutput_items){
        // keep trying to open files until we are ready
        while(d_impl->d_ready == false){
            // exit when we run out of files
            if(d_list.size()<1){
                printf("reached end of pl\n");
                // possibly branch to an idle state here to dump 0's instead of exiting?
                return -1;
            }
        d_impl->open(d_list[0]);
        d_list.erase(d_list.begin(), d_list.begin()+1);
    }

    // decode more data
    d_impl->readData(d_data);
  }

  if(!d_impl->d_meta.empty()){
    
    d_impl->d_meta.clear();
    }

  // copy data to our output buffer
  memcpy(out, &d_data[0], noutput_items*sizeof(int16_t));
  d_data.erase(d_data.begin(), d_data.begin()+noutput_items);
  return noutput_items;
}

