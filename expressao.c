#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "expressao.h"

#define MAX_STACK 256
#define PI 3.14159265358979323846

typedef struct {
    float data[MAX_STACK];
    int top;
} FloatStack;

void fs_init(FloatStack *s) {
    s->top = -1;
}

int fs_is_empty(FloatStack *s) {
    return s->top < 0;
}

int fs_push(FloatStack *s, float v) {
    if (s->top >= MAX_STACK - 1) return 0;
    s->data[++(s->top)] = v;
    return 1;
}

int fs_pop(FloatStack *s, float *v) {
    if (s->top < 0) return 0;
    *v = s->data[(s->top)--];
    return 1;
}

typedef struct {
    char expr[512];
    int prec;
} Node;

typedef struct {
    Node data[MAX_STACK];
    int top;
} NodeStack;

void ns_init(NodeStack *s) {
    s->top = -1;
}

int ns_is_empty(NodeStack *s) {
    return s->top < 0;
}

int ns_push(NodeStack *s, Node n) {
    if (s->top >= MAX_STACK - 1) return 0;
    s->data[++(s->top)] = n;
    return 1;
}

int ns_pop(NodeStack *s, Node *n) {
    if (s->top < 0) return 0;
    *n = s->data[(s->top)--];
    return 1;
}

static void getNextToken(const char **str, char *token) {
    int i = 0;
    const char *p = *str;

    while (*p == ' ' || *p == '\t' || *p == '\n' || *p == '\r') {
        p++;
    }

    if (*p == '\0') {
        token[0] = '\0';
        *str = p;
        return;
    }

    while (*p != ' ' && *p != '\t' && *p != '\n' && *p != '\r' && *p != '\0') {
        token[i++] = *p;
        p++;
    }
    token[i] = '\0';
    *str = p;
}

static int isOperator(const char *t) {
    return (strlen(t) == 1 && (t[0] == '+' || t[0] == '-' || t[0] == '*' ||
                               t[0] == '/' || t[0] == '%' || t[0] == '^'));
}

static int isFunction(const char *t) {
    return (!strcmp(t, "raiz") || !strcmp(t, "sen") || !strcmp(t, "cos") ||
            !strcmp(t, "tg")   || !strcmp(t, "log"));
}

static int precedence(char op) {
    switch (op) {
        case '+':
        case '-':
            return 1;
        case '*':
        case '/':
        case '%':
            return 2;
        case '^':
            return 3;
        default:
            return 4;
    }
}
static float deg2rad(float deg) {
    return (float)(deg * (PI / 180.0));
}

float getValorPosFixa(char *StrPosFixa) {
    FloatStack s;
    const char *p = StrPosFixa;
    char token[64];

    fs_init(&s);

    while (1) {
        getNextToken(&p, token);
        if (token[0] == '\0') break;

        if (isOperator(token)) {
            float b, a;
            if (!fs_pop(&s, &b) || !fs_pop(&s, &a)) {
                return 0.0f;
            }
            switch (token[0]) {
                case '+': fs_push(&s, a + b); break;
                case '-': fs_push(&s, a - b); break;
                case '*': fs_push(&s, a * b); break;
                case '/': fs_push(&s, a / b); break;
                case '%': fs_push(&s, fmodf(a, b)); break;
                case '^': fs_push(&s, powf(a, b)); break;
            }
        } else if (isFunction(token)) {
            float x;
            if (!fs_pop(&s, &x)) {
                return 0.0f;
            }

            if (!strcmp(token, "raiz")) {
                fs_push(&s, sqrtf(x));
            } else if (!strcmp(token, "log")) {
                fs_push(&s, log10f(x));
            } else if (!strcmp(token, "sen")) {
                fs_push(&s, sinf(deg2rad(x)));
            } else if (!strcmp(token, "cos")) {
                fs_push(&s, cosf(deg2rad(x)));
            } else if (!strcmp(token, "tg")) {
                fs_push(&s, tanf(deg2rad(x)));
            }
        } else {
            float v = (float)atof(token);
            fs_push(&s, v);
        }
    }

    if (s.top != 0) {
        return 0.0f;
    }
    return s.data[0];
}

char * getFormaInFixa(char *Str) {
    static char resultado[512]; 
    NodeStack s;
    const char *p = Str;
    char token[64];

    ns_init(&s);
    resultado[0] = '\0';

    while (1) {
        getNextToken(&p, token);
        if (token[0] == '\0') break;

        if (isOperator(token)) {
            Node right, left, novo;
            char tempLeft[512], tempRight[512], buf[512];
            char op = token[0];
            int precOp = precedence(op);

            if (!ns_pop(&s, &right) || !ns_pop(&s, &left)) {
                return NULL;
            }

            strcpy(tempLeft, left.expr);
            if (left.prec < precOp) {
                snprintf(buf, sizeof(buf), "(%s)", tempLeft);
                strcpy(tempLeft, buf);
            }

            strcpy(tempRight, right.expr);
            {
                int needParensRight = 0;
                if (right.prec < precOp) {
                    needParensRight = 1;
                } else {
                    if ((op == '-' || op == '/') && right.prec == precOp) {
                        needParensRight = 1;
                    }
                    if (op == '^' && right.prec == precOp) {
                        needParensRight = 1;
                    }
                }
                if (needParensRight) {
                    snprintf(buf, sizeof(buf), "(%s)", tempRight);
                    strcpy(tempRight, buf);
                }
            }

            snprintf(novo.expr, sizeof(novo.expr), "%s%c%s", tempLeft, op, tempRight);
            novo.prec = precOp;

            if (!ns_push(&s, novo)) {
                return NULL;
            }

        } else if (isFunction(token)) {
            Node arg, novo;
            if (!ns_pop(&s, &arg)) {
                return NULL;
            }
            snprintf(novo.expr, sizeof(novo.expr), "%s(%s)", token, arg.expr);
            novo.prec = precedence(0);
            if (!ns_push(&s, novo)) {
                return NULL;
            }
        } else {
            Node n;
            strncpy(n.expr, token, sizeof(n.expr));
            n.expr[sizeof(n.expr) - 1] = '\0';
            n.prec = precedence(0);
            if (!ns_push(&s, n)) {
                return NULL;
            }
        }
    }

    if (s.top != 0) {
        return NULL;
    }

    strcpy(resultado, s.data[0].expr);
    return resultado;
}