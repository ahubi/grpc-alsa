# grpcalsa
This project demonstrates how to access ALSA backend via grpc. It uses streaming capabilities of grpc and ALSA to play / record audio. The server implements the grpc interface and has access to ALSA. The client is using the grpc interface for sending pcm samples to server and receiving pcm samples. First client plays a sine tone of 5 seconds and then records from microphone 5 seconds of data.

![Architecture](/doc/architecture.png)

## interface
```
#grpcalsa.proto
service GrpcAlsa {
  rpc PlayStream(stream AudioData) returns (PlayStatus){}
  rpc RecordStream(RecordRequest) returns (stream AudioData){}
  rpc GetStreamParameter(StreamType) returns (StreamParameter){}
}
```
Please see protos folder for complete definition.

## build environment
Tested with ubuntu 20.04, gcc 7.4.0. Please install all required prerequisites for grpc, see grpc documentation.
This project pulls grpc from github and builds it from source.
Check cmake files for details.