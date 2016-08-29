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
#include <boost/foreach.hpp>
#include <boost/format.hpp>

mediatools_audiosource_s_sptr
mediatools_make_audiosource_s (std::vector<std::string> args)
{
	return mediatools_audiosource_s_sptr (new mediatools_audiosource_s (args));
}

mediatools_audiosource_s::mediatools_audiosource_s (std::vector<std::string> args)
	: gr::sync_block ("audiosource_s",
		gr::io_signature::make (0, 0, 0),
		gr::io_signature::make (1,1, sizeof (int16_t))),
    d_list(args), d_skip(false)
{
    d_impl = new mediatools_audiosource_impl();
    message_port_register_in(pmt::mp("enqueue"));
    message_port_register_in(pmt::mp("skip"));
    set_msg_handler(pmt::mp("enqueue"), boost::bind(&mediatools_audiosource_s::enqueue_pmt, this, _1));
    set_msg_handler(pmt::mp("skip"), boost::bind(&mediatools_audiosource_s::skip_pmt, this, _1));
    message_port_register_out(pmt::mp("change"));
}


mediatools_audiosource_s::~mediatools_audiosource_s ()
{
    d_impl->close();
}

#define EXIT_ON_LIST_END    false

int
mediatools_audiosource_s::work (int noutput_items,
			gr_vector_const_void_star &input_items,
			gr_vector_void_star &output_items)
{
	int16_t *out = (int16_t *) output_items[0];
    if(d_skip){
        d_impl->d_ready = false;
        d_skip = false;
        }

    // make sure we have enough data to output
    while(d_data.size() < noutput_items){
        // keep trying to open files until we are ready
        while(d_impl->d_ready == false){
            // exit when we run out of files
            if(d_list.size()<1){
                if(EXIT_ON_LIST_END){
                    // exit angrily
                    printf("reached end of pl\n");
                    return -1;
                } else {
                    // generate silence
                    memset(out, 0x00, noutput_items*sizeof(int16_t));
                    return noutput_items;
                }
            }
            d_impl->open(d_list[0]);
            d_list.erase(d_list.begin(), d_list.begin()+1);
        }

        // decode more data
        d_impl->readData(d_data);
    }

  // generate stream tags with media metadata
  typedef std::map<std::string,std::string>::value_type metaitemtype;
  if(!d_impl->d_meta.empty()){
    d_impl->d_meta["queued_remain"] = (boost::format("%d") % (d_list.size())).str();
    BOOST_FOREACH( const metaitemtype &item, d_impl->d_meta )
    {
        gr::tag_t t;
        t.key = pmt::mp((boost::format("audiosource_%s")%item.first).str());
        t.value = pmt::mp(item.second);
        t.srcid = pmt::mp(alias());
        t.offset = nitems_written(0);
        add_item_tag(0, t);
    }
    d_impl->d_meta.clear();
    pmt::pmt_t msg = pmt::mp((int)d_list.size());
    message_port_pub(pmt::mp("change"), msg);
    }


    // copy data to our output buffer
  memcpy(out, &d_data[0], noutput_items*sizeof(int16_t));
  d_data.erase(d_data.begin(), d_data.begin()+noutput_items);
  return noutput_items;
}

