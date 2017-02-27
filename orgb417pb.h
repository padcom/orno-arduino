// -----------------------------------------------------------------------------
// ORNO library
//
// Author: Matthias Hryniszak
// -----------------------------------------------------------------------------

#ifndef _ORGB417PB_H
#define _ORGB417PB_H

#include "Arduino.h"

class ORGB417PB {
  public:
    ORGB417PB(int interrupt);
    long code();
    bool has_code();
    void clear_code();
  private:
    static unsigned int  state;
    static unsigned long prev_ts;
    static unsigned int  nibble;
    static unsigned int  bits;
    static unsigned long value;
    static void isr();
};

#endif /* _ORGB417PB_H */
