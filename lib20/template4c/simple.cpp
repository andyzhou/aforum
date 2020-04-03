/*
 * Example using template library to implement
 * a sample template engine class.
 *
 * This program uses the same template as
 * the sample written in C.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "tpllib.h"

/* Sample implementation as C++ class */
class Tpl
{
    public:
        Tpl();
        Tpl(const char* filename);
        Tpl(const Tpl& otherTpl);
        ~Tpl();
        int getStatus();
        void setField(const char *fieldName, int value);
        void setField(const char *fieldName, const char* value, int length);
        void setFieldGlobal(const char *fieldName, int value);
        void setFieldGlobal(const char *fieldName, const char* value, int length);
        void selectSection(const char *sectionName);
        void appendSection();
        void deselectSection();
        int getLength() const;
        void getContent(char *outputBuffer) const;
        int httpWrite(int fd) const;

    private:
        tpl_t theTpl;
        int status;
};

Tpl::Tpl()
{
    fputs("Error: the default constructor gives you nothing", stderr);
}

Tpl::Tpl(const char* filename)
{
    tpl_init(&theTpl);
    status = tpl_load(&theTpl, filename);

    if (status != TPL_OK)
        fprintf(stderr, "Error loading template file (%d)\n", status);
}

Tpl::Tpl(const Tpl& otherTpl)
{
    tpl_init(&theTpl);
    tpl_copy(&theTpl, &otherTpl.theTpl);
}

Tpl::~Tpl()
{
    tpl_release(&theTpl);
}

int Tpl::getStatus()
{
    return status;
}

void Tpl::setField(const char *fieldName, int value)
{
    tpl_set_field_int(&theTpl, fieldName, value);
}

void Tpl::setField(const char *fieldName, const char* value, int length)
{
    tpl_set_field(&theTpl, fieldName, value, length);
}

void Tpl::setFieldGlobal(const char *fieldName, int value)
{
    tpl_set_field_int_global(&theTpl, fieldName, value);
}

void Tpl::setFieldGlobal(const char *fieldName, const char* value, int length)
{
    tpl_set_field_global(&theTpl, fieldName, value, length);
}

void Tpl::selectSection(const char *sectionName)
{
    tpl_select_section(&theTpl, sectionName);
}

void Tpl::appendSection()
{
    tpl_append_section(&theTpl);
}

void Tpl::deselectSection()
{
    tpl_deselect_section(&theTpl);
}

int Tpl::getLength() const
{
    return tpl_length(&theTpl);
}

void Tpl::getContent(char *outputBuffer) const
{
    tpl_get_content(&theTpl, outputBuffer);
}

int Tpl::httpWrite(int fd) const
{
    return tpl_http_write(&theTpl, fd);
}

/*
 * Sample program loops through command line
 * arg strings and inserts each string into a
 * field in a table row, one string per row.
 */
int main(int argc, char **argv)
{
    Tpl tpl("simple.tpl");
    int i;

    if (tpl.getStatus() != TPL_OK)
        return EXIT_FAILURE;
    
    tpl.setFieldGlobal("ROWS", argc - 1);
    tpl.selectSection("ROW");
        
    for (i = 1; i < argc; i++)
    {
        int j = (i - 1) * 10;
        
        tpl.setField("TYPE", i % 2);    
        tpl.setField("FIELD1", i);
        tpl.setField("FIELD2", argv[i], strlen(argv[i]));
        tpl.setField("FIELD3", j + 3);
        tpl.setField("FIELD4", j + 4);
        tpl.setField("FIELD5", j + 5);
        tpl.setField("FIELD6", j + 6);
        tpl.setField("FIELD7", j + 7);
        
        tpl.appendSection();
    }
    tpl.deselectSection();

    char *buffer = new char[tpl.getLength() + 1];
    tpl.getContent(buffer);

    puts(buffer);

    
    return EXIT_SUCCESS;
}