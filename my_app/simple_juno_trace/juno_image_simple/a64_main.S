.file "a64_main.S"
.section A64_main, "ax"
.text
.global a64_main
.type a64_main, "function"
.global func1
.type func1, "function"

a64_main:
    mov x0, #0
    mov x1, #1
    mov x2, #2
    mov x3, #3
    cmp x0, #0
    beq func1
    mov x4, #4
    mov x5, #5
func1:
    mov x6, #6
    cmp x0, #1
    beq func1
    mov x7, #7
/* The debugger should set a breakpoint here */
    mov x8, #8

.end
