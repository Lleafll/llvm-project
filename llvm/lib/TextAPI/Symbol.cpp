//===- Symbol.cpp ---------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// Implements the Symbol.
//
//===----------------------------------------------------------------------===//

#include "llvm/TextAPI/Symbol.h"
#include <string>

namespace llvm {
namespace MachO {

#if !defined(NDEBUG) || defined(LLVM_ENABLE_DUMP)
LLVM_DUMP_METHOD void Symbol::dump(raw_ostream &OS) const {
  std::string Result;
  if (isUndefined())
    Result += "(undef) ";
  if (isWeakDefined())
    Result += "(weak-def) ";
  if (isWeakReferenced())
    Result += "(weak-ref) ";
  if (isThreadLocalValue())
    Result += "(tlv) ";
  switch (Kind) {
  case SymbolKind::GlobalSymbol:
    Result += Name.str();
    break;
  case SymbolKind::ObjectiveCClass:
    Result += "(ObjC Class) " + Name.str();
    break;
  case SymbolKind::ObjectiveCClassEHType:
    Result += "(ObjC Class EH) " + Name.str();
    break;
  case SymbolKind::ObjectiveCInstanceVariable:
    Result += "(ObjC IVar) " + Name.str();
    break;
  }
  OS << Result;
}
#endif

Symbol::const_filtered_target_range
Symbol::targets(ArchitectureSet Architectures) const {
  std::function<bool(const Target &)> FN =
      [Architectures](const Target &Target) {
        return Architectures.has(Target.Arch);
      };
  return make_filter_range(Targets, FN);
}

bool Symbol::operator==(const Symbol &O) const {
  // Older Tapi files do not express all these symbol flags. In those
  // cases, ignore those differences.
  auto RemoveFlag = [](const Symbol &Sym, SymbolFlags &Flag) {
    if (Sym.isData())
      Flag &= ~SymbolFlags::Data;
    if (Sym.isText())
      Flag &= ~SymbolFlags::Text;
  };
  SymbolFlags LHSFlags = Flags;
  SymbolFlags RHSFlags = O.Flags;
  // Ignore Text and Data for now.
  RemoveFlag(*this, LHSFlags);
  RemoveFlag(O, RHSFlags);
  return std::tie(Name, Kind, Targets, LHSFlags) ==
         std::tie(O.Name, O.Kind, O.Targets, RHSFlags);
}

SimpleSymbol parseSymbol(StringRef SymName, const SymbolFlags Flags) {
  if (SymName.startswith(ObjC1ClassNamePrefix))
    return {SymName.drop_front(ObjC1ClassNamePrefix.size()),
            SymbolKind::ObjectiveCClass};
  if (SymName.startswith(ObjC2ClassNamePrefix))
    return {SymName.drop_front(ObjC2ClassNamePrefix.size()),
            SymbolKind::ObjectiveCClass};
  if (SymName.startswith(ObjC2MetaClassNamePrefix))
    return {SymName.drop_front(ObjC2MetaClassNamePrefix.size()),
            SymbolKind::ObjectiveCClass};
  if (SymName.startswith(ObjC2EHTypePrefix)) {
    // When classes without ehtype are used in try/catch blocks
    // a weak-defined symbol is exported. In those cases, treat these as a
    // global instead.
    if ((Flags & SymbolFlags::WeakDefined) == SymbolFlags::WeakDefined)
      return {SymName, SymbolKind::GlobalSymbol};
    return {SymName.drop_front(ObjC2EHTypePrefix.size()),
            SymbolKind::ObjectiveCClassEHType};
  }

  if (SymName.startswith(ObjC2IVarPrefix))
    return {SymName.drop_front(ObjC2IVarPrefix.size()),
            SymbolKind::ObjectiveCInstanceVariable};
  return {SymName, SymbolKind::GlobalSymbol};
}

} // end namespace MachO.
} // end namespace llvm.
