// tetris_stack_aventureiro.c
// Sistema com FILA + PILHA para Tetris Stack (Nível Aventureiro)

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define QUEUE_SIZE 5
#define STACK_SIZE 3

// ----------------------------------------------------------
// Estruturas
// ----------------------------------------------------------

typedef struct {
    char type;   // 'I', 'O', 'T', 'L'
    int id;      // identificador único
} Piece;

// Fila circular
typedef struct {
    Piece items[QUEUE_SIZE];
    int front;
    int rear;
    int count;
} PieceQueue;

// Pilha linear
typedef struct {
    Piece items[STACK_SIZE];
    int top;
} PieceStack;

// ----------------------------------------------------------
// Variáveis globais
// ----------------------------------------------------------

static int nextId = 1;
const char POSSIBLE_TYPES[] = { 'I', 'O', 'T', 'L' };
const int N_TYPES = sizeof(POSSIBLE_TYPES) / sizeof(POSSIBLE_TYPES[0]);

// ----------------------------------------------------------
// Funções de Fila
// ----------------------------------------------------------

void initQueue(PieceQueue *q) {
    q->front = 0;
    q->rear = 0;
    q->count = 0;
}

int isQueueFull(const PieceQueue *q) { return q->count == QUEUE_SIZE; }
int isQueueEmpty(const PieceQueue *q) { return q->count == 0; }

int enqueue(PieceQueue *q, Piece p) {
    if (isQueueFull(q)) return 0;
    q->items[q->rear] = p;
    q->rear = (q->rear + 1) % QUEUE_SIZE;
    q->count++;
    return 1;
}

int dequeue(PieceQueue *q, Piece *out) {
    if (isQueueEmpty(q)) return 0;
    if (out) *out = q->items[q->front];
    q->front = (q->front + 1) % QUEUE_SIZE;
    q->count--;
    return 1;
}

// ----------------------------------------------------------
// Funções da Pilha
// ----------------------------------------------------------

void initStack(PieceStack *s) {
    s->top = -1;
}

int isStackFull(const PieceStack *s) { return s->top == STACK_SIZE - 1; }
int isStackEmpty(const PieceStack *s) { return s->top == -1; }

int push(PieceStack *s, Piece p) {
    if (isStackFull(s)) return 0;
    s->items[++s->top] = p;
    return 1;
}

int pop(PieceStack *s, Piece *out) {
    if (isStackEmpty(s)) return 0;
    if (out) *out = s->items[s->top];
    s->top--;
    return 1;
}

// ----------------------------------------------------------
// Geração de peças
// ----------------------------------------------------------

Piece gerarPeca() {
    Piece p;
    p.type = POSSIBLE_TYPES[rand() % N_TYPES];
    p.id = nextId++;
    return p;
}

// Inicializa fila com 5 peças
void preencherFilaInicial(PieceQueue *q) {
    while (!isQueueFull(q)) {
        enqueue(q, gerarPeca());
    }
}

// ----------------------------------------------------------
// Impressão das estruturas
// ----------------------------------------------------------

void mostrarFila(const PieceQueue *q) {
    printf("\nFila (frente → trás): ");
    if (isQueueEmpty(q)) {
        printf("(vazia)");
    } else {
        for (int i = 0; i < q->count; i++) {
            int pos = (q->front + i) % QUEUE_SIZE;
            Piece p = q->items[pos];
            printf("[%c:%d]%s", p.type, p.id, (i == q->count - 1 ? "" : " -> "));
        }
    }
    printf("\n");
}

void mostrarPilha(const PieceStack *s) {
    printf("Pilha de reserva (topo ↓): ");
    if (isStackEmpty(s)) {
        printf("(vazia)");
    } else {
        for (int i = s->top; i >= 0; i--) {
            Piece p = s->items[i];
            printf("[%c:%d]%s", p.type, p.id, (i == 0 ? "" : " | "));
        }
    }
    printf("\n\n");
}

// ----------------------------------------------------------
// Entrada segura
// ----------------------------------------------------------

int lerInteiro() {
    char buf[32];
    fgets(buf, sizeof(buf), stdin);
    return atoi(buf);
}

// ----------------------------------------------------------
// Programa Principal
// ----------------------------------------------------------

int main() {
    srand((unsigned) time(NULL));

    PieceQueue fila;
    PieceStack pilha;

    initQueue(&fila);
    initStack(&pilha);

    preencherFilaInicial(&fila);

    while (1) {
        printf("===== NÍVEL AVENTUREIRO - TETRIS STACK =====\n");
        printf("1 - Jogar peça\n");
        printf("2 - Reservar peça\n");
        printf("3 - Usar peça reservada\n");
        printf("0 - Sair\n");
        printf("Opção: ");

        int opcao = lerInteiro();

        if (opcao == 0) {
            printf("Saindo...\n");
            break;
        }

        if (opcao == 1) {
            // Jogar peça (remove da fila e gera nova automaticamente)
            Piece jogada;

            if (dequeue(&fila, &jogada)) {
                printf("\nVocê jogou a peça: [%c:%d]\n", jogada.type, jogada.id);

                Piece nova = gerarPeca();
                enqueue(&fila, nova);

                printf("Nova peça gerada automaticamente: [%c:%d]\n\n", nova.type, nova.id);
            }

        } else if (opcao == 2) {
            // Reservar peça (push)
            if (isStackFull(&pilha)) {
                printf("\nA pilha de reserva está cheia!\n\n");
            } else {
                Piece p;
                dequeue(&fila, &p);
                push(&pilha, p);
                printf("\nPeça reservada: [%c:%d]\n\n", p.type, p.id);

                // Repor peça na fila
                enqueue(&fila, gerarPeca());
            }

        } else if (opcao == 3) {
            // Usar peça reservada (pop)
            if (isStackEmpty(&pilha)) {
                printf("\nNenhuma peça reservada!\n\n");
            } else {
                Piece usada;
                pop(&pilha, &usada);
                printf("\nVocê usou a peça reservada: [%c:%d]\n\n", usada.type, usada.id);
            }

        } else {
            printf("Opção inválida!\n\n");
        }

        // Mostrar estado geral
        mostrarFila(&fila);
        mostrarPilha(&pilha);
    }

    return 0;
}
