// RUN: %check_clang_tidy %s bugprone-integer-literals-signedness %t

void f(int);
void g(unsigned);

void assign_unsigned_literal_to_signed() {
  f(1U);
  // CHECK-MESSAGES: :[[@LINE-1]]:5: warning: unsigned integer literal assigned to signed integer [bugprone-integer-literals-signedness]
  // CHECK-FIXES: f(1);
}

void assign_signed_literal_to_unsigned() {
  g(1);
  // CHECK-MESSAGES: :[[@LINE-1]]:5: warning: signed integer literal assigned to unsigned integer [bugprone-integer-literals-signedness]
  // CHECK-FIXES: g(1U);
}

void assign_signed_literal_to_signed() {
  f(123);  // OK
}

void assign_unsigned_literal_to_unsigned() {
  g(456U);  // OK
}
