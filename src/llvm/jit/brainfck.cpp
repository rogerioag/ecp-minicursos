#include "llvm/ExecutionEngine/ExecutionEngine.h"
#include "llvm/ExecutionEngine/GenericValue.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constant.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/GlobalVariable.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Verifier.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Transforms/InstCombine/InstCombine.h"
#include "llvm/Transforms/Scalar/GVN.h"
#include "llvm/Transforms/Scalar/Reassociate.h"
#include "llvm/Transforms/Scalar/SimplifyCFG.h"

#include <cstdint>
#include <cstdio>
#include <fstream>
#include <llvm/IR/PassManager.h>
#include <memory>
#include <string>

using namespace llvm;

static cl::opt<std::string> InputFilename(cl::Positional,
                                          cl::desc("<input brainf>"));

static cl::opt<std::string> OutputFilename("o", cl::desc("Output filename"),
                                           cl::value_desc("filename"));

static cl::opt<bool> JIT("jit", cl::desc("Run program Just-In-Time"));
static cl::opt<bool> OPT("opt", cl::desc("Apply the optimization passes"));
static cl::opt<bool> PrintModule("print",
                                 cl::desc("Prints the compiled module"));

/// Names used during the IR generation
constexpr char MAIN_FUNC_LABEL[] = "main";
constexpr char BRAIN_FUNC_LABEL[] = "brain";
constexpr char MODULE_LABEL[] = "brainfck";
constexpr char TAPE_LABEL[] = "tape";
constexpr char HEAD_LABEL[] = "head";
constexpr char LOOP_HEADER_LABEL[] = "loop.header";
constexpr char LOOP_BODY_LABEL[] = "loop.body";
constexpr char LOOP_END_LABEL[] = "loop.end";
constexpr char LOOP_COND_LABEL[] = "loop.cond";

/// Value used to reference the memory
constexpr uint32_t TAPE_SIZE = 655360;
constexpr uint32_t HEAD_POS = TAPE_SIZE / 2;

class BrainFckJIT {
private:
  /// File to be compiled and executed
  std::ifstream file;

  /// Output file
  std::unique_ptr<raw_fd_ostream> output = nullptr;

  /// Defines the Analysis Managers that are necessary for the optimization
  std::unique_ptr<FunctionAnalysisManager> FAM;
  std::unique_ptr<ModuleAnalysisManager> MAM;

  /// Defines the Pass Manager that is going to be applied in the brain function
  std::unique_ptr<FunctionPassManager> FPM;

  /// LLVM structure that stores types, constants and overall metadata
  std::unique_ptr<LLVMContext> ctx;

  /// Main container for all the LLVM IR being generated. Stores information
  /// such as functions, global variables, libraries
  std::unique_ptr<Module> mod;

  /// Defines the basic API functions
  FunctionCallee getChar; /* i32 @getchar() */
  FunctionCallee putChar; /* i32 @putchar(i32) */

  /// Pointer for the tape
  Value *tapePtr;

  /// Pointer for a variable that stores the position of the head
  Value *headPositionVar;

  /// Stores the number of brackes in the code
  uint32_t numBrackets = 0;

public:
  BrainFckJIT(const char *filePath) {
    file = std::ifstream(filePath);
    if (!file.is_open()) {
      std::string err =
          "Error: The file " + std::string(filePath) + " could not be opened\n";
      throw err;
    }
  }

  BrainFckJIT(const char *filePath, const char *outputPath) {
    file = std::ifstream(filePath);
    if (!file.is_open()) {
      std::string err =
          "Error: The file " + std::string(filePath) + " could not be opened\n";
      throw err;
    }

    std::error_code error;
    output = std::make_unique<raw_fd_ostream>(outputPath, error);
    if (error.value() < 0) {
      std::string err = "Error: " + error.message() + "\n";
      throw err;
    }
  }

  void print() { mod->print(outs(), nullptr); }

  void dump() {
    if (output != nullptr) {
      mod->print(*output, nullptr);
    }
  }

