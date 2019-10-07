inicio

    inteiro: i;

    funcao fatorial(a):inteiro;
        inteiro: a;
    inicio
        se a <= 1
            entao
                inicio
                    fatorial <- 1;
                fim
            senao
                inicio
                    fatorial <- a * fatorial(a-1);
                fim
        fimse;
    fim

    i <- fatorial(3);
    imprima("Fatorial: 3! = ",i,"\n");

    imprima("Fatorial: 5! = ", fatorial(5),"\n");

    imprima("Fatorial: (3!)! = ", fatorial(fatorial(3)),"\n");

    imprima("\nFim do programa!");

fim
