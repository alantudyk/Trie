#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
// #include <lzma.h>

typedef uint32_t node_t;
typedef struct temp_t {
    uint32_t char_code, is_last;
    struct temp_t *first_child, *next_sibling;
} temp_t;

static size_t i = 1;
static node_t N[1 << 20];
static temp_t T[1 << 20];

#define T_to_N (t->char_code << 26) + (t->is_last << 25) + ((t->next_sibling != NULL) << 24)

static void traversal(temp_t *t, uint32_t self_index) {
    switch (self_index & 3) {
        case 0: N[self_index >> 8] = T_to_N;
                if (t->next_sibling != NULL)
                    traversal(t->next_sibling, (i++ << 8) + 1);
                if (t->first_child != NULL)
                    traversal(t->first_child,  (N[self_index >> 8] += i++) << 8);
                if (t->next_sibling != NULL)
                    traversal(t->next_sibling, (((self_index >> 8) + 1) << 8) + 2);
                break;
        case 1: N[self_index >> 8] = T_to_N;
                if (t->next_sibling != NULL)
                    traversal(t->next_sibling, (i++ << 8) + 1);
                break;
        case 2: if (t->first_child != NULL)
                    traversal(t->first_child,  (N[self_index >> 8] += i++) << 8);
                if (t->next_sibling != NULL)
                    traversal(t->next_sibling, (((self_index >> 8) + 1) << 8) + 2);
    }
}

int main(void) {
    
    if (system("xz -dkc dictionary.txt.mapped_chars.xz > .tmp.txt.mapped_chars")) goto e;
    FILE *f = fopen(".tmp.txt.mapped_chars", "rb");
    if (f == NULL) goto e;
    temp_t *t;
    
    for (;;) {
        
        uint8_t c[100], *d = c, *D;
        for (;; d++) {
            if (!fread(d, 1, 1, f)) goto b;
            if (*d == 34) break;
        }
        
        for (t = T, D = d, d = c; d < D; d++) {
            if (t->first_child == NULL) {
                t = t->first_child = T + i++;
                t->char_code = *d;
                continue;
            }
            t = t->first_child;
            while (t->char_code != *d && t->next_sibling != NULL)
                t = t->next_sibling;
            if (t->char_code != *d) {
                t = t->next_sibling = T + i++;
                t->char_code = *d;
            }
        }
        
        t->is_last = 1;
    }
    
b:  if (fclose(f) || system("rm .tmp.txt.mapped_chars") ||
        (f = fopen(".trie.dump", "wb")) == NULL) goto e;
    
    i = 1; if (T->first_child != NULL) traversal(T->first_child, (N[0] = i++) << 8);
    
    if (fwrite(N, 4, i, f) != i || fclose(f)) goto e;
    if (system("xz -zce .trie.dump > trie.dump.xz && rm .trie.dump")) goto e;
    
    return 0; e: return 1;
}
