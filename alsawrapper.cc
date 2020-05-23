#include "alsawrapper.h"

AlsaWrapper::AlsaWrapper(int access)
: device_(default_device) 
, access_((snd_pcm_access_t)access)
, period_size_(1024)
, buffer_size_(period_size_*4)
, rate_(48000)
, channels_(1)
, sample_size_(2)
, format_(SND_PCM_FORMAT_S16)
, handle_(0)
{}

AlsaWrapper::~AlsaWrapper(){

}
long AlsaWrapper::read(char* buffer, const long size){
  return 0;
}

long AlsaWrapper::write(const char* buffer, const long size){
  return 0;
}

int AlsaWrapper::set_hw_params(){
  snd_pcm_hw_params_t *params;

  int err;

  if ((err = snd_pcm_hw_params_malloc(&params)) < 0) {
    cerr <<"snd_pcm_hw_params_malloc error " << snd_strerror(err) << endl;
    return err;
  }
  /* choose all parameters */
  if ((err = snd_pcm_hw_params_any(handle_, params)) < 0) {
    cerr <<"snd_pcm_hw_params_any" << snd_strerror(err) << endl;
    return err;
  }
  /* set hardware resampling */
  if ((err = snd_pcm_hw_params_set_rate_resample(handle_, params, rate_)) < 0){
    cerr << "snd_pcm_hw_params_set_rate_resample " << snd_strerror(err) << endl;
    return err;
  }
  /* set the interleaved read/write format */
  if ((err = snd_pcm_hw_params_set_access(handle_, params, access_)) < 0) {
    cerr << "snd_pcm_hw_params_set_access" << snd_strerror(err) << endl;
    return err;
  }
  /* set the sample format */
  if ((err = snd_pcm_hw_params_set_format(handle_, params, format_)) < 0) {
    cerr << "snd_pcm_hw_params_set_format" << snd_strerror(err) << endl;
    return err;
  }
  /* set the count of channels */
  if ((err = snd_pcm_hw_params_set_channels(handle_, params, channels_)) < 0) {
    cerr << "snd_pcm_hw_params_set_channels "  << snd_strerror(err) << endl;
    return err;
  }
  /* set the stream rate */
  if ((err = snd_pcm_hw_params_set_rate_near(handle_, params, &rate_, 0)) < 0) {
    cerr << "snd_pcm_hw_params_set_rate_near " << snd_strerror(err) << endl;
    return err;
  }

  if ((err = snd_pcm_hw_params_set_buffer_size(handle_, params, buffer_size_)) < 0) {
    cerr << "snd_pcm_hw_params_set_buffer_size" << snd_strerror(err) << endl;
    return err;
  }
  if ((err = snd_pcm_hw_params_set_period_size(handle_, params, period_size_, 0)) < 0) {
    cerr << "snd_pcm_hw_params_set_period_size" << snd_strerror(err) << endl;
    return err;
  }

  if ((err = snd_pcm_hw_params(handle_, params)) < 0) {
    cerr << "snd_pcm_hw_params " << snd_strerror(err) << endl;
    return err;
  }
  snd_pcm_hw_params_free(params);
  if ((err = snd_pcm_prepare(handle_)) < 0) {
    cerr << "snd_pcm_prepare "<< snd_strerror(err) << endl;
    return err;
  }
  return 0;
}
int AlsaWrapper::xrun_recovery(int err) {
  if (err == -EPIPE) { /* under-run */
    if ((err = snd_pcm_prepare(handle_)) < 0)
      cerr << "snd_pcm_prepare " << snd_strerror(err) << endl;
    return 0;
  } else if (err == -ESTRPIPE) {
    while ((err = snd_pcm_resume(handle_)) == -EAGAIN)
      sleep(1); /* wait until the suspend flag is released */
    if (err < 0) {
      err = snd_pcm_prepare(handle_);
      if (err < 0)
        cerr << "snd_pcm_prepare: " << snd_strerror(err) << endl;
    }
    return 0;
  }
  return err;
}
