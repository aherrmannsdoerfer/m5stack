#pragma once

#include "namespace.h"
#include "crgb.h"
#include "ref.h"
#include "fx/storage/filebuffer.h"
#include "fx/storage/bytestream.h"
#include "fx/frame.h"

FASTLED_NAMESPACE_BEGIN

FASTLED_SMART_REF(DataStream);

// DataStream is a class that reads data from a file or a stream.
// It's sort of a merging point for the two types of data sources.
class DataStream: public Referent {
 public:

  enum Type {
    kStreaming,
    kFile,
  };

  explicit DataStream(int bytes_per_frame);

  bool begin(FileHandleRef h);
  bool beginStream(ByteStreamRef s);
  void Close();
  int32_t BytesPerFrame();
  bool ReadPixel(CRGB* dst);
  size_t ReadBytes(uint8_t* dst, size_t len);

  bool readFrame(Frame* frame);
  int32_t FramesRemaining() const;
  int32_t FramesDisplayed() const;
  bool available() const;

  int32_t BytesRemaining() const;
  int32_t BytesRemainingInFrame() const;
  bool Rewind();  // Returns false on failure, which can happen for streaming mode.
  Type getType() const;  // Returns the type of the video stream (kStreaming or kFile)
  
 private:
  void init(int bytes_per_frame);
  int32_t mBytesPerFrame;
  FileHandleRef mFileHandle;
  FileBufferRef mFileBuffer;
  ByteStreamRef mByteStream;
  bool mUsingByteStream;

protected:
  virtual ~DataStream();
};

FASTLED_NAMESPACE_END