; conta il numero di 1 in una stringa
; formato dell'input:
; ...$[count]I01001001100100101001$...

echo Programma count

memsize 100
initsymbol $
set_tape 20 I01001001100100101001
head_position 20

; istruzioni 

; stato q0
+ $ I q1 I > 

; stato q1 : cerco il primo uno a sinistra e lo sostituisco con un *
+ q1 0 q1 0 > ; ignoro lo 0
+ q1 1 q2 * < ; sostituisco un 1 con un *
+ q1 $ !  $ > ; ho finito!

; stato q2 : torno all'inizio 
+ q2 0 q2 0 < ; ignoro lo 0
+ q2 1 q2 1 < ; ignoro l'1 
+ q2 I q3 I < ; sono arrivato all'inizio 

; stato q3: incremento il contatore 
+ q3 0 q4 1 > ; incremento  
+ q3 1 q4 2 > ; incremento  
+ q3 2 q4 3 > ; incremento  
+ q3 3 q4 4 > ; incremento  
+ q3 4 q4 5 > ; incremento  
+ q3 5 q4 6 > ; incremento  
+ q3 6 q4 7 > ; incremento  
+ q3 7 q4 8 > ; incremento  
+ q3 8 q4 9 > ; incremento  
+ q3 9 q3 0 < ; incremento  
+ q3 $ q4 1 > ; incremento  

; q4 torno al primo *
+ q4 0 q4 0 > ; sposto
+ q4 1 q4 1 > ; sposto
+ q4 2 q4 2 > ; sposto
+ q4 3 q4 3 > ; sposto
+ q4 4 q4 4 > ; sposto
+ q4 5 q4 5 > ; sposto
+ q4 6 q4 6 > ; sposto
+ q4 7 q4 7 > ; sposto
+ q4 8 q4 8 > ; sposto
+ q4 9 q4 9 > ; sposto
+ q4 I q4 I > ; sposto 
+ q4 * q1 1 > ; cambio l'* con 1 e torno all'inizio 

echo Fine caricamento