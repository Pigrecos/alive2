#pragma once

// Copyright (c) 2018-present The Alive2 Authors.
// Distributed under the MIT license that can be found in the LICENSE file.

#include "ir/value.h"
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace IR {

class Constant : public Value {
public:
  Constant(Type &type, std::string &&name) : Value(type, std::move(name)) {}
  void print(std::ostream &os) const override;
  // <value, UB>
  virtual std::pair<smt::expr, smt::expr> toSMT_cnst() const;
  StateValue toSMT(State &s) const override;
};


class ConstantBinOp final : public Constant {
public:
  enum Op { ADD, SUB, SDIV, UDIV };

private:
  Constant &lhs, &rhs;
  Op op;

public:
  ConstantBinOp(Type &type, Constant &lhs, Constant &rhs, Op op);
  std::pair<smt::expr, smt::expr> toSMT_cnst() const override;
  smt::expr getTypeConstraints() const override;
};


class ConstantFn final : public Constant {
  enum Fn { LOG2, WIDTH } fn;
  std::vector<Value*> args;

public:
  ConstantFn(Type &type, std::string_view name, std::vector<Value*> &&args);
  std::pair<smt::expr, smt::expr> toSMT_cnst() const override;
};

struct ConstantFnException {
  std::string str;
  ConstantFnException(std::string &&str) : str(std::move(str)) {}
};


class Predicate {
public:
  virtual void print(std::ostream &os) const = 0;
  virtual smt::expr toSMT() const = 0;
};


class BoolPred final : public Predicate {
public:
  enum Pred { AND, OR };

private:
  Predicate &lhs, &rhs;
  Pred pred;

public:
  BoolPred(Predicate &lhs, Predicate &rhs, Pred pred)
    : lhs(lhs), rhs(rhs), pred(pred) {}
  void print(std::ostream &os) const override;
  smt::expr toSMT() const override;
};


class CmpPred final : public Predicate {
public:
  enum Pred { EQ, NE, SLE, SLT, SGE, SGT, ULE, ULT, UGE, UGT };

private:
  Constant &lhs, &rhs;
  Pred pred;

public:
  CmpPred(Constant &lhs, Constant &rhs, Pred pred)
    : lhs(lhs), rhs(rhs), pred(pred) {}
  void print(std::ostream &os) const override;
  smt::expr toSMT() const override;
};

}
