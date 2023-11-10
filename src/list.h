#ifndef LIST_H_
#define LIST_H_

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <assert.h>

#include "utils/macros.h"
#include "log/log.h"
#include "utils/html.h"
#include "utils/ptr_valid.h"
#include "log/graph_log.h"

#define DEBUG

typedef int Elem_t;

#define ELEM_T_PRINTF "%d"

struct ListNode {
    static const Elem_t POISON = __INT_MAX__ - 13;  //< poison value

    ListNode* prev = nullptr;   //< previus element pointer

    Elem_t elem = POISON;       //< element value

    ListNode* next = nullptr;   //< next element pointer
};

// poison node
const ListNode POISON_LIST_NODE = {nullptr,
                                   ListNode::POISON,
                                   nullptr};

/**
 * @brief Specifies List data
 */
struct List {
    static const ssize_t UNITIALISED_VAL = -1;  //< default value

    // error codes
    enum Results {
        OK                   = 0x000000,
        ALLOC_ERR            = 0x000002,

        ALREADY_INITIALISED  = 0x000004,
        UNITIALISED          = 0x000008,
        INVALID_NODE_PTR     = 0x000010,
        POISON_VAL_FOUND     = 0x000020,
        NEGATIVE_SIZE        = 0x001000,
        INVALID_PTR_GIVEN    = 0x020000,
        DAMAGED_PATH         = 0x040000,
    };

    ListNode* head = nullptr;   //< List head pointer
    ListNode* tail = nullptr;   //< List tail pointer

    ssize_t size     = UNITIALISED_VAL;     //< number of elements in list

#ifdef DEBUG
    VarCodeData var_data;   //< keeps data about list variable (name, file, line number)
#endif // #ifdef DEBUG

};

/**
 * @brief Returns true if list was initialised
 *
 * @param list
 * @return true
 * @return false
 */
inline bool list_is_initialised(const List* list) {
    return !(list->size == list->UNITIALISED_VAL &&
             list->head == nullptr && list->tail == nullptr);
}

#define LIST_FOREACH(list_, ptr_, log_i_)                                               \
    for (; ptr_ != nullptr && log_i_ <= (list_).size; ptr_ = (ptr_)->next, (log_i_)++)

#define LIST_IS_FOREACH_VALID(list_, log_i_, ...)   do {    \
            if (log_i_ != (list_).size) {                   \
                __VA_ARGS__;                                \
            }                                               \
        } while (0)

/**
 * @brief (Use macros LIST_CTOR) List constructor
 *
 * @param list
 * @return int
 */
int list_ctor(List* list);

/**
 * @brief List destructor
 *
 * @param list
 * @return int
 */
int list_dtor(List* list);

/**
 * @brief Inserts element after ptr
 *
 * @param list
 * @param ptr
 * @param elem
 * @param inserted_ptr
 * @return int
 */
int list_insert_after(List* list, ListNode* ptr, const Elem_t elem, ListNode** inserted_ptr);

/**
 * @brief Inserts element before ptr
 *
 * @param list
 * @param ptr
 * @param elem
 * @param inserted_ptr
 * @return int
 */
inline int list_insert_before(List* list, ListNode* ptr, const Elem_t elem, ListNode** inserted_ptr) {
    return list_insert_after(list, ptr->prev, elem, inserted_ptr);
}

/**
 * @brief Inserts element at the end of the list
 *
 * @param list
 * @param elem
 * @param inserted_ptr
 * @return int
 */
inline int list_pushback(List* list, const Elem_t elem, ListNode** inserted_ptr) {
    return list_insert_after(list, list->tail, elem, inserted_ptr);
}

/**
 * @brief Inserts element at the beginning of the list
 *
 * @param list
 * @param elem
 * @param inserted_ptr
 * @return int
 */
inline int list_pushfront(List* list, const Elem_t elem, ListNode** inserted_ptr) {
    return list_insert_before(list, list->head, elem, inserted_ptr);
}

/**
 * @brief Deletes element by physical index
 *
 * @param list
 * @param ptr
 * @return int
 */
int list_delete(List* list, ListNode* ptr);

/**
 * @brief Deletes all elements
 *
 * @param list
 * @return int
 */
int list_clear(List* list);

/**
 * @brief (Use macros LIST_VERIFY) Verifies list data and fields
 *
 * @param list
 * @return int
 */
int list_verify(const List* list);

/**
 * @brief Returns ptr to element with given value (the first one)
 *
 * @param list
 * @param elem
 * @param ptr returnable value. nullptr if not found
 * @return int
 */
int list_find_by_value(const List* list, const Elem_t elem, ListNode** ptr);

/**
 * @brief Returns logical index of element with specified ptr
 *
 * @param list
 * @param ptr
 * @param logical_i returnable value. -1 if not found
 * @return int
 */
int list_logical_index_by_ptr(const List* list, const ListNode* ptr, ssize_t* logical_i);

/**
 * @brief Returns ptr to element with specified logical index
 *
 * @param list
 * @param logical_i
 * @param ptr returnable value. nullptr if not found
 */
int list_find_by_logical_index(const List* list, ssize_t logical_i, ListNode** ptr);


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

#ifdef DEBUG

    /**
     * @brief (Use macros LIST_CTOR or LIST_CTOR_CAP) Constructor wrapper for debug mode
     *
     * @param list
     * @param var_data
     * @return int
     */
    int list_ctor_debug(List* list, const VarCodeData var_data);

    /**
     * @brief Constructor
     *
     * @param list
     */
    #define LIST_CTOR(list) list_ctor_debug(list, VAR_CODE_DATA_PTR(list));

    /**
     * @brief Verifies list data and fields
     *
     * @param list
     */
    #define LIST_VERIFY(list) list_verify(list)

    /**
     * @brief List assert macros (LIST_VERIFY, LIST_OK, and if not ok - return)
     *
     * @param list
     */
    #define LIST_ASSERT(list)       \
                LIST_VERIFY(list);  \
                LIST_OK(list, res); \
                                    \
                if (res != list->OK)\
                    return res

    /**
     * @brief Checks if res is OK, if not, prints error and dump
     *
     * @param list
     * @param res
     */
    #define LIST_OK(list, res)   do {                           \
                                    if (res != list->OK) {      \
                                        list_print_error(res);  \
                                        LIST_DUMP(list);        \
                                    }                           \
                                } while (0)

    /**
     * @brief Prints list dump to log
     *
     * @param list
     */
    #define LIST_DUMP(list) list_dump(list, VAR_CODE_DATA())

#else //< #ifndef DEBUG

    /**
     * @brief Constructor
     *
     * @param list
     */
    #define LIST_CTOR(list) list_ctor(list);

    /**
     * @brief Verifies list data and fields (enabled only in DEBUG mode)
     *
     * @param list
     */
    #define LIST_VERIFY(list) List::OK

    /**
     * @brief List assert macros (enabled only in DEBUG mode)
     *
     * @param list
     */
    #define LIST_ASSERT(list) 0

    /**
     * @brief Checks if res is OK (enabled only in DEBUG mode)
     *
     * @param list
     * @param res
     */
    #define LIST_OK(list, res) (void) 0

    /**
     * @brief Prints list dump to log (enabled only in DEBUG mode)
     *
     * @param list
     */
    #define LIST_DUMP(list) (void) 0

#endif //< #ifdef DEBUG

#endif //< #ifndef LIST_H_
