svn co http://svn.tapr.org/repos_sdr_hpsdr/trunk/W5WC/PowerSDR_HPSDR_mRX_PS/Source/wdsp
# svn co http://svn.tapr.org/repos_sdr_hpsdr/trunk/W5WC/Thetis/Source/wdsp

patch -p 0 < patches/wdsp.patch

cp -a patches/linux_port.* wdsp/
cp -a patches/Makefile wdsp/

rm -f wdsp/fftw3.h wdsp/libfftw3-3.* wdsp/wdsp.* wdsp/wisdom.c

make -C wdsp
