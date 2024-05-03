    .text

/* Switch from current_thread to next_thread. Make next_thread
 * the current_thread, and set next_thread to 0.
 * Use eax as a temporary register; it is caller saved.
 */
    .globl thread_switch
thread_switch:
    # guarda el contexto del hilo actual en la pila: la instruccion
    # pushal: guarda EAX, EBX, ECX, EDX, ESP, EBP, ESI, EDI en la pila
    # (el registro ESP guarda el puntero a la pila).
    pusha

    # current_thread->sp = esp
    movl current_thread, %eax
    movl %esp, (%eax)

    # esp = next_thread->sp
    movl next_thread, %eax
    movl (%eax), %esp

    # current_thread = next_thread
    movl %eax, current_thread

    # next_thread = 0
    movl $0x0, next_thread

    # restaura el contexto del nuevo thread
    popa
    
    # pop return address from stack
    ret