  void compile() {
    if (OPT) {
      InitializePassManager();
    }

    InitializeModule();
    DeclareExternFunctions();
    AllocateTape();
    AllocateHeadPosition();
    InsertBrainFunction();
    InsertMainFunction();

    if (verifyModule(*mod, &errs())) {
      dump();
      std::string err = "Error: Compilation was not successful\n";
      throw err;
    }
  }

  void execute() {
    InitializeNativeTarget();
    InitializeNativeTargetAsmPrinter();
    Module &M = *mod;
    ExecutionEngine *ee = EngineBuilder(std::move(mod)).create();
    std::vector<GenericValue> args;
    Function *brainf_func = M.getFunction(BRAIN_FUNC_LABEL);
    GenericValue gv = ee->runFunction(brainf_func, args);
  }

private:
  /// @brief Creates the Function Pass Manager and inserts the pass passes
  void InitializePassManager() {
    MAM = std::make_unique<ModuleAnalysisManager>();
    FAM = std::make_unique<FunctionAnalysisManager>();
    FPM = std::make_unique<FunctionPassManager>();

    // Do simple "peephole" optimizations and bit-twiddling optzns.
    FPM->addPass(InstCombinePass());
    // Reassociate expressions.
    FPM->addPass(ReassociatePass());
    // Eliminate Common SubExpressions.
    FPM->addPass(GVNPass());
    // Simplify the control flow graph (deleting unreachable blocks, etc).
    FPM->addPass(SimplifyCFGPass());

    PassBuilder PB;
    PB.registerModuleAnalyses(*MAM);
    PB.registerFunctionAnalyses(*FAM);
  }

  /// @brief Initialize the structures necessary to the compilation module
  void InitializeModule() {
    ctx = std::make_unique<LLVMContext>();
    mod = std::make_unique<Module>(MODULE_LABEL, *ctx);
  }

  /// @brief Declares I/O functions used during execution
  void DeclareExternFunctions() {
    // declare i32 @getchar()
    getChar = mod->getOrInsertFunction("getchar", IntegerType::getInt8Ty(*ctx));

    // declare i32 @putchar(i32)
    putChar = mod->getOrInsertFunction("putchar", IntegerType::getInt8Ty(*ctx),
                                       IntegerType::getInt8Ty(*ctx));
  }

  /// @brief Generate the array(tape) that is going to be used as memory
  ///
  /// IR:
  ///   @memory = internal global [30000 x i8] zeroinitializer
  void AllocateTape() {
    Type *I = IntegerType::getInt8Ty(*ctx);
    ArrayType *A = ArrayType::get(I, TAPE_SIZE);
    tapePtr = new GlobalVariable(*mod, A, false, Function::ExternalLinkage,
                                 ConstantAggregateZero::get(A), TAPE_LABEL);
  }

  /// @brief Generate the variable that stores the head position
  ///
  /// IR:
  ///   @head = internal global i32 zeroinitializer
  void AllocateHeadPosition() {
    Type *I = IntegerType::getInt32Ty(*ctx);
    headPositionVar =
        new GlobalVariable(*mod, I, false, Function::ExternalLinkage,
                           ConstantAggregateZero::get(I), HEAD_LABEL);
  }

  /// @brief Creates the brain function and inserts it in the Module
  ///
  /// IR:
  ///   define void @brain() {
  ///       ...
  ///       ret void
  ///   }
  void InsertBrainFunction() {
    // define void @brain()
    FunctionType *brainDecl =
        FunctionType::get(Type::getVoidTy(mod->getContext()), {}, false);

    // Inserts the brain function in the module
    Function *brainFunc =
        Function::Create(brainDecl, Function::ExternalLinkage, "brain", *mod);

    BasicBlock *BB = InsertBrainIR(brainFunc);
    ReturnInst::Create(mod->getContext(), BB);

    if (OPT) {
      FPM->run(*brainFunc, *FAM);
    }
  }

