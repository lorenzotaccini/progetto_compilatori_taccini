#ifndef DRIVER_HPP
#define DRIVER_HPP
/************************* IR related modules ******************************/
#include "llvm/ADT/APFloat.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Verifier.h"
#include "llvm/IR/GlobalVariable.h"

/**************** C++ modules and generic data types ***********************/
#include <cstdio>
#include <cstdlib>
#include <map>
#include <string>
#include <vector>
#include <variant>

#include "parser.hpp"

using namespace llvm;

// Dichiarazione del prototipo yylex per Flex
// Flex va proprio a cercare YY_DECL perché
// deve espanderla (usando M4) nel punto appropriato
# define YY_DECL \
  yy::parser::symbol_type yylex (driver& drv)
// Per il parser è sufficiente una forward declaration
YY_DECL;

// Classe che organizza e gestisce il processo di compilazione
class driver
{
public:
  driver();
  std::map<std::string, AllocaInst *> NamedValues; // Tabella associativa in cui ogni 
            // chiave x è una variabile e il cui corrispondente valore è un'istruzione 
            // che alloca uno spazio di memoria della dimensione necessaria per 
            // memorizzare un variabile del tipo di x (nel nostro caso solo double)
  RootAST* root;      // A fine parsing "punta" alla radice dell'AST
  int parse (const std::string& f);
  std::string file;
  bool trace_parsing; // Abilita le tracce di debug el parser
  void scan_begin (); // Implementata nello scanner
  void scan_end ();   // Implementata nello scanner
  bool trace_scanning;// Abilita le tracce di debug nello scanner
  yy::location location; // Utillizata dallo scannar per localizzare i token
  void codegen();
};

typedef std::variant<std::string,double> lexval;
const lexval NONE = 0.0;

//
enum initType {
  ASSIGNMENT,
  BINDING,
  INIT
};

// Classe base dell'intera gerarchia di classi che rappresentano
// gli elementi del programma
class RootAST {
public:
  virtual ~RootAST() {};
  virtual lexval getLexVal() const {return NONE;};
  virtual Value *codegen(driver& drv) { return nullptr; };
};

// Classe che rappresenta la sequenza di statement
class SeqAST : public RootAST {
private:
  RootAST* first;
  RootAST* continuation;

public:
  SeqAST(RootAST* first, RootAST* continuation);
  Value *codegen(driver& drv) override;
};

/// classe che rappresenta tutti i nodi STMT
class StmtAST : public RootAST{};

/// classe che rappresenta tutte le inizializzazioni

class InitAST : public StmtAST{
  private:
    std::string Name;
  public:
    virtual std::string& getName();
    virtual initType getType();
};

/// ExprAST - Classe base per tutti i nodi espressione
class ExprAST : public StmtAST {};

/// NumberExprAST - Classe per la rappresentazione di costanti numeriche
class NumberExprAST : public ExprAST {
private:
  double Val;

public:
  NumberExprAST(double Val);
  lexval getLexVal() const override;
  Value *codegen(driver& drv) override;
};

/// VariableExprAST - Classe per la rappresentazione di riferimenti a variabili
class VariableExprAST : public ExprAST {
private:
  std::string Name;
  ExprAST* Expr;
  bool Isarray;
  
public:
  VariableExprAST(const std::string &Name, ExprAST* Expr=nullptr, bool Isarray=false);
  lexval getLexVal() const override;
  Value *codegen(driver& drv) override;
};

/// BinaryExprAST - Classe per la rappresentazione di operatori binari
class BinaryExprAST : public ExprAST {
private:
  char Op;
  ExprAST* LHS;
  ExprAST* RHS;

public:
  BinaryExprAST(char Op, ExprAST* LHS, ExprAST* RHS);
  Value *codegen(driver& drv) override;
};

