# grpc-alsa
This project demonstrates how to access ALSA backend via grpc. It uses streaming capabilities of grpc. server implements the interface and has access to ALSA. client is using server via rpc. First client plays a sine tone of 5 seconds and then records from microphone 5 seconds of data. 

![Architecture](/doc/architecture.png)

## interface
```
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