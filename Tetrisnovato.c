// Nível Novato - Fila circular de 5 peças para "Tetris Stack"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define QUEUE_SIZE 5

typedef struct {
    char type;   // 'I','O','T','L' (char)
    int id;      // identificador único
} Piece;

typedef struct {
    Piece items[QUEUE_SIZE];
    int front;   //  elemento da frente (para ser removido)
    int rear;    //  onde será colocado no proximo elemento
    int count;   // número de elementos que estão na fila
} PieceQueue;

// prototipos
void initQueue(PieceQueue *q);
int isEmpty(const PieceQueue *q);
int isFull(const PieceQueue *q);
int enqueue(PieceQueue *q, Piece p);
int dequeue(PieceQueue *q, Piece *out);
Piece gerarPeca(void);
void preencherInicial(PieceQueue *q);
void mostrarFila(const PieceQueue *q);
void limparBufferEntrada(void);

// contador de ids global 
static int nextId = 1;

const char POSSIBLE_TYPES[] = { 'I', 'O', 'T', 'L' };
const int N_TYPES = sizeof(POSSIBLE_TYPES) / sizeof(POSSIBLE_TYPES[0]);

// Comecar a fila
void initQueue(PieceQueue *q) {
    q->front = 0;
    q->rear = 0;
    q->count = 0;
}

int isEmpty(const PieceQueue *q) {
    return q->count == 0;
}

int isFull(const PieceQueue *q) {
    return q->count == QUEUE_SIZE;
}

int enqueue(PieceQueue *q, Piece p) {
    if (isFull(q)) return 0;
    q->items[q->rear] = p;
    q->rear = (q->rear + 1) % QUEUE_SIZE;
    q->count++;
    return 1;
}

int dequeue(PieceQueue *q, Piece *out) {
    if (isEmpty(q)) return 0;
    if (out) *out = q->items[q->front];
    q->front = (q->front + 1) % QUEUE_SIZE;
    q->count--;
    return 1;
}

Piece gerarPeca(void) {
    Piece p;
    p.type = POSSIBLE_TYPES[rand() % N_TYPES];
    p.id = nextId++;
    return p;
}

void preencherInicial(PieceQueue *q) {
    while (!isFull(q)) {
        Piece p = gerarPeca();
        enqueue(q, p);
    }
}

void mostrarFila(const PieceQueue *q) {
    printf("\nEstado atual da fila (capacidade %d):\n", QUEUE_SIZE);
    printf("+----+------+------+\n");
    printf("|Pos | Type |  ID  |\n");
    printf("+----+------+------+\n");

    for (int i = 0; i < QUEUE_SIZE; ++i) {
        int occupied = 0;

        for (int k = 0; k < q->count; ++k) {
            int pos = (q->front + k) % QUEUE_SIZE;
            if (pos == i) { occupied = 1; break; }
        }

        if (occupied) {
            Piece p = q->items[i];
            printf("| %2d |  %c   | %4d |\n", i, p.type, p.id);
        } else {
            printf("| %2d |  -   |  ---- |\n", i);
        }
    }
    printf("+----+------+------+\n");
    printf("Front index: %d, Rear index: %d, Count: %d\n", q->front, q->rear, q->count);

    if (!isEmpty(q)) {
        printf("Ordem (frente -> fim): ");
        for (int k = 0; k < q->count; ++k) {
            int pos = (q->front + k) % QUEUE_SIZE;
            printf("[%c:%d]%s", q->items[pos].type, q->items[pos].id,
                   (k==q->count-1) ? "" : " -> ");
        }
        printf("\n");
    }
}

void limparBufferEntrada(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {}
}

// ------------- codigo MENU -------------------------

int main(void) {
    srand((unsigned)time(NULL));

    PieceQueue queue;
    initQueue(&queue);
    preencherInicial(&queue);

    int opcao = 0;
    while (1) {
        printf("\n=== Tetris Stack - Fila de Peças (Novato) ===\n");
        printf("1) Visualizar fila\n");
        printf("2) Jogar peça (dequeue) e inserir automaticamente nova peça\n");
        printf("3) Inserir manualmente nova peça (enqueue)\n");
        printf("4) Sair\n");
        printf("Escolha uma opcao: ");

        if (scanf("%d", &opcao) != 1) {
            printf("Entrada inválida. Digite um número.\n");
            limparBufferEntrada();
            continue;
        }

        if (opcao == 1) {
            mostrarFila(&queue);

        } else if (opcao == 2) {
            Piece removed;
            if (dequeue(&queue, &removed)) {
                printf("\nVocê jogou a peça: Type=%c, ID=%d\n", removed.type, removed.id);

                Piece nova = gerarPeca();
                enqueue(&queue, nova);

                printf("Nova peça inserida automaticamente: Type=%c, ID=%d\n", nova.type, nova.id);
            } else {
                printf("\nFila vazia. Nada para jogar.\n");
            }

            mostrarFila(&queue);

        } else if (opcao == 3) {
            if (isFull(&queue)) {
                printf("\nFila cheia. Remova/jogue uma peça antes de inserir.\n");
            } else {
                Piece p;

                printf("Deseja (1) gerar peça automaticamente ou (2) escolher tipo? ");
                int sub;

                if (scanf("%d", &sub) != 1) {
                    limparBufferEntrada();
                    printf("Entrada inválida.\n");
                    continue;
                }

                if (sub == 1) {
                    p = gerarPeca();

                } else {
                    printf("Escolha tipo (I O T L): ");

                    limparBufferEntrada();   // <-- corrigido!
                    char buf[10];
                    fgets(buf, sizeof(buf), stdin); // <-- seguro agora

                    char ch = '\0';
                    for (int i = 0; buf[i] != '\0'; ++i) {
                        if (buf[i] != ' ' && buf[i] != '\n' && buf[i] != '\t') {
                            ch = buf[i];
                            break;
                        }
                    }

                    int valid = 0;
                    for (int i = 0; i < N_TYPES; ++i)
                        if (ch == POSSIBLE_TYPES[i]) valid = 1;

                    if (!valid) {
                        printf("Tipo inválido. Geração automática será usada.\n");
                        p = gerarPeca();
                    } else {
                        p.type = ch;
                        p.id = nextId++;
                    }
                }

                if (enqueue(&queue, p)) {
                    printf("Peça inserida: Type=%c, ID=%d\n", p.type, p.id);
                }

            }

            mostrarFila(&queue);

        } else if (opcao == 4) {
            printf("Saindo...\n");
            break;

        } else {
            printf("Opcao inválida.\n");
        }
    }
    return 0;
}
