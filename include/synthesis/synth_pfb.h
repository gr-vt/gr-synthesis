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


#ifndef INCLUDED_SYNTHESIS_SYNTH_PFB_H
#define INCLUDED_SYNTHESIS_SYNTH_PFB_H

#include <synthesis/api.h>
#include <gnuradio/block.h>

namespace gr {
  namespace synthesis {

    /*!
     * \brief <+description of block+>
     * \ingroup synthesis
     *
     */
    class SYNTHESIS_API synth_pfb : virtual public gr::block
    {
     public:
      typedef boost::shared_ptr<synth_pfb> sptr;

      /*!
       * \brief Return a shared_ptr to a new instance of synthesis::synth_pfb.
       *
       * To avoid accidental use of raw pointers, synthesis::synth_pfb's
       * constructor is in a private implementation
       * class. synthesis::synth_pfb::make is the public interface for
       * creating new instances.
       */
      static sptr make(int nchans, const std::vector<float>& taps, bool twox);

      /*!
       * Resets the filterbank's filter taps with the new prototype filter
       * \param taps    (vector/list of floats) The prototype filter to
       *                populate the filterbank.
       */
      virtual void set_taps(const std::vector<float> &taps) = 0;

      /*!
       * Print all of the filterbank taps to screen.
       */
      virtual void print_taps() = 0;

      /*!
       * Return a vector<vector<>> of the filterbank taps
       */
      virtual std::vector<std::vector<float> > taps() const = 0;
    };

  } // namespace synthesis
} // namespace gr

#endif /* INCLUDED_SYNTHESIS_SYNTH_PFB_H */

