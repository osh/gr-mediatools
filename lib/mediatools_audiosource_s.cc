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

#include <gr_io_signature.h>
#include <mediatools_audiosource_s.h>


mediatools_audiosource_s_sptr
mediatools_make_audiosource_s (std::vector<std::string> args)
{
	return mediatools_audiosource_s_sptr (new mediatools_audiosource_s (args));
}


mediatools_audiosource_s::mediatools_audiosource_s (std::vector<std::string> args)
	: gr_sync_block ("audiosource_s",
		gr_make_io_signature (0, 0, 0),
		gr_make_io_signature (1,1, sizeof (int16_t))),
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
	const int16_t *in = (const int16_t *) input_items[0];
	int16_t *out = (int16_t *) output_items[0];

	// Do <+signal processing+>
    while(d_data.size() < noutput_items){
    // make sure we have a file open
        while(d_impl->d_ready == false){
            if(d_list.size()<1){
                printf("reached end of pl\n");
                // possibly branch to an idle state here to dump 0's instead of exiting?
                return 0;
            }
        d_impl->open(d_list[0]);
        d_list.erase(d_list.begin(), d_list.begin()+1);
    }

    // grab new data and output
    d_impl->readData(d_data);
  }

    
	// Tell runtime system how many output items we produced.
	return noutput_items;
}

