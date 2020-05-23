#include "/usr/include/alsa/asoundlib.h"
#include <string>
#include <iostream>

using namespace std;

static const char* default_device = "plughw:0,0"; 

class AlsaWrapper{

public:
  explicit AlsaWrapper(int access);
  ~AlsaWrapper();
  long read(char* buffer, const long size);
  long write(const char* buffer, const long size);

private:
int set_hw_params();
int xrun_recovery(int err);
string device_;
snd_pcm_access_t access_;
snd_pcm_sframes_t period_size_;
snd_pcm_sframes_t buffer_size_;
unsigned int rate_;
unsigned int channels_;
unsigned sample_size_;
snd_pcm_format_t format_;
snd_pcm_t *handle_;

};