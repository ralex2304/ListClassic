#include "log/log.h"
#include "list.h"

LogFileData log_file = {"log"};

int main() {
    log_open_file(&log_file, "wb");

    List list = {};
    LIST_CTOR(&list);

    ListNode* inserted = nullptr;
    LIST_DUMP(&list);

    list_insert_after(&list, nullptr, 0, &inserted);
    list_insert_after(&list, inserted, 10, &inserted);
    ListNode* middle = inserted;
    list_insert_after(&list, inserted, 20, &inserted);

    LIST_DUMP(&list);

    list_insert_after(&list, middle, 15, &inserted);

    LIST_DUMP(&list);

    list_insert_after(&list, middle, 20, &inserted);

    LIST_DUMP(&list);

    list_insert_after(&list, middle, 50, &inserted);
    list_insert_after(&list, middle, 60, &inserted);
    list_insert_after(&list, middle, 70, &inserted);
    list_insert_after(&list, inserted, 80, &inserted);
    list_insert_after(&list, inserted, 90, &inserted);
    list_pushfront(&list, -10, &inserted);

    LIST_DUMP(&list);

    list_dtor(&list);

    log_close_file(&log_file);
}
