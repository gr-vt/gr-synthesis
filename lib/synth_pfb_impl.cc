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
#include "synth_pfb_impl.h"

namespace gr {
  namespace synthesis {

    synth_pfb::sptr
    synth_pfb::make(int nchans, const std::vector<float>& taps, bool twox)
    {
      return gnuradio::get_initial_sptr
        (new synth_pfb_impl(nchans, taps, twox));
    }

    /*
     * The private constructor
     */
    synth_pfb_impl::synth_pfb_impl(int nchans, const std::vector<float>& taps, bool twox)
      : gr::block("synth_pfb",
              gr::io_signature::make(nchans, nchans, sizeof(gr_complex)),
              gr::io_signature::make(1, 1, sizeof(gr_complex))),
        polyphase_filterbank(nchans, taps, false),//gr::filter::kernel::polyphase_filterbank(nchans, taps, false),
        d_updated(false)
    {
      // set up 2x multiplier; if twox==True, set to 2, otherwise to 1
      d_twox = (twox ? 2 : 1);
      if(d_nfilts % d_twox != 0) {
        throw std::invalid_argument("pfb_synthesizer_ccf_impl: number of channels must be even for 2x oversampling.\n");
      }

      // The over sampling rate must be rationally related to the number of channels
      // in that it must be N/i for i in [1,N], which gives an outputsample rate
      // of [fs/N, fs] where fs is the input sample rate.
      // This tests the specified input sample rate to see if it conforms to this
      // requirement within a few significant figures.
      const double srate = nchans / float(d_twox);
      const double rsrate = round(srate);
      if(fabs(srate - rsrate) > 0.00001)
        throw std::invalid_argument("pfb_channelizer: oversample rate must be N/i for i in [1, N]");

      //set_relative_rate(double(nchans)/double(d_twox));

      d_rate_ratio = (int)rintf(d_nfilts/d_twox);

      // Calculate the number of outputs per filtering round to evenly
      // align the input vectors with the outputs
      d_output_multiple = d_nfilts/d_twox;
      set_output_multiple(d_output_multiple);

      set_taps(taps);

      //print_taps();

    }

    /*
     * Our virtual destructor.
     */
    synth_pfb_impl::~synth_pfb_impl()
    {
    }

    void
    synth_pfb_impl::forecast (int noutput_items, gr_vector_int &ninput_items_required)
    {
      //printf("OUT(%d),IN(%d)\n",noutput_items,noutput_items/(d_nfilts/d_twox));
      if(noutput_items/(d_nfilts/d_twox)){
        for(size_t idx = 0; idx<d_nfilts; idx++){
          ninput_items_required[idx] = noutput_items/(d_nfilts/d_twox) +d_taps_per_filter;
        }
      }
      else
      {
        for(size_t idx = 0; idx<d_nfilts; idx++){
          ninput_items_required[idx] = 1+d_taps_per_filter;
        }
      }
    }

