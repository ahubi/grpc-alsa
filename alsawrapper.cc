#include "alsawrapper.h"

AlsaWrapper::AlsaWrapper(int stream)
: device_(default_device) 
, stream_((snd_pcm_stream_t)stream)
, period_size_(1024)
, buffer_size_(period_size_*4)
, rate_(24000)
, channels_(1)
, sample_size_(2)
, format_(SND_PCM_FORMAT_S16)
, handle_(0)
, access_(SND_PCM_ACCESS_RW_INTERLEAVED){
  int err;
  if ((err = snd_pcm_open(&handle_, device_.c_str(), stream_, 0)) == 0){
    cout << "alsa device opened " << device_ << endl;
    if (set_hw_params() < 0){
      cerr << "set_hw_params failed" << endl;
      if ((err=snd_pcm_close(handle_)) < 0){
        cerr << "snd_pcm_close " << snd_strerror(err) << endl;
      }
    }
  }else{
    cerr << "snd_pcm_open " << snd_strerror(err) << endl;
    handle_ = 0;
  }
}

AlsaWrapper::~AlsaWrapper() {
  int err;
  if (handle_)
    if ((err = snd_pcm_close(handle_)) < 0) {
      cerr << "snd_pcm_close " << snd_strerror(err) << endl;
    }
}

long AlsaWrapper::read(char* buffer, const long size){
  snd_pcm_sframes_t frames;
  if(good() && readable()){
    while((frames = snd_pcm_readi(handle_, buffer, framesize(size))<0))
      if (snd_pcm_recover(handle_, frames, 0) < 0)
        return -1;
  }
  return frames;
}

long AlsaWrapper::write(const char* buffer, const long size){
  snd_pcm_sframes_t frames;
  if(good() && writable()){
    while((frames = snd_pcm_writei(handle_, buffer, framesize(size))<0))
      if (snd_pcm_recover(handle_, frames, 0) < 0)
        return -1;
  }
  return frames;
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