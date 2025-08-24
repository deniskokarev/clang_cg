#include "llvm/IR/Function.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Instructions.h"
#include "llvm/Analysis/CallGraph.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/IR/PassManager.h"
#include <set>
#include <map>
#include <string>

using namespace llvm;

static cl::opt<std::string> OutputFilename("yaml-callgraph-output", 
    cl::desc("Output filename for YAML callgraph"), 
    cl::init("callgraph.yaml"));

namespace {

struct YAMLCallGraphPass : public PassInfoMixin<YAMLCallGraphPass> {
    PreservedAnalyses run(Module &M, ModuleAnalysisManager &AM);
    
private:
    void generateYAML(Module &M, CallGraph &CG, const std::string &filename);
    std::string getFunctionId(Function *F);
    std::string sanitizeFunctionName(const std::string &name);
};

std::string YAMLCallGraphPass::getFunctionId(Function *F) {
    if (!F) return "null";
    
    // Use function address as unique ID
    std::string id = "func_";
    raw_string_ostream OS(id);
    OS << (void*)F;
    return OS.str();
}

std::string YAMLCallGraphPass::sanitizeFunctionName(const std::string &name) {
    std::string sanitized = name;
    // Replace special YAML characters
    for (char &c : sanitized) {
        if (c == '"' || c == '\'' || c == '\\' || c == '\n' || c == '\r') {
            c = '_';
        }
    }
    return sanitized;
}

void YAMLCallGraphPass::generateYAML(Module &M, CallGraph &CG, const std::string &filename) {
    std::error_code EC;
    raw_fd_ostream File(filename, EC, sys::fs::OF_Text);
    
    if (EC) {
        errs() << "Error opening output file: " << EC.message() << "\n";
        return;
    }
    
    File << "callgraph:\n";
    File << "  functions:\n";
    
    // Collect all functions and their call relationships
    std::map<Function*, std::set<Function*>> callMap;
    
    for (auto &F : M) {
        if (F.isDeclaration()) continue;
        
        callMap[&F]; // Ensure function is in map even if it has no calls
        
        // Analyze function calls
        for (auto &BB : F) {
            for (auto &I : BB) {
                if (auto *CI = dyn_cast<CallInst>(&I)) {
                    Function *Callee = CI->getCalledFunction();
                    if (Callee && !Callee->isIntrinsic()) {
                        callMap[&F].insert(Callee);
                    }
                } else if (auto *II = dyn_cast<InvokeInst>(&I)) {
                    Function *Callee = II->getCalledFunction();
                    if (Callee && !Callee->isIntrinsic()) {
                        callMap[&F].insert(Callee);
                    }
                }
            }
        }
    }
    
    // Output YAML format
    for (const auto &entry : callMap) {
        Function *F = entry.first;
        const std::set<Function*> &callees = entry.second;
        
        File << "    - id: \"" << getFunctionId(F) << "\"\n";
        File << "      name: \"" << sanitizeFunctionName(F->getName().str()) << "\"\n";
        File << "      children:\n";
        
        if (callees.empty()) {
            File << "        []\n";
        } else {
            for (Function *callee : callees) {
                File << "        - \"" << getFunctionId(callee) << "\"\n";
            }
        }
    }
    
    File.close();
    outs() << "YAML call graph written to: " << filename << "\n";
}

PreservedAnalyses YAMLCallGraphPass::run(Module &M, ModuleAnalysisManager &AM) {
    auto &CG = AM.getResult<CallGraphAnalysis>(M);
    
    generateYAML(M, CG, OutputFilename);
    
    return PreservedAnalyses::all();
}

} // end anonymous namespace

// Plugin registration
extern "C" LLVM_ATTRIBUTE_WEAK ::llvm::PassPluginLibraryInfo
llvmGetPassPluginInfo() {
    return {
        LLVM_PLUGIN_API_VERSION, "YAMLCallGraphPass", LLVM_VERSION_STRING,
        [](PassBuilder &PB) {
            PB.registerPipelineParsingCallback(
                [](StringRef Name, ModulePassManager &MPM,
                   ArrayRef<PassBuilder::PipelineElement>) {
                    if (Name == "yaml-callgraph") {
                        MPM.addPass(YAMLCallGraphPass());
                        return true;
                    }
                    return false;
                }
            );
        }
    };
}