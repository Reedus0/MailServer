#include <stdio.h>
#include "list.h"
#include "func.h"
#include "make.h"

int test_list_insert() {
    struct list first_list = init_list();
    struct list second_list = init_list();

    list_insert(&first_list, &second_list);

    if (ptr_except_eq(first_list.next, &second_list)) {
        printf("%s OK\n", __func__);
        return 1;
    }
    printf("%s NOT OK\n", __func__);
    return 0;
}

int test_list_remove_last() {
    struct list first_list = init_list();
    struct list second_list = init_list();

    list_insert(&first_list, &second_list);
    list_remove(&second_list);

    if (ptr_except_eq(first_list.next, NULL)) {
        printf("%s OK\n", __func__);
        return 1;
    }
    printf("%s NOT OK\n", __func__);
    return 0;
}

int test_list_remove_center() {
    struct list first_list = init_list();
    struct list second_list = init_list();
    struct list third_list = init_list();

    list_insert(&first_list, &second_list);
    list_insert(&second_list, &third_list);
    list_remove(&second_list);

    if (ptr_except_eq(first_list.next, &third_list) && ptr_except_eq(third_list.prev, &first_list)) {
        printf("%s OK\n", __func__);
        return 1;
    }
    printf("%s NOT OK\n", __func__);
    return 0;
}