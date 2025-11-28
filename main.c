#include <stdio.h>
#include <string.h>
#include "expressao.h"

int main() {

    char entrada[512];

    printf("=====================================\n");
    printf("  Avaliador de Expressoes POSFIXA\n");
    printf("=====================================\n");
    printf("Digite uma expressao pos-fixa.\n");
    printf("Use 'sair' para encerrar.\n\n");

    while (1) {
        printf("POS-FIXA > ");
        fgets(entrada, sizeof(entrada), stdin);

        entrada[strcspn(entrada, "\n")] = '\0';

        if (strcmp(entrada, "sair") == 0)
            break;

        if (strlen(entrada) == 0)
            continue;

        char *infixa = getFormaInFixa(entrada);
        float valor = getValorPosFixa(entrada);

        printf("\n");
        if (infixa != NULL)
            printf("Infixa: %s\n", infixa);
        else
            printf("Infixa: ERRO na conversao\n");

        printf("Valor: %.6f\n", valor);
        printf("-------------------------------------\n\n");
    }

    printf("Programa encerrado.\n");
    return 0;
}
