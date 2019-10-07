
   .text
   .globl main
main:
    subu $sp, $sp, 32
    sw $ra, 20($sp)
    sw $fp, 16($sp)
    addiu $fp, $sp, 28
    la $t0, global_i 	 # NAME
    li $t1, 0
    add $t0, $t0, $t1
		 # iniciando caller
    subu $sp, $sp, 24
    li $t1, 3
    sw $t1, 4($sp)
    addiu $sp, $sp, 24
    jal subprog_fatorial
    move $t1, $v0
    sw $t1, 0($t0) 	 # MOVE MEM-Exp
    li $v0, 4 # IMPRIMA
    la $a0, LITERAL_7
    syscall
    la $t0, global_i 	 # NAME
    li $t1, 0
    add $t0, $t0, $t1
    lw $t0, 0($t0) 	 # MEM
    li $v0, 1 	 # IMPRIMA
    move $a0, $t0
    syscall
    li $v0, 4 # IMPRIMA
    la $a0, LITERAL_6
    syscall
    li $v0, 4 # IMPRIMA
    la $a0, LITERAL_5
    syscall
		 # iniciando caller
    subu $sp, $sp, 24
    li $t0, 5
    sw $t0, 4($sp)
    addiu $sp, $sp, 24
    jal subprog_fatorial
    move $t0, $v0
    li $v0, 1 	 # IMPRIMA
    move $a0, $t0
    syscall
    li $v0, 4 # IMPRIMA
    la $a0, LITERAL_4
    syscall
    li $v0, 4 # IMPRIMA
    la $a0, LITERAL_3
    syscall
		 # iniciando caller
    subu $sp, $sp, 24
		 # iniciando caller
    subu $sp, $sp, 24
    li $t0, 3
    sw $t0, 4($sp)
    addiu $sp, $sp, 24
    jal subprog_fatorial
    move $t0, $v0
    sw $t0, 4($sp)
    addiu $sp, $sp, 24
    jal subprog_fatorial
    move $t0, $v0
    li $v0, 1 	 # IMPRIMA
    move $a0, $t0
    syscall
    li $v0, 4 # IMPRIMA
    la $a0, LITERAL_2
    syscall
    li $v0, 4 # IMPRIMA
    la $a0, LITERAL_1
    syscall
    lw $ra, 20($sp)
    lw $fp, 16($sp)
    addiu $sp, $sp, 32
    jr $ra

   .text
    .globl subprog_fatorial

subprog_fatorial:
	# Callee - Prologo 

    subu $sp, $sp, 124
    sw $ra, 0($sp)
    sw $fp, 4($sp)
    addiu $fp, $sp, 100
    sw $t0, 8($sp)
    sw $t1, 12($sp)
    sw $t2, 16($sp)
    sw $t3, 20($sp)
    sw $t4, 24($sp)
    sw $t5, 28($sp)
    sw $t6, 32($sp)
    sw $t7, 36($sp)
    sw $t8, 40($sp)
    sw $t9, 44($sp)
    sw $s0, 48($sp)
    sw $s1, 52($sp)
    sw $s2, 56($sp)
    sw $s3, 60($sp)
    sw $s4, 64($sp)
    sw $s5, 68($sp)
    sw $s6, 72($sp)
    sw $s7, 76($sp)
    li $t1, 1
    move $s6, $t1
    li $t3, 4
    add $t2, $fp, $t3
    lw $t2, 0($t2) 	 # MEM
    li $t3, 1
    ble $t2, $t3, VERDADEIRO_12
    jr FALSO_13

FALSO_13:
    li $t3, 0
    move $s6, $t3

VERDADEIRO_12:
    li $t4, 0
    bne $s6, $t4, ENTAO_10
    jr SENAO_9

ENTAO_10:
    li $t4, 1
    move $v0, $t4
    jr FIMSE_11

SENAO_9:
    li $t7, 4
    add $t6, $fp, $t7
    lw $t6, 0($t6) 	 # MEM
		 # iniciando caller
    subu $sp, $sp, 24
    li $t9, 4
    add $t8, $fp, $t9
    lw $t8, 0($t8) 	 # MEM
    li $t9, 1
    sub $t8, $t8, $t9
    sw $t8, 4($sp)
    addiu $sp, $sp, 24
    jal subprog_fatorial
    move $t8, $v0
    mul $t6, $t6, $t8
    move $v0, $t6

FIMSE_11:
    lw $ra, 0($sp)
    lw $fp, 4($sp)
    lw $t0, 8($sp)
    lw $t1, 12($sp)
    lw $t2, 16($sp)
    lw $t3, 20($sp)
    lw $t4, 24($sp)
    lw $t5, 28($sp)
    lw $t6, 32($sp)
    lw $t7, 36($sp)
    lw $t8, 40($sp)
    lw $t9, 44($sp)
    lw $s0, 48($sp)
    lw $s1, 52($sp)
    lw $s2, 56($sp)
    lw $s3, 60($sp)
    lw $s4, 64($sp)
    lw $s5, 68($sp)
    lw $s6, 72($sp)
    lw $s7, 76($sp)
    addiu $sp, $sp, 124
    jr $ra
    .data
    .globl global_i
global_i:
    .word 0 
    .rdata
LITERAL_7:
    .asciiz "Fatorial: 3! = "
    .rdata
LITERAL_6:
    .asciiz "\n"
    .rdata
LITERAL_5:
    .asciiz "Fatorial: 5! = "
    .rdata
LITERAL_4:
    .asciiz "\n"
    .rdata
LITERAL_3:
    .asciiz "Fatorial: (3!)! = "
    .rdata
LITERAL_2:
    .asciiz "\n"
    .rdata
LITERAL_1:
    .asciiz "\nFim do programa!"
