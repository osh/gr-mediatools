# Copyright 2011 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# GNU Radio is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3, or (at your option)
# any later version.
#
# GNU Radio is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with GNU Radio; see the file COPYING.  If not, write to
# the Free Software Foundation, Inc., 51 Franklin Street,
# Boston, MA 02110-1301, USA.

########################################################################
# Setup library
########################################################################
include(GrPlatform) #define LIB_SUFFIX
add_library(gnuradio-mediatools SHARED mediatools_audiosource_s.cc mediatools_audiosource_impl.cc)
target_link_libraries(gnuradio-mediatools 
        ${Boost_LIBRARIES} 
        ${GNURADIO_RUNTIME_LIBRARIES}
        ${GNURADIO_PMT_LIBRARIES}
        ${AVCODEC_LIBRARY}
        ${AVFORMAT_LIBRARY}
        ${AVUTIL_LIBRARY}
)
set_target_properties(gnuradio-mediatools PROPERTIES DEFINE_SYMBOL "gnuradio_mediatools_EXPORTS")

########################################################################
# Install built library files
########################################################################
install(TARGETS gnuradio-mediatools
    LIBRARY DESTINATION lib${LIB_SUFFIX} # .so/.dylib file
    ARCHIVE DESTINATION lib${LIB_SUFFIX} # .lib file
    RUNTIME DESTINATION bin              # .dll file
)

########################################################################
# Build and register unit test
########################################################################

include(GrTest)
set(GR_TEST_TARGET_DEPS gnuradio-mediatools)
#turn each test cpp file into an executable with an int main() function
add_definitions(-DBOOST_TEST_DYN_LINK -DBOOST_TEST_MAIN)

#add_executable(qa_mediatools_audiosource_s qa_mediatools_audiosource_s.cc)
#target_link_libraries(qa_mediatools_audiosource_s gnuradio-mediatools ${Boost_LIBRARIES})
#GR_ADD_TEST(qa_mediatools_audiosource_s qa_mediatools_audiosource_s)
