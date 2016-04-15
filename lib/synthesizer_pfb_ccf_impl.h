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

#ifndef INCLUDED_SYNTHESIS_SYNTHESIZER_PFB_CCF_IMPL_H
#define INCLUDED_SYNTHESIS_SYNTHESIZER_PFB_CCF_IMPL_H

#include <synthesis/synthesizer_pfb_ccf.h>
#include <synthesis/synth_selector.h>
#include <synthesis/synth_pfb.h>
#include <gnuradio/blocks/vector_to_streams.h>

namespace gr {
  namespace synthesis {

    class synthesizer_pfb_ccf_impl : public synthesizer_pfb_ccf
    {
     private:
      synth_selector::sptr              d_pre_pfb;
      synth_pfb::sptr                   d_pfb;
      blocks::vector_to_streams::sptr   d_splitter;

     public:
      synthesizer_pfb_ccf_impl(int nfilts, int connections, const std::vector<float>& taps, bool twox);
      ~synthesizer_pfb_ccf_impl();


      void set_taps(const std::vector<float> &taps);
      std::vector<std::vector<float> > taps() const;
      void print_taps();

      void set_channel_map(const std::vector<int> &map);
      std::vector<int> channel_map() const;

      void declare_sample_delay(int delay);
    };

  } // namespace synthesis
} // namespace gr

#endif /* INCLUDED_SYNTHESIS_SYNTHESIZER_PFB_CCF_IMPL_H */

