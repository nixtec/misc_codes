#include <stdio.h>
#include <assert.h>
#include <simclist.h>

#undef NDEBUG

void stampa(list_t *l);

int main() {
    list_t tl, tl2, tl3;
    unsigned int num;
    int el, el2, i;
    list_hash_t lhash1, lhash2;


    list_init(&tl);
    assert(list_size(&tl) == 0);

    printf("1\n");
    
    lhash1 = list_hash(&tl);

    num = 50;
    list_attributes_copy(&tl, list_meter_int32_t, 1);
    list_attributes_comparator(&tl, list_comparator_int32_t);
    list_attributes_hash_computer(&tl, list_hashcomputer_int32_t);

    for (i = 0; i < num; i++) {
        list_append(&tl, &i);
    }
    assert(list_size(&tl) == num);

    /* min/max */
    printf("2\n");
    assert(*(int *)list_get_max(&tl) == num-1);
    printf("3\n");
    assert(*(int *)list_get_min(&tl) == 0);
    printf("4\n");

    /* incorrect deletion */
    assert(list_insert_at(&tl, &el, num+1) < 0);
    printf("5\n");
    assert(list_delete_at(&tl, num) < 0);
    printf("6\n");
    assert(list_delete_range(&tl, 0, num+1) < 0);
    printf("7\n");
    assert(list_delete_range(&tl, num, num/2) < 0);

    /* hashes */
    printf("8\n");
    lhash2 = list_hash(&tl);
    assert(lhash2 != lhash1);

    /* find and contains */
    printf("9\n");
    el = num-1;
    assert(list_contains(&tl, &el) && (list_find(&tl, &el) == list_size(&tl)-1) );

    /* sorting */
    printf("10\n");
    list_sort(&tl, 1);

    /* iteration sessions */
    el2 = el = -1;
    list_iterator_start(&tl);
    while (list_iterator_hasnext(&tl)) {
        el2 = *(int *)list_iterator_next(&tl);
        if (el > el2)
            break;
        el = el2;
    }
    list_iterator_stop(&tl);
    assert(el2 == num-1);

    /* legal deletion */
    printf("11\n");
    list_delete_range(&tl, 0, num/2);
    assert(list_size(&tl) == (num-1)/2);
    assert(*(int *)list_get_at(&tl, 0) == (num/2 +1));

    /* concatenation, and hashes */
    printf("12\n");
    lhash1 = list_hash(&tl);
    assert(lhash1 != lhash2);

    printf("13\n");
    list_init(&tl2);    /* tl2 empty */
    list_concat(&tl, &tl2, &tl3);
    list_attributes_hash_computer(&tl3, list_hashcomputer_int32_t);
    assert(list_hash(&tl) == list_hash(&tl3));

    printf("14\n");
    list_destroy(&tl3);
    list_concat(&tl2, &tl, &tl3);
    list_attributes_hash_computer(&tl3, list_hashcomputer_int32_t);
    assert(list_hash(&tl) == list_hash(&tl3));

    printf("15\n");
    list_delete_range(&tl3, 1, list_size(&tl3)-2);
    el = 123;
    list_append(&tl3, &el);
    assert(list_size(&tl3) == 3 && list_find(&tl3, &el) == (list_size(&tl3)-1));


    list_destroy(&tl);

    printf("==========\nAll tests passed.\n");
    return 0;
}

void stampa(list_t *l) {
    int i;

    for (i = 0; i < list_size(l); i++) {
        printf ("> %d\n", *(int *)list_get_at(l, i));
    }
}

