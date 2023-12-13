// RUN: %check_clang_tidy %s bugprone-integer-literals-signedness %t

void f(int);
void g(unsigned);

void pass_unsigned_literal_to_signed() {
  f(1U);
  // CHECK-MESSAGES: :[[@LINE-1]]:5: warning: unsigned integer literal assigned to signed integer [bugprone-integer-literals-signedness]
  // CHECK-FIXES: f(1);
}

void pass_signed_literal_to_unsigned() {
  g(2);
  // CHECK-MESSAGES: :[[@LINE-1]]:5: warning: signed integer literal assigned to unsigned integer [bugprone-integer-literals-signedness]
  // CHECK-FIXES: g(2U);
}

void pass_signed_literal_to_signed() {
  f(3);  // OK
}

void pass_unsigned_literal_to_unsigned() {
  g(4U);  // OK
}

struct SignedMember {
  int a = {};
};

struct UnsignedMember {
  unsigned a = {};
};

void assign_unsigned_literal_to_signed_member() {
  auto s = SignedMember{};
  s.a = 6U;
  // CHECK-MESSAGES: :[[@LINE-1]]:9: warning: unsigned integer literal assigned to signed integer [bugprone-integer-literals-signedness]
  // CHECK-FIXES: s.a = 6;
}

void assign_signed_literal_to_unsigned_member() {
  auto u = UnsignedMember{};
  u.a = 5;
  // CHECK-MESSAGES: :[[@LINE-1]]:9: warning: signed integer literal assigned to unsigned integer [bugprone-integer-literals-signedness]
  // CHECK-FIXES: u.a = 5U;
}

void assign_signed_literal_to_signed_member() {
  auto s = SignedMember{};
  s.a = 7;  // OK
}

void assign_unsigned_literal_to_unsigned_member() {
  auto u = UnsignedMember{};
  u.a = 8U;  // OK
}