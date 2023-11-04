#ifndef DUMP_H_
#define DUMP_H_

#include "list_config.h"
#include "utils/macros.h"
#include "log/log.h"
#include "log/graph_log.h"
#include "utils/html.h"
#include "utils/ptr_valid.h"

/**
 * @brief (Use LIST_DUMP macros) Dumps list data to log
 *
 * @param list
 * @param call_data
 */
void list_dump(const List* list, const VarCodeData call_data);

/**
 * @brief Dumps list array to dot file
 *
 * @param list
 * @param img_filename returns image filename
 * @return true
 * @return false
 */
bool list_dump_dot(const List* list, char* img_filename);

/**
 * @brief Prints text error to log by error code
 *
 * @param err_code
 */
void list_print_error(const int err_code);

#endif //< #ifndef DUMP_H_
