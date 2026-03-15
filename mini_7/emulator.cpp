#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
#include <optional>
#include <cassert>

using std::cout;
using std::stoi;

namespace Emulator {
  enum Reg {
    R0, R1, R2, R3
  };

  struct EmulatorState {
    const size_t regs_size = 4;
    const size_t mem_size  = 1024;

    int* _registers;
    int* _mem;

    EmulatorState() {
      _registers = new int[regs_size];
      _mem = new int[mem_size];
    }

    ~EmulatorState() {
      delete[] _registers;
      delete[] _mem;
    }

    size_t _pc = 0; // program counter register
  };

  class Instruction {
  public:
    virtual void eval(EmulatorState& emul) = 0;
    virtual ~Instruction() = default;
  };

  class RegisterImmediateInstruction: public Instruction {
  public:
    const size_t register_idx;
    const int immediate;

    RegisterImmediateInstruction(size_t register_idx, int immediate) : register_idx(register_idx), immediate(immediate) {}
    ~RegisterImmediateInstruction() = default;
  };

  class RegistersInstruction: public Instruction {
  public:
    const size_t register_idx1;
    const size_t register_idx2;

    RegistersInstruction(size_t register_idx1, size_t register_idx2) : register_idx1(register_idx1), register_idx2(register_idx2) {}
    ~RegistersInstruction() = default;
  };

  class BranchInstruction: public Instruction {
  public:
    const size_t immediate;

    BranchInstruction(size_t immediate) : immediate(immediate) {}
    ~BranchInstruction() = default;
  };

  class MovImm : public RegisterImmediateInstruction {
  using RegisterImmediateInstruction::RegisterImmediateInstruction;
  public:
    void eval(EmulatorState &emul) {
      emul._registers[register_idx] = immediate;
      emul._pc++;
    }
  };

  class AddImm : public RegisterImmediateInstruction {
  using RegisterImmediateInstruction::RegisterImmediateInstruction;
  public:
    void eval(EmulatorState &emul) {
      emul._registers[register_idx] += immediate;
      emul._pc++;
    }
  };

  class SubImm : public RegisterImmediateInstruction {
  using RegisterImmediateInstruction::RegisterImmediateInstruction;
  public:
    void eval(EmulatorState &emul) {
      emul._registers[register_idx] -= immediate;
      emul._pc++;
    }
  };

  class MulImm : public RegisterImmediateInstruction {
  using RegisterImmediateInstruction::RegisterImmediateInstruction;
  public:
    void eval(EmulatorState &emul) {
      emul._registers[register_idx] *= immediate;
      emul._pc++;
    }
  };

  class DivImm : public RegisterImmediateInstruction {
  using RegisterImmediateInstruction::RegisterImmediateInstruction;
  public:
    void eval(EmulatorState &emul) {
      emul._registers[register_idx] /= immediate;
      emul._pc++;
    }
  };

  class MovReg : public RegistersInstruction {
  public:
    using RegistersInstruction::RegistersInstruction;
    void eval(EmulatorState &emul) {
      emul._registers[register_idx1] = emul._registers[register_idx2];
      emul._pc++;
    }
  };

  class AddReg : public RegistersInstruction {
  public:
    using RegistersInstruction::RegistersInstruction;
    void eval(EmulatorState &emul) {
      emul._registers[register_idx1] += emul._registers[register_idx2];
      emul._pc++;
    }
  };

  class SubReg : public RegistersInstruction {
  public:
    using RegistersInstruction::RegistersInstruction;
    void eval(EmulatorState &emul) {
      emul._registers[register_idx1] -= emul._registers[register_idx2];
      emul._pc++;
    }
  };

  class MulReg : public RegistersInstruction {
  public:
    using RegistersInstruction::RegistersInstruction;
    void eval(EmulatorState &emul) {
      emul._registers[register_idx1] *= emul._registers[register_idx2];
      emul._pc++;
    }
  };

  class DivReg : public RegistersInstruction {
  public:
    using RegistersInstruction::RegistersInstruction;
    void eval(EmulatorState &emul) {
      emul._registers[register_idx1] /= emul._registers[register_idx2];
      emul._pc++;
    }
  };

  class LoadImm : public RegisterImmediateInstruction {
  using RegisterImmediateInstruction::RegisterImmediateInstruction;
  public:
    void eval(EmulatorState &emul) {
      assert(immediate >= 0);
      emul._registers[register_idx] = emul._mem[immediate];
      emul._pc++;
    }
  };

  class StoreImm : public RegisterImmediateInstruction {
  using RegisterImmediateInstruction::RegisterImmediateInstruction;
  public:
    void eval(EmulatorState &emul) {
      assert(immediate >= 0);
      emul._mem[immediate] = emul._registers[register_idx];
      emul._pc++;
    }
  };

