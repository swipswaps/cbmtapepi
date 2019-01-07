
// Marcel Timm, RhinoDevel, 2019jan05

#include <stdint.h>
#include <stdbool.h>

#include "str.h"

static bool is_upper_case_letter(char const c)
{
    return c >= 'A' && c <= 'Z';
}
static bool is_lower_case_letter(char const c)
{
    return c >= 'a' && c <= 'z';
}
static char get_upper_case(char const c)
{
    static char const diff = 'a' - 'A';

    if(is_lower_case_letter(c))
    {
        return c - diff;
    }
    return '?';
}
static char get_lower_case(char const c)
{
    static char const diff = 'a' - 'A';

    if(is_upper_case_letter(c))
    {
        return c + diff;
    }
    return '?';
}

uint32_t str_get_len(char const * const s)
{
    uint32_t ret_val = 0;

    while(s[ret_val] != '\0')
    {
        ++ret_val;
    }
    return ret_val;
}

void str_to_upper(char * const s_out, char const * const s_in)
{
    int i = 0;

    for(;s_in[i] != '\0';++i)
    {
        if(is_lower_case_letter(s_in[i]))
        {
            s_out[i] = get_upper_case(s_in[i]);
            continue;
        }
        s_out[i] = s_in[i];
    }
    s_out[i] = '\0';//s_in[i];
}

void str_to_lower(char * const s_out, char const * const s_in)
{
    int i = 0;

    for(;s_in[i] != '\0';++i)
    {
        if(is_upper_case_letter(s_in[i]))
        {
            s_out[i] = get_lower_case(s_in[i]);
            continue;
        }
        s_out[i] = s_in[i];
    }
    s_out[i] = '\0';//s_in[i];
}