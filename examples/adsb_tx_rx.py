#!/usr/bin/env python2
# -*- coding: utf-8 -*-
##################################################
# GNU Radio Python Flow Graph
# Title: ADS-B Receiver
# Author: Matt Hostetter and Ederson Machado
# Description: This program uses an USRP tx channel to transmit packets and an other USRP Rx channel to receive ADS-B packets.
# GNU Radio version: 3.7.14.0
##################################################

from gnuradio import analog
from gnuradio import blocks
from gnuradio import channels
from gnuradio import eng_notation
from gnuradio import gr
from gnuradio import uhd
from gnuradio import zeromq
from gnuradio.eng_option import eng_option
from gnuradio.filter import firdes
from optparse import OptionParser
import ADSB
import adsb
import time


class adsb_tx_rx(gr.top_block):

    def __init__(self, address0='addr=192.168.1.100'):
        gr.top_block.__init__(self, "ADS-B Receiver")

        ##################################################
        # Parameters
        ##################################################
        self.address0 = address0

        ##################################################
        # Variables
        ##################################################
        self.usrp_tx_gain = usrp_tx_gain = 0
        self.tx_gain = tx_gain = 0.05
        self.threshold = threshold = 0.025
        self.gain = gain = 100
        self.fs = fs = 2e6
        self.fc = fc = 1090e6
        self.delay_in_bytes = delay_in_bytes = 3000000

        ##################################################
        # Blocks
        ##################################################
        self.zeromq_pub_msg_sink_0 = zeromq.pub_msg_sink('tcp://127.0.0.1:5001', 10)
        self.uhd_usrp_source_0 = uhd.usrp_source(
        	",".join((address0, "")),
        	uhd.stream_args(
        		cpu_format="fc32",
        		channels=range(1),
        	),
        )
        self.uhd_usrp_source_0.set_samp_rate(fs)
        self.uhd_usrp_source_0.set_center_freq(fc, 0)
        self.uhd_usrp_source_0.set_gain(gain, 0)
        self.uhd_usrp_source_0.set_antenna('RX2', 0)
        self.uhd_usrp_source_0.set_auto_dc_offset(False, 0)
        self.uhd_usrp_source_0.set_auto_iq_balance(False, 0)
        self.uhd_usrp_sink_0 = uhd.usrp_sink(
        	",".join((address0, "")),
        	uhd.stream_args(
        		cpu_format="fc32",
        		channels=range(1),
        	),
        )
        self.uhd_usrp_sink_0.set_samp_rate(fs)
        self.uhd_usrp_sink_0.set_center_freq(fc, 0)
        self.uhd_usrp_sink_0.set_gain(usrp_tx_gain, 0)
        self.uhd_usrp_sink_0.set_antenna('TX/RX', 0)
        self.channels_iqbal_gen_0_1 = channels.iqbal_gen(1, 0, 0)
        self.blocks_uchar_to_float_0 = blocks.uchar_to_float()
        self.blocks_throttle_0_0_0 = blocks.throttle(gr.sizeof_char*1, fs,True)
        self.blocks_null_sink_0 = blocks.null_sink(gr.sizeof_float*1)
        self.blocks_multiply_const_vxx_0_0 = blocks.multiply_const_vcc((1, ))
        self.blocks_multiply_const_vxx_0 = blocks.multiply_const_vcc((tx_gain, ))
        self.blocks_float_to_complex_0 = blocks.float_to_complex(1)
        self.blocks_delay_0 = blocks.delay(gr.sizeof_char*1, delay_in_bytes)
        self.blocks_complex_to_mag_squared_0 = blocks.complex_to_mag_squared(1)
        self.analog_const_source_x_0_0 = analog.sig_source_f(0, analog.GR_CONST_WAVE, 0, 0, 0)
        self.adsb_framer_1 = adsb.framer(fs, threshold)
        self.adsb_demod_0 = adsb.demod(fs)
        self.adsb_decoder_0 = adsb.decoder("All Messages", "None", "Brief")
        self.ADSB_adsbSingleMsgGen_b_0 = ADSB.adsbSingleMsgGen_b(1)



        ##################################################
        # Connections
        ##################################################
        self.msg_connect((self.adsb_decoder_0, 'decoded'), (self.zeromq_pub_msg_sink_0, 'in'))
        self.msg_connect((self.adsb_demod_0, 'demodulated'), (self.adsb_decoder_0, 'demodulated'))
        self.connect((self.ADSB_adsbSingleMsgGen_b_0, 0), (self.blocks_throttle_0_0_0, 0))
        self.connect((self.adsb_demod_0, 0), (self.blocks_null_sink_0, 0))
        self.connect((self.adsb_framer_1, 0), (self.adsb_demod_0, 0))
        self.connect((self.analog_const_source_x_0_0, 0), (self.blocks_float_to_complex_0, 1))
        self.connect((self.blocks_complex_to_mag_squared_0, 0), (self.adsb_framer_1, 0))
        self.connect((self.blocks_delay_0, 0), (self.blocks_uchar_to_float_0, 0))
        self.connect((self.blocks_float_to_complex_0, 0), (self.blocks_multiply_const_vxx_0_0, 0))
        self.connect((self.blocks_multiply_const_vxx_0, 0), (self.uhd_usrp_sink_0, 0))
        self.connect((self.blocks_multiply_const_vxx_0_0, 0), (self.channels_iqbal_gen_0_1, 0))
        self.connect((self.blocks_throttle_0_0_0, 0), (self.blocks_delay_0, 0))
        self.connect((self.blocks_uchar_to_float_0, 0), (self.blocks_float_to_complex_0, 0))
        self.connect((self.channels_iqbal_gen_0_1, 0), (self.blocks_multiply_const_vxx_0, 0))
        self.connect((self.uhd_usrp_source_0, 0), (self.blocks_complex_to_mag_squared_0, 0))

    def get_address0(self):
        return self.address0

    def set_address0(self, address0):
        self.address0 = address0

    def get_usrp_tx_gain(self):
        return self.usrp_tx_gain

    def set_usrp_tx_gain(self, usrp_tx_gain):
        self.usrp_tx_gain = usrp_tx_gain
        self.uhd_usrp_sink_0.set_gain(self.usrp_tx_gain, 0)


    def get_tx_gain(self):
        return self.tx_gain

    def set_tx_gain(self, tx_gain):
        self.tx_gain = tx_gain
        self.blocks_multiply_const_vxx_0.set_k((self.tx_gain, ))

    def get_threshold(self):
        return self.threshold

    def set_threshold(self, threshold):
        self.threshold = threshold
        self.adsb_framer_1.set_threshold(self.threshold)

    def get_gain(self):
        return self.gain

    def set_gain(self, gain):
        self.gain = gain
        self.uhd_usrp_source_0.set_gain(self.gain, 0)


    def get_fs(self):
        return self.fs

    def set_fs(self, fs):
        self.fs = fs
        self.uhd_usrp_source_0.set_samp_rate(self.fs)
        self.uhd_usrp_sink_0.set_samp_rate(self.fs)
        self.blocks_throttle_0_0_0.set_sample_rate(self.fs)

    def get_fc(self):
        return self.fc

    def set_fc(self, fc):
        self.fc = fc
        self.uhd_usrp_source_0.set_center_freq(self.fc, 0)
        self.uhd_usrp_sink_0.set_center_freq(self.fc, 0)

    def get_delay_in_bytes(self):
        return self.delay_in_bytes

    def set_delay_in_bytes(self, delay_in_bytes):
        self.delay_in_bytes = delay_in_bytes
        self.blocks_delay_0.set_dly(self.delay_in_bytes)


def argument_parser():
    description = 'This program uses an USRP tx channel to transmit packets and an other USRP Rx channel to receive ADS-B packets. '
    parser = OptionParser(usage="%prog: [options]", option_class=eng_option, description=description)
    parser.add_option(
        "", "--address0", dest="address0", type="string", default='addr=192.168.1.100',
        help="Set IP Address, Dev 0 [default=%default]")
    return parser


def main(top_block_cls=adsb_tx_rx, options=None):
    if options is None:
        options, _ = argument_parser().parse_args()

    tb = top_block_cls(address0=options.address0)
    tb.start()
    try:
        raw_input('Press Enter to quit: ')
    except EOFError:
        pass
    tb.stop()
    tb.wait()


if __name__ == '__main__':
    main()
