#include "list.h"

extern LogFileData log_file;

#define CHECK_AND_RETURN(clause_, error_, ...)  if (clause_) {          \
                                                    res |= error_;      \
                                                    LIST_OK(list, res); \
                                                    __VA_ARGS__;        \
                                                    return res;         \
                                                }

int list_ctor(List* list) {
    assert(list);

    int res = list->OK;

    CHECK_AND_RETURN(list_is_initialised(list), list->ALREADY_INITIALISED);

    list->size = 0;

    return res | LIST_ASSERT(list);
}

int list_dtor(List* list) {
    int res = LIST_VERIFY(list);
    LIST_OK(list, res);

    list_clear(list);

    list->size = list->UNITIALISED_VAL;

    return res;
}

int list_find_by_logical_index(const List* list, ssize_t logical_i, ListNode** ptr) {
    assert(ptr);
    int res = LIST_ASSERT(list);

    CHECK_AND_RETURN(logical_i >= list->size || logical_i < 0, list->INVALID_PTR_GIVEN, {
                                                               *ptr = nullptr;});

    *ptr = list->head;
    ssize_t log_i = 0;
    LIST_FOREACH(*list, *ptr, log_i) {
        if (logical_i-- == 0) {
            return res;
        }
    }

    LIST_IS_FOREACH_VALID(*list, log_i, {
        res |= list->DAMAGED_PATH;
        LIST_OK(list, res);
    });

    *ptr = nullptr;

    return res;
}

int list_find_by_value(const List* list, const Elem_t elem, ListNode** ptr) {
    assert(ptr);
    int res = LIST_ASSERT(list);

    CHECK_AND_RETURN(elem == ListNode::POISON, list->POISON_VAL_FOUND, {
                                               *ptr = nullptr;});

    *ptr = list->head;
    ssize_t log_i = 0;
    LIST_FOREACH(*list, *ptr, log_i) {
        if ((*ptr)->elem == elem) {
            return res;
        }
    }

    LIST_IS_FOREACH_VALID(*list, log_i, {
        res |= list->DAMAGED_PATH;
        LIST_OK(list, res);
    });

    *ptr = nullptr;

    return res;
}

int list_logical_index_by_ptr(const List* list, const ListNode* ptr, ssize_t* logical_i) {
    assert(logical_i);
    int res = LIST_ASSERT(list);

    CHECK_AND_RETURN(ptr == nullptr, list->INVALID_PTR_GIVEN);

    ListNode* cur_ptr = list->head;
    ssize_t log_i = 0;
    LIST_FOREACH(*list, cur_ptr, log_i) {
        if (cur_ptr == ptr) {
            *logical_i = log_i;
            return res;
        }
    }

    LIST_IS_FOREACH_VALID(*list, log_i, {
        res |= list->DAMAGED_PATH;
        LIST_OK(list, res);
    });

    *logical_i = -1;

    return res;
}

#define CHECK_ERR_(clause, err) if (clause) res |= err

int list_verify(const List* list) {
    assert(list);

    int res = list->OK;

    CHECK_AND_RETURN(!list_is_initialised(list), list->UNITIALISED);

    CHECK_ERR_(list->size < 0, list->NEGATIVE_SIZE);

    ListNode* prev_ptr = nullptr;

    ListNode* ptr = list->head;
    ssize_t log_i = 0;
    LIST_FOREACH(*list, ptr, log_i) {
        if (!is_ptr_valid(ptr)) {
            res |= list->INVALID_NODE_PTR;
            res |= list->DAMAGED_PATH;
            break;
        }

        CHECK_ERR_(ptr->elem == ListNode::POISON, list->POISON_VAL_FOUND);
        CHECK_ERR_(ptr->prev != prev_ptr, list->DAMAGED_PATH);

        prev_ptr = ptr;
    }

    CHECK_ERR_(log_i != list->size, list->DAMAGED_PATH);

    return res;
}
#undef CHECK_ERR_

int list_insert_after(List* list, ListNode* ptr, const Elem_t elem, ListNode** inserted_ptr) {
    assert(inserted_ptr);
    int res = LIST_ASSERT(list);

    *inserted_ptr = (ListNode*)calloc(1, sizeof(ListNode));

    (*inserted_ptr)->prev = ptr;

    if (ptr == nullptr) {
        (*inserted_ptr)->next = list->head;

        if (list->head != nullptr)
            list->head->prev = *inserted_ptr;
    } else {
        (*inserted_ptr)->next = ptr->next;

        if (ptr->next != nullptr)
            ptr->next->prev = *inserted_ptr;

        ptr->next = *inserted_ptr;
    }

    if ((*inserted_ptr)->prev == nullptr)
        list->head = *inserted_ptr;

    if ((*inserted_ptr)->next == nullptr)
        list->tail = *inserted_ptr;

    (*inserted_ptr)->elem = elem;

    list->size++;

    return res | LIST_ASSERT(list);
}

int list_delete(List* list, ListNode* ptr) {
    int res = LIST_ASSERT(list);

    CHECK_AND_RETURN(ptr == nullptr, list->INVALID_PTR_GIVEN);

    if (ptr->next == nullptr)
        list->tail = ptr->prev;
    else
        (ptr->next)->prev = ptr->prev;

    if (ptr->prev == nullptr)
        list->head = ptr->next;
    else
        (ptr->prev)->next = ptr->next;

    ptr->elem = ListNode::POISON;

    FREE(ptr);

    list->size--;

    return res | LIST_ASSERT(list);
}

int list_clear(List* list) {
    int res = LIST_ASSERT(list);

    while (list->size > 0) {
        res |= list_delete(list, list->head);
        LIST_OK(list, res);
    }

    return res | LIST_ASSERT(list);
}

#ifdef DEBUG

int list_ctor_debug(List* list, const VarCodeData var_data) {
    assert(list);

    list->var_data = var_data;

    return list_ctor(list);
}

#endif //< #ifdef DEBUG


#undef CHECK_AND_RETURN
