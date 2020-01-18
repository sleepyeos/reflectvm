/* anewkirk */

#include "reflect.h"
#include <stdlib.h>

int main(int argc, char *argv[]) {
  RVM *r = new_rvm();
  load_code(r, argv[1]);
  run(r);

  free(r);
}
