#pragma once

// Standard lib
#include <vector>

class SrtGenericPNGParser
{
  public:
  static int DecodePNG(
      std::vector<unsigned char>& out_image,
      unsigned long& image_width,
      unsigned long& image_height,
      const unsigned char* in_png,
      unsigned int in_size,
      bool convert_to_rgba32);
};
