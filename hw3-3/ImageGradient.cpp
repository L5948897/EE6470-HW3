#include "ImageGradient.h"

ImageGradient::ImageGradient( sc_module_name n ): sc_module( n )
{
#ifndef NATIVE_SYSTEMC
	HLS_FLATTEN_ARRAY(val);
#endif
	SC_THREAD( do_filter );
	sensitive << i_clk.pos();
	dont_initialize();
	reset_signal_is(i_rst, false);

#ifndef NATIVE_SYSTEMC
	i_grey.clk_rst(i_clk, i_rst);
  o_result.clk_rst(i_clk, i_rst);
#endif
}

// gausian mask
const int mask[MASK_N][MASK_X][MASK_Y] = {{{1, 4, 7, 4, 1}, 
                                          {4, 16, 26, 16, 4}, 
                                          {7, 26, 41, 26, 7},
                                          {4, 16, 26, 16, 4},
                                          {1, 4, 7, 4, 1}}};

void ImageGradient::do_filter() {
	{
#ifndef NATIVE_SYSTEMC
		HLS_DEFINE_PROTOCOL("main_reset");
		i_grey.reset();
		o_result.reset();
#endif
		wait();
	}
	while (true) {
		for (unsigned int i = 0; i<MASK_N; ++i) {
			val[i] = 0;
			wait();
		}
		for (unsigned int v = 0; v<MASK_Y; ++v) {
			for (unsigned int u = 0; u<MASK_X; ++u) {
#ifndef NATIVE_SYSTEMC
				unsigned char grey = i_grey.get();
#else
				unsigned char grey = i_grey.read();
#endif
				for (unsigned int i = 0; i != MASK_N; ++i) {
					val[i] += grey * mask[i][u][v];
					wait();
				}
			}
		}
		int total = 0;
		/*for (unsigned int i = 0; i != MASK_N; ++i) {
			total += val[i] * val[i];
			wait();
		}*/
		total = (int)(val[0]);
#ifndef NATIVE_SYSTEMC
		o_result.put(total);
#else
		o_result.write(total);
#endif
	}
}
