#include "segment.h"
#include <stdlib.h>

void init_segment(struct Segment *segment, Servo *servo, float length) {
  segment->servo = servo;
  segment->length = length;
}
