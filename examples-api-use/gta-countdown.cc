// -*- mode: c++; c-basic-offset: 2; indent-tabs-mode: nil; -*-
//
// Example how to display an image, including animated images using
// ImageMagick. For a full utility that does a few more things, have a look
// at the led-image-viewer in ../utils
//
// Showing an image is not so complicated, essentially just copy all the
// pixels to the canvas. How to get the pixels ? In this example we're using
// the graphicsmagick library as universal image loader library that
// can also deal with animated images.
// You can of course do your own image loading or use some other library.
//
// This requires an external dependency, so install these first before you
// can call `make image-example`
//   sudo apt-get update
//   sudo apt-get install libgraphicsmagick++-dev libwebp-dev -y
//   make image-example

#include "led-matrix.h"
#include "graphics.h"

#include <getopt.h>
#include <math.h>
#include <signal.h>
#include <stdio.h>
#include <unistd.h>

#include <stdlib.h>
#include <string.h>
#include <ctime>

#include <exception>
#include <Magick++.h>
#include <magick/image.h>

using rgb_matrix::Canvas;
using rgb_matrix::FrameCanvas;
using rgb_matrix::RGBMatrix;

using namespace rgb_matrix;

// Make sure we can exit gracefully when Ctrl-C is pressed.
volatile bool interrupt_received = false;
static void InterruptHandler(int signo)
{
  interrupt_received = true;
}

using ImageVector = std::vector<Magick::Image>;

// Given the filename, load the image and scale to the size of the
// matrix.
// // If this is an animated image, the resutlting vector will contain multiple.
static ImageVector LoadImageAndScaleImage(const char *filename,
                                          int target_width,
                                          int target_height)
{
  ImageVector result;

  ImageVector frames;
  try
  {
    readImages(&frames, filename);
  }
  catch (std::exception &e)
  {
    if (e.what())
      fprintf(stderr, "%s\n", e.what());
    return result;
  }

  if (frames.empty())
  {
    fprintf(stderr, "No image found.");
    return result;
  }

  // Animated images have partial frames that need to be put together
  if (frames.size() > 1)
  {
    Magick::coalesceImages(&result, frames.begin(), frames.end());
  }
  else
  {
    result.push_back(frames[0]); // just a single still image.
  }

  for (Magick::Image &image : result)
  {
    image.scale(Magick::Geometry(target_width, target_height));
  }

  return result;
}

// Copy an image to a Canvas. Note, the RGBMatrix is implementing the Canvas
// interface as well as the FrameCanvas we use in the double-buffering of the
// animted image.
void CopyImageToCanvas(const Magick::Image &image, Canvas *canvas)
{
  const int offset_x = 1, offset_y = 2; // If you want to move the image.
  // Copy all the pixels to the canvas.
  for (size_t y = 0; y < image.rows(); ++y)
  {
    for (size_t x = 0; x < image.columns(); ++x)
    {
      const Magick::Color &c = image.pixelColor(x, y);
      if (c.alphaQuantum() < 256)
      {
        canvas->SetPixel(x + offset_x, y + offset_y,
                         ScaleQuantumToChar(c.redQuantum()),
                         ScaleQuantumToChar(c.greenQuantum()),
                         ScaleQuantumToChar(c.blueQuantum()));
      }
    }
  }
}

int usage(const char *progname)
{
  fprintf(stderr, "Usage: %s [led-matrix-options] -i <image-filename> -f <BDF font>\n",
          progname);
  rgb_matrix::PrintMatrixFlags(stderr);
  return 1;
}

