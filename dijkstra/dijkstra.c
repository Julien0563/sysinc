#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define INF UINT32_MAX

typedef struct edge_struct {
    char src[4];
    char dst[4];
    uint16_t mi;
    uint16_t name; /* interstate # */
} edge_t;

typedef struct node_struct {
    char name[4];
    edge_t **out;
    size_t n_out, cap;
    uint32_t dist;
    struct node_struct *prev;
    int done;
} node_t;

typedef struct graph_struct {
    edge_t *edges;
    size_t n_edges;
    node_t *nodes;
    size_t n_nodes;
} graph_t;

static int read_edges(const char *path, graph_t *g) {
    FILE *f = fopen(path, "r");
    if (!f) { perror(path); return -1; }

    size_t cap = 16;
    g->edges = malloc(cap * sizeof(edge_t));
    g->n_edges = 0;
    if (!g->edges) { fclose(f); return -1; }

    char a[16], b[16];
    unsigned mi, name;
    while (fscanf(f, "%15s %15s %u %u", a, b, &mi, &name) == 4) {
        if (strlen(a) != 3 || strlen(b) != 3) {
            fprintf(stderr, "bad city code: %s %s\n", a, b);
            fclose(f);
            return -1;
        }
        if (g->n_edges == cap) {
            cap *= 2;
            edge_t *tmp = realloc(g->edges, cap * sizeof(edge_t));
            if (!tmp) { fclose(f); return -1; }
            g->edges = tmp;
        }
        edge_t *e = &g->edges[g->n_edges++];
        strcpy(e->src, a);
        strcpy(e->dst, b);
        e->mi = (uint16_t)mi;
        e->name = (uint16_t)name;
    }
    fclose(f);
    return 0;
}

static void print_edges(const graph_t *g) {
    for (size_t i = 0; i < g->n_edges; i++) {
        const edge_t *e = &g->edges[i];
        printf("%s\t%s\t%u\t%u\n", e->src, e->dst, e->mi, e->name);
    }
}

static node_t *find_node(graph_t *g, const char *name) {
    for (size_t i = 0; i < g->n_nodes; i++)
        if (strcmp(g->nodes[i].name, name) == 0) return &g->nodes[i];
    return NULL;
}

static int add_node(graph_t *g, const char *name) {
    if (find_node(g, name)) return 0;
    node_t *tmp = realloc(g->nodes, (g->n_nodes + 1) * sizeof(node_t));
    if (!tmp) return -1;
    g->nodes = tmp;
    node_t *n = &g->nodes[g->n_nodes++];
    memset(n, 0, sizeof *n);
    strcpy(n->name, name);
    n->dist = INF;
    return 0;
}

static int add_out(node_t *n, edge_t *e) {
    if (n->n_out == n->cap) {
        size_t nc = n->cap ? n->cap * 2 : 2;
        edge_t **tmp = realloc(n->out, nc * sizeof(edge_t *));
        if (!tmp) return -1;
        n->out = tmp;
        n->cap = nc;
    }
    n->out[n->n_out++] = e;
    return 0;
}

static int build_nodes(graph_t *g) {
    g->nodes = NULL;
    g->n_nodes = 0;
    for (size_t i = 0; i < g->n_edges; i++) {
        if (add_node(g, g->edges[i].src) || add_node(g, g->edges[i].dst))
            return -1;
    }
    for (size_t i = 0; i < g->n_edges; i++) {
        edge_t *e = &g->edges[i];
        if (add_out(find_node(g, e->src), e) || add_out(find_node(g, e->dst), e))
            return -1;
    }
    return 0;
}

static void free_graph(graph_t *g) {
    for (size_t i = 0; i < g->n_nodes; i++) free(g->nodes[i].out);
    free(g->nodes);
    free(g->edges);
    g->nodes = NULL; g->edges = NULL;
    g->n_nodes = g->n_edges = 0;
}

static int dijkstra(graph_t *g, const char *start) {
    node_t *s = find_node(g, start);
    if (!s) return -1;
    for (size_t i = 0; i < g->n_nodes; i++) {
        g->nodes[i].dist = INF;
        g->nodes[i].prev = NULL;
        g->nodes[i].done = 0;
    }
    s->dist = 0;

    for (size_t iter = 0; iter < g->n_nodes; iter++) {
        node_t *u = NULL;
        for (size_t i = 0; i < g->n_nodes; i++) {
            node_t *c = &g->nodes[i];
            if (!c->done && c->dist != INF && (!u || c->dist < u->dist)) u = c;
        }
        if (!u) break;
        u->done = 1;

        for (size_t k = 0; k < u->n_out; k++) {
            edge_t *e = u->out[k];
            const char *other = strcmp(e->src, u->name) == 0 ? e->dst : e->src;
            node_t *v = find_node(g, other);
            uint32_t nd = u->dist + e->mi;
            if (nd < v->dist) { v->dist = nd; v->prev = u; }
        }
    }
    return 0;
}

static void print_path(const node_t *n) {
    if (n->prev) { print_path(n->prev); printf(" -> "); }
    printf("%s", n->name);
}

int main(int argc, char **argv) {
    const char *path  = argc > 1 ? argv[1] : "graph.tsv";
    const char *start = argc > 2 ? argv[2] : "SEA";

    graph_t g = {0};
    if (read_edges(path, &g) != 0) { free_graph(&g); return 1; }
    if (build_nodes(&g) != 0)      { free_graph(&g); return 1; }

    printf("== %zu edges ==\n", g.n_edges);
    print_edges(&g);

    printf("\n== %zu nodes ==\n", g.n_nodes);
    for (size_t i = 0; i < g.n_nodes; i++)
        printf("%s (degree %zu)\n", g.nodes[i].name, g.nodes[i].n_out);

    if (dijkstra(&g, start) != 0) {
        fprintf(stderr, "unknown start node: %s\n", start);
        free_graph(&g);
        return 1;
    }
    printf("\n== shortest paths from %s ==\n", start);
    for (size_t i = 0; i < g.n_nodes; i++) {
        node_t *n = &g.nodes[i];
        if (n->dist == INF) { printf("%s: unreachable\n", n->name); continue; }
        printf("%s: %u mi  ", n->name, n->dist);
        print_path(n);
        printf("\n");
    }

    free_graph(&g);
    return 0;
}
