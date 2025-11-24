#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define QUEUE_SIZE 5
#define STACK_SIZE 3
#define MAX_HISTORY 50

// -------------------------------------------------------------------
// Estruturas
// -------------------------------------------------------------------

typedef struct {
    char type;
    int id;
} Piece;

typedef struct {
    Piece items[QUEUE_SIZE];
    int front, rear, count;
} PieceQueue;

typedef struct {
    Piece items[STACK_SIZE];
    int top;
} PieceStack;

// Histórico para Undo
typedef struct {
    int tipo;      // operação executada
    Piece a, b;    // peças envolvidas
} Historico;

// -------------------------------------------------------------------

static int nextId = 1;
const char TYPES[] = { 'I','O','T','L' };

// -------------------------------------------------------------------
// Inicialização de estruturas
// -------------------------------------------------------------------

void initQueue(PieceQueue *q) {
    q->front = q->rear = q->count = 0;
}

void initStack(PieceStack *s) {
    s->top = -1;
}

// -------------------------------------------------------------------

int q_isFull(const PieceQueue *q) { return q->count == QUEUE_SIZE; }
int q_isEmpty(const PieceQueue *q) { return q->count == 0; }

int s_isFull(const PieceStack *s) { return s->top == STACK_SIZE - 1; }
int s_isEmpty(const PieceStack *s) { return s->top == -1; }

// -------------------------------------------------------------------

Piece gerar() {
    Piece p;
    p.type = TYPES[rand() % 4];
    p.id = nextId++;
    return p;
}

// FILA --------------------------------------------------------------

int enqueue(PieceQueue *q, Piece p) {
    if (q_isFull(q)) return 0;
    q->items[q->rear] = p;
    q->rear = (q->rear + 1) % QUEUE_SIZE;
    q->count++;
    return 1;
}

int dequeue(PieceQueue *q, Piece *out) {
    if (q_isEmpty(q)) return 0;
    if (out) *out = q->items[q->front];
    q->front = (q->front + 1) % QUEUE_SIZE;
    q->count--;
    return 1;
}

// PILHA -------------------------------------------------------------

int push(PieceStack *s, Piece p) {
    if (s_isFull(s)) return 0;
    s->items[++s->top] = p;
    return 1;
}

int pop(PieceStack *s, Piece *out) {
    if (s_isEmpty(s)) return 0;
    if (out) *out = s->items[s->top];
    s->top--;
    return 1;
}

// -------------------------------------------------------------------
// Impressão
// -------------------------------------------------------------------

void printQueue(const PieceQueue *q) {
    printf("Fila: ");
    if (q_isEmpty(q)) { printf("(vazia)\n"); return; }

    for (int i = 0; i < q->count; i++) {
        int pos = (q->front + i) % QUEUE_SIZE;
        Piece p = q->items[pos];
        printf("[%c:%d]%s", p.type, p.id, (i == q->count - 1 ? "" : " -> "));
    }
    printf("\n");
}

void printStack(const PieceStack *s) {
    printf("Pilha: ");
    if (s_isEmpty(s)) { printf("(vazia)\n\n"); return; }

    for (int i = s->top; i >= 0; i--) {
        Piece p = s->items[i];
        printf("[%c:%d]%s", p.type, p.id, (i == 0 ? "" : " | "));
    }
    printf("\n\n");
}

// -------------------------------------------------------------------
// Undo System
// -------------------------------------------------------------------

void salvar(Historico h[], int *topo, int tipo, Piece a, Piece b) {
    h[++(*topo)] = (Historico){ tipo, a, b };
}

// -------------------------------------------------------------------

