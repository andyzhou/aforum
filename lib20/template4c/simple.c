/*
 * Using template functions to generate table rows.
 */

#include <stdio.h>
#include <stdlib.h>

#include "tpllib.h"

int main(void)
{
    tpl_t *tpl = tpl_alloc();
    int i;
    int rows = 5;
    char *content;

    /* Load template */
    if (tpl_load(tpl, "simple.tpl") != TPL_OK)
    {
        (void)puts("Error loading template file!");
        return EXIT_FAILURE;
    }

    tpl_set_field_int_global(tpl, "ROWS", rows);
    tpl_select_section(tpl, "ROW");
        
    for (i = 1; i <= rows; i++)
    {
        int j = (i - 1) * 10;
        
        tpl_set_field_int(tpl, "TYPE", i % 2);
            
        tpl_set_field_fmt(tpl, "FIELD1", "Row %d", i);
        tpl_set_field_int(tpl, "FIELD2", j + 2);
        tpl_set_field_int(tpl, "FIELD3", j + 3);
        tpl_set_field_int(tpl, "FIELD4", j + 4);
        tpl_set_field_int(tpl, "FIELD5", j + 5);
        tpl_set_field_int(tpl, "FIELD6", j + 6);
        tpl_set_field_int(tpl, "FIELD7", j + 7);
        
        tpl_append_section(tpl);
    }
    tpl_deselect_section(tpl);

    content = malloc(tpl_length(tpl) + 1);

    tpl_get_content(tpl, content);

    (void)puts(content);

    tpl_free(tpl);

    return EXIT_SUCCESS;
}
