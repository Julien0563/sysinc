/* list_t.c */

/* Implements list_t as a singly linked list (Option 0x0).

   Each element lives in its own malloc'd two-slot cell: the data pointer
   and a pointer to the next cell (NULL at the end).

   The list_t a client holds is a pointer to a small header that points at
   the first and last cells. Because the header never moves, the list_t a
   client holds stays valid when the list grows, shrinks, or becomes empty.
   The header also keeps the length and the tail so append is O(1). */

#include "list_t.h"

struct cell {
    void *data;
    struct cell *next;
};

struct list {
    size_t len;
    struct cell *head;
    struct cell *tail;
};

#define LST(l) ((struct list *)(l))

static void *xmalloc(size_t n)
{
    void *p = malloc(n);
    if (p == NULL) {
        fprintf(stderr, "list_t: out of memory\n");
        exit(1);
    }
    return p;
}

static struct cell *new_cell(void *data)
{
    struct cell *c = xmalloc(sizeof *c);
    c->data = data;
    c->next = NULL;
    return c;
}

/* Unlinks and frees the cell after prev (or the first cell if prev is
   NULL). Returns the data it held. The caller guarantees the cell exists. */
static void *unlink_after(struct list *s, struct cell *prev)
{
    struct cell *c = prev ? prev->next : s->head;
    void *data = c->data;

    if (prev)
        prev->next = c->next;
    else
        s->head = c->next;
    if (s->tail == c)
        s->tail = prev;
    s->len--;
    free(c);
    return data;
}

list_t list_new()
{
    struct list *s = xmalloc(sizeof *s);
    s->len = 0;
    s->head = NULL;
    s->tail = NULL;
    return (list_t)s;
}

void list_clear(list_t l)
{
    struct list *s = LST(l);
    struct cell *c = s->head, *next;

    while (c != NULL) {
        next = c->next;
        free(c);
        c = next;
    }
    s->len = 0;
    s->head = NULL;
    s->tail = NULL;
}

void list_free(list_t l)
{
    list_clear(l);
    free(l);
}

void list_print(list_t l)
{
    struct cell *c;

    printf("[");
    for (c = LST(l)->head; c != NULL; c = c->next)
        printf("%lu%s", (unsigned long)(uintptr_t)c->data,
               c->next ? ", " : "");
    printf("]\n");
}

void list_append(list_t l, void *x)
{
    struct list *s = LST(l);
    struct cell *c = new_cell(x);

    if (s->tail)
        s->tail->next = c;
    else
        s->head = c;
    s->tail = c;
    s->len++;
}

void list_extend(list_t l1, list_t l2)
{
    /* Fix the count up front so list_extend(l, l) terminates. */
    size_t n = LST(l2)->len;
    struct cell *c = LST(l2)->head;

    for (; n > 0; n--, c = c->next)
        list_append(l1, c->data);
}

void list_insert(list_t l, size_t i, void *x)
{
    struct list *s = LST(l);
    struct cell *c, *prev;
    size_t k;

    /* Like Python, an index past the end means "at the end". */
    if (i >= s->len) {
        list_append(l, x);
        return;
    }

    c = new_cell(x);
    if (i == 0) {
        c->next = s->head;
        s->head = c;
    } else {
        prev = s->head;
        for (k = 1; k < i; k++)
            prev = prev->next;
        c->next = prev->next;
        prev->next = c;
    }
    s->len++;
}

bool list_remove(list_t l, void *x)
{
    struct list *s = LST(l);
    struct cell *prev = NULL, *c;

    for (c = s->head; c != NULL; prev = c, c = c->next) {
        if (c->data == x) {
            unlink_after(s, prev);
            return TRUE;
        }
    }
    return FALSE;
}

void *list_pop(list_t l, size_t i)
{
    struct list *s = LST(l);
    struct cell *prev = NULL;
    size_t k;

    if (i >= s->len) {
        fprintf(stderr, "list_pop: index %lu out of range\n", (unsigned long)i);
        exit(1);
    }
    if (i > 0) {
        prev = s->head;
        for (k = 1; k < i; k++)
            prev = prev->next;
    }
    return unlink_after(s, prev);
}

size_t list_index(list_t l, void *x)
{
    struct cell *c;
    size_t i = 0;

    for (c = LST(l)->head; c != NULL; c = c->next, i++)
        if (c->data == x)
            return i;

    fprintf(stderr, "list_index: element not in list\n");
    exit(1);
}

uint64_t list_count(list_t l, void *x)
{
    struct cell *c;
    uint64_t n = 0;

    for (c = LST(l)->head; c != NULL; c = c->next)
        if (c->data == x)
            n++;
    return n;
}

void list_reverse(list_t l)
{
    struct list *s = LST(l);
    struct cell *prev = NULL, *c = s->head, *next;

    s->tail = s->head;
    while (c != NULL) {
        next = c->next;
        c->next = prev;
        prev = c;
        c = next;
    }
    s->head = prev;
}

list_t list_copy(list_t l)
{
    list_t r = list_new();
    list_extend(r, l);
    return r;
}
