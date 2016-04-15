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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gnuradio/io_signature.h>
#include "synthesizer_pfb_ccf_impl.h"

namespace gr {
  namespace synthesis {

    synthesizer_pfb_ccf::sptr
    synthesizer_pfb_ccf::make(int nfilts, int connections, const std::vector<float>& taps, bool twox)
    {
      return gnuradio::get_initial_sptr
        (new synthesizer_pfb_ccf_impl(nfilts, connections, taps, twox));
    }

    /*
     * The private constructor
     */
    synthesizer_pfb_ccf_impl::synthesizer_pfb_ccf_impl(int nfilts, int connections, const std::vector<float>& taps, bool twox)
      : gr::hier_block2("synthesizer_pfb_ccf",
              gr::io_signature::make(connections, connections, sizeof(gr_complex)),
              gr::io_signature::make(1, 1, sizeof(gr_complex)))
    {
      float xtwo = (twox ? 2. : 1.);
      d_pre_pfb = synth_selector::make(nfilts,xtwo);
      d_pfb = synth_pfb::make(nfilts, taps, twox);
      d_splitter = blocks::vector_to_streams::make(sizeof(gr_complex),nfilts);

      for(size_t cn = 0; cn < connections; cn++){
        connect(self(), cn, d_pre_pfb, cn);
      }

      connect(d_pre_pfb, 0, d_splitter, 0);

      for(size_t ft = 0; ft < nfilts; ft++){
        connect(d_splitter, ft, d_pfb, ft);
      }

      connect(d_pfb, 0, self(), 0);
    }

    /*
     * Our virtual destructor.
     */
    synthesizer_pfb_ccf_impl::~synthesizer_pfb_ccf_impl()
    {
    }

    void
    synthesizer_pfb_ccf_impl::set_channel_map(const std::vector<int>& map)
    {
      d_pre_pfb->set_channel_map(map);
    }

    std::vector<int>
    synthesizer_pfb_ccf_impl::channel_map() const
    {
      return d_pre_pfb->channel_map();
    }

    void
    synthesizer_pfb_ccf_impl::set_taps(const std::vector<float>& taps)
    {
      d_pfb->set_taps(taps);
    }

    std::vector< std::vector<float> >
    synthesizer_pfb_ccf_impl::taps() const
    {
      return d_pfb->taps();
    }

    void
    synthesizer_pfb_ccf_impl::print_taps()
    {
      d_pfb->print_taps();
    }

    void
    synthesizer_pfb_ccf_impl::declare_sample_delay(int delay)
    {
      d_pfb->declare_sample_delay(delay);
    }


  } /* namespace synthesis */
} /* namespace gr */

