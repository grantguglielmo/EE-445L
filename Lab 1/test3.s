		EXPORT Test4
        AREA    DATA, ALIGN=2
T       SPACE   4
N       SPACE   4
        AREA    |.text|, CODE, READONLY, ALIGN=2
        THUMB
Test4 PUSH {R4,R5,R6,LR}
      MOV R0,#0
      LDR R1,=N   ;pointer to N
      LDR R2,=T   ;pointer to T
      MOV R3,#125   
      MOV R4,#64
      MOV R5,#1000   
loop4 STR R0,[R1]          ; N is volatile
      MUL R6,R0,R3         ; N*125
      ADD R6,R6,R4         ; N*125+64
      LSR R6,R6,#7         ; (N*125+64)/128
      ADD R6,R6,R5         ; 1000+(N*125+64)/128
      STR R6,[R2]          ; T = 1000+(N*125+64)/128
      ADD R0,R0,#1
      CMP R0,#4096
      BNE loop4
      POP {R4,R5,R6,PC}
		ALIGN
	END