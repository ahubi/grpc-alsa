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
using grpcalsa::StreamType;
using grpcalsa::StreamParameter;

using namespace std;

// Logic and data behind the server's behavior.
class GrpcAlsaServiceImpl final : public GrpcAlsa::Service {
public:
  GrpcAlsaServiceImpl(){}
  ~GrpcAlsaServiceImpl(){}
  
  Status PlayStream(ServerContext* context, 
                    ServerReader<AudioData>* reader, 
                    PlayStatus* response){
    cout << "--> " << __func__ << endl;
    AudioData data;
    
    unique_ptr<AlsaWrapper> alsa_writer(new AlsaWrapper(SND_PCM_STREAM_PLAYBACK));

    while (reader->Read(&data))
      alsa_writer->write(data.data().data(), data.data().size());
  
    alsa_writer->drain();
    response->set_status(0);
    cout << "<-- " << __func__ << endl;
    return Status::OK;
  }
  Status RecordStream(ServerContext* context, const RecordRequest* request,
                      ServerWriter<AudioData>* writer) {
    cout << "--> " << __func__ << endl;
    AudioData data;
    unique_ptr<AlsaWrapper> alsareader(new AlsaWrapper(SND_PCM_STREAM_CAPTURE));
    long size = alsareader->periodsize()*alsareader->framesize();
    char* buffer = new char[size];
    //Record 10 x period size to have longer recordings
    for (size_t i = 0; i < request->duration()*10; i++)
    {
      long readsize = alsareader->read(buffer, size);
      data.set_data(buffer, readsize);
      writer->Write(data);
    }
    if (buffer)
      delete [] buffer;
    cout << "<-- " << __func__ << endl;
    return Status::OK;
  }
  Status GetStreamParameter(ServerContext* context, 
                            const StreamType* request, 
                            StreamParameter* response) override {
    cout << "--> " << __func__ << endl;
    unique_ptr<AlsaWrapper> a(new AlsaWrapper(SND_PCM_STREAM_PLAYBACK));                          
    response->set_buffersize(a->periodsize()*a->framesize());
    response->set_format(a->fromat());
    response->set_framerate(a->rate());
    response->set_samplesize(a->samplesize());
    cout << "<-- " << __func__ << endl;
    return Status::OK;
  }
};

void RunServer() {
  cout << "gRPC " << __func__ << endl;

  std::string server_address("localhost:50051");
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
