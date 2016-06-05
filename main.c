#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <math.h>
#include <pthread.h>
#include <sys/mman.h>

#include <fftw3.h>

#include "comm.h"

volatile float *rx_data, *tx_data;
volatile uint16_t *rx_cntr, *tx_cntr;
volatile uint8_t *rx_rst, *tx_rst;

void *rx_data_handler(void *arg);
void *tx_data_handler(void *arg);

int main()
{
  int fd;
  FILE *wisdom_file;
  pthread_t thread;
  char *name = "/dev/mem";
  void *cfg, *sts;
  volatile uint32_t *rx_freq, *tx_freq;
  volatile uint16_t *rx_rate, *tx_rate;

  if((fd = open(name, O_RDWR)) < 0)
  {
    perror("open");
    return EXIT_FAILURE;
  }

  sts = mmap(NULL, sysconf(_SC_PAGESIZE), PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0x40000000);
  cfg = mmap(NULL, sysconf(_SC_PAGESIZE), PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0x40001000);
  rx_data = mmap(NULL, 2*sysconf(_SC_PAGESIZE), PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0x40002000);
  tx_data = mmap(NULL, 2*sysconf(_SC_PAGESIZE), PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0x40008000);

  rx_rst = ((uint8_t *)(cfg + 0));
  rx_rate = ((uint16_t *)(cfg + 4));
  rx_freq = ((uint32_t *)(cfg + 8));
  rx_cntr = ((uint16_t *)(sts + 12));

  tx_rst = ((uint8_t *)(cfg + 1));
  tx_rate = ((uint16_t *)(cfg + 20));
  tx_freq = ((uint32_t *)(cfg + 24));
  tx_cntr = ((uint16_t *)(sts + 18));

  /* set default rx phase increment */
  *rx_freq = (uint32_t)floor(720000 / 125.0e6 * (1<<30) + 0.5);
  /* set default rx sample rate */
  *rx_rate = 1250;

  /* set default tx phase increment */
  *tx_freq = (uint32_t)floor(720000 / 125.0e6 * (1<<30) + 0.5);
  /* set default tx sample rate */
  *tx_rate = 1250;

  if((wisdom_file = fopen("wdsp-fftw-wisdom.txt", "r")))
  {
    fftw_import_wisdom_from_file(wisdom_file);
    fclose(wisdom_file);
  }

  OpenChannel(0, 512, 4096, 48000, 48000, 48000, 0, 0, 0.010, 0.025, 0.000, 0.010, 0);
  OpenChannel(1, 512, 4096, 48000, 48000, 48000, 1, 0, 0.010, 0.025, 0.000, 0.010, 0);

  if((wisdom_file = fopen("wdsp-fftw-wisdom.txt", "w")))
  {
    fftw_export_wisdom_to_file(wisdom_file);
    fclose(wisdom_file);
  }

  SetRXAShiftRun(0, 0);
  SetRXAAMDRun(0, 1);
  SetRXAMode(0, RXA_LSB);
  SetRXABandpassRun(0, 1);
  SetRXABandpassFreqs(0, -3000.0, 300.0);
  SetRXAAGCMode(0, 3);
  SetRXAAGCTop(0, 40.0);
  RXANBPSetRun(0, 0);

  SetTXAMode(1, TXA_LSB);
  SetTXACompressorRun(1, 1);
  SetTXABandpassRun(1, 1);
  SetTXABandpassFreqs(1, -3000.0, 300.0);
  SetPSRunCal(1, 0);

  *rx_rst |= 1;
  *rx_rst &= ~1;

  *tx_rst |= 1;
  *tx_rst &= ~1;

  SetChannelState(0, 1, 0);
  SetChannelState(1, 1, 0);

  if(pthread_create(&thread, NULL, rx_data_handler, NULL) < 0)
  {
    perror("pthread_create");
    return EXIT_FAILURE;
  }
  pthread_detach(thread);

  if(pthread_create(&thread, NULL, tx_data_handler, NULL) < 0)
  {
    perror("pthread_create");
    return EXIT_FAILURE;
  }
  pthread_detach(thread);

  while(1) usleep(100000);
}

void *rx_data_handler(void *arg)
{
  int32_t i, error;
  double buffer0[1024], buffer1[1024];
  float buffer2[1024];

  while(1)
  {
    if(*rx_cntr >= 2048)
    {
      *rx_rst |= 1;
      *rx_rst &= ~1;
    }

    while(*rx_cntr < 1024) usleep(1000);

    for(i = 0; i < 1024; ++i) buffer0[i] = *rx_data;

    fexchange0(0, buffer0, buffer1, &error);

    for(i = 0; i < 1024; ++i) buffer2[i] = buffer1[i];

    fwrite(buffer2, 4, 1024, stdout);
    fflush(stdout);
  }

  return NULL;
}

void *tx_data_handler(void *arg)
{
  int32_t i, error;
  float buffer0[1024];
  double buffer1[1024], buffer2[1024];

  while(1)
  {
    while(*tx_cntr > 1024) usleep(1000);

    if(*tx_cntr == 0)
    {
      for(i = 0; i < 1024; ++i) *tx_data = 0.0;
    }

    fread(buffer0, 4, 1024, stdin);

    for(i = 0; i < 1024; ++i) buffer1[i] = buffer0[i];

    fexchange0(1, buffer1, buffer2, &error);

    for(i = 0; i < 1024; ++i) *tx_data = buffer2[i];
  }

  return NULL;
}

