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


#ifndef INCLUDED_SYNTHESIS_SYNTH_SELECTOR_H
#define INCLUDED_SYNTHESIS_SYNTH_SELECTOR_H

#include <synthesis/api.h>
#include <gnuradio/block.h>

namespace gr {
  namespace synthesis {

    /*!
     * \brief <+description of block+>
     * \ingroup synthesis
     *
     */
    class SYNTHESIS_API synth_selector : virtual public gr::block
    {
     public:
      typedef boost::shared_ptr<synth_selector> sptr;

      /*!
       * \brief Return a shared_ptr to a new instance of synthesis::synth_selector.
       *
       * To avoid accidental use of raw pointers, synthesis::synth_selector's
       * constructor is in a private implementation
       * class. synthesis::synth_selector::make is the public interface for
       * creating new instances.
       */
      static sptr make(int nchans, float oversample_rate);

      /*!
       * Set the channel map. Channels are numbers as:
       * <pre>
       *  N/2+1 | ... | N-1 | 0 | 1 |  2 | ... | N/2
       * <------------------- 0 -------------------->
       *                     freq
       * </pre>
       *
       * So output stream 0 comes from channel 0, etc. Setting a new
       * channel map allows the user to specify which channel in frequency
       * he/she wants to got to which output stream.
       *
       * The map should have the same number of elements as the number
       * of output connections from the block. The minimum value of
       * the map is 0 (for the 0th channel) and the maximum number is
       * N-1 where N is the number of channels.
       *
       * We specify M as the number of output connections made where M
       * <= N, so only M out of N channels are driven to an output
       * stream. The number of items in the channel map should be at
       * least M long. If there are more channels specified, any value
       * in the map over M-1 will be ignored. If the size of the map
       * is less than M the behavior is unknown (we don't wish to
       * check every entry into the work function).
       *
       * This means that if the channelizer is splitting the signal up
       * into N channels but only M channels are specified in the map
       * (where M <= N), then M output streams must be connected and
       * the map and the channel numbers used must be less than
       * N-1. Output channel number can be reused, too. By default,
       * the map is [0...M-1] with M = N.
       */
      virtual void set_channel_map(const std::vector<int> &map) = 0;

      /*!
       * Gets the current channel map.
       */
      virtual std::vector<int> channel_map() const = 0;
    };

  } // namespace synthesis
} // namespace gr

#endif /* INCLUDED_SYNTHESIS_SYNTH_SELECTOR_H */

