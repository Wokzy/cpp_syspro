Here is an simple Very Reduced Instruction Set Computer (vrisc) architecture.
It has only four integer registers `R0`, `R1`, `R2`, `R3`. And also has `1024` RAM memory cells.

The instruction set of vrisc assembly is:

| Instruction  | Semantics      |
| -----------  | ---------      |
| `Mov Ri Rj`  | `Ri = Rj`      |
| `Mov Ri x `  | `Ri = x`       |
| `Add Ri Rj`  | `Ri += Rj`     |
| `Add Ri x`   | `Ri += x`      |
| `Sub Ri Rj`  | `Ri -= Rj`     |
| `Sub Ri x`   | `Ri -= x`      |
| `Mul Ri Rj`  | `Ri *= Rj`     |
| `Mul Ri x`   | `Ri *= x`      |
| `Div Ri Rj`  | `Ri /= Rj`     |
| `Div Ri x`   | `Ri /= x`      |
| `Load Ri x`  | `Ri = Mem[x]`  |
| `Store Ri x` | `Mem[x] = Ri`  |
| `Jmp x`      | `pc = x`       |
| `Jmpz x`     | `pc = if R0 == 0 then pc = x else pc++` |

Where `x` is an immediate.

Your task is to write a simple emulator of this architecture.
The emulator must be implemented in the OOP paradigm. 
In file `emulator.cpp` you can find the template for your solution. 
File contains the base class of all instructions `Instruction`. All other instructions should be derived from it.
At the end of the emulation emulator should return value on the `R0` register.
Feel free to modify this source.

Don't forget about tests!
