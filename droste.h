#include <stdint.h>

typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;

typedef struct {
    uint8 r, g, b;
} Pixel;

extern uint32 width, height;
extern Pixel transpColor;

Pixel pixel(uint8 r, uint8 g, uint8 b);
int pixel_eq(Pixel x, Pixel y);

void transform(Pixel *obmp, Pixel *ibmp);
