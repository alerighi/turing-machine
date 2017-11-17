; implementazione in turing machine del problema busy beaver
; dimostrazione soluzione con 5 stati. 

; Available commands:
;    - quit (q) : quit program
;    - help (?) : show a help message
;    - source (<) <filename> : loads <filename>
;    - step (s) [number of steps] => execute ont (N) steps
;    - clear (C) => clears the program
;    - run => run the machine to halt (break with CTRL-C one time)
;    - add (+) <src>, <read>, <dest>, <write>, <{<,>}> => add instruction
;    - del (-) <n> => delete instruction n
;    - reset (r) => reset the machine
;    - print (pp) => print the program
;    - state (ps) => print the machine state
;    - state_full (psf) : print the machine state, all tape
;    - memsize <size> => set the size of memory
;    - initsymbol <symbol> => set the initial symbol
; 	 - echo <message> : shows a message 


echo Busy beaver program

memsize 25000 ; set machine memory size
initsymbol 0  ; fill the tape with 0s

clear ; clear the program memory (in case another program was loaded befor)

; set the transition function:
; let S, D states, R, W symbols and D '<' or '>'.
; the function meand from state S if you read R go to state D, 
; write W to the tape, and move the head left (<) or right (>)
;<S><R><D><W><D> 
+ $ 0 A 0 > 
+ A 0 B 1 >
+ A 1 C 1 <
+ B 0 C 1 >
+ B 1 B 1 >
+ C 0 D 1 >
+ C 1 E 0 <
+ D 0 A 1 <
+ D 1 D 1 < 
+ E 0 ! 1 > 
+ E 1 A 0 < 
 
print ; prints the program on the screen

reset ; reset the machine

echo Starting execution
run ; executes the program 

echo Done execution 
ps ; print result (short)

echo Done

