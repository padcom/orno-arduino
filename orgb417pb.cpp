// -----------------------------------------------------------------------------
// ORNO library
//
// Author: Matthias Hryniszak
// -----------------------------------------------------------------------------

#include "Arduino.h"
#include "orgb417pb.h"

#define STATE_WAIT_PREAMBLE       0
#define STATE_READ_WIRE_BIT_HIGH  1
#define STATE_READ_WIRE_BIT_LOW   2
#define STATE_WAIT                3

#define PULSE  400
#define SIGMA  (PULSE/2)
#define BITS   24

static unsigned int   ORGB417PB::state = STATE_WAIT_PREAMBLE;
static unsigned long  ORGB417PB::prev_ts = 0;
static unsigned int   ORGB417PB::nibble = 0;
static unsigned int   ORGB417PB::bits = 0;
static unsigned long  ORGB417PB::value = 0;

ORGB417PB::ORGB417PB(int interrupt, int transmitter) {
  state   = 0;
  prev_ts = 0;
  nibble  = 0;
  bits    = 0;
  value   = 0;
  tx      = transmitter;
  pinMode(tx, OUTPUT);
  attachInterrupt(interrupt, isr, CHANGE);
}

bool ORGB417PB::has_code() {
  return ORGB417PB::state == STATE_WAIT;
}

void ORGB417PB::clear_code() {
  ORGB417PB::value = 0;
  ORGB417PB::state = STATE_WAIT_PREAMBLE;
}

long ORGB417PB::code() {
  return ORGB417PB::value;
}

void ORGB417PB::send(unsigned long code, int repeat) {
  for (int j = 0; j < repeat; j++) {
    digitalWrite(tx, HIGH);
    delayMicroseconds(400);
    digitalWrite(tx, LOW);
    delayMicroseconds(2400);
    for (int i = 23; i >= 0; i--) {
      int low, high;
      if ((code & (1l<<i)) == 0) {
        low = PULSE * 1;
        high = PULSE * 3;
      } else {
        low = PULSE * 3;
        high = PULSE * 1;
      }
      digitalWrite(tx, HIGH);
      delayMicroseconds(low);
      digitalWrite(tx, LOW);
      delayMicroseconds(high);
    }
  }
}

void ORGB417PB::isr() {
  long curr = micros();
  long len = curr - ORGB417PB::prev_ts;
  ORGB417PB::prev_ts = curr;

  switch (ORGB417PB::state) {
    case STATE_WAIT_PREAMBLE:
      if (abs(len - (PULSE * 6)) < SIGMA) {
        ORGB417PB::value = 0;
        ORGB417PB::bits  = 0;
        ORGB417PB::state = STATE_READ_WIRE_BIT_HIGH;
      }
      break;
    case STATE_READ_WIRE_BIT_HIGH:
      // read higher wire bit
      if (abs(len - (PULSE * 3)) < SIGMA) {
        ORGB417PB::nibble = 30;
        ORGB417PB::state = STATE_READ_WIRE_BIT_LOW;
      } else if (abs(len - (PULSE * 1)) < SIGMA) {
        ORGB417PB::nibble = 10;
        ORGB417PB::state = STATE_READ_WIRE_BIT_LOW;
      } else {
        ORGB417PB::state = STATE_WAIT_PREAMBLE;
      }
      break;
    case STATE_READ_WIRE_BIT_LOW:
      // read lower wire bit
      if (abs(len - (PULSE * 3)) < SIGMA) {
        ORGB417PB::nibble += 3;
        ORGB417PB::state = STATE_READ_WIRE_BIT_HIGH;
      } else if (abs(len - (PULSE * 1)) < SIGMA) {
        ORGB417PB::nibble += 1;
        ORGB417PB::state = STATE_READ_WIRE_BIT_HIGH;
      } else {
        ORGB417PB::state = STATE_WAIT_PREAMBLE;
      }

      // decode wire bits to data bits
      if (ORGB417PB::state == STATE_READ_WIRE_BIT_HIGH) {
        if (ORGB417PB::nibble == 13) {
          ORGB417PB::value <<= 1; ORGB417PB::bits++;
        } else if (ORGB417PB::nibble == 31) {
          ORGB417PB::value <<= 1; ORGB417PB::value |= 1; ORGB417PB::bits++;
        } else {
          ORGB417PB::state = STATE_WAIT_PREAMBLE;
        }
      }

      // entire value has been read
      if (ORGB417PB::state == STATE_READ_WIRE_BIT_HIGH && ORGB417PB::bits == BITS) {
        ORGB417PB::state = STATE_WAIT;
      }
      break;
    case STATE_WAIT:
      break;
    default:
      ORGB417PB::state = STATE_WAIT_PREAMBLE;
      break;
  }
}

