#include "graph_log.h"

bool create_img(const char* input_filename, const char* output_filename) {
    assert(input_filename);
    assert(output_filename);

    static const size_t MAX_COMMAND_LEN = 256;
    char command[MAX_COMMAND_LEN] = {};

    strcat(command, "dot -Tsvg ");
    strcat(command, input_filename);
    strcat(command, " > ");
    strcat(command, output_filename);

    if (system(command) != 0)
        return false;

    return true;
}
