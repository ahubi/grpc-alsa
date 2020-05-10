#include <iostream>
#include <memory>
#include <string>

#include <grpcpp/grpcpp.h>

#include "gprc-alsa.grpc.pb.h"

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;
using gprc-alsa::HelloRequest;
using gprc-alsa::HelloReply;
using gprc-alsa::Greeter;
using gprc-alsa::AudioData;
using gprc-alsa::PlayStatus;
using grpc::ClientWriter;
using grpc::ClientContext;



int main(int argc, char** argv) {
  std::string target_str;
  std::string arg_str("--target");
  if (argc > 1) {
    std::string arg_val = argv[1];
    size_t start_pos = arg_val.find(arg_str);
    if (start_pos != std::string::npos) {
      start_pos += arg_str.size();
      if (arg_val[start_pos] == '=') {
        target_str = arg_val.substr(start_pos + 1);
      } else {
        std::cout << "The only correct argument syntax is --target=" << std::endl;
        return 0;
      }
    } else {
      std::cout << "The only acceptable argument is --target=" << std::endl;
      return 0;
    }
  } else {
    target_str = "localhost:50051";
  }
  return 0;
}
