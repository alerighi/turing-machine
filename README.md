# Turing Machine
Turing machine simulator written in C++

### Install
Compile the program with `make` on UNIX systems. Can also be compiled on windows manually with `cl`.
The program includes a ncurses GUI, so make sure you have the appropriate header files (`libncurses-dev` on Ubuntu)

### Usage
If you run the program with no arguments, it will start in command line mode. If you run it with the parameter `-gui` it will start in ncurses mode. Make sure to have at least a terminal that is 120x35 for the best experience. 

In command mode, you can enter the following commands (with the alias indicated between brackets):
- `load (<) [path]` : load program from file
- `save (>) [path]` : save the current program to file 
- `run (r)` : execute the machine till it goes to a halt state
- `step (s) [nsteps]` : execute `nsteps` computations steps. Default 1. 
- `memorysize [nbytes]` : set the size of the tape to `nbytes`
- `initialsymbol [symbol]` : set the initla symbol for the tape
- `set_tape [start] [string]` : put `string` on the tape starting from `start`
- `set_state [state]` : set the state to `state`
- `move_head [pos]` : move the head to pos 
- `add (+) [from] [read] [to] [write] [dir]` : add a new instruction. From `from` if you read `read` go to `to`, write `write` and move the head to `dir`. `dir` is `<` for left and `>` for right.
- `del (-) [n]` : deletes the instruction number `n``
- `print_program (pp)` : print the program
- `print_state (ps)` : print the machine state 
- `print_state_full (psf)` : print the state showing the whole tape
- `clear (C)` : clears the program
- `reset (R)` : reset the machine
- `echo [string]` : prints `string`
- `quit (q)` : quit
- `help (?)` : show help message

In GUI mode, you can:
- move tape with left/right arrow keys
- move head with `<` and `>`
- scroll program with up/down arrow keys
- run with `r`
- step with `s`
- enter command mode `:`
- reset machine with `R``
- `\` change tape symbol 

