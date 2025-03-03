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
    element_t *cur_elem =                                                 \
        (strcmp(pos, "head"))   ? list_first_entry(head, element_t, list) \
        : (strcmp(pos, "tail")) ? list_last_entry(head, element_t, list)  \
                                : NULL;                                   \
    list_del(&cur_elem->list);                                            \
    if (cur_elem && sp) {                                                 \
        size_t temp = strlen(cur_elem->value);                            \
        strncpy(sp, cur_elem->value, temp + 1);                           \
        sp[temp + 1] = '\0';                                              \
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
    struct list_head *prev = head->prev;
    q_insert_new(prev, s, "tail");
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
    struct list_head **indir = &head, *fast = head;

    for (; fast->next != head && fast->next->next != head;
         fast = fast->next->next)
        indir = &(*indir)->next;
    struct list_head *del = *indir;
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
}

/* Sort elements of queue in ascending/descending order */
void q_sort(struct list_head *head, bool descend) {}

/* Remove every node which has a node with a strictly less value anywhere to
 * the right side of it */
int q_ascend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    return 0;
}

/* Remove every node which has a node with a strictly greater value anywhere to
 * the right side of it */
int q_descend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    return 0;
}

/* Merge all the queues into one sorted queue, which is in ascending/descending
 * order */
int q_merge(struct list_head *head, bool descend)
{
    // https://leetcode.com/problems/merge-k-sorted-lists/
    return 0;
}
