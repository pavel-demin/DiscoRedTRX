devcfg=/sys/devices/soc0/amba/f8007000.devcfg
test -d $devcfg/fclk/fclk0 || echo fclk0 > $devcfg/fclk_export
echo 1 > $devcfg/fclk/fclk0/enable
echo 143000000 > $devcfg/fclk/fclk0/set_rate

cat sdr_transceiver_emb.bit > /dev/xdevcfg

./main | aplay -fFLOAT_LE -c1 -r48000
