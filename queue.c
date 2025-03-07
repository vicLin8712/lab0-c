#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "queue.h"

/* Notice: sometimes, Cppcheck would find the potential NULL pointer bugs,
 * but some of them cannot occur. You can suppress them by adding the
 * following line.
 *   cppcheck-suppress nullPointer
 */
#define q_insert_new(head, s, pos)                                     \
    if (!head)                                                         \
        return false;                                                  \
    element_t *new = test_malloc(sizeof(element_t));                   \
    if (!new)                                                          \
        return false;                                                  \
    new->value = (char *) test_malloc((strlen(s) + 1) * sizeof(char)); \
    if (!new->value) {                                                 \
        q_release_element(new);                                        \
        return false;                                                  \
    }                                                                  \
    strncpy(new->value, s, strlen(s) + 1);                             \
    !strcmp(pos, "head") ? list_add(&new->list, head)                  \
                         : list_add_tail(&new->list, head);            \
    return true;


#define elem_remove(head, sp, bufsize, pos)                               \
    if (!head || list_empty(head)) {                                      \
        return NULL;                                                      \
    }                                                                     \
    element_t *cur_elem = (!strcmp(pos, "head"))                          \
                              ? (list_first_entry(head, element_t, list)) \
                              : (list_last_entry(head, element_t, list)); \
    list_del(&cur_elem->list);                                            \
    if (cur_elem && sp) {                                                 \
        size_t temp = strlen(cur_elem->value);                            \
        if (temp > bufsize - 1)                                           \
            temp = bufsize - 1;                                           \
        strncpy(sp, cur_elem->value, temp);                               \
        sp[temp] = '\0';                                                  \
    }                                                                     \
    return cur_elem;

#define elem_del(elem)     \
    list_del(&elem->list); \
    q_release_element(elem);

/* Create an empty queue */
struct list_head *q_new()
{
    struct list_head *head = malloc(sizeof(struct list_head));
    if (!head)
        return NULL;
    INIT_LIST_HEAD(head);
    return head;
}

/* Free all storage used by queue */
void q_free(struct list_head *head)
{
    if (!head)
        return;
    element_t *entry, *safe;
    list_for_each_entry_safe (entry, safe, head, list) {
        q_release_element(entry);
    }
    free(head);
}

/* Insert an element at head of queue */
bool q_insert_head(struct list_head *head, char *s)
{
    q_insert_new(head, s, "head");
}

/* Insert an element at tail of queue */
bool q_insert_tail(struct list_head *head, char *s)
{
    q_insert_new(head, s, "tail");
}

/* Remove an element from head of queue */
element_t *q_remove_head(struct list_head *head, char *sp, size_t bufsize)
{
    elem_remove(head, sp, bufsize, "head");
}

/* Remove an element from tail of queue */
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    elem_remove(head, sp, bufsize, "tail");
}

/* Return number of elements in queue */
int q_size(struct list_head *head)
{
    if (!head)
        return 0;

    int len = 0;
    struct list_head *li;

    list_for_each (li, head)
        len++;
    return len;
}

/* Delete the middle node in queue */
bool q_delete_mid(struct list_head *head)
{
    if (!head || list_is_singular(head))
        return NULL;
    struct list_head *indir = head, *fast = indir->next;

    for (; fast->next != head && fast->next->next != head;
         fast = fast->next->next)
        indir = indir->next;
    struct list_head *del = indir->next;
    list_del(del);
    q_release_element(list_entry(del, element_t, list));
    // https://leetcode.com/problems/delete-the-middle-node-of-a-linked-list/
    return true;
}

/* Delete all nodes that have duplicate string */
bool q_delete_dup(struct list_head *head)
{
    // https://leetcode.com/problems/remove-duplicates-from-sorted-list-ii/
    if (!head || list_empty(head))
        return NULL;
    element_t *entry, *safe;
    bool dup = false;
    list_for_each_entry_safe (entry, safe, head, list) {
        if (entry->list.next != head &&
            !strcmp(entry->value,
                    list_entry(entry->list.next, element_t, list)->value)) {
            dup = true;
            elem_del(entry);
        } else if (dup) {
            elem_del(entry);
            dup = false;
        }
    }
    return true;
}