/// CallExprAST - Classe per la rappresentazione di chiamate di funzione
class CallExprAST : public ExprAST {
private:
  std::string Callee;
  std::vector<ExprAST*> Args;  // ASTs per la valutazione degli argomenti

public:
  CallExprAST(std::string Callee, std::vector<ExprAST*> Args);
  lexval getLexVal() const override;
  Value *codegen(driver& drv) override;
};


/// IfExprAST
class IfExprAST : public ExprAST {
  private:
    ExprAST* cond;
    ExprAST* trueexp; 
    ExprAST* falseexp; 

  public:
    IfExprAST(ExprAST* cond, ExprAST* trueexp, ExprAST* falseexp);
    Value *codegen(driver& drv) override;
};

/// BlockAST

class BlockAST : public ExprAST {
  private:
    std::vector<InitAST*> Def;
    std::vector<StmtAST*> Stmts;
  public:
  BlockAST(std::vector<InitAST*> Def,std::vector<StmtAST*> Stmts);
  BlockAST(std::vector<StmtAST*> Stmts);
  Value *codegen(driver& drv) override;
};



/// VarBindingsAST

class VarBindingsAST : public InitAST{
  private:
    std::string Name;
    ExprAST* Val;
  public:
    VarBindingsAST(std::string Name, ExprAST* Val);
    AllocaInst* codegen(driver& drv) override;
    initType getType() override;
    std::string& getName() override;
};

/// ArrayBindingAST

class ArrayBindingAST : public InitAST{
  private:
    std::string Name;
    double Size;
    std::vector<ExprAST*> Values;
  public:
    ArrayBindingAST(std::string Name, double Size, std::vector<ExprAST*> Values=std::vector<ExprAST*>());
    AllocaInst* codegen(driver& drv) override;
    initType getType() override;
    std::string& getName() override;
};

/// AssigmentExprAST 

class AssignmentExprAST : public InitAST{
  private:
    std::string Name;
    ExprAST* Val;
    ExprAST* index;
    bool Isarray;
  public:
    AssignmentExprAST(std::string Name, ExprAST* Val, ExprAST* index=nullptr,bool Isarray=false);
    Value* codegen(driver& drv) override;
    initType getType() override;
    std::string& getName() override;
};

/// variabile globale

class GlobalVariableAST: public RootAST{
  private:
    std::string Name;
    double Size;
    bool Isarray;
  public:
    GlobalVariableAST(std::string Name,double Size=1,bool Isarray=false);
    Value* codegen(driver& drv) override;
    std::string& getName();
};

/// Classe per la rappresentazione del blocco IF

class IfStmtAST: public StmtAST{
  private:
    ExprAST* cond;
    StmtAST* trueblock; 
    StmtAST* falseblock; 
  public:
    IfStmtAST(ExprAST* cond, StmtAST* trueblock, StmtAST* falseblock);
    IfStmtAST(ExprAST* cond, StmtAST* trueblock);
    Value* codegen(driver& drv) override;
};

/// CLasse per la rappresentazione del blocco FOR

class ForStmtAST: public StmtAST{
  private:
    InitAST* init;
    ExprAST* cond;
    AssignmentExprAST* step;
    StmtAST* body;
  public:
    ForStmtAST(InitAST* init, ExprAST* cond, AssignmentExprAST* step, StmtAST* body);
    Value* codegen(driver& drv) override;
};


class PrototypeAST : public RootAST {
private:
  std::string Name;
  std::vector<std::string> Args;
  bool emitcode;

public:
  PrototypeAST(std::string Name, std::vector<std::string> Args);
  const std::vector<std::string> &getArgs() const;
  lexval getLexVal() const override;
  Function *codegen(driver& drv) override;
  void noemit();
};


/// FunctionAST - Classe che rappresenta la definizione di una funzione
class FunctionAST : public RootAST {
private:
  PrototypeAST* Proto;
  ExprAST* Body;
  bool external;
  
public:
  FunctionAST(PrototypeAST* Proto, ExprAST* Body);
  Function *codegen(driver& drv) override;
};

#endif // ! DRIVER_HH