    void
    synth_pfb_impl::set_taps(const std::vector<float>& taps)
    {
      gr::thread::scoped_lock guard(d_mutex);

      if(taps.size()){

        std::vector<float> tmp_taps = taps;

        size_t t_count = tmp_taps.size();
        size_t add_zeros = (d_nfilts-(t_count%d_nfilts))%d_nfilts;
        while(add_zeros--)
          tmp_taps.push_back(0.);

        //printf("taps: [");
        //for(size_t j = 0; j < tmp_taps.size(); j++) {
        //  printf(" %.4e", tmp_taps[j]);
        //}
        //printf("]\n\n");

        if(d_twox > 1){
          std::vector< std::vector<float> > xtaps(d_nfilts);
          t_count = tmp_taps.size()/d_nfilts;
          for(size_t idx = 0; idx < d_nfilts; idx++){
            xtaps[idx].resize(t_count * 2,0.);
            if(idx < d_nfilts/2){
              for(size_t ind = 0; ind < t_count; ind++){
                xtaps[idx][2*ind] = tmp_taps[idx + ind*d_nfilts];
                xtaps[idx][2*ind+1] = 0.;
                //printf("xtaps[%d][%d] = tmp_taps[%d]\n",int(idx),int(ind),int(ind*d_nfilts+idx));
              }
            }
            else{
              for(size_t ind = 0; ind < t_count; ind++){
                xtaps[idx][2*ind] = 0.;
                xtaps[idx][2*ind+1] = tmp_taps[idx + ind*d_nfilts];
                //printf("xtaps[%d][%d] = tmp_taps[%d]\n",int(idx),int(ind),int(ind*d_nfilts+idx));
              }
            }
          }

          //for(size_t idx = 0; idx < d_nfilts; idx++) {
          //  printf("filter[%lu]: [", idx);
          //  for(size_t j = 0; j < xtaps[0].size(); j++) {
          //    printf(" %.4e", xtaps[idx][j]);
          //  }
          //  printf("]\n\n");
          //}

          tmp_taps.resize(d_nfilts*xtaps[0].size());
          for(size_t ind = 0; ind < xtaps[0].size(); ind++){
            for(size_t idx = 0; idx < d_nfilts; idx++){
              tmp_taps[ind*d_nfilts+idx] = xtaps[idx][ind];
            }
          }

          //printf("taps: [");
          //for(size_t j = 0; j < tmp_taps.size(); j++) {
          //  printf(" %.4e", tmp_taps[j]);
          //}
          //printf("]\n\n");
        }

        //gr::filter::kernel::polyphase_filterbank::set_taps(tmp_taps);
        polyphase_filterbank::set_taps(tmp_taps);
        set_history(d_taps_per_filter);
        //printf("History is %d\n",d_taps_per_filter);
        d_updated = true;
      }
    }

    std::vector< std::vector<float> >
    synth_pfb_impl::taps() const
    {
      //return gr::filter::kernel::polyphase_filterbank::taps();
      return polyphase_filterbank::taps();
    }

    void
    synth_pfb_impl::print_taps()
    {
      //gr::filter::kernel::polyphase_filterbank::print_taps();
      polyphase_filterbank::print_taps();
    }

    int
    synth_pfb_impl::general_work (int noutput_items,
                       gr_vector_int &ninput_items,
                       gr_vector_const_void_star &input_items,
                       gr_vector_void_star &output_items)
    {
      gr::thread::scoped_lock guard(d_mutex);

      gr_complex *in = (gr_complex *) input_items[0];
      gr_complex *out = (gr_complex *) output_items[0];

      if(d_updated){
        d_updated = false;
        return 0;
      }

      size_t counter = nitems_read(0);
      size_t outer = nitems_written(0);
      //for(size_t idx = 0; idx<ninput_items.size(); idx++){
      //  printf("There are %d inputs on input %lu\n",ninput_items[idx],idx);
      //}

      size_t n,ii(0),oo(0);

      // Critically sampled
      if(d_twox == 1){
        while(oo < noutput_items){
          //printf("Read(%lu), in_count(%lu)",counter,counter+ii);
          for(n = 0; n < d_nfilts; n++){
            //printf(", out(%lu)",outer+oo);
            in = (gr_complex*) input_items[n];
            out[oo++] = d_fir_filters[n]->filter(&in[ii]);
          }
          //printf("\n");
          ii++;
        }
      }
      // 2x oversampling
      else{
        while(oo < noutput_items){
          //printf("Read(%lu), in_count(%lu)",counter,counter+ii);
          for(n = 0; n < d_nfilts/2; n++){
            in = (gr_complex*) input_items[n];
            out[oo]    = d_fir_filters[n]->filter(&in[ii]);

            //printf(", out(%lu)",outer+oo);

            in = (gr_complex*) input_items[n+d_nfilts/2];
            out[oo++] += d_fir_filters[n+d_nfilts/2]->filter(&in[ii]);
          }
          //printf("\n");
          ii++;
        }
      }



      // Tell runtime system how many input items we consumed on
      // each input stream.
      consume_each (ii);

      // Tell runtime system how many output items we produced.
      return noutput_items;
    }

  } /* namespace synthesis */
} /* namespace gr */

