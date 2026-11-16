// Host-side unit tests for the Modbus register codec.
// Run with:  pio test -e native

#include <string.h>
#include <unity.h>

#include "modbus_codec.h"

// Reference value 0x11223344 laid out in every Modbus word/byte order.
static const uint32_t REF = 0x11223344UL;
static const uint16_t REGS_ABCD[2] = {0x1122, 0x3344};
static const uint16_t REGS_CDAB[2] = {0x3344, 0x1122};
static const uint16_t REGS_BADC[2] = {0x2211, 0x4433};
static const uint16_t REGS_DCBA[2] = {0x4433, 0x2211};

static tag_value_t value_u32(uint32_t v) {
  tag_value_t t;
  memset(&t, 0, sizeof(t));
  t.u32v = v;
  return t;
}

void setUp(void) {}
void tearDown(void) {}

// ---------------------------------------------------------------- 16-bit

void test_encode_uint16(void) {
  tag_value_t v;
  memset(&v, 0, sizeof(v));
  v.u16v = 0xBEEF;
  uint16_t regs[2] = {0, 0};
  encode_uint16(&v, regs);
  TEST_ASSERT_EQUAL_HEX16(0xBEEF, regs[0]);
}

void test_encode_int16_negative(void) {
  tag_value_t v;
  memset(&v, 0, sizeof(v));
  v.i16v = -2;
  uint16_t regs[2] = {0, 0};
  encode_int16(&v, regs);
  TEST_ASSERT_EQUAL_HEX16(0xFFFE, regs[0]);
}

// ---------------------------------------------------------------- 32-bit decode

void test_decode_uint32_abcd(void) {
  tag_value_t v;
  decode_uint32(REGS_ABCD, WORD_ABCD, &v);
  TEST_ASSERT_EQUAL_HEX32(REF, v.u32v);
}

void test_decode_uint32_cdab(void) {
  tag_value_t v;
  decode_uint32(REGS_CDAB, WORD_CDAB, &v);
  TEST_ASSERT_EQUAL_HEX32(REF, v.u32v);
}

void test_decode_int32_negative_abcd(void) {
  const uint16_t regs[2] = {0xFFFF, 0xFFFE};
  tag_value_t v;
  decode_int32(regs, WORD_ABCD, &v);
  TEST_ASSERT_EQUAL_INT32(-2, v.i32v);
}

void test_decode_int32_negative_cdab(void) {
  const uint16_t regs[2] = {0xFFFE, 0xFFFF};
  tag_value_t v;
  decode_int32(regs, WORD_CDAB, &v);
  TEST_ASSERT_EQUAL_INT32(-2, v.i32v);
}

// ---------------------------------------------------------------- 32-bit encode

void test_encode_uint32_abcd(void) {
  tag_value_t v = value_u32(REF);
  uint16_t regs[2] = {0, 0};
  encode_uint32(&v, WORD_ABCD, regs);
  TEST_ASSERT_EQUAL_HEX16_ARRAY(REGS_ABCD, regs, 2);
}

void test_encode_uint32_cdab(void) {
  tag_value_t v = value_u32(REF);
  uint16_t regs[2] = {0, 0};
  encode_uint32(&v, WORD_CDAB, regs);
  TEST_ASSERT_EQUAL_HEX16_ARRAY(REGS_CDAB, regs, 2);
}

void test_encode_int32_round_trip(void) {
  const word_order_t orders[] = {WORD_ABCD, WORD_CDAB};
  for (unsigned i = 0; i < sizeof(orders) / sizeof(orders[0]); i++) {
    tag_value_t in;
    memset(&in, 0, sizeof(in));
    in.i32v = -123456789;
    uint16_t regs[2] = {0, 0};
    tag_value_t out;
    encode_int32(&in, orders[i], regs);
    decode_int32(regs, orders[i], &out);
    TEST_ASSERT_EQUAL_INT32(in.i32v, out.i32v);
  }
}

void test_encode_float32_abcd(void) {
  tag_value_t v;
  memset(&v, 0, sizeof(v));
  v.f32v = 1.5f;  // IEEE-754: 0x3FC00000
  uint16_t regs[2] = {0, 0};
  encode_float32(&v, WORD_ABCD, regs);
  TEST_ASSERT_EQUAL_HEX16(0x3FC0, regs[0]);
  TEST_ASSERT_EQUAL_HEX16(0x0000, regs[1]);
}

void test_encode_float32_cdab(void) {
  tag_value_t v;
  memset(&v, 0, sizeof(v));
  v.f32v = 1.5f;
  uint16_t regs[2] = {0, 0};
  encode_float32(&v, WORD_CDAB, regs);
  TEST_ASSERT_EQUAL_HEX16(0x0000, regs[0]);
  TEST_ASSERT_EQUAL_HEX16(0x3FC0, regs[1]);
}

// ---------------------------------------------------------------- known gaps
// BADC / DCBA are accepted by the config parser but the codec silently treats
// them as ABCD. These tests describe the correct behaviour and are ignored
// until the codec is fixed (roadmap Phase 1). Remove the TEST_IGNORE line to
// enable each one.

void test_decode_uint32_badc(void) {
  TEST_IGNORE_MESSAGE("Phase 1: WORD_BADC not implemented in codec");
  tag_value_t v;
  decode_uint32(REGS_BADC, WORD_BADC, &v);
  TEST_ASSERT_EQUAL_HEX32(REF, v.u32v);
}

void test_decode_uint32_dcba(void) {
  TEST_IGNORE_MESSAGE("Phase 1: WORD_DCBA not implemented in codec");
  tag_value_t v;
  decode_uint32(REGS_DCBA, WORD_DCBA, &v);
  TEST_ASSERT_EQUAL_HEX32(REF, v.u32v);
}

int main(int, char**) {
  UNITY_BEGIN();
  RUN_TEST(test_encode_uint16);
  RUN_TEST(test_encode_int16_negative);
  RUN_TEST(test_decode_uint32_abcd);
  RUN_TEST(test_decode_uint32_cdab);
  RUN_TEST(test_decode_int32_negative_abcd);
  RUN_TEST(test_decode_int32_negative_cdab);
  RUN_TEST(test_encode_uint32_abcd);
  RUN_TEST(test_encode_uint32_cdab);
  RUN_TEST(test_encode_int32_round_trip);
  RUN_TEST(test_encode_float32_abcd);
  RUN_TEST(test_encode_float32_cdab);
  RUN_TEST(test_decode_uint32_badc);
  RUN_TEST(test_decode_uint32_dcba);
  return UNITY_END();
}