void desfazer(Historico h[], int *htop, PieceQueue *fila, PieceStack *pilha) {
    if (*htop < 0) {
        printf("\nNada para desfazer!\n\n");
        return;
    }

    Historico u = h[(*htop)--];

    switch (u.tipo) {

        case 1: // desfazer jogar peça
            dequeue(fila, NULL);
            enqueue(fila, u.a);
            printf("\nDesfeito: Jogar peça\n");
            break;

        case 2: // desfazer reservar peça
            pop(pilha, NULL);
            enqueue(fila, u.a);
            printf("\nDesfeito: Reservar peça\n");
            break;

        case 3: // desfazer usar peça reservada
            push(pilha, u.a);
            printf("\nDesfeito: Usar peça reservada\n");
            break;

        case 4: // desfazer troca
            // mesma troca novamente restaura
            {
                int pos = fila->front;
                Piece tmp = fila->items[pos];
                fila->items[pos] = pilha->items[pilha->top];
                pilha->items[pilha->top] = tmp;
            }
            printf("\nDesfeito: Troca\n");
            break;

        case 6: // desfazer inversão total
            // simplesmente troca fila e pilha de novo
            {
                Piece tempFila[QUEUE_SIZE];
                Piece tempPilha[STACK_SIZE];
                int fc = fila->count;
                int pst = pilha->top;

                for (int i = 0; i < fc; i++)
                    tempFila[i] = fila->items[(fila->front+i)%QUEUE_SIZE];

                for (int i = 0; i <= pst; i++)
                    tempPilha[i] = pilha->items[i];

                initQueue(fila);
                initStack(pilha);

                // restaurar
                for (int i = 0; i < pst+1 && i < QUEUE_SIZE; i++)
                    enqueue(fila, tempPilha[i]);

                for (int i = 0; i < fc && i < STACK_SIZE; i++)
                    push(pilha, tempFila[i]);
            }
            printf("\nDesfeito: Inverter fila com pilha\n");
            break;
    }

    printQueue(fila);
    printStack(pilha);
}

// Inverter Fila <-> Pilha

void inverterEstruturas(PieceQueue *fila, PieceStack *pilha) {
    Piece tempFila[QUEUE_SIZE];
    Piece tempPilha[STACK_SIZE];

    int fc = fila->count;
    int pst = pilha->top;

    for (int i = 0; i < fc; i++)
        tempFila[i] = fila->items[(fila->front + i) % QUEUE_SIZE];

    for (int i = 0; i <= pst; i++)
        tempPilha[i] = pilha->items[i];

    initQueue(fila);
    initStack(pilha);

    for (int i = 0; i <= pst && i < QUEUE_SIZE; i++)
        enqueue(fila, tempPilha[i]);

    for (int i = 0; i < fc && i < STACK_SIZE; i++)
        push(pilha, tempFila[i]);
}


int lerInt() { char b[20]; fgets(b,20,stdin); return atoi(b); }


int main() {
    srand(time(NULL));

    PieceQueue fila;
    PieceStack pilha;
    Historico hist[MAX_HISTORY];
    int htop = -1;

    initQueue(&fila);
    initStack(&pilha);

    while (!q_isFull(&fila))
        enqueue(&fila, gerar());

    while (1) {
        printf("===== NÍVEL MESTRE =====\n");
        printf("1 - Jogar peça\n");
        printf("2 - Reservar peça\n");
        printf("3 - Usar reservada\n");
        printf("4 - Trocar fila <-> pilha\n");
        printf("5 - Desfazer\n");
        printf("6 - Inverter estruturas\n");
        printf("0 - Sair\n>> ");

        int op = lerInt();

      if (op == 0) {
    printf("Saindo...\n");
    break;
}
        Piece a, b;

        switch (op) {

            case 1: // jogar
                dequeue(&fila, &a);
                salvar(hist,&htop,1,a,(Piece){0});
                enqueue(&fila, gerar());
                break;

            case 2: // reservar
                if (!s_isFull(&pilha)) {
                    dequeue(&fila, &a);
                    push(&pilha, a);
                    salvar(hist,&htop,2,a,(Piece){0});
                    enqueue(&fila, gerar());
                }
                break;

            case 3: // usar reservada
                if (!s_isEmpty(&pilha)) {
                    pop(&pilha, &a);
                    salvar(hist,&htop,3,a,(Piece){0});
                }
                break;

            case 4: // troca topo <-> frente
                if (!s_isEmpty(&pilha)) {
                    int pos = fila.front;
                    a = fila.items[pos];
                    b = pilha.items[pilha.top];
                    fila.items[pos] = b;
                    pilha.items[pilha.top] = a;
                    salvar(hist,&htop,4,a,b);
                }
                break;

            case 5:
                desfazer(hist,&htop,&fila,&pilha);
                continue;

            case 6:
                inverterEstruturas(&fila,&pilha);
                salvar(hist,&htop,6,(Piece){0},(Piece){0});
                break;
        }

        printQueue(&fila);
        printStack(&pilha);
    }
}
