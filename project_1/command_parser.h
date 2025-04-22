int num_args(char*input);

char** parse_command(char*input,int num_commands);

void free_parsed(char**parsed,int num_args);

void trim_trailing_whitespace(char *str);