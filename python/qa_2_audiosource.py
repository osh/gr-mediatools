#!/usr/bin/python
import sys;
from gnuradio import gr,audio;
import mediatools;
sys.path.append(".");

# Construct playlist
pl = mediatools.strvec(["1.mp3","2.ogg"]);

# Setup sample rates
dac_sample_rate = 44100;
file_sample_rate = 44100;
default_device = "";

# Instantiate blocks
tb = gr.top_block();
src = mediatools.audiosource_s(pl);
s2c = gr.interleaved_short_to_complex();
c2f = gr.complex_to_float();
mc = gr.multiply_const_cc(1.0/(32768.0)); # scale to the +1 / -1 range
sink = audio.sink(dac_sample_rate,default_device);

# Connect up flowgraph
tb.connect(src,s2c,mc,c2f);  # connect src->scalar->float conversion
tb.connect(c2f,sink);           # connect left channel
tb.connect((c2f,1),(sink,1));   # connect right channel

# And go
tb.run();





