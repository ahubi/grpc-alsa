#include <iostream>
#include <memory>
#include <string>

#include <grpcpp/grpcpp.h>

#include "grpcalsa.grpc.pb.h"

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;
using grpcalsa::AudioData;
using grpcalsa::PlayStatus;
using grpc::ClientWriter;
using grpc::ClientContext;
using namespace std;


int main(int argc, char** argv) {
  string target_str;
  string arg_str("--target");
  if (argc > 1) {
    string arg_val = argv[1];
    size_t start_pos = arg_val.find(arg_str);
    if (start_pos != std::string::npos) {
      start_pos += arg_str.size();
      if (arg_val[start_pos] == '=') {
        target_str = arg_val.substr(start_pos + 1);
      } else {
        cout << "The only correct argument syntax is --target=" << std::endl;
        return 0;
      }
    } else {
      cout << "The only acceptable argument is --target=" << std::endl;
      return 0;
    }
  } else {
    target_str = "localhost:50051";
  }
  cout << "Hello gRPC" << endl;
  return 0;
}