// Returns a std::tm where:
//   tm_mday = days remaining
//   tm_hour = hours remaining
//   tm_min  = minutes remaining
//   tm_sec  = seconds remaining
// Calculated using the system's local time.
std::tm getTimeRemaining(int year, int month, int day)
{
  std::time_t now = std::time(nullptr); // current local/system time
  std::tm result{};
  localtime_r(&now, &result);

  std::tm target{};
  target.tm_year = year - 1900;
  target.tm_mon = month - 1;
  target.tm_mday = day;
  target.tm_hour = 0;
  target.tm_min = 0;
  target.tm_sec = 0;
  target.tm_isdst = result.tm_isdst; // let mktime figure out daylight saving

  std::time_t targetTime = std::mktime(&target); // treated as local time

  long long secondsRemaining = static_cast<long long>(targetTime - now);
  if (secondsRemaining < 0)
    secondsRemaining = 0;

  result.tm_mday = secondsRemaining / 86400;
  result.tm_hour = (secondsRemaining % 86400) / 3600;
  result.tm_min = (secondsRemaining % 3600) / 60;
  result.tm_sec = secondsRemaining % 60;

  return result;
}

// Converts the "remaining time" tm struct into a char buffer.
// Returns the number of characters written (excluding null terminator),
// or a negative value on error.
int formatTimeRemaining(const std::tm &remaining, char *buffer, size_t bufferSize)
{
  return std::snprintf(buffer, bufferSize, "%d:%02d:%02d:%02d", remaining.tm_mday, remaining.tm_hour, remaining.tm_min, remaining.tm_sec);
}

int main(int argc, char *argv[])
{
  Magick::InitializeMagick(*argv);

  // Initialize the RGB matrix with
  RGBMatrix::Options matrix_options;
  rgb_matrix::RuntimeOptions runtime_opt;
  if (!rgb_matrix::ParseOptionsFromFlags(&argc, &argv, &matrix_options, &runtime_opt))
  {
    return usage(argv[0]);
  }

  // We accept multiple format lines
  Color color(255, 255, 0);
  Color bg_color(0, 0, 0);
  Color outline_color(0, 0, 0);

  const char *bdf_font_file = NULL;
  const char *filename = NULL;

  int opt;
  while ((opt = getopt(argc, argv, "f:i:")) != -1)
  {
    switch (opt)
    {
    case 'f':
      bdf_font_file = strdup(optarg);
      break;
    case 'i':
      filename = strdup(optarg);
      break;
    default:
      return usage(argv[0]);
    }
  }

  if (bdf_font_file == NULL)
  {
    fprintf(stderr, "Need to specify BDF font-file with -f\n");
    return usage(argv[0]);
  }

  signal(SIGTERM, InterruptHandler);
  signal(SIGINT, InterruptHandler);

  /*
   * Load font. This needs to be a filename with a bdf bitmap font.
   */
  rgb_matrix::Font font;
  if (!font.LoadFont(bdf_font_file))
  {
    fprintf(stderr, "Couldn't load font '%s'\n", bdf_font_file);
    return 1;
  }

  
  RGBMatrix *matrix = RGBMatrix::CreateFromOptions(matrix_options, runtime_opt);
  if (matrix == NULL)
    return 1;

  // Hardcode countdown clock position
  const int x = 9;
  const int y = 52;

  FrameCanvas *offscreen = matrix->CreateFrameCanvas();

  char text_buffer[256];
  struct timespec next_time;
  next_time.tv_sec = time(NULL);
  next_time.tv_nsec = 0;
  struct tm remaining;

  ImageVector images = LoadImageAndScaleImage(filename, matrix->width(), matrix->height());

  while (!interrupt_received)
  {
    // Fill the previous text section with background color (clear screen)
    offscreen->SubFill(x, y, 64-x, 64-y, bg_color.r, bg_color.g, bg_color.b);

    CopyImageToCanvas(images[0], matrix);
    remaining = getTimeRemaining(2026, 11, 19);
    formatTimeRemaining(remaining, text_buffer, sizeof(text_buffer));
    rgb_matrix::DrawText(offscreen, font, x, y + font.baseline(), color, NULL, text_buffer, 0);

    // Wait until we're ready to show it.
    clock_nanosleep(CLOCK_REALTIME, TIMER_ABSTIME, &next_time, NULL);

    // Atomic swap with double buffer
    offscreen = matrix->SwapOnVSync(offscreen);

    next_time.tv_sec += 1;
  }

  matrix->Clear();
  delete matrix;

  return 0;
}