  /// @brief Creates the main function and inserts it in the Module
  ///
  /// Creates the main function in the previously allocated Module. This main
  /// is responsible to call the compiled brainfck code.
  /// This should be the last function to be called.
  ///
  /// IR:
  ///   define i32 @main(i32 %argc, ptr %argv) {
  ///   main:
  ///       call void @brainf()
  ///       ret i32 0
  ///   }
  void InsertMainFunction() {
    //
    FunctionType *mainDecl =
        FunctionType::get(Type::getInt32Ty(mod->getContext()),
                          {Type::getInt32Ty(mod->getContext()),
                           PointerType::getUnqual(mod->getContext())},
                          false);

    // Inserts the main function in the module and rename its parameters (argc,
    // argv)
    Function *mainFunction = Function::Create(
        mainDecl, Function::ExternalLinkage, MAIN_FUNC_LABEL, *mod);
    Value *arg = mainFunction->getArg(0);
    arg->setName("argc");
    arg = mainFunction->getArg(1);
    arg->setName("argv");

    BasicBlock *BB =
        BasicBlock::Create(mod->getContext(), MAIN_FUNC_LABEL, mainFunction);
    CallInst *brainf_call =
        CallInst::Create(mod->getFunction(BRAIN_FUNC_LABEL), "", BB);
    brainf_call->setTailCall(false);
    ReturnInst::Create(mod->getContext(),
                       ConstantInt::get(mod->getContext(), APInt(32, 0)), BB);
  }

  /// @brief Initializes the values necessary to run the brain function
  ///
  /// Inserts the "init" label, initialize the head position to 0 and then
  /// inserts the rest of the Basic Blocks.
  ///
  /// IR:
  ///   define void @brain() {
  ///   init:
  ///       store i32 0, %head.pos, align 1
  ///       ...
  ///
  BasicBlock *InsertBrainIR(Function *brainFunc) {
    BasicBlock *BB = BasicBlock::Create(*ctx, "init", brainFunc);
    IRBuilder<> builder(BB);

    // Initialize head position to 0
    builder.CreateStore(builder.getInt32(HEAD_POS), headPositionVar);
    return InsertBrainIR_r(brainFunc, std::make_unique<IRBuilder<>>(BB), BB);
  }

  /// @brief Parses the BrainFck file and inserts the respective IR
  BasicBlock *InsertBrainIR_r(Function *brainfck,
                              std::unique_ptr<IRBuilder<>> builder,
                              BasicBlock *BB) {
    char input;
    while (file.get(input)) {
      switch (input) {
      case '-':
      case '+': {
        InsertIncIR(*builder, input == '+' ? 1 : -1);
        break;
      }
      case '<':
      case '>': {
        InsertPtrIncIR(*builder, input == '>' ? 1 : -1);
        break;
      }
      case ',': {
        InsertGetCharIR(*builder);
        break;
      }
      case '.': {
        InsertPutCharIR(*builder);
        break;
      }
      case '[': {
        numBrackets++;
        BB = InsertStartLoopIR(*builder, brainfck);
        builder = std::make_unique<IRBuilder<>>(BB);
        break;
      }
      case ']': {
        if (numBrackets == 0) {
          std::string err = "Error: ']' found without a pair \n";
          throw err;
        }

        numBrackets--;
        return InsertLoopExitCondIR(*builder, brainfck);
      }
      default:
        break;
      }
    }

    if (numBrackets != 0) {
      std::string err = "Error: Incorrect number of pair brackets\n";
      throw err;
    }

    return BB;
  }

  /// @brief Inserts the increment instruction (+ or -)
  ///
  /// IR:
  ///   %regX = load i32, ptr @head, align 4
  ///   %regY = getelementptr i8, ptr @memory, i32 %regX
  ///   %regZ = load i8, ptr %1, align 1
  ///   %regW = add i8 %regZ, 1
  ///   store i8 %regW, ptr %regY, align 1
  ///
  void InsertIncIR(IRBuilder<> &builder, int inc) {
    Value *pos = builder.CreateLoad(builder.getInt32Ty(), headPositionVar);
    Value *cellPtr = builder.CreateGEP(builder.getInt8Ty(), tapePtr, pos);
    Value *cell = builder.CreateLoad(builder.getInt8Ty(), cellPtr);
    Value *newValue = builder.CreateAdd(cell, builder.getInt8(inc));
    builder.CreateStore(newValue, cellPtr);
  }