/* Swap every two adjacent nodes */
void q_swap(struct list_head *head)
{
    // https://leetcode.com/problems/swap-nodes-in-pairs/
    if (!head || list_is_singular(head) || list_empty(head))
        return;
    for (struct list_head *curr = head->next;
         curr != head && curr->next != head; curr = curr->next)
        list_move(curr->next, curr->prev);
}

/* Reverse elements in queue */
void q_reverse(struct list_head *head)
{
    if (!head || list_is_singular(head) || list_empty(head))
        return;
    struct list_head *node, *safe;
    list_for_each_safe (node, safe, head)
        list_move(node, head);
}

/* Reverse the nodes of the list k at a time */
void q_reverseK(struct list_head *head, int k)
{
    // https://leetcode.com/problems/reverse-nodes-in-k-group/
    if (!head || list_empty(head) || k <= 1)
        return;
    if (k == 2) {
        q_swap(head);
        return;
    }
    struct list_head *dummy_head, *dummy_end, *safe;
    struct list_head temp;
    int count = k - 1;
    for (dummy_end = (head)->next, safe = dummy_end->next, dummy_head = head;
         dummy_end != head; dummy_end = safe, safe = dummy_end->next, --count) {
        if (count == 0) {
            list_cut_position(&temp, dummy_head, dummy_end);
            q_reverse(&temp);
            list_splice_init(&temp, dummy_head);
            dummy_head = safe->prev;
            count = k;
        }
    }
}


void merge(struct list_head *head,
           struct list_head *left,
           struct list_head *right,
           bool descend)
{
    while (!list_empty(left) && !list_empty(right)) {
        element_t const *l = list_first_entry(left, element_t, list);
        element_t const *r = list_first_entry(right, element_t, list);
        if (((descend * 2) - 1) * strcmp(l->value, r->value) >= 0) {
            list_move_tail(left->next, head);
        } else {
            list_move_tail(right->next, head);
        }
    }
    if (!list_empty(left))
        list_splice_tail_init(left, head);
    else
        list_splice_tail_init(right, head);
}
/* Sort elements of queue in ascending/descending order */
void q_sort(struct list_head *head, bool descend)
{
    if (!head || list_empty(head) || list_is_singular(head))
        return;
    struct list_head *slow = head->next, *fast = head->next->next;
    while (fast != head && fast->next != head) {
        slow = slow->next;
        fast = fast->next->next;
    }
    struct list_head l, r;
    INIT_LIST_HEAD(&l);
    INIT_LIST_HEAD(&r);
    list_cut_position(&l, head, slow);
    list_splice_init(head, &r);
    q_sort(&l, descend);
    q_sort(&r, descend);
    merge(head, &l, &r, descend);
}
/* Remove every node which has a node with a strictly less value anywhere to
 * the right side of it */
int q_ascend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    if (!head || list_empty(head) || list_is_singular(head))
        return 0;
    int size = 0;
    element_t *entry, *safe;
    element_t const *max = list_first_entry(head, element_t, list);
    list_for_each_entry_safe (entry, safe, head, list) {
        if (strcmp(entry->value, max->value) <= 0 &&
            entry != list_first_entry(head, element_t, list)) {
            list_del(&entry->list);
            q_release_element(entry);
        } else {
            size++;
            max = entry;
        }
    }
    return size;
}

/* Remove every node which has a node with a strictly greater value anywhere to
 * the right side of it */
int q_descend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    q_reverse(head);
    int size = q_ascend(head);
    q_reverse(head);
    return size;
}

/* Merge all the queues into one sorted queue, which is in ascending/descending
 * order */
int q_merge(struct list_head *head, bool descend)
{
    if (!head || list_empty(head))
        return 0;
    int len = 0;
    queue_contex_t *first_q = list_entry(head->next, queue_contex_t, chain);
    struct list_head *tmp, *tmphead = head->next;
    list_for_each (tmp, tmphead) {
        tmphead = head;
        queue_contex_t *another_q = list_entry(tmp, queue_contex_t, chain);
        len += another_q->size;
        LIST_HEAD(tmp_head);
        merge(&tmp_head, first_q->q, another_q->q, descend);
        list_splice(&tmp_head, first_q->q);
    }
    return len;
}
