#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

typedef struct
{
    char *buffer;
    size_t buffer_length; // size_t is guaranteed to hold any array index
    ssize_t input_length;
} InputBuffer;

typedef enum
{
    META_COMMAND_SUCCESS,
    META_COMMAND_UNRECOGNIZED_COMMAND
} MetaCommandResult;
typedef enum
{
    STATEMENT_INSERT,
    STATEMENT_SELECT
} StatementType;

typedef struct
{
    StatementType type;
} Statement;

typedef enum
{
    PREPARE_SUCCESS,
    PREPARE_UNRECOGNISED_STATEMENT
} PrepareResult;

InputBuffer *new_input_buffer()
{
    InputBuffer *input_buffer = (InputBuffer *)malloc(sizeof(InputBuffer));
    input_buffer->buffer = NULL;
    input_buffer->buffer_length = 0;
    input_buffer->input_length = 0;
    return input_buffer;
}

PrepareResult prepare_statement(InputBuffer *input_buffer, Statement *statement)
{
    // use strncmp as we only want to compare the first n bytes,
    // normally insert is followed by other words
    if (strncmp(input_buffer->buffer, "insert", 6) == 0)
    {
        statement->type = STATEMENT_INSERT;
        return PREPARE_SUCCESS;
    }
    if (strcmp(input_buffer->buffer, "select") == 0)
    {
        statement->type = STATEMENT_SELECT;
        return PREPARE_SUCCESS;
    }
    return PREPARE_UNRECOGNISED_STATEMENT;
}
void execute_statement(Statement *statement)
{
    switch (statement->type)
    {
    case (STATEMENT_INSERT):
        printf("Perform Insert action\n");
        break;
    case (STATEMENT_SELECT):
        printf("Perform Select action\n");
        break;
    }
}

void print_prompt()
{
    printf("db > ");
}
void read_input(InputBuffer *input_buffer)
{
    ssize_t bytes_read_in = getline(&(input_buffer->buffer), &(input_buffer->buffer_length), stdin);
    if (bytes_read_in <= 0)
    {
        printf("Error reading input\n");
        exit(EXIT_FAILURE);
    }
    // remove new line at end of the prompt
    input_buffer->buffer_length = bytes_read_in - 1;
    input_buffer->buffer[bytes_read_in - 1] = 0;
}
void close_input_buffer(InputBuffer *input_buffer)
{
    free(input_buffer->buffer);
    free(input_buffer);
}
MetaCommandResult do_meta_command(InputBuffer *input_buffer)
{
    if (strcmp(input_buffer->buffer, ".exit") == 0)
    {
        close_input_buffer(input_buffer);
        exit(EXIT_SUCCESS);
    }
    else
    {
        return META_COMMAND_UNRECOGNIZED_COMMAND;
    }
}

int main(int argc, char *argv[])
{
    InputBuffer *input_buffer = new_input_buffer();
    while (true)
    {
        print_prompt();
        read_input(input_buffer);
        if (input_buffer->buffer[0] == '.')
        {
            switch (do_meta_command(input_buffer))
            {
            case META_COMMAND_SUCCESS:
                continue;

            case META_COMMAND_UNRECOGNIZED_COMMAND:
                printf("Unrecognised command %s\n", input_buffer->buffer);
                continue;
            }
        }
        Statement statement;
        switch (prepare_statement(input_buffer, &statement))
        {
        case (PREPARE_SUCCESS):
            break;
        case (PREPARE_UNRECOGNISED_STATEMENT):
            printf("Unrecognised keyword at start of: %s\n", input_buffer->buffer);
            continue;
        }
        execute_statement(&statement);
    }
}
