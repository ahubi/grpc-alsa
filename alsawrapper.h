#include <iostream>
#include <string>

#include "/usr/include/alsa/asoundlib.h"

using namespace std;

static const char* default_device = "plughw:0,0";

class AlsaWrapper {
 public:
  explicit AlsaWrapper(int access);
  ~AlsaWrapper();
  long read(char* buffer, const long size);
  long write(const char* buffer, const long size);
  int drain(){return snd_pcm_drain(handle_);};
  inline bool good(){return handle_ != 0;};
  inline bool readable() {return stream_ == SND_PCM_STREAM_CAPTURE;};
  inline bool writable() {return stream_ == SND_PCM_STREAM_PLAYBACK;};
  inline long framesize() {return channels_*sample_size_;};
  inline long framesize(unsigned int size) {return size / framesize();};
  inline long buffersize() {return buffer_size_;};
  inline long periodsize() {return period_size_;};
  inline uint rate() {return rate_;};
  inline uint fromat() {return format_;};
  inline uint samplesize() {return sample_size_;};
 private:
  AlsaWrapper();
  AlsaWrapper(AlsaWrapper&);
  AlsaWrapper& operator=(const AlsaWrapper&);

  int set_hw_params();

  string device_;
  snd_pcm_stream_t stream_;
  snd_pcm_sframes_t period_size_;
  snd_pcm_sframes_t buffer_size_;
  uint rate_;
  uint channels_;
  uint sample_size_;
  snd_pcm_format_t format_;
  snd_pcm_t* handle_;
  snd_pcm_access_t access_;
};