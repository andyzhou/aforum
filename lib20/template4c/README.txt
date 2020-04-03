Template engine library for C/C++ programs.


Concept
________________________________________________________________________________

The idea is to allow creation of clean HTML templates that can be validated
using W3C validator.

A template is made up of text and fields (i.e. placeholders). The template can
contain sections.

Sections are considered templates themselves and may contain text, fields and
sections.


Markup example
________________________________________________________________________________

Field markup:
    @@SAMPLE_FIELD_NAME@@

Section markup:
    <!-- @@SAMPLE_SECTION_NAME@@ -->
    Row number @@NUMBER@@ with some really interesting text
    <!-- @@SAMPLE_SECTION_NAME@@ -->

NOTE: the markup can be changed by changing the defines in the header file.


Usage
________________________________________________________________________________

The following steps would normally be used (refer to markup example above):

1) allocate/initialize a template:
    tpl_t my_tpl;
    tpl_init(&my_tpl);

2) load the template from a file:
    int status = tpl_load(&my_tpl, "template_file_name.tpl");

3) set some fields:
    tpl_set_field(&my_tpl, "SAMPLE_FIELD_NAME", "foo", strlen("foo"));

4) use section operations to create multiple variations of the same content:
    tpl_select_section(&my_tpl, "SAMPLE_SECTION_NAME");
    for (i = 0; i < 10; i++)
    {
        tpl_set_field_int(&my_tpl, "NUMBER", i);
        tpl_append_section(&my_tpl);
    }
    tpl_deselect_section(&my_tpl);

5) get the content length (like strlen()):
    int content_length = tpl_length(&my_tpl);

6) allocate memory and get the content (similar to strcpy()):
    char *content_buffer = malloc(content_length + 1);
    tpl_get_content(&my_tpl, content_buffer);
    do something with the content (print, save to file, or whatever)

7) release memory used for the template
    tpl_release(&my_tpl);


See the programs and templates included in the samples directory.

Read the function prototypes and comments in the header file to see what
other functions are available.

