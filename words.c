#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
// #include <lzma.h>

#define CHAR_CODE (*n >> 26)
#define IS_LAST ((*n >> 25) & 1)
#define FIRST_CHILD (*n & ((1u << 24) - 1))
#define NEXT_SIBLING ((*n >> 24) & 1)

typedef uint32_t node_t;
static node_t N[1 << 20];
static uint8_t s[100], c[100], *d, *D, *p;

static void traversal(const node_t *const n, uint8_t *const _z) {
    
    uint8_t c = CHAR_CODE, *z = _z;
    
         if (c <  16) *z++ = 208, *z++ = c + 176;
    else if (c <  32) *z++ = 209, *z++ = c + 112;
    else if (c == 32) *z++ = 209, *z++ =     145;
    else *z++ = 45;
    
    if (IS_LAST) { z[0] = '\n', z[1] = '\0'; printf("\t%s", s); }
    
    if (FIRST_CHILD ) traversal(N + FIRST_CHILD, z);
    if (NEXT_SIBLING) traversal(n + 1,          _z);
}

int main(void) {
    
    if (system("xz -dkc trie.dump.xz > .trie.dump")) goto e;
    FILE *f = fopen(".trie.dump", "rb");
    if (f == NULL) goto e;
    int _ = fread(N, 1, 4 << 20, f);
    if (fclose(f) || system("rm .trie.dump")) goto e;
    const node_t *n;
    
    for (;;) {
        
        printf("Enter Prefix: ");
        if (fgets(s, 100, stdin) == NULL) goto e;
        if (s[0] == 'q' && s[1] == '\n') return 0;
        
        for (d = c, p = s;; p++) {
            switch (*p) {
                case  10: goto b;
                case  45: *d++ = 33;
                          break;
                case 208: ++p;
                          if (176 <= *p && *p <= 191)
                              *d++ = *p - 176;
                          else goto h;
                          break;
                case 209: ++p;
                          if (128 <= *p && *p <= 143)
                              *d++ = *p - 112;
                          else if (*p == 145) *d++ = 32;
                          else goto h;
                          break;
                default : goto h;
            }
        }
        
    b:  for (n = N, D = d, d = c; d < D; d++) {
            if (FIRST_CHILD == 0) { n = NULL; break; }
            n = N + FIRST_CHILD;
            while (CHAR_CODE != *d && NEXT_SIBLING == 1) n++;
            if (CHAR_CODE != *d) { n = NULL; break; }
        }
        
        if (n == NULL) { printf ("\tPrefix Not Found\n"); continue; }
        
        if (IS_LAST) printf("\t%s", s);
        if (FIRST_CHILD) traversal(N + FIRST_CHILD, p);
        
        continue;
        
    h:  printf("\tInvalid Input\n"
               "\tOnly lowercase russian letters and '-' are allowed\n"
               "\tEnter 'q' for exit\n");
    }
    
    e: return 1;
}
