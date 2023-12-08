// RUN: %check_clang_tidy %s bugprone-integer-literals-signedness %t

void f(int);
void assign_unsigned_literal_to_signed() {
  f(1U);
  // CHECK-MESSAGES: :[[@LINE-1]]:6: warning: unsigned integer literal assigned to signed integer [bugprone-integer-literals-signedness]
}

void assign_signed_literal_to_signed() {
  f(1);  // OK
}
