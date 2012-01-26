#ifndef _PIH_LIB_
#define _PIH_LIB_ 1

#include <stdlib.h>

typedef enum {
    OUT_NONE = 0,
    OUT_NORMAL = 1,
    OUT_DEBUG = 2,
    OUT_SHOW_ALL = 3
} out_level_t;

typedef struct __cell *cell_t;
typedef struct __list *list_t;
typedef struct __hash_table *hash_table_t;
typedef struct __iterator *iterator_t;
typedef struct __string_buffer *string_buffer_t;
typedef struct __shared_ptr *shared_ptr_t;

struct __list
{
	size_t size;
	cell_t first;
};


struct __hash_table
{
	size_t size;  // Qtd de listas na hash table
	size_t count; // Qtd de elementos guardados na hash table
	// These arent used in searches
	list_t keys;
	list_t *elems;
};

struct __cell
{
	char *id;
	void *data;
	cell_t next;
};

struct __iterator
{
	cell_t actual;
	list_t list;
	int    (*has_next) (iterator_t);
	void*  (*next)     (iterator_t);
};

struct __string_buffer
{
	list_t strings;
	int total_length;
};

struct __shared_ptr
{
      list_t refs;
      void *ptr;
};


void setlevel(out_level_t level);
void print_message(out_level_t show_level, char *fmt, ...);

int min_length(char *s1, char *s2);
int max_length(char *s0, char *s2);

list_t read_file_list(char *filelist_name);

list_t list_create();
list_t list_clone(list_t list);
void* list_add(const list_t, char *id, void *data);
void* list_push(const list_t, char *, void *);
cell_t list_pop(const list_t);
list_t list_union(list_t list_a, list_t list_b);
iterator_t list_iterator(const list_t list);
iterator_t list_iterator_continues(const iterator_t iterator);
void *list_get_by_pos(const list_t list, const size_t pos);
void list_iterator_destroy(iterator_t *iterator);
iterator_t list_iterator_reset(const iterator_t iterator);
void* list_remove_by_id(list_t list, const char *id);
void list_destroy(list_t *list);
void list_destroy_all(list_t *list, int (*destroy_item)(void *));

void list_print(list_t list, void (*print)(char *, void *));
list_t list_sort(const list_t list, int (*comparer)(const void *, const void *));
list_t list_sort_by_data(const list_t list, int (*comparer)(const void *, const void *));
list_t list_cartesian_product(list_t list);
cell_t list_search(const list_t list, const char *id);

int __list_has_next(const iterator_t iterator);
void *__list_next(const iterator_t iterator);

hash_table_t hash_table_create();
void* hash_table_add(hash_table_t, char *, void *);
void* hash_table_get(hash_table_t hash_table, char *id);
void* hash_table_remove_by_id(hash_table_t hash_table, const char *id);
iterator_t hash_table_iterator(hash_table_t hash_table);
void hash_table_print(hash_table_t hash_table, void (*print_func)(char *, void *));
hash_table_t hash_table_clone(hash_table_t hash_table);
hash_table_t hash_table_clone_all(hash_table_t hash_table, void* (*clone_item)(void *));
hash_table_t hash_table_destroy(hash_table_t *hash_table);
int hash_table_destroy_all(hash_table_t *hash_table, int (*destroy_item)(void *));

list_t read_directory(const char *path, list_t filters);
char* get_directory(const char *full_path);
char* get_file_name(const char *filename);
char* get_file_extension(const char *filename);
char *get_file_from_dir(const char *full_path);

string_buffer_t string_buffer_create();
string_buffer_t string_buffer_add(string_buffer_t sb, char *fmt, ...);
char *string_buffer_to_string(string_buffer_t sb);

void print_dummy(char *id, void* data);
void print_data_char(char *id, void* data);

int compare_doubles (const double *da, const double *db);
int compare_strings (const void *va, const void *vb);

int destroy_string(void *s);

size_t combination(size_t l_n, size_t l_k);

shared_ptr_t shared_ptr_create(void *data, void* ref);
void shared_ptr_inc_ref(shared_ptr_t shared_ptr, void *ref);
void shared_ptr_rem_ref(shared_ptr_t *shared_ptr, void *ref);
void shared_ptr_destroy(shared_ptr_t *shared_ptr);

#endif
