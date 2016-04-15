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

#ifndef INCLUDED_SYNTHESIS_SYNTH_PFB_IMPL_H
#define INCLUDED_SYNTHESIS_SYNTH_PFB_IMPL_H

#include <synthesis/synth_pfb.h>
#include <gnuradio/filter/polyphase_filterbank.h>
#include <gnuradio/filter/fir_filter.h>
#include <gnuradio/thread/thread.h>

namespace gr {
  namespace synthesis {

    class synth_pfb_impl : public synth_pfb, gr::filter::kernel::polyphase_filterbank
    {
     private:
      bool                d_updated;
      //float               d_oversample_rate;
      int                 d_twox;
      //int                *d_idxlut;
      int                 d_rate_ratio;
      int                 d_output_multiple;
      gr::thread::mutex   d_mutex; // mutex to protect set/work access

     public:
      synth_pfb_impl(int nchans, const std::vector<float>& taps, bool twox);
      ~synth_pfb_impl();

      // Where all the action really happens
      void forecast (int noutput_items, gr_vector_int &ninput_items_required);

      int general_work(int noutput_items,
           gr_vector_int &ninput_items,
           gr_vector_const_void_star &input_items,
           gr_vector_void_star &output_items);

      void set_taps(const std::vector<float> &taps);
      std::vector<std::vector<float> > taps() const;
      void print_taps();
    };

  } // namespace synthesis
} // namespace gr

#endif /* INCLUDED_SYNTHESIS_SYNTH_PFB_IMPL_H */

