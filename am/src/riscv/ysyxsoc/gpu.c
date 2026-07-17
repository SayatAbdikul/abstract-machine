#include <am.h>
#include <stdint.h>

#define FB_BASE   ((volatile uint32_t *)0x21000000u)
#define FB_WIDTH  640
#define FB_HEIGHT 480
#define FB_BYTES  0x200000

void __am_gpu_config(AM_GPU_CONFIG_T *config) {
  config->present = true;
  config->has_accel = false;
  config->width = FB_WIDTH;
  config->height = FB_HEIGHT;
  config->vmemsz = FB_BYTES;
}

void __am_gpu_status(AM_GPU_STATUS_T *status) {
  status->ready = true;
}

void __am_gpu_fbdraw(AM_GPU_FBDRAW_T *draw) {
  if (draw->pixels == NULL || draw->w <= 0 || draw->h <= 0) return;

  uint32_t *source = (uint32_t *)draw->pixels;
  int source_x = 0;
  int source_y = 0;
  int x = draw->x;
  int y = draw->y;
  int width = draw->w;
  int height = draw->h;

  if (x < 0) { source_x = -x; width += x; x = 0; }
  if (y < 0) { source_y = -y; height += y; y = 0; }
  if (x + width > FB_WIDTH) width = FB_WIDTH - x;
  if (y + height > FB_HEIGHT) height = FB_HEIGHT - y;
  if (width <= 0 || height <= 0) return;

  // Source pixels and the hardware framebuffer share the 0x00RRGGBB format.
  // The original draw width remains the source stride after clipping.
  for (int row = 0; row < height; ++row) {
    uint32_t *src = source + (source_y + row) * draw->w + source_x;
    volatile uint32_t *dst = FB_BASE + (y + row) * FB_WIDTH + x;
    for (int column = 0; column < width; ++column) dst[column] = src[column];
  }

  // `draw->sync` is intentionally a no-op: NVBoard continuously samples the
  // VGA pins and refreshes its window without a software doorbell.
}
