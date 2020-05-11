#include <iostream>
#include <memory>
#include <string>

#include <grpcpp/grpcpp.h>

#include <grpcpp/health_check_service_interface.h>
#include <grpcpp/ext/proto_server_reflection_plugin.h>

#include "grpcalsa.grpc.pb.h"

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;
using grpc::ServerReader;
using grpcalsa::AudioData;
using grpcalsa::PlayStatus;


void RunServer() {
}

int main(int argc, char** argv) {
  RunServer();

  return 0;
}
