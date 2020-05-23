#include <iostream>
#include <memory>
#include <string>

#include <grpcpp/grpcpp.h>

#include <grpcpp/health_check_service_interface.h>
#include <grpcpp/ext/proto_server_reflection_plugin.h>

#include "grpcalsa.grpc.pb.h"
#include "alsawrapper.h"

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;
using grpc::ServerReader;
using grpc::ServerWriter;
using grpcalsa::GrpcAlsa;
using grpcalsa::AudioData;
using grpcalsa::PlayStatus;
using grpcalsa::RecordRequest;

using namespace std;

// Logic and data behind the server's behavior.
class GrpcAlsaServiceImpl final : public GrpcAlsa::Service {
public:
  GrpcAlsaServiceImpl(){
    alsareader_ = new AlsaWrapper(SND_PCM_STREAM_CAPTURE);
    alsawriter_ = new AlsaWrapper(SND_PCM_STREAM_PLAYBACK);
    buffer_ = new char[alsareader_->periodsize()];
  }
  ~GrpcAlsaServiceImpl(){
    if(alsareader_)
      delete alsareader_; 
    if(alsawriter_)
      delete alsawriter_;
    if (buffer_)
      delete [] buffer_;
  }
  Status PlayStream(ServerContext* context, 
                    ServerReader<AudioData>* reader, 
                    PlayStatus* response){
    AudioData data;
    while (reader->Read(&data)) {
      alsawriter_->write(data.data().data(), data.data().size());
    }
    response->set_status(77);
    return Status::OK;
  }
  Status RecordStream(ServerContext* context, const RecordRequest* request,
                      ServerWriter<AudioData>* writer) {
    AudioData data;
    long readsize = alsareader_->read(buffer_, alsareader_->periodsize());
    data.set_data(buffer_, readsize);
    writer->Write(data);
    return Status::OK;
  }
private:
 AlsaWrapper* alsawriter_;
 AlsaWrapper* alsareader_;
 char* buffer_;

};

void RunServer() {
  cout << "gRPC " << __func__ << endl;

  std::string server_address("0.0.0.0:50051");
  GrpcAlsaServiceImpl service;

  grpc::EnableDefaultHealthCheckService(true);
  grpc::reflection::InitProtoReflectionServerBuilderPlugin();
  ServerBuilder builder;
  // Listen on the given address without any authentication mechanism.
  builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
  // Register "service" as the instance through which we'll communicate with
  // clients. In this case it corresponds to an *synchronous* service.
  builder.RegisterService(&service);
  // Finally assemble the server.
  std::unique_ptr<Server> server(builder.BuildAndStart());
  std::cout << "Server listening on " << server_address << std::endl;

  // Wait for the server to shutdown. Note that some other thread must be
  // responsible for shutting down the server for this call to ever return.
  server->Wait();
}

int main(int argc, char** argv) {
  RunServer();

  return 0;
}
