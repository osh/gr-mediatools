#!/usr/bin/python

from gnuradio import gr,audio

tb = gr.top_block();
src = gr.file_source(gr.sizeof_float,"/dev/urandom");
sink = audio.sink( 44100, "", True);

tb.connect(src,sink);
tb.run();





