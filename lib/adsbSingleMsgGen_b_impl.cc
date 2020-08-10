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
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
//
#include <math.h>
#include <gnuradio/io_signature.h>
#include "adsbSingleMsgGen_b_impl.h"
//
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>

//
// ============================== Main function  ============================
//
#if MODE_TEST // Declare variable used in the tests
 int a=0;
 int firstMessage=0;
// #include <delay.h>
#endif // MODE_TEST
//
namespace gr {
  namespace ADSB {

    adsbSingleMsgGen_b::sptr
    adsbSingleMsgGen_b::make(int fixedMsg)
    {
      return gnuradio::get_initial_sptr
        (new adsbSingleMsgGen_b_impl(fixedMsg));
    }
//
// ============================ Block constructor  ==========================
//
// The constructor of the block ( adsbSingleMsgGen_b_impl ) calls the
// constructor of the base class block gr::block(...)
//
// The arguments inside  gr::block( ) represents the block name and a call
// to make function.
//
// <+MIN_OUT+> and <+MAX_OUT+> represents the maximum and number of ports.
// <ITYPE> and <OTYPE> indicates the datatypes for the input and output
//  port/s which needs to be filled out manually.
//
    adsbSingleMsgGen_b_impl::adsbSingleMsgGen_b_impl(int fixedMsg)
      : gr::sync_block("adsbSingleMsgGen_b",    // The block name
              gr::io_signature::make(0, 0, 0),  // There isn't inputs,
              gr::io_signature::make(1, 1, sizeof(unsigned char))){
              // One output
//
        d_fixedMsg = fixedMsg;
        set_output_multiple(PACKET_SIZE);
        /*  Ensure we'll produce a number of output
            items that is a multiple of sa value (240). This value is
            the number of samples encoded (8(Preamble) + 112(Payload))x2 = 240
        */
//
    }
//
// =========================== Virtual Destructor ===========================
//
    adsbSingleMsgGen_b_impl::~adsbSingleMsgGen_b_impl(){
//
    }
//
// ============================ Fixed Message ===============================
//
// FixeMsg is an user input to choose between single and multiple messages
//
    void adsbSingleMsgGen_b_impl::set_fixedMsg(int fixedMsg){
        d_fixedMsg = fixedMsg;
    }
//
// ================================ Bytes2bits ==============================
//
    void adsbSingleMsgGen_b_impl::bytes2bits(unsigned char* bits,
        unsigned char* bytes, int bytesLength){
//
// This function will convert each bit of a byte in a new byte. The new byte
// can have the values 0 (00000000) or 1 (00000001).
//
// The input is a array of bytes, with have a lenght specified by
//  bytestLength, and the output will be saved in the address pointed by bits.
//
        int byteIndex;
        int bitIndex;
        unsigned char currentByte;
//
        for(byteIndex=0;byteIndex<bytesLength;byteIndex++){
            currentByte = bytes[byteIndex];
            for(bitIndex=byteIndex*BYTE_LEN;bitIndex<(byteIndex+1)*BYTE_LEN;bitIndex++){
                bits[bitIndex]=(currentByte & 0x80); // Extract the first bit
                currentByte = (currentByte << 1); // Shift to take te next bit
            }
        }
    }
//
// ========================= CRC24  - Mode S coding  =======================
//
// Parity table for MODE S Messages.
// The table contains 112 elements, every element corresponds to a bit set
// in the message, starting from the first bit of actual data after the
// preamble.
//
// For messages of 112 bit, the whole table is used.
// For messages of 56 bits only the last 56 elements are used.
//
// The algorithm is as simple as xoring all the elements in this table
// for which the corresponding bit on the message is set to 1.
//
// The latest 24 elements in this table are set to 0 as the checksum at the
// end of the message should not affect the computation.
//
// Note: this function can be used with DF11 and DF17, other modes have
// the CRC xored with the sender address as they are reply to interrogations,
// but a casual listener can't split the address from the checksum.
//
    uint32_t modes_checksum_table[112] = {
//
        0x3935ea, 0x1c9af5, 0xf1b77e, 0x78dbbf,
        0xc397db, 0x9e31e9, 0xb0e2f0, 0x587178,
        0x2c38bc, 0x161c5e, 0x0b0e2f, 0xfa7d13,
        0x82c48d, 0xbe9842, 0x5f4c21, 0xd05c14,
        0x682e0a, 0x341705, 0xe5f186, 0x72f8c3,
        0xc68665, 0x9cb936, 0x4e5c9b, 0xd8d449,
        0x939020, 0x49c810, 0x24e408, 0x127204,
        0x093902, 0x049c81, 0xfdb444, 0x7eda22,
        0x3f6d11, 0xe04c8c, 0x702646, 0x381323,
        0xe3f395, 0x8e03ce, 0x4701e7, 0xdc7af7,
        0x91c77f, 0xb719bb, 0xa476d9, 0xadc168,
        0x56e0b4, 0x2b705a, 0x15b82d, 0xf52612,
        0x7a9309, 0xc2b380, 0x6159c0, 0x30ace0,
        0x185670, 0x0c2b38, 0x06159c, 0x030ace,
        0x018567, 0xff38b7, 0x80665f, 0xbfc92b,
        0xa01e91, 0xaff54c, 0x57faa6, 0x2bfd53,
        0xea04ad, 0x8af852, 0x457c29, 0xdd4410,
        0x6ea208, 0x375104, 0x1ba882, 0x0dd441,
        0xf91024, 0x7c8812, 0x3e4409, 0xe0d800,
        0x706c00, 0x383600, 0x1c1b00, 0x0e0d80,
        0x0706c0, 0x038360, 0x01c1b0, 0x00e0d8,
        0x00706c, 0x003836, 0x001c1b, 0xfff409,
        0x000000, 0x000000, 0x000000, 0x000000,
        0x000000, 0x000000, 0x000000, 0x000000,
        0x000000, 0x000000, 0x000000, 0x000000,
        0x000000, 0x000000, 0x000000, 0x000000,
        0x000000, 0x000000, 0x000000, 0x000000,
        0x000000, 0x000000, 0x000000, 0x000000
    };
//
    uint32_t modesChecksum(unsigned char *msg, int bits){

        uint32_t   crc = 0;
        uint32_t   rem = 0;
        int        offset = (bits == 112) ? 0 : (112-56);
        uint8_t    theByte = *msg;
        uint32_t * pCRCTable = &modes_checksum_table[offset];
        int j;

        //We don't really need to include the checksum itself
        bits -= 24;
        for(j = 0; j < bits; j++){
            if ((j & 7) == 0)
                theByte = *msg++;

            //If bit is set, xor with corresponding table entry.
            if (theByte & 0x80) {crc ^= *pCRCTable;}
            pCRCTable++;
            theByte = theByte << 1;
        }
        rem = (msg[0] << 16) | (msg[1] << 8) | msg[2]; // message checksum
        return ((crc ^ rem) & 0x00FFFFFF); // 24 bit checksum syndrome.
}
//
// ================================ PPMmodulate ==============================
//
// This function will produce a PPM output for each input bit. Ex. for the
// a bit 1 (00000001) the output will be 10 =(b00000001 b00000000).

// The output bits will be saved in the addres pointed by out.
//
    void adsbSingleMsgGen_b_impl::PPMmodulate(unsigned char msgBit,
            unsigned char* out, unsigned int msgIndex){
//
        if(msgBit){
            out[msgIndex] = 1;
            out[msgIndex+1] = 0;
        }
        else{
            out[msgIndex] = 0;
            out[msgIndex+1] = 1;
        }
    }
//
// ============================= Generates PPM  =============================
//
    void adsbSingleMsgGen_b_impl::genPPMBitStream(unsigned char* outputBits,
                unsigned char* inputBytes, unsigned int inputSize){
//
// This function will give the final PPM output. It calls the others
// functions to generate the PPM output.
//
// The bits are modulated in PPM. Ex. input 0xFF output = 0x01 0x00 0x01 0x00
// 0x01 0x00 0x01 0x00 0x01 0x00 0x01 0x00 0x01 0x00 0x01 0x00.
//
        // Allocates the requested(inputSize) memory and returns a pointer
        // to it > bitstream.
        unsigned char* bitStream = (unsigned char*)malloc(inputSize*BYTE_LEN);
        unsigned int bitIndex;
        // Call the function with will generate one byte for
        // each bit of inputBytes.
        bytes2bits(bitStream,inputBytes,inputSize);
        // After convert each bit in one byte with the function
        // bytes2bits, applies the PPM modulation in the bits
        for(bitIndex=0;bitIndex<inputSize*BYTE_LEN;bitIndex++){
//
            PPMmodulate(bitStream[bitIndex],outputBits,PPM_BIT_LEN*bitIndex);
        }
        free(bitStream);
    }

//
// =============================== Work function  =============================
//
// This is the main function of the block. The processor will call this function
// in the beggining of the execution . It will process the calls of the
// others functions, manage the variables
//
    int adsbSingleMsgGen_b_impl::work(int noutput_items,
              gr_vector_const_void_star &input_items,
              gr_vector_void_star &output_items){

        const unsigned int adsbPreambleSize = 16;   // Size in bits (including PPM)
        const unsigned int adsbDataBlockSize = 11;  // Size in bytes
        static unsigned char adsbData[11];          // Fixed allocation data array
        const unsigned int adsbCRCBlockSize = 3;    // Size in bytes
        static unsigned char crcBytes[3];           // Fixed allocation crc array
        const unsigned char adsbPreamble[] = {1,0,1,0,0,0,0,1,0,1,0,0,0,0,0,0};
//      const unsigned char adsbFixedData[] ={  0x8F,0x4D,0x20,0x23,0x58,
//                                              0x7F,0x34,0x5E,0x35,0x83,0x7E};
//        const unsigned char adsbFixedData[] = { 0x8d,0x4d,0x20,0x23,0x58,
//                                                0x79,0x24,0x53,0xEF,0x85,0x8B};
//        const unsigned char adsbFixedData[] = { 0x8d,0x3c,0x11,0xFB,0x58,
//                                                0x17,0x74,0x7D,0x50,0x5D,0x29};
//        const unsigned char adsbFixedData[] = { 0x8F,0x4D,0x20,0x23,0x58,
//                                                0x77,0xB0,0xBC,0x01,0x99,0x6F};
        const unsigned char adsbFixedData2[] = { 0x8D,0x75,0x80,0x4B,0x58,
                                                0x0f,0xf2,0xcf,0x7e,0x9b,0xa6};
        const unsigned char adsbFixedData3[] = { 0x8D,0x75,0x80,0x4B,0x58,
                                                0x0f,0xf6,0xb2,0x83,0xeb,0x7a};
        const unsigned char adsbFixedData[] = { 0x8D,0x4d,0x20,0x23,0x58,
                                                0x79,0x24,0x53,0xef,0x85,0x8b};
        const unsigned char adsbFixedData1[] = { 0x8f,0x4d,0x20,0x23,0x58,
                                                0x77,0xd0,0xbc,0x7d,0x99,0x55};
        unsigned char *out = (unsigned char *) output_items[0]; // Pointer to the output.
        unsigned char *Packet = (unsigned char*)malloc(PACKET_SIZE);    // Pointer to mount the
                                                                // ADSB packet
        // Packet = 16 bytes preamble + (88bytes adsb data + 24 bytes) *2 (PPM) = 240 bytes.
        unsigned int dataCRC24;
        unsigned int dataIndex;
        uint32_t CRC24;



#if MODE_TEST
// Print the preamble
	static unsigned char fillPack[240];
	
for(int c=0;c<PACKET_SIZE;c++){
	    fillPack[c]=1;
	}
	memcpy(Packet,fillPack,PACKET_SIZE);
	int i;	
	if(a<2){
            printf("\n\n=============== INICIAL PACKET ================== \n");
            printf("\n Packet = \t");
            for(i=0;i<PACKET_SIZE;i++){
                if(remainder(i,16)==0){printf("\n");printf("\t\t\t\t");}
                if(remainder(i,2)==0){printf("\t");}
                printf("%d", Packet[i]);
            }
        }
	

#endif // MODE_TEST

        // Start putting the preamble into the Packet
        memcpy(Packet,adsbPreamble,adsbPreambleSize*sizeof(unsigned char));
//
#if MODE_TEST
// Print the preamble
        
        if(a<2){
            printf("\n\n=============== PUT THE PREAMBLE IN THE PACKET ================== \n");
            printf("\n Packet = \t");
            for(i=0;i<PACKET_SIZE;i++){
                if(remainder(i,16)==0){printf("\n");printf("\t\t\t\t");}
                if(remainder(i,2)==0){printf("\t");}
                printf("%d", Packet[i]);
            }
        }
#endif // MODE_TEST
//
        if(d_fixedMsg){
            // Put the ADSBdata in the adsb data register.
            if(firstMessage==0){
                memcpy(adsbData,adsbFixedData,adsbDataBlockSize*sizeof(unsigned char));
                firstMessage++;
            }
            else if(firstMessage==1){
                memcpy(adsbData,adsbFixedData1,adsbDataBlockSize*sizeof(unsigned char));
                firstMessage++;
            }
            else if(firstMessage==2){
                memcpy(adsbData,adsbFixedData2,adsbDataBlockSize*sizeof(unsigned char));
                firstMessage++;
            }
            else if(firstMessage==3){
                memcpy(adsbData,adsbFixedData3,adsbDataBlockSize*sizeof(unsigned char));
                firstMessage=0;
            }
            else{}
        }
        else{
            for(dataIndex=0;dataIndex<adsbDataBlockSize;dataIndex++){
                // Function not used yet
                // adsbData[dataIndex]=rand()%256;
            }
        }
        // Generates the crc24  (3bytes) for the ADSBdata
        CRC24=modesChecksum(adsbData,112);
        crcBytes[0] = (CRC24 & 0xFF0000)>>16;
        crcBytes[1] = (CRC24 & 0x00FF00)>>8;
        crcBytes[2] = (CRC24 & 0x000FF);
        //Add  the adsb data in PPM format in the packet
        genPPMBitStream(&Packet[adsbPreambleSize],adsbData,adsbDataBlockSize);
//
#if MODE_TEST
// Print the ADSB data
        if(a<2){
            printf("\n\n ============= PUT THE ADSBDATA IN THE PACKET ================== \n");
            printf("\n Packet = \t");
            for(i=0;i< PACKET_SIZE;i++){
                if(remainder(i,16)==0){printf("\n");printf("\t\t\t\t");}
                if(remainder(i,2)==0){printf("\t");}
                printf("%d", Packet[i]);
            }
        }
#endif // MODE_TEST
//
        // Add the crcBytes in PPM format in the Packet
        genPPMBitStream(&Packet[adsbPreambleSize+PPM_BIT_LEN*BYTE_LEN*adsbDataBlockSize],crcBytes,adsbCRCBlockSize);
//
#if MODE_TEST
//Print the Packet with the CRC data
        if(a<2){
            printf("\n\n ============= PUT THE CRCDATA IN THE PACKET ================== \n");
            printf("\n\n CrC = %02X%02X%02X \n",crcBytes[0],crcBytes[1],crcBytes[2]);
            printf("\n Packet = \t");
            for(i=0;i< PACKET_SIZE;i++){
                if(remainder(i,16)==0){printf("\n");printf("\t\t\t\t");}
                if(remainder(i,2)==0){printf("\t");}
                printf("%d", Packet[i]);
            }
        printf("\n\n");
        }
#endif // MODE_TEST
//

#if MODE_TEST_2
// Generates a text file to be use in the reception simulation
        if(a<1){
            FILE * fp;
            fp = fopen (DIR, "w+");
            fprintf(fp,"%s \n","TEXT");
            int ii;
            for(ii=0;ii<PACKET_SIZE;ii++){
                fprintf(fp, "%d\t%d \n",Packet[ii],0);
            }
            fclose(fp);
        }
#endif // MODE_TEST
//
        //Send the final packet to the output
        memcpy(out,Packet,adsbPreambleSize + PPM_BIT_LEN*BYTE_LEN*(adsbDataBlockSize+adsbCRCBlockSize));
//
#if MODE_TEST
// Print the final output
        if(a<2){
            printf("\n\n ============= PUT THE PACKET IN THE OUT ================== \n");
            printf("\n out = \t");
            for(i=0;i< PACKET_SIZE;i++){
                if(remainder(i,16)==0){printf("\n");printf("\t\t\t\t");}
                if(remainder(i,2)==0){printf("\t");}
                printf("%d", out[i]);
            }
        printf("\n\n");
        }
#endif // MODE_TEST
//
        // Tell runtime system how many output items we produced.
        unsigned int generatedItems = adsbPreambleSize+PPM_BIT_LEN*BYTE_LEN*(adsbDataBlockSize+adsbCRCBlockSize);
//
#if MODE_TEST
// Print the number of items produced by packet.
        if(a<1){
             printf("\n number of items produced: %d \n", generatedItems);
        }
        a++;
        //firstMessage++;
#endif // MODE_TEST
//
        return generatedItems;
    }
  } /* namespace ADSB */
} /* namespace gr */

