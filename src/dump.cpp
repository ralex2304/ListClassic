#include "list.h"

extern LogFileData log_file;

#define LOG_(...) log_printf(&log_file, __VA_ARGS__)

#ifdef DEBUG

void list_dump(const List* list, const VarCodeData call_data) {
    assert(list);

    LOG_(HTML_BEGIN);

    LOG_("    list_dump() called from %s:%d %s\n"
         "    %s[%p] initialised in %s:%d %s \n",
         call_data.file, call_data.line, call_data.func,
         list->var_data.name, list,
         list->var_data.file, list->var_data.line, list->var_data.func);

    LOG_("    {\n");
    LOG_("    size           = %zd\n", list->size);
    LOG_("    head           = %p\n",  list->head);
    LOG_("    tail           = %p\n",  list->tail);
    LOG_("        {\n");

    if (!is_ptr_valid(list->head)) {
        if (!list_is_initialised(list))
            LOG_(HTML_RED("        can't read (invalid pointer)\n"));

        LOG_("        }\n"
             "    }\n" HTML_END);
        return;
    }

    LOG_("        "" %*s | %*s | %*s | elem\n", -14, "ptr", -14, "prev", -14, "next");

    ListNode* ptr = list->head;
    ssize_t log_i = 0;
    LIST_FOREACH(*list, ptr, log_i) {
        LOG_("        "" %14p | %14p | %14p | " ELEM_T_PRINTF "\n",
                         ptr, ptr->prev, ptr->next, ptr->elem);
    }

    LOG_("        }\n"
         "    }\n" HTML_END);

    char img_filename[log_file.MAX_FILENAME_LEN] = {};

    list_dump_dot(list, img_filename);

    LOG_("<img src=\"../../%s\">\n", img_filename);
}
#undef LOG_

#define FPRINTF_(...) if (fprintf(file, __VA_ARGS__) == 0) return false
bool list_dump_dot(const List* list, char* img_filename) {
    #define BACKGROUND_COLOR "\"#1f1f1f\""
    #define FONT_COLOR       "\"#000000\""
    #define NODE_PREFIX      "elem_"
    #define NODE_PARAMS      "shape=\"record\", style=\"filled\", fillcolor=\"#6e7681\""
    #define ZERO_NODE_PARAMS "shape=\"record\", style=\"filled\", fillcolor=\"#6e7681\", color=yellow"

    assert(list);

    static size_t dot_number = 0;

    char dot_filename[log_file.MAX_FILENAME_LEN] = {};

    size_t str_len = strncat_len(dot_filename, log_file.timestamp_dir, log_file.MAX_FILENAME_LEN);
    snprintf(dot_filename + str_len, log_file.MAX_FILENAME_LEN - str_len,
             "%zd", dot_number);
    str_len = strncat_len(dot_filename, ".dot", log_file.MAX_FILENAME_LEN);

    FILE* file = fopen(dot_filename, "wb");
    if (file == nullptr)
        return false;

    FPRINTF_("digraph List{\n"
             "    graph [bgcolor=" BACKGROUND_COLOR "];\n"
             "    node[color=white, fontcolor=" FONT_COLOR ", fontsize=14];\n");

    FPRINTF_(NODE_PREFIX "zero [" ZERO_NODE_PARAMS ", label=\" head = %p | "
                                                           "tail = %p\"];\n",
             list->head, list->tail);

    ListNode* ptr = list->head;
    ssize_t log_i = 0;

    LIST_FOREACH(*list, ptr, log_i) {
        FPRINTF_(NODE_PREFIX "%zd [" NODE_PARAMS ", label=\" <p>prev = %p | {<i>ptr = %p |", log_i, ptr->prev, ptr);

        if (ptr->elem == ListNode::POISON) {
            FPRINTF_("<e>elem = PZN} | ");
        } else {
            FPRINTF_("<e>elem = " ELEM_T_PRINTF "} | ", ptr->elem);
        }

        FPRINTF_("<n>next = %p}\"", ptr->next);

        FPRINTF_("];\n");
    }

    FPRINTF_(NODE_PREFIX "zero");
    for (log_i = 0; log_i < list->size; log_i++) {
        FPRINTF_("->" NODE_PREFIX "%zd", log_i);
    }
    FPRINTF_(" [weight=10000, color=transparent, arrowtail=none];\n");

    ptr = list->head;
    log_i = 0;

    LIST_FOREACH(*list, ptr, log_i) {
        if (ptr->next != nullptr)
            FPRINTF_(NODE_PREFIX "%zd:<n>->" NODE_PREFIX "%zd:<n> [color=green];\n", log_i, log_i + 1);

        if (ptr->prev != nullptr)
            FPRINTF_(NODE_PREFIX "%zd:<p>->" NODE_PREFIX "%zd:<p> [color=blue];\n", log_i, log_i - 1);
    }

    FPRINTF_("head [shape=rect, label=\"HEAD\", color=yellow, fillcolor=\"#7293ba\",style=filled];\n");
    FPRINTF_("tail [shape=rect, label=\"TAIL\", color=yellow, fillcolor=\"#7293ba\",style=filled];\n");

    FPRINTF_("head->tail[weight=100, color=transparent];");

    FPRINTF_("{rank=same; head; " NODE_PREFIX "0}\n");
    FPRINTF_("{rank=same; tail; " NODE_PREFIX "%zd}\n", list->size - 1);

    FPRINTF_("}\n");

    if (fclose(file) != 0) {
        perror("Error closing file");
        return false;
    }

    str_len = strncat_len(img_filename, log_file.timestamp_dir, log_file.MAX_FILENAME_LEN);
    snprintf(img_filename + str_len, log_file.MAX_FILENAME_LEN - str_len, "%zd", dot_number++);
    str_len = strncat_len(img_filename, ".svg", log_file.MAX_FILENAME_LEN);

    if (!create_img(dot_filename, img_filename)) {
        fprintf(stderr, "Error creating dot graph\n");
        return false;
    }

    return true;
}
#undef FPRINTF_

#endif //< #ifdef DEBUG

#define PRINT_ERR_(code, descr)  if ((err_code) & List::code)                                       \
                                    log_printf(&log_file,                                           \
                                               HTML_TEXT(HTML_RED("!!! " #code ": " descr "\n")));
void list_print_error(const int err_code) {
    if (err_code == List::OK) {
        log_printf(&log_file, HTML_TEXT(HTML_GREEN("No error\n")));
    } else {
        PRINT_ERR_(ALREADY_INITIALISED, "Constructor called for already initialised or corrupted list");
        PRINT_ERR_(UNITIALISED,         "List is not initialised");
        PRINT_ERR_(INVALID_NODE_PTR,    "ListNode pointer is not valid for writing");
        PRINT_ERR_(ALLOC_ERR,           "Can't allocate memory");
        PRINT_ERR_(POISON_VAL_FOUND,    "There is poison value in list");
        PRINT_ERR_(NEGATIVE_SIZE,       "Negative list.size");
        PRINT_ERR_(INVALID_PTR_GIVEN,   "Invalid pointer given");
        PRINT_ERR_(DAMAGED_PATH,        "List is damaged. Invalid path");
    }
}
#undef PRINT_ERR_
