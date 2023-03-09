#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <functional>
#include <execinfo.h>

void dump_backtrace(void)
{
    int j, nptrs;
    void* buffer[100];
    char** strings;
    nptrs = backtrace(buffer, 100);
    printf("backtrace retrun %d address\n", nptrs);

    strings = backtrace_symbols(buffer, nptrs);
    if (strings == NULL)
        return;
    for (j = 0; j < nptrs; j++)
    {
        printf("[%02d] %s\n", j, strings[j]);
    } 
    free(strings);
}

void* my_malloc(size_t size)
{
    void* addr = __builtin_return_address(0);
    char** caller = backtrace_symbols(&addr, 1);
    printf("caller addr:%p\n", addr);
    printf("caller:%s\n", caller[0]);
    void* parent_addr = __builtin_return_address(1);
    char** caller_parent = backtrace_symbols(&parent_addr, 1);
    printf("caller addr parent:%p\n", parent_addr);
    printf("caller:%s\n", caller_parent[0]);
    dump_backtrace();
    return malloc(size);
}

void caller()
{
    u_int8_t* buf = (u_int8_t*)my_malloc(10);
    if (buf)
    {
        free(buf);
    }
}

/*编译参数需加rdynamic(g++ -g -rdynamic _backtrace.c -o target)*/
int main(void)
{
    caller();
    return 0;
}