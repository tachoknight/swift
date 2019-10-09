//===--- ASTGen.h ---------------------------------------------------------===//
//
// This source file is part of the Swift.org open source project
//
// Copyright (c) 2014 - 2019 Apple Inc. and the Swift project authors
// Licensed under Apache License v2.0 with Runtime Library Exception
//
// See https://swift.org/LICENSE.txt for license information
// See https://swift.org/CONTRIBUTORS.txt for the list of Swift project authors
//
//===----------------------------------------------------------------------===//

#ifndef SWIFT_PARSE_ASTGEN_H
#define SWIFT_PARSE_ASTGEN_H

#include "swift/AST/ASTContext.h"
#include "swift/AST/Decl.h"
#include "swift/AST/Expr.h"
#include "swift/AST/TypeRepr.h"
#include "swift/Parse/PersistentParserState.h"
#include "swift/Syntax/SyntaxNodes.h"
#include "llvm/ADT/DenseMap.h"

namespace swift {
class ComponentIdentTypeRepr;
class TupleTypeRepr;

/// Generates AST nodes from Syntax nodes.
class Parser;
class ASTGen {
  ASTContext &Context;

  /// Type cache to prevent multiple transformations of the same syntax node.
  llvm::DenseMap<syntax::SyntaxNodeId, TypeRepr *> TypeCache;

  Parser &P;

  // FIXME: remove when Syntax can represent all types and ASTGen can handle
  // them
  /// Exprs that cannot be represented by Syntax or generated by ASTGen.
  llvm::DenseMap<SourceLoc, Expr *> Exprs;

  /// Decl attributes that cannot be represented by Syntax or generated by
  /// ASTGen.
  llvm::DenseMap<SourceLoc, DeclAttributes> ParsedDeclAttrs;

public:
  ASTGen(ASTContext &Context, Parser &P) : Context(Context), P(P) {}

  SourceLoc generate(const syntax::TokenSyntax &Tok, const SourceLoc Loc);

  SourceLoc generateIdentifierDeclName(const syntax::TokenSyntax &Tok,
                                       const SourceLoc, Identifier &Identifier);

public:
  //===--------------------------------------------------------------------===//
  // Decls.

  Decl *generate(const syntax::DeclSyntax &Decl, const SourceLoc Loc);
  TypeDecl *generate(const syntax::AssociatedtypeDeclSyntax &Decl,
                     const SourceLoc Loc);
  TypeDecl *generate(const syntax::TypealiasDeclSyntax &Decl,
                     const SourceLoc Loc);

  TrailingWhereClause *generate(const syntax::GenericWhereClauseSyntax &syntax,
                                const SourceLoc Loc);
  MutableArrayRef<TypeLoc>
  generate(const syntax::TypeInheritanceClauseSyntax &syntax,
           const SourceLoc Loc, bool allowClassRequirement);

private:
  DeclAttributes
  generateDeclAttributes(const syntax::Syntax &D, SourceLoc Loc,
                         bool includeComments);

  void generateFreeStandingGenericWhereClause(
                                const syntax::GenericWhereClauseSyntax &syntax,
                                const SourceLoc Loc,
                                GenericParamList *genericParams);

public:
  //===--------------------------------------------------------------------===//
  // Expressions.

  Expr *generate(const syntax::ExprSyntax &Expr, const SourceLoc Loc);
  Expr *generate(const syntax::IdentifierExprSyntax &Expr, const SourceLoc Loc);
  Expr *generate(const syntax::SuperRefExprSyntax &Expr, const SourceLoc Loc);
  Expr *generate(const syntax::ArrayExprSyntax &Expr, const SourceLoc Loc);
  Expr *generate(const syntax::DictionaryExprSyntax &Expr, const SourceLoc Loc);
  Expr *generate(const syntax::EditorPlaceholderExprSyntax &Expr,
                 const SourceLoc Loc);
  Expr *generate(const syntax::SpecializeExprSyntax &Expr, const SourceLoc Loc);
  Expr *generate(const syntax::IntegerLiteralExprSyntax &Expr,
                 const SourceLoc Loc);
  Expr *generate(const syntax::FloatLiteralExprSyntax &Expr,
                 const SourceLoc Loc);
  Expr *generate(const syntax::NilLiteralExprSyntax &Expr, const SourceLoc Loc);
  Expr *generate(const syntax::BooleanLiteralExprSyntax &Expr,
                 const SourceLoc Loc);
  Expr *generate(const syntax::PoundFileExprSyntax &Expr, const SourceLoc Loc);
  Expr *generate(const syntax::PoundLineExprSyntax &Expr, const SourceLoc Loc);
  Expr *generate(const syntax::PoundColumnExprSyntax &Expr,
                 const SourceLoc Loc);
  Expr *generate(const syntax::PoundFunctionExprSyntax &Expr,
                 const SourceLoc Loc);
  Expr *generate(const syntax::PoundDsohandleExprSyntax &Expr,
                 const SourceLoc Loc);
  Expr *generate(const syntax::UnknownExprSyntax &Expr, const SourceLoc Loc);

  std::pair<DeclName, DeclNameLoc> generateUnqualifiedDeclName(
      const syntax::TokenSyntax &idTok,
      const Optional<syntax::DeclNameArgumentsSyntax> &args,
      const SourceLoc Loc);

private:
  void validateCollectionElement(Expr *elementExpr);

  Expr *generateMagicIdentifierLiteralExpression(
      const syntax::TokenSyntax &PoundToken, const SourceLoc Loc);

  static MagicIdentifierLiteralExpr::Kind
  getMagicIdentifierLiteralKind(tok Kind);

public:
  //===--------------------------------------------------------------------===//
  // Types.

  TypeRepr *generate(const syntax::TypeSyntax &Type, const SourceLoc Loc,
                     bool IsSILFuncDecl = false);
  TypeRepr *generate(const syntax::SomeTypeSyntax &Type, const SourceLoc Loc);
  TypeRepr *generate(const syntax::CompositionTypeSyntax &Type,
                     const SourceLoc Loc);
  TypeRepr *generate(const syntax::SimpleTypeIdentifierSyntax &Type,
                     const SourceLoc Loc);
  TypeRepr *generate(const syntax::MemberTypeIdentifierSyntax &Type,
                     const SourceLoc Loc);
  TypeRepr *generate(const syntax::DictionaryTypeSyntax &Type,
                     const SourceLoc Loc);
  TypeRepr *generate(const syntax::ArrayTypeSyntax &Type, const SourceLoc Loc);
  TypeRepr *generate(const syntax::TupleTypeSyntax &Type, const SourceLoc Loc);
  TypeRepr *generate(const syntax::AttributedTypeSyntax &Type,
                     const SourceLoc Loc);
  TypeRepr *generate(const syntax::FunctionTypeSyntax &Type,
                     const SourceLoc Loc);
  TypeRepr *generate(const syntax::MetatypeTypeSyntax &Type,
                     const SourceLoc Loc);
  TypeRepr *generate(const syntax::OptionalTypeSyntax &Type,
                     const SourceLoc Loc);
  TypeRepr *generate(const syntax::ImplicitlyUnwrappedOptionalTypeSyntax &Type,
                     const SourceLoc Loc);
  TypeRepr *generate(const syntax::ClassRestrictionTypeSyntax &Type,
                     const SourceLoc Loc);
  TypeRepr *generate(const syntax::SILBoxTypeSyntax &Type, const SourceLoc Loc,
                     bool IsSILFuncDecl);
  TypeRepr *generate(const syntax::SILFunctionTypeSyntax &Type,
                     const SourceLoc Loc, bool IsSILFuncDecl);
  TypeRepr *generate(const syntax::CodeCompletionTypeSyntax &Type,
                     const SourceLoc Loc);
  TypeRepr *generate(const syntax::UnknownTypeSyntax &Type,
                     const SourceLoc Loc);

  TypeAttributes
  generateTypeAttributes(const syntax::AttributeListSyntax &syntax,
                         const SourceLoc Loc);

private:
  TupleTypeRepr *
  generateTuple(const syntax::TokenSyntax &LParen,
                const syntax::TupleTypeElementListSyntax &Elements,
                const syntax::TokenSyntax &RParen, const SourceLoc Loc,
                bool IsFunction = false);

  void gatherTypeIdentifierComponents(
      const syntax::TypeSyntax &Component, const SourceLoc Loc,
      llvm::SmallVectorImpl<ComponentIdentTypeRepr *> &Components);

  template <typename T>
  TypeRepr *generateSimpleOrMemberIdentifier(const T &Type,
                                             const SourceLoc Loc);

  template <typename T>
  ComponentIdentTypeRepr *generateIdentifier(const T &Type,
                                             const SourceLoc Loc);

public:
  //===--------------------------------------------------------------------===//
  // Generics.

  void generate(const syntax::GenericArgumentClauseSyntax &Arg,
                const SourceLoc Loc, SourceLoc &lAngleLoc, SourceLoc &rAngleLoc,
                SmallVectorImpl<TypeRepr *> &args);

  GenericParamList *
  generate(const syntax::GenericParameterClauseListSyntax &clause,
           const SourceLoc Loc);
  GenericParamList *generate(const syntax::GenericParameterClauseSyntax &clause,
                             const SourceLoc Loc);
  Optional<RequirementRepr>
  generate(const syntax::GenericRequirementSyntax &req, const SourceLoc Loc);
  LayoutConstraint generate(const syntax::LayoutConstraintSyntax &req,
                            const SourceLoc Loc);

public:
  //===--------------------------------------------------------------------===//
  // Utilities.

  /// Copy a numeric literal value into AST-owned memory, stripping underscores
  /// so the semantic part of the value can be parsed by APInt/APFloat parsers.
  static StringRef copyAndStripUnderscores(StringRef Orig, ASTContext &Context);

private:
  StringRef copyAndStripUnderscores(StringRef Orig);

  /// Advance \p Loc to the first token of the \p Node.
  /// \p Loc must be the leading trivia of the first token in the tree in which
  /// \p Node resides.
  static SourceLoc advanceLocBegin(const SourceLoc &Loc,
                                   const syntax::Syntax &Node);

  /// Advance \p Loc to the last non-missing token of the \p Node or, if it
  /// doesn't contain any, the last non-missing token preceding it in the tree.
  /// \p Loc must be the leading trivia of the first token in the tree in which
  /// \p Node resides
  static SourceLoc advanceLocEnd(const SourceLoc &Loc,
                                 const syntax::Syntax &Node);

  ValueDecl *lookupInScope(DeclName Name);

  void addToScope(ValueDecl *D, bool diagnoseRedefinitions = true);

  TypeRepr *cacheType(syntax::TypeSyntax Type, TypeRepr *TypeAST);

  TypeRepr *lookupType(syntax::TypeSyntax Type);

public:
  void addExpr(Expr *Expr, const SourceLoc Loc);
  bool hasExpr(const SourceLoc Loc) const;
  Expr *takeExpr(const SourceLoc Loc);

  void addDeclAttributes(DeclAttributes attrs, const SourceLoc Loc);
  bool hasDeclAttributes(SourceLoc Loc) const;
  DeclAttributes takeDeclAttributes(const SourceLoc Loc);
};
} // namespace swift

#endif // SWIFT_PARSE_ASTGEN_H