  /// @brief Inserts the pointer increment instruction (> or <)
  ///
  /// IR:
  ///   %regX = load i32, ptr @head, align 4
  ///   %regY = add i32 %regX, 1
  ///   store i8 %regY, ptr @head, align 1
  ///
  void InsertPtrIncIR(IRBuilder<> &builder, int inc) {
    Value *pos = builder.CreateLoad(builder.getInt32Ty(), headPositionVar);
    Value *newValue = builder.CreateAdd(pos, builder.getInt32(inc));
    builder.CreateStore(newValue, headPositionVar);
  }

  /// @brief Inserts the call to the function 'i32 @putchar(i32)'
  ///
  /// IR:
  ///   %regX = load i32, ptr @head, align 4
  ///   %regY = getelementptr i8, ptr @memory, i32 %regX
  ///   %regZ = load i8, ptr %regX, align 1
  ///   %regW = call i8 @putchar(i8 %regZ)
  ///
  void InsertPutCharIR(IRBuilder<> &builder) {
    Value *pos = builder.CreateLoad(builder.getInt32Ty(), headPositionVar);
    Value *cellPtr = builder.CreateGEP(builder.getInt8Ty(), tapePtr, pos);
    Value *cell = builder.CreateLoad(builder.getInt8Ty(), cellPtr);
    Value *putcharParams[] = {cell};
    CallInst *putcharCall = builder.CreateCall(putChar, putcharParams);
    putcharCall->setTailCall(false);
  }

  /// @brief Inserts the call to the function 'i32 @getchar()'
  ///
  /// IR:
  ///   %regX = call i8 @getchar()
  ///   %regY = load i32, ptr @head, align 4
  ///   %regZ = getelemtnptr i8, ptr @memory, i32 %regY
  ///   store i8 %regX, ptr %regZ, align 1
  ///
  void InsertGetCharIR(IRBuilder<> &builder) {
    CallInst *getCharCall = builder.CreateCall(getChar);
    getCharCall->setTailCall(false);
    Value *read = builder.CreateTrunc(getCharCall, builder.getInt8Ty());
    Value *pos = builder.CreateLoad(builder.getInt32Ty(), headPositionVar);
    Value *cellPtr = builder.CreateGEP(builder.getInt8Ty(), tapePtr, pos);
    builder.CreateStore(read, cellPtr);
  }

  /// @brief Helper function to insert a conditional branch
  ///
  /// IR:
  ///       %regA = load i32, ptr @head, align 4
  ///       %regB = getelementptr i8, ptr @memory, i32 %regA
  ///       %regC = load i8, ptr %regB, align 1
  ///       %regD = icmp ne i8 %regC, 0
  ///       br i1 %regD, label %loop.then, label %loop.else
  ///
  void insertCondBranchIR(BasicBlock *predBlock, BasicBlock *thenBlock,
                          BasicBlock *elseBlock) {
    std::unique_ptr<IRBuilder<>> builder =
        std::make_unique<IRBuilder<>>(predBlock);

    Value *pos = builder->CreateLoad(builder->getInt32Ty(), headPositionVar);
    Value *cellPtr = builder->CreateGEP(builder->getInt8Ty(), tapePtr, pos);
    Value *cell = builder->CreateLoad(builder->getInt8Ty(), cellPtr);
    Value *cond = builder->CreateICmpNE(cell, builder->getInt8(0));
    builder->CreateCondBr(cond, thenBlock, elseBlock);
  }

