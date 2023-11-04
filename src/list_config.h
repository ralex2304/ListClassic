#ifndef LIST_CONFIG_
#define LIST_CONFIG_

#include <ctype.h>

#include "utils/macros.h"

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


#endif //<
