/*
 * project.c
 * test parallel port [pin15] and take action accordingly
 */ 
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

#ifndef _NO_AUDIO_
#include <ao/ao.h>	/* audio output library */
#include <math.h>	/* audio helper math function */
#endif
#include "parapin.h"	/* parallel pin library */

#ifndef _NO_AUDIO_
static void init_audio_output(void);
static void shut_audio_output(void);
#endif
static void do_something(void); /* do something when something happened */

/* pins may only be queried if:
 * 1. they are permanent input pins (10, 11, 12, 13, 15)
 * 2. they are bidirectional pins configured as input pins
 */
#define PIN_TO_CHECK LP_PIN15		/* we will check pin 15 */
#define DEFAULT_SLEEP_US 1000

static int count;

void handle_signal(int signo);

int main(int argc, char *argv[])
{
  int sleeptime; /* default is 1 ms */

  if (pin_init_user(LPT1) < 0) /* initialize, ioperm, etc */
    exit(1);

  /*
   * if you use any of the data pins [LP_PIN{02-09}] then do the
   * following
   */
//  pin_input_mode(LP_DATA_PINS | LP_SWITCHABLE_PINS);

  signal(SIGINT, handle_signal); /* handle if interrupted */
  signal(SIGHUP, handle_signal); /* handle if interrupted */
  signal(SIGQUIT, handle_signal); /* handle if interrupted */

  if (argc > 1) {
#ifdef _DEBUG_
    fprintf(stderr, "Setting sleep time to %s us\n", argv[1]);
#endif
    sleeptime = (int) strtol(argv[1], (char **) NULL, 10);
  } else {
    sleeptime = DEFAULT_SLEEP_US;
  }

#ifndef _NO_AUDIO_
  init_audio_output();
#endif

  while (1) {
    if (pin_is_set(PIN_TO_CHECK)) { /* something happened */
#ifdef _DEBUG_
      fprintf(stderr, "Pin high %d\n", ++count);
#endif
      do_something();
    } else {
      count = 0; /* reset counter */
    }
    usleep(sleeptime); /* sleep N microseconds, delay */
  }

  return 0; /* never reach here */
}

#ifndef _NO_AUDIO_
static char *buffer;	/* audio buffer */
static int buf_size;
#define BUF_SIZE 4096
#endif
static void do_something(void)
{
#ifndef _NO_AUDIO_
  ao_device *device;
  ao_sample_format format;
  int default_driver;
  int sample;
  float freq = 440.0;
  int i;

  /* setup for default driver */
  default_driver = ao_default_driver_id();

  format.bits = 16;
  format.channels = 2;
  format.rate = 44100;
  format.byte_format = AO_FMT_LITTLE; /* little-endian */

  /* open driver */
  device = ao_open_live(default_driver, &format, NULL); /* NULL=no option */
  if (device == NULL) {
    fprintf(stderr, "Error opening device.\n");
    return;
  }

  if (!buffer) {
    buf_size = format.bits/8 * format.channels * format.rate;
    buffer = calloc(buf_size, sizeof(char));
  }

  for (i = 0; i < format.rate; i++) {
    sample = (int) (0.75 * 32768.0 *
       	sin(3 * M_PI * freq * ((float) i/format.rate)));
    /* put the same stuff in the left and right channel */
    buffer[4*i] = buffer[4*i+2] = sample & 0xff;
    buffer[4*i+1] = buffer[4*i+3] = (sample >> 8) & 0xff;
  }

  for (i = 0; i < 5; i++) { /* loop so that the sound lasts long */
    /* play it */
    ao_play(device, buffer, buf_size);
  }

  /* close and shutdown */
  ao_close(device);
#else	/* _NO_AUDIO_ */
#ifndef _DEBUG_
  fprintf(stderr, "Alarm Alarm Alarm...%d\n", count);
#endif
#endif
}

void handle_signal(int signo)
{
#ifdef _DEBUG_
  fprintf(stderr, "Signal <%d> caught. Exiting...\n", signo);
#endif
#ifndef _NO_AUDIO_
  shut_audio_output();
#endif
  exit(signo);
}

#ifndef _NO_AUDIO_
static void init_audio_output(void)
{
#ifdef _DEBUG_
  fprintf(stderr, "Initializing audio output...\n");
#endif
  /* initialize */
  ao_initialize();

}

static void shut_audio_output(void)
{
#ifdef _DEBUG_
  fprintf(stderr, "Shutting down audio output\n");
#endif

  if (buffer)
    free(buffer);

  ao_shutdown();
}
#endif
