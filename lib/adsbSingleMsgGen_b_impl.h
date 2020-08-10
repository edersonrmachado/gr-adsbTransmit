/* -*- c++ -*- */
/*
 * Copyright 2015 <+YOU OR YOUR COMPANY+>.
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


#ifndef INCLUDED_ADSB_ADSBSINGLEMSGGEN_B_IMPL_H
#define INCLUDED_ADSB_ADSBSINGLEMSGGEN_B_IMPL_H

#include <ADSB/adsbSingleMsgGen_b.h>

// To see the tests MODE_TEST = 1.
#define MODE_TEST   1
#define MODE_TEST_2 0
#define BYTE_LEN        8
#define PPM_BIT_LEN     2
#define PACKET_SIZE     240
#define DIR     "/home/aqualtune/binaryOutput.txt"
namespace gr {
  namespace ADSB {

    class adsbSingleMsgGen_b_impl : public adsbSingleMsgGen_b
    {
     private:
      int d_fixedMsg; //True: generates a fixed message, False: generates a random message
      void bytes2bits(unsigned char* bits,unsigned char* bytes, int bytesLength);
      unsigned int crc24(unsigned char* data, int len, unsigned int crc); //computes CRC24
      void PPMmodulate(unsigned char msgBit, unsigned char* out, unsigned int msgIndex); //applies PPM
      void genPPMBitStream(unsigned char* outputBits,
                            unsigned char* inputBytes, unsigned int inputSize); //generates PPM bit stream

     public:
      adsbSingleMsgGen_b_impl(int fixedMsg);
      ~adsbSingleMsgGen_b_impl();

      // Where all the action really happens
      int work(int noutput_items,
	       gr_vector_const_void_star &input_items,
	       gr_vector_void_star &output_items);
      void set_fixedMsg(int fixedMsg);
    };

  } // namespace ADSB
} // namespace gr

#endif /* INCLUDED_ADSB_ADSBSINGLEMSGGEN_B_IMPL_H */