  /// @brief Inserts basic blocks of the loop
  ///
  /// IR:
  ///   loop.header:                         ; preds = %label
  ///       %regX = load i32, ptr @head, align 4
  ///       %regY = getelementptr i8, ptr @memory, i32 %regX
  ///       %regZ = load i8, ptr %regY, align 1
  ///       %regW = icmp ne i8 %regZ, 0
  ///       br i1 %regW, label %loop.body, label %loop.end
  ///
  ///   loop.body:                            ; preds = %loop.cond, %loop.header
  ///     ...
  ///     br label %loop.cond
  ///
  ///   loop.end:
  ///       ...
  ///
  ///   loop.cond:                             ; preds = %loop.body
  ///       %regA = load i32, ptr @head, align 4
  ///       %regB = getelementptr i8, ptr @memory, i32 %regA
  ///       %regC = load i8, ptr %regB, align 1
  ///       %regD = icmp ne i8 %regC, 0
  ///       br i1 %regD, label %loop.body, label %loop.end
  ///
  BasicBlock *InsertStartLoopIR(IRBuilder<> &builder, Function *brainfck) {
    BasicBlock *LoopHeader =
        BasicBlock::Create(*ctx, LOOP_HEADER_LABEL, brainfck);
    BasicBlock *LoopBody = BasicBlock::Create(*ctx, LOOP_BODY_LABEL, brainfck);
    BasicBlock *LoopEnd = BasicBlock::Create(*ctx, LOOP_END_LABEL, brainfck);

    // Inconditional branch to the header
    // loop.header:
    builder.CreateBr(LoopHeader);
    builder.SetInsertPoint(LoopHeader);

    // br [loop.body] [loop.end]
    insertCondBranchIR(LoopHeader, LoopBody, LoopEnd);

    // loop.body:
    builder.SetInsertPoint(LoopHeader);
    BasicBlock *LoopCond = InsertBrainIR_r(
        brainfck, std::make_unique<IRBuilder<>>(LoopBody), LoopBody);

    // br [loop.body] [loop.end]
    insertCondBranchIR(LoopCond, LoopBody, LoopEnd);

    builder.SetInsertPoint(LoopEnd);
    return LoopEnd;
  }

  /// @brief Inserts basic block that verifies the exit condition of a loop
  ///
  /// IR:
  ///   loop.cond:                             ; preds = %loop.body
  ///       %regA = load i32, ptr @head, align 4
  ///       %regB = getelementptr i8, ptr @memory, i32 %regA
  ///       %regC = load i8, ptr %regB, align 1
  ///       %regD = icmp ne i8 %regC, 0
  ///       br i1 %regD, label %loop.body, label %loop.end
  BasicBlock *InsertLoopExitCondIR(IRBuilder<> &builder, Function *brainfck) {
    BasicBlock *LoopCond =
        BasicBlock::Create(mod->getContext(), LOOP_COND_LABEL, brainfck);
    builder.CreateBr(LoopCond);
    builder.SetInsertPoint(LoopCond);
    return LoopCond;
  }
};

int main(int argc, char **argv) {
  cl::ParseCommandLineOptions(argc, argv, " BrainF compiler\n");

  try {
    std::unique_ptr<BrainFckJIT> brainFck;

    if (InputFilename.empty()) {
      errs() << argv[0] << " <brainfck> [output]\n";
      errs() << "Error: The filename of the program to be compiled need to be "
                "specify.\nUse --help to see the options.\n";
      return -1;
    }

    std::string inputname = InputFilename;
    if (OutputFilename.empty()) {
      brainFck = std::make_unique<BrainFckJIT>(inputname.c_str());
      brainFck->compile();
    } else {
      std::string outputname = OutputFilename;
      brainFck =
          std::make_unique<BrainFckJIT>(inputname.c_str(), outputname.c_str());
      brainFck->compile();
      brainFck->dump();
    }

    if (PrintModule)
      brainFck->print();

    if (JIT) {
      brainFck->execute();
      fflush(nullptr);
    }
  } catch (std::string err) {
    errs() << err;
    return -1;
  }

  return 0;
}