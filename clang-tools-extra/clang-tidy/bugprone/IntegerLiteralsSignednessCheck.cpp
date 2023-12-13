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
      implicitCastExpr(hasDescendant(
          integerLiteral(hasType(isSignedInteger())).bind("signed"))),
      this);
  Finder->addMatcher(
      implicitCastExpr(hasDescendant(
          integerLiteral(hasType(isUnsignedInteger())).bind("unsigned"))),
      this);
}

void IntegerLiteralsSignednessCheck::check(
    const MatchFinder::MatchResult &Result) {
  if (const auto *MatchedDecl =
          Result.Nodes.getNodeAs<IntegerLiteral>("unsigned")) {
    diag(MatchedDecl->getLocation(),
         "unsigned integer literal assigned to signed integer")
        << FixItHint::CreateRemoval(
               MatchedDecl->getLocation().getLocWithOffset(1));
  }
  if (const auto *MatchedDecl =
          Result.Nodes.getNodeAs<IntegerLiteral>("signed")) {
    diag(MatchedDecl->getLocation(),
         "signed integer literal assigned to unsigned integer")
        << FixItHint::CreateInsertion(
               MatchedDecl->getLocation().getLocWithOffset(1), "U");
  }
}

} // namespace clang::tidy::bugprone
