//===--- IntegerLiteralsSignednessCheck.cpp - clang-tidy ------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "IntegerLiteralsSignednessCheck.h"
#include "clang/AST/ASTContext.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"

using namespace clang::ast_matchers;

namespace clang::tidy::bugprone {

void IntegerLiteralsSignednessCheck::registerMatchers(MatchFinder *Finder) {
  Finder->addMatcher(
      callExpr(
          hasArgument(
              0, integerLiteral(hasType(isUnsignedInteger())).bind("signed")),
          callee(functionDecl(hasParameter(0, hasType(isSignedInteger()))))),
      this);
}

void IntegerLiteralsSignednessCheck::check(
    const MatchFinder::MatchResult &Result) {
  const auto *SignedMatchedDecl =
      Result.Nodes.getNodeAs<IntegerLiteral>("signed");
  if (SignedMatchedDecl) {
    diag(SignedMatchedDecl->getLocation(),
         "unsigned integer literal assigned to signed integer")
        << FixItHint::CreateInsertion(SignedMatchedDecl->getLocation(), "U");
    diag(SignedMatchedDecl->getLocation(), "insert 'U'",
         DiagnosticIDs::Warning);
    return;
  }
}

} // namespace clang::tidy::bugprone
