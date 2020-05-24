#include <grpcpp/grpcpp.h>

#include <iostream>
#include <memory>
#include <string>
#include <fstream>

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
using grpcalsa::StreamType;
using grpcalsa::StreamParameter;

using namespace std;

class GrpcAlsaClient {
 public:
  GrpcAlsaClient(std::shared_ptr<Channel> channel)
      : stub_(GrpcAlsa::NewStub(channel)) {}

  int PlayStream(string filename) {
    // Context for the client. It could be used to convey extra information to
    // the server and/or tweak certain RPC behaviors.
    ClientContext context, ctx;
    PlayStatus play_status;
    StreamParameter sp;
    StreamType st;
    ifstream file(filename, ios::in | ios::binary | ios::ate);
    
    //Get alsa related parameter from server
    Status s = stub_->GetStreamParameter(&ctx, st, &sp);
    int size = sp.buffersize() > 0 ? sp.buffersize() : 1024;
    char* memblock = new char[size];

    if (file.is_open()) {
      cout << "playing file: " << filename << endl;
      file.seekg(0, ios::beg);
      unique_ptr<ClientWriter<AudioData> > writer(
            stub_->PlayStream(&context, &play_status));
      AudioData d;
      while (file.read(memblock, size)) {
        d.set_data(memblock, size);
        bool s = writer->Write(d);
      }
      writer->WritesDone();
      Status status = writer->Finish();
      // Act upon its status.
      if (status.ok()) {
        cout << "Done streaming with status: " << play_status.status()
             << std::endl;
      } else {
        std::cout << status.error_code() << ": " << status.error_message()
                  << std::endl;
      }
    }else
    {
      cerr << "file not open: " << strerror(errno) << endl;
    }
    
    delete [] memblock;
    file.close();
    return 0;
  }

  int RecordStream(string filename) {
    // Context for the client. It could be used to convey extra information to
    // the server and/or tweak certain RPC behaviors.
    ClientContext context;
    RecordRequest record_request;
    record_request.set_duration(5);
    record_request.set_id(10);
    
    AudioData d;
    unique_ptr<ClientReader<AudioData> > reader(
        stub_->RecordStream(&context, record_request));
    ofstream file(filename, ios::out | ios::binary);
    if (file.is_open()) {
      cout << "recording to file: " << filename << endl;
      while(reader->Read(&d)){
        file.write(d.data().data(), d.data().size());
      }
      Status status = reader->Finish();
      file.close();
      // Act upon its status.
      if (status.ok()) {
        cout << "finished recording with status OK" << endl;
        return 0;
      } else {
        cout << status.error_code() << ": " << status.error_message() << endl;
        return -1;
      }
    }
    return 0;
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
  GrpcAlsaClient gaclient(channel);
  gaclient.PlayStream("../sine_16bit_1ch_24000Hz.wav");
  gaclient.RecordStream("recorded_stream.wav");
  
  return 0;
}
