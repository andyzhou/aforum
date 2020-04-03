/*
 * NPH CGI example program
 * Shows how to use template functions library
 * with nested sections.
 */

#include <stdio.h>
#include <stdlib.h>

#include "tpllib.h"

int main(void)
{
    int i, j, k;
    int categories = 3;
    int questions = 5;
    int options = 7;
    char *content;
    tpl_t *tpl = tpl_alloc();

    /* Load template file */
    if (tpl_load(tpl, "advanced.tpl") != TPL_OK)
    {
        (void)puts("Error loading template file!");
        return EXIT_FAILURE;
    }
    
    /* Set some fields */
    tpl_set_field_int(tpl, "CATEGORIES", categories);
    tpl_set_field_int_global(tpl, "QUESTIONS", questions);
    tpl_set_field_int(tpl, "OPTIONS", options);
    
    tpl_select_section(tpl, "CATEGORY");    
    for (i=1; i<=categories; i++)
    {
        tpl_set_field_int(tpl, "CATEGORY_NR", i);
        tpl_select_section(tpl, "QUESTION");

        for (j=1; j<=questions; j++)
        {
            tpl_set_field_int(tpl, "TYPE", j%2);
            tpl_set_field_int(tpl, "QUESTION_NR", (i-1)*questions+j);
        
            tpl_select_section(tpl, "OPTION");
        
            for (k=1; k<=options; k++)
            {
                tpl_set_field_fmt(tpl,
                                  "OPTION_VAL",
                                  "Answer %03d",
                                  ((i-1)*questions+j-1)*options+k);

                tpl_append_section(tpl);
            }
            tpl_deselect_section(tpl);

            tpl_append_section(tpl);
        }
        tpl_deselect_section(tpl);

        tpl_append_section(tpl);
    }
    tpl_deselect_section(tpl);

    content = malloc(tpl_length(tpl) + 1);

    tpl_get_content(tpl, content);

    (void)puts(content);
    
    tpl_free(tpl);

    return EXIT_SUCCESS;
}
