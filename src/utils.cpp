#include "utils.h"

void parse_next(String *vla_input_ptr, char token, int *from_index, float *ret) {
  int id = vla_input_ptr->indexOf(token, *from_index);
  *ret = vla_input_ptr->substring(*from_index, id).toFloat();
  *from_index = id + 1;
}

VLAInput parse_input(String vla_input_str) {
  int id = 0;
  const char tok = ',';
  VLAInput ret;

  vla_input_str.trim();
  try {
    for (int i=0; i<8; i++) {
      parse_next(&vla_input_str, tok, &id, (float *)&ret + i);
    }
  }
  catch(std::exception e) {
    Serial.print(e.what());
    ret.err = 1;
  }

  return ret;
}
