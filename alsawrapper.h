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
  inline long framesize(unsigned int size) {return size / (channels_*sample_size_);};
  inline long buffersize() {return buffer_size_;};
  inline long periodsize() {return period_size_;};
 private:
  AlsaWrapper();
  AlsaWrapper(AlsaWrapper&);
  AlsaWrapper& operator=(const AlsaWrapper&);

  int set_hw_params();

  string device_;
  snd_pcm_stream_t stream_;
  snd_pcm_sframes_t period_size_;
  snd_pcm_sframes_t buffer_size_;
  unsigned int rate_;
  unsigned int channels_;
  unsigned sample_size_;
  snd_pcm_format_t format_;
  snd_pcm_t* handle_;
  snd_pcm_access_t access_;
};