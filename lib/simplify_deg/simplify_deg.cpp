#include "simplify_deg.h"

int SimplifyDeg(int deg_) {
      int deg = deg_;
      if (deg > 180) {
            deg -= 360;
      }
      if (deg < -180) {
            deg += 360;
      }

      return deg;
}