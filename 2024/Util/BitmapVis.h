#define NOMINMAX
#define _WIN32_WINNT 0x0601
#pragma warning (push)
#pragma warning (disable: 4668 4917 4365 4987 4623 4626 5027)
#include <windows.h>
#include <wincodec.h>
#include <wincodecsdk.h>
#pragma warning (pop)

#include "ComPtr.h"

inline void SaveWicTexture(const wchar_t *path, u32 width, u32 height, ArrayView<u32> argbColors)
{
  CoInitialize(nullptr);

  FixedArray<u32> swizzledColors { argbColors.Count() };
  for (u32 i = 0; i < argbColors.Count(); i++)
  {
    swizzledColors[i] =
      (argbColors[i] & 0xFF00FF00)
      | ((argbColors[i] & 0x000000FF) << 16)
      |((argbColors[i] & 0x00FF0000) >> 16);
  }

  ComPtr<IWICImagingFactory> imagingFactory;
  CoCreateInstance(
    CLSID_WICImagingFactory,
    nullptr,
    CLSCTX_INPROC_SERVER,
    IID_IWICImagingFactory,
    (void**)imagingFactory.AddressForReplace());

  ComPtr<IWICStream> stream;
  imagingFactory->CreateStream(stream.AddressForReplace());
  stream->InitializeFromFilename(path, GENERIC_WRITE);

  ComPtr<IWICBitmapEncoder> encoder;
  imagingFactory->CreateEncoder(GUID_ContainerFormatPng, nullptr, encoder.AddressForReplace());
  encoder->Initialize(stream,  WICBitmapEncoderNoCache);

  ComPtr<IWICBitmapFrameEncode> frameEncode;
  encoder->CreateNewFrame(frameEncode.AddressForReplace(), nullptr);
  frameEncode->Initialize(nullptr);
  frameEncode->SetSize(width, height);

  auto guid = GUID_WICPixelFormat32bppBGRA;
  frameEncode->SetPixelFormat(&guid);

  if (guid != GUID_WICPixelFormat32bppBGRA)
  {
    char exceptionStr[1024];
    std::snprintf(exceptionStr, sizeof(exceptionStr), "Failed to set expected pixel format");
    throw std::runtime_error(exceptionStr);
  }

  frameEncode->WritePixels(
    height,
    width * sizeof(uint32_t),
    width * height * sizeof(uint32_t),
    const_cast<uint8_t *>(reinterpret_cast<const uint8_t*>(swizzledColors.Elements())));

  frameEncode->Commit();
  encoder->Commit();

  frameEncode = nullptr;
  encoder = nullptr;

  stream->Commit(STGC_OVERWRITE);
  stream = nullptr;
}


template <typename T, typename Func>
void WriteAsPNG(const Array2D<T> &array, const wchar_t *path, Func &&f)
{
  s32 scale = 4;
  FixedArray<u32> pixels { array.Width() * array.Height() * scale * scale };
  for (ssz y = 0; y < array.Height(); y++)
  {
    for (ssz x = 0; x < array.Width(); x++)
    {
      auto color = f(array[{x, y}], x, y);
      for (ssz i = 0; i < scale; i++)
      {
        for (ssz j = 0; j < scale; j++)
        {
          ssz pxY = y * scale + i;
          ssz pxX = x * scale + j;
          pixels[pxY * scale * array.Width() + pxX] = color;
        }
      }
    }
  }

  SaveWicTexture(path, u32(array.Width() * scale), u32(array.Height() * scale), pixels);
}