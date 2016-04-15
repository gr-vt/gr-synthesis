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
#include "synth_selector_impl.h"
#include <stdio.h>

namespace gr {
  namespace synthesis {

    synth_selector::sptr
    synth_selector::make(int nchans, float oversample_rate)
    {
      return gnuradio::get_initial_sptr
        (new synth_selector_impl(nchans, oversample_rate));
    }

    /*
     * The private constructor
     */
    synth_selector_impl::synth_selector_impl(int nchans, float oversample_rate)
      : gr::block("synth_selector",
              gr::io_signature::make(1, nchans, sizeof(gr_complex)),
              gr::io_signature::make(1, 1, nchans*sizeof(gr_complex))),
        d_updated(false), d_nchans(nchans), d_os_rate(oversample_rate), d_state(0)
    {
      // The over sampling rate must be rationally related to the number of channels
      // in that it must be N/i for i in [1,N], which gives an outputsample rate
      // of [fs/N, fs] where fs is the input sample rate.
      // This tests the specified input sample rate to see if it conforms to this
      // requirement within a few significant figures.
      const double srate = nchans / oversample_rate;
      const double rsrate = round(srate);
      if(fabs(srate - rsrate) > 0.00001)
        throw std::invalid_argument("synth_selector: oversample rate must be N/i for i in [1, N]");

      d_rate_ratio = nchans / oversample_rate;

      // Default channel map. The channel map specifies which input
      // goes to which output channel; so out[0] comes from
      // channel_map[0].
      d_channel_map.resize(d_nchans);
      for(unsigned int i = 0; i < d_nchans; i++){
        d_channel_map[i] = i;
      }

      // Create the FFT to handle the input spinning of the channels
      d_fft = new fft::fft_complex(d_nchans, false);

      // Zero all inputs for if not all channels need connections;
      memset( d_fft->get_inbuf(), 0, d_nchans*sizeof(gr_complex) );

      d_pointer = -d_rate_ratio;

      //printf("NChans(%d), OSR(%1.3f), Rate Ratio (%d), Starting(%d)\n",d_nchans,d_os_rate,d_rate_ratio,d_pointer);
    }

    /*
     * Our virtual destructor.
     */
    synth_selector_impl::~synth_selector_impl()
    {
      delete d_fft;
    }

    void
    synth_selector_impl::forecast (int noutput_items, gr_vector_int &ninput_items_required)
    {
      size_t connected = ninput_items_required.size();
      for(size_t idx = 0; idx < connected; idx++){
        ninput_items_required[idx] = noutput_items;
      }
    }

    void
    synth_selector_impl::set_channel_map(const std::vector<int>& map)
    {
      gr::thread::scoped_lock guard(d_mutex);

      if(map.size() > 0) {
        unsigned int max = (unsigned int)*std::max_element(map.begin(), map.end());
        if(max >= d_nchans) {
          throw std::invalid_argument("synth_selector_impl::set_channel_map: map range out of bounds.\n");
        }
        d_channel_map = map;
        // clear out the input fft buffer
        memset( d_fft->get_inbuf(), 0, d_nchans*sizeof(gr_complex) );
      }
    }

    std::vector<int>
    synth_selector_impl::channel_map() const
    {
      return d_channel_map;
    }

    int
    synth_selector_impl::general_work (int noutput_items,
                       gr_vector_int &ninput_items,
                       gr_vector_const_void_star &input_items,
                       gr_vector_void_star &output_items)
    {
      gr::thread::scoped_lock guard(d_mutex);

      gr_complex *in = (gr_complex *) input_items[0];
      gr_complex *out = (gr_complex *) output_items[0];

      if(d_updated) {
        d_updated = false;
        return 0;
      }

      size_t ninputs = input_items.size();
      size_t count(0),ii(0);

      int last(0),i(d_pointer),j(0);

      //printf("Connections(%lu), start(%d)\n",ninputs,i);

      while(count++ < noutput_items){
        for(size_t nn = 0; nn < ninputs; nn++){
          //printf("(Input (%lu,%lu))(FFT on %d)\n",nn,ii,d_channel_map[nn]);
          in = (gr_complex*) input_items[nn];
          d_fft->get_inbuf()[d_channel_map[nn]] = in[ii];
        }
        ii++;

        d_fft->execute();
        //printf("FFT Done\n");

        // handle out mapping based on state
        j=0;
        i = (i+d_rate_ratio)%d_nchans;
        last = i;
        //printf("i(%d),last(%d)\n",i,last);
        while(i < d_nchans){
          out[i++] = d_fft->get_outbuf()[j++];
        }
        i = 0;
        //printf("i(%d),last(%d)\n",i,last);
        while(i < last){
          out[i++] = d_fft->get_outbuf()[j++];
        }
        //printf("i(%d),last(%d)\n",i,last);

        //printf("Output Mapped\n");

        out += d_nchans;
      }

      d_pointer = last;
      //printf("Finished on %d\n",d_pointer);

      // Tell runtime system how many input items we consumed on
      // each input stream.
      consume_each (noutput_items);

      // Tell runtime system how many output items we produced.
      return noutput_items;
    }

  } /* namespace synthesis */
} /* namespace gr */

