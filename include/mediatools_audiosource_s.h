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

#ifndef INCLUDED_MEDIATOOLS_AUDIOSOURCE_S_H
#define INCLUDED_MEDIATOOLS_AUDIOSOURCE_S_H

#include <mediatools_api.h>
#include <gnuradio/sync_block.h>
#include <mediatools_audiosource_impl.h>

class mediatools_audiosource_s;
typedef boost::shared_ptr<mediatools_audiosource_s> mediatools_audiosource_s_sptr;

MEDIATOOLS_API mediatools_audiosource_s_sptr mediatools_make_audiosource_s (std::vector<std::string> args);

/*!
 * \brief <+description+>
 *
 */
class MEDIATOOLS_API mediatools_audiosource_s : public gr::sync_block
{
	friend MEDIATOOLS_API mediatools_audiosource_s_sptr mediatools_make_audiosource_s (std::vector<std::string> args);

	mediatools_audiosource_s (std::vector<std::string> args);

    mediatools_audiosource_impl* d_impl;

    std::vector<std::string> d_list;
    std::vector<int16_t> d_data;
    bool d_skip;

    void enqueue_pmt(pmt::pmt_t file){
        if(pmt::is_symbol(file))
            enqueue(pmt::symbol_to_string(file));
        }
    void skip_pmt(pmt::pmt_t _){
        skip();
        }

 public:
	~mediatools_audiosource_s ();
    void enqueue(std::string file){
        d_list.push_back(file);
        }
    void skip(){
        d_skip = true;
        d_data.clear();
        }
	int work (int noutput_items,
		gr_vector_const_void_star &input_items,
		gr_vector_void_star &output_items);
};

#endif /* INCLUDED_MEDIATOOLS_AUDIOSOURCE_S_H */

