struct command {
    char** arg_array;
    int arg_count;
};

int count_args(char*input);

struct command create_command(char*input);

void free_command(struct command c1);

int count_lines(char* filename);

struct command* read_file(char* filename,int num_lines);

void free_command_array(struct command* command_array, int line_count);
