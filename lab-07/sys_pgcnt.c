int
sys_pgcnt(void)
{
    pde_t *pde;  // Puntero a una entrada en la tabla de directorio.
    pte_t *pte;  // Puntero a una entrada en una tabla de páginas.

    int i = 0;
    int j = 0;
    int cnt = 0;

    // PTE_P: Si la página está en memoria.
    // PTE_U: Si la página corresponde a un proceso de usuario.
    unsigned int flag = PTE_P | PTE_U;

    for(i=0; i < NPDENTRIES; i++)
        // i-ésima entrada en la tabla de directorio.
        pde = &((myproc()->pgdir)[i]);

        // Si no es NULL y hay páginas en memoria
        if(*pde & PTE_P) {
            pte = (pte_t*) P2V(PTE_ADDR(*pde));

            // Recorre cada entrada de la tabla secundaria
            for(j=0; j < NPTENTRIES; j++) {
                // Si esta en memoria la página, la cuenta
                if((pte[j] & flag) == flag) {
                   cnt = cnt + 1;
                }
            }
        }
    }

    return cnt;
}
