#include <am.h>
#include <klib.h>
#include <riscv/riscv.h>

#define VGACTL_ADDR 0xa0000100u
#define FB_ADDR     0xa1000000u
#define SYNC_ADDR   (VGACTL_ADDR + 4)

void __am_gpu_init() {
}

void __am_gpu_config(AM_GPU_CONFIG_T *cfg) {
  uint32_t config = inl(VGACTL_ADDR);
  int width = config >> 16;
  int height = config & 0xffff;

  *cfg = (AM_GPU_CONFIG_T) {
    .present = true, .has_accel = false,
    .width = width, .height = height,
    .vmemsz = width * height * (int)sizeof(uint32_t)
  };
}

void __am_gpu_fbdraw(AM_GPU_FBDRAW_T *ctl) {
  if (ctl->pixels != NULL) {
    AM_GPU_CONFIG_T cfg;
    __am_gpu_config(&cfg);

    assert(ctl->x >= 0 && ctl->y >= 0);
    assert(ctl->w >= 0 && ctl->h >= 0);
    assert(ctl->x + ctl->w <= cfg.width);
    assert(ctl->y + ctl->h <= cfg.height);

    uint32_t *fb = (uint32_t *)FB_ADDR;
    uint32_t *pixels = (uint32_t *)ctl->pixels;
    for (int y = 0; y < ctl->h; y ++) {
      memcpy(&fb[(ctl->y + y) * cfg.width + ctl->x],
             &pixels[y * ctl->w],
             ctl->w * sizeof(uint32_t));
    }
  }

  if (ctl->sync) {
    outl(SYNC_ADDR, 1);
  }
}

void __am_gpu_status(AM_GPU_STATUS_T *status) {
  status->ready = true;
}
