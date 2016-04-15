/* -*- c++ -*- */
/* 
 * Copyright 2016 <+YOU OR YOUR COMPANY+>.
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

#ifndef INCLUDED_SYNTHESIS_SYNTH_SELECTOR_IMPL_H
#define INCLUDED_SYNTHESIS_SYNTH_SELECTOR_IMPL_H

#include <synthesis/synth_selector.h>
#include <gnuradio/thread/thread.h>
#include <gnuradio/fft/fft.h>
#include <string.h>

namespace gr {
  namespace synthesis {

    class synth_selector_impl : public synth_selector
    {
     private:
      size_t                  d_state;
      bool                    d_updated;
      int                     d_nchans;
      int                     d_rate_ratio;
      int                     d_pointer;
      float                   d_os_rate;
      std::vector<int>        d_channel_map;
      gr::thread::mutex       d_mutex; // mutex to protect set/work access
      gr::fft::fft_complex*   d_fft;

     public:
      synth_selector_impl(int nchans, float oversample_rate);
      ~synth_selector_impl();

      // Where all the action really happens
      void forecast (int noutput_items, gr_vector_int &ninput_items_required);

      int general_work(int noutput_items,
           gr_vector_int &ninput_items,
           gr_vector_const_void_star &input_items,
           gr_vector_void_star &output_items);

      void set_channel_map(const std::vector<int> &map);
      std::vector<int> channel_map() const;
    };

  } // namespace synthesis
} // namespace gr

#endif /* INCLUDED_SYNTHESIS_SYNTH_SELECTOR_IMPL_H */

