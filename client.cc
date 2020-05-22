#include <grpcpp/grpcpp.h>

#include <iostream>
#include <memory>
#include <string>

#include "grpcalsa.grpc.pb.h"

using grpc::Channel;
using grpc::ClientContext;
using grpc::ClientWriter;
using grpc::ClientReader;
using grpc::Status;
using grpcalsa::AudioData;
using grpcalsa::GrpcAlsa;
using grpcalsa::PlayStatus;
using grpcalsa::RecordRequest;
using namespace std;

class GrpcAlsaClient {
 public:
  GrpcAlsaClient(std::shared_ptr<Channel> channel)
      : stub_(GrpcAlsa::NewStub(channel)) {}

  int PlayStream(const char* data) {
    // Context for the client. It could be used to convey extra information to
    // the server and/or tweak certain RPC behaviors.
    ClientContext context;
    PlayStatus play_status;
    
    AudioData d;
    d.set_data(data);
    unique_ptr<ClientWriter<AudioData> > writer(
        stub_->PlayStream(&context, &play_status));

    bool s = writer->Write(d);
    writer->WritesDone();
    Status status = writer->Finish();
    // Act upon its status.
    if (status.ok()) {
      cout << "Done streaming with status: " << play_status.status() << std::endl;
      return 0;
    } else {
      std::cout << status.error_code() << ": " << status.error_message()
                << std::endl;
      return -1;
    }
  }

  int RecordStream(char* data, int size) {
    // Context for the client. It could be used to convey extra information to
    // the server and/or tweak certain RPC behaviors.
    ClientContext context;
    RecordRequest record_request;
    record_request.set_duration(size);
    record_request.set_id(185);
    
    AudioData d;
    d.set_data(data);
    unique_ptr<ClientReader<AudioData> > reader(
        stub_->RecordStream(&context, record_request));

    bool s = reader->Read(&d);
    Status status = reader->Finish();
    // Act upon its status.
    if (status.ok()) {
      cout << "Done streaming with status: " << endl;
      return 0;
    } else {
      cout << status.error_code() << ": " << status.error_message()
                << endl;
      return -1;
    }
  }

 private:
  std::unique_ptr<GrpcAlsa::Stub> stub_;
};

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
  cout << "gRPC client started " << target_str << endl;
  std::shared_ptr<Channel> channel(grpc::CreateChannel(
      target_str, grpc::InsecureChannelCredentials()));
  GrpcAlsaClient gac(channel);
  gac.PlayStream("Hello World");
  char* buffer = new char[16];
  gac.RecordStream(buffer, sizeof(buffer));
  
  delete [] buffer;
  return 0;
}