  class JmpImm : public BranchInstruction {
  public:
    using BranchInstruction::BranchInstruction;
    void eval(EmulatorState &emul) {
      emul._pc = immediate;
    }
  };

  class JmpzImm : public BranchInstruction {
  public:
    using BranchInstruction::BranchInstruction;
    void eval(EmulatorState &emul) {
      if (emul._registers[R0] == 0) {
        emul._pc = immediate;
      } else {
        emul._pc++;
      }
    }
  };

  /* This function accepts the program written in the vrisc assembly
   * If the input program is correct, returns sequence of insturction, corresponding to the input program.
   * If the input text is incorrect, the behaviour is undefined
   */

  Instruction *parse_immediate_instruction(std::vector<std::string> &tokens) {
    size_t first = tokens[1][1] - '0';
    size_t second = stoi(tokens[2]);

    if (tokens[0] == "Mov") {
      return new MovImm(first, second);
    } else if (tokens[0] == "Add") {
      return new AddImm(first, second);
    } else if (tokens[0] == "Sub") {
      return new SubImm(first, second);
    } else if (tokens[0] == "Mul") {
      return new MulImm(first, second);
    } else if (tokens[0] == "Div") {
      return new DivImm(first, second);
    } else if (tokens[0] == "Load") {
      return new LoadImm(first, second);
    } else if (tokens[0] == "Store") {
      return new StoreImm(first, second);
    }

    assert(false);
  }

  Instruction *parse_registers_instruction(std::vector<std::string> &tokens) {

    size_t first = tokens[1][1] - '0';
    size_t second = tokens[2][1] - '0';

    if (tokens[0] == "Mov") {
      return new MovReg(first, second);
    } else if (tokens[0] == "Add") {
      return new AddReg(first, second);
    } else if (tokens[0] == "Sub") {
      return new SubReg(first, second);
    } else if (tokens[0] == "Mul") {
      return new MulReg(first, second);
    } else if (tokens[0] == "Div") {
      return new DivReg(first, second);
    }

    assert(false);
  }

  std::vector<Instruction*> parse(const std::string& program) {
    std::vector<Instruction*> res;

    std::vector<std::string> tokens(3, "");
    size_t token_idx = 0;
    for (size_t i = 0; i < program.size(); i++) {
      if (program[i] == '\n') {
        if (tokens[0] == "") continue;
        if (tokens[2] != "") {
          if (tokens[2][0] == 'R')
            res.push_back(parse_registers_instruction(tokens));
          else
            res.push_back(parse_immediate_instruction(tokens));
        } else {
          if (tokens[0] == "Jmp") {
            res.push_back(new JmpImm(std::stoi(tokens[1])));
          } else {
            res.push_back(new JmpzImm(std::stoi(tokens[1])));
          }
        }

        tokens[0] = "";
        tokens[1] = "";
        tokens[2] = "";
        token_idx = 0;
        continue;
      }

      if (program[i] == ' ') {
        if (tokens[token_idx] == "") continue;
        if (token_idx == 2) continue;
        token_idx++;
        continue;
      }

      tokens[token_idx] += program[i];
    }

    return res;
  }
  
  
  /* Emulate receive a program, written in the vrisc assembly,
   * in case of the correct program, emulate returns R0 value at the end of the emulation.
   * If the program is incorrect, that is, either its text is not vrisc assembly language or it contains UB(endless cycles),
   * the behaviour of emulate if also undefined. Handle these cases in any way.
   */
  int emulate(const std::string& program_text) {
    // Feel free to change code of this function
    std::vector<Instruction*> program = parse(program_text);

    EmulatorState state;

    while (state._pc < program.size()) {
      program[state._pc]->eval(state);
    }

    for (size_t i = 0; i < program.size(); i++) {
      delete program[i];
    }

    return state._registers[R0];
  }
}

// Simple helper for file as single line. Feel free to change it or delete it completely
std::optional<std::string> readStringFromFile(const std::string& filename) {
  std::ifstream file{filename};

  if (!file) return {};

  std::stringstream buf;
  buf << file.rdbuf();

  return buf.str();
}

int main() {
  // For writing test you can write programs directly in raw string literals
  std::string factorial = R"(
    Mov R0 5
    Mov R1 1    
    Jmpz 6  
                  
    Mul R1 R0   
    Sub R0 1    
    Jmp 2       

    Mov R0 R1
    Div R0 10
  )";

  // The result should be 12
  int fact5 = Emulator::emulate(factorial);

  cout << fact5 << '\n';

  
  // Or you can use file IO
  std::string filename = "factorial.vrisc";
  std::optional<std::string> program = readStringFromFile("factorial.vrisc");

  if (!program) {
    std::cerr << "Can't open file" << std::endl;
    return 1;
  }

  // And this should be 120
  int another_fact = Emulator::emulate(*program);
  cout << another_fact << '\n';

  return 0;
}
