#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <math.h>
#include <pthread.h>
#include <termios.h>
#include <sys/mman.h>
#include <sys/ioctl.h>

#include <alsa/asoundlib.h>
#include <fftw3.h>

#include "comm.h"

volatile float *rx_data, *tx_data;
volatile uint16_t *rx_cntr, *tx_cntr;
volatile uint8_t *rx_rst, *tx_rst, *gpio;

void *rx_data_handler(void *arg);
void *tx_data_handler(void *arg);

int main()
{
  int fd, uart, running, filter, mode;
  FILE *wisdom_file;
  pthread_t thread;
  void *cfg, *sts;
  volatile uint32_t *rx_phase, *tx_phase;
  volatile uint16_t *rx_rate, *tx_rate;
  unsigned long size = 0;
  struct termios tty;
  uint8_t buffer[6];
  uint8_t code;
  uint32_t data;
  uint8_t crc8;
  int32_t rx_freq, tx_freq, shift;
  long min, max;
  snd_mixer_t *handle;
  snd_mixer_selem_id_t *sid;
  snd_mixer_elem_t *playback, *recording;
  double cutoff[5][10][2] = {
    {{-613, -587}, {-625, -575}, {-650, -550}, {-725, -475}, {-800, -400}, {-850, -350}, {-900, -300}, {-975, -225}, {-1000, -200}, {-1100, -100}},
    {{587, 613}, {575, 625}, {550, 650}, {475, 725}, {400, 800}, {350, 850}, {300, 900}, {225, 975}, {200, 1000}, {100, 1100}},
    {{-1150, -150}, {-1950, -150}, {-2250, -150}, {-2550, -150}, {-2850, -150}, {-3050, -150}, {-3450, -150}, {-3950, -150}, {-4550, -150}, {-5150, -150}},
    {{150, 1150}, {150, 1950}, {150, 2250}, {150, 2550}, {150, 2850}, {150, 3050}, {150, 3450}, {150, 3950}, {150, 4550}, {150, 5150}},
    {{-2500, 2500}, {-3000, 3000}, {-3500, 3500}, {-4000, 4000}, {-4500, 4500}, {-5000, 5000}, {-6000, 6000}, {-8000, 8000}, {-9000, 9000}, {-10000, 10000}}};

  if((fd = open("/dev/mem", O_RDWR)) < 0)
  {
    perror("open");
    return EXIT_FAILURE;
  }

  if((uart = open("/dev/ttyPS1", O_RDWR|O_NOCTTY|O_NDELAY)) < 0)
  {
    perror("open");
    return EXIT_FAILURE;
  }

  tcgetattr(uart, &tty);
  cfsetspeed(&tty, (speed_t)B115200);
  tty.c_cflag &= ~PARENB;
  tty.c_cflag &= ~CSTOPB;
  tty.c_cflag &= ~CSIZE;
  tty.c_cflag |= CS8 | CLOCAL;
  tcflush(uart, TCIFLUSH);
  tcsetattr(uart, TCSANOW, &tty);

  snd_mixer_open(&handle, 0);
  snd_mixer_attach(handle, "default");
  snd_mixer_selem_register(handle, NULL, NULL);
  snd_mixer_load(handle);

  snd_mixer_selem_id_alloca(&sid);
  snd_mixer_selem_id_set_index(sid, 0);
  snd_mixer_selem_id_set_name(sid, "Speaker");
  playback = snd_mixer_find_selem(handle, sid);

  snd_mixer_selem_id_alloca(&sid);
  snd_mixer_selem_id_set_index(sid, 0);
  snd_mixer_selem_id_set_name(sid, "Mic");
  recording = snd_mixer_find_selem(handle, sid);

  sts = mmap(NULL, sysconf(_SC_PAGESIZE), PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0x40000000);
  cfg = mmap(NULL, sysconf(_SC_PAGESIZE), PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0x40001000);
  rx_data = mmap(NULL, 2*sysconf(_SC_PAGESIZE), PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0x40002000);
  tx_data = mmap(NULL, 2*sysconf(_SC_PAGESIZE), PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0x40008000);

  rx_rst = ((uint8_t *)(cfg + 0));
  rx_rate = ((uint16_t *)(cfg + 4));
  rx_phase = ((uint32_t *)(cfg + 8));
  rx_cntr = ((uint16_t *)(sts + 12));

  tx_rst = ((uint8_t *)(cfg + 1));
  tx_rate = ((uint16_t *)(cfg + 20));
  tx_phase = ((uint32_t *)(cfg + 24));
  tx_cntr = ((uint16_t *)(sts + 18));

  gpio = ((uint8_t *)(cfg + 2));

  /* set default rx sample rate */
  *rx_rate = 1250;

  /* set default tx sample rate */
  *tx_rate = 1250;

  /* set PTT pin to low */
  *gpio = 0;

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
  SetRXABandpassRun(0, 1);
  SetRXAAGCMode(0, 3);
  SetRXAAGCTop(0, 40.0);
  RXANBPSetRun(0, 0);

  SetTXACompressorRun(1, 1);
  SetTXABandpassRun(1, 1);
  SetPSRunCal(1, 0);

  mode = 3;
  shift = 0;
  filter = 4;
  rx_freq = 7200000000;
  tx_freq = 7200000000;

  *rx_phase = (uint32_t)floor((rx_freq + shift) / 125.0e6 * (1<<30) + 0.5);
  *tx_phase = (uint32_t)floor(tx_freq / 125.0e6 * (1<<30) + 0.5);

  SetRXAMode(0, RXA_LSB);
  SetTXAMode(1, TXA_LSB);

  SetRXABandpassFreqs(0, cutoff[mode][filter][0], cutoff[mode][filter][1]);
  SetTXABandpassFreqs(1, cutoff[mode][filter][0], cutoff[mode][filter][1]);

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

  running = 1;
  while(running)
  {
    usleep(50000);
    /* update RX meter */
    while(1)
    {
      ioctl(uart, FIONREAD, &size);
      if(size < 6) break;
      read(uart, buffer, 6);
      code = *(uint8_t *)(buffer + 0);
      data = *(uint32_t *)(buffer + 1);
      crc8 = *(uint8_t *)(buffer + 5);
      switch(code)
      {
        case 1:
          if(data > 62000000) continue;
          rx_freq = data;
          *rx_phase = (uint32_t)floor((rx_freq + shift) / 125.0e6 * (1<<30) + 0.5);
          break;
        case 2:
          if(data > 62000000) continue;
          tx_freq = data;
          *tx_phase = (uint32_t)floor(tx_freq / 125.0e6 * (1<<30) + 0.5);
          break;
        case 3:
          snd_mixer_selem_get_playback_volume_range(playback, &min, &max);
          snd_mixer_selem_set_playback_volume_all(playback, data * max / 9);
          break;
        case 4:
          snd_mixer_selem_get_playback_volume_range(recording, &min, &max);
          snd_mixer_selem_set_playback_volume_all(recording, data * max / 9);
          break;
        case 5:
          if(data > 9) continue;
          filter = data;
          SetRXABandpassFreqs(0, cutoff[mode][filter][0], cutoff[mode][filter][1]);
          SetTXABandpassFreqs(1, cutoff[mode][filter][0], cutoff[mode][filter][1]);
          break;
        case 6:
          if(data > 4) continue;
          mode = data;
          switch(mode)
          {
            case 0:
              shift = 600;
              SetRXAMode(0, RXA_CWL);
              SetTXAMode(1, TXA_CWL);
              break;
            case 1:
              shift = -600;
              SetRXAMode(0, RXA_CWU);
              SetTXAMode(1, TXA_CWU);
              break;
            case 2:
              shift = 0;
              SetRXAMode(0, RXA_LSB);
              SetTXAMode(1, TXA_LSB);
              break;
            case 3:
              shift = 0;
              SetRXAMode(0, RXA_USB);
              SetTXAMode(1, TXA_USB);
              break;
            case 4:
              shift = 0;
              SetRXAMode(0, RXA_AM);
              SetTXAMode(1, TXA_AM);
              break;
          }
          *rx_phase = (uint32_t)floor((rx_freq + shift) / 125.0e6 * (1<<30) + 0.5);
          SetRXABandpassFreqs(0, cutoff[mode][filter][0], cutoff[mode][filter][1]);
          SetTXABandpassFreqs(1, cutoff[mode][filter][0], cutoff[mode][filter][1]);
          break;
        case 8:
          if(data > 1) continue;
          *gpio = data;
          break;
        case 9:
          running = 0;
          break;
      }
    }
  }

  /* set PTT pin to low */
  *gpio = 0;

  return EXIT_SUCCESS;
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

    if(*gpio & 1)
    {
      for(i = 0; i < 1024; ++i) *tx_data = buffer2[i];
    }
    else
    {
      for(i = 0; i < 1024; ++i) *tx_data = 0.0;
    }
  }

  return NULL;
}

