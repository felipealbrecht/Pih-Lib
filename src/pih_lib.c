/***************************************************************************
* pih_lib.c
* (C) 2006 Felipe Albrecht (felipe.albrecht@gmail.com)

#   This program is free software; you can redistribute it and/or modify
#   it under the terms of the GNU General Public License as published by
#   the Free Software Foundation; either version 2 of the License, or
#   (at your option) any later version.
#
#   This program is distributed in the hope that it will be useful,
#   but WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#   GNU General Public License for more details.
#
#   You should have received a copy of the GNU General Public License
#   along with this program; if not, write to the Free Software
#   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

# pih_lib.c:
# A group of usefull functions

****************************************************************/

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>

#include <dirent.h>

#include "pih_lib.h"

#define HASH_TABLE_SIZE 10

#define MAX_STRING_LEN 256

#define ID_STRING_LEN 32

#define LIST_SIZE(l) (l==NULL?0:l->size)

#define HAS_NEXT(it) (it->has_next(it))
#define GET_NEXT_AS(it,type) ((type) it->next(it))

static inline void* __get_id(cell_t c);
static inline void* __get_data(cell_t c);
static list_t __list_sort(const list_t list, int (*comparer)(const void *, const void *),  void* (*getter)(cell_t));

static char *name = NULL;
static out_level_t actual_level = OUT_NORMAL;

void setlevel(out_level_t level)
{
	actual_level = level;
}

void print_message(out_level_t show_level, char *fmt, ...)
{
	va_list args;

	if (show_level <= actual_level) {
		fflush(stdout);

		va_start(args, fmt);
		vfprintf(stdout, fmt, args);
		va_end(args);
	}
}

int max_length(char *s1, char *s2)
{
	assert(s1 != NULL);
	assert(s2 != NULL);

	int len_s1, len_s2;

	len_s1 = strlen(s1);
	len_s2 = strlen(s2);

	if (len_s1 >= len_s2) {
		return len_s1;
	} else {
		return len_s2;
	}
}

char *ptr_to_string(void *ptr)
{
   char *s = malloc(sizeof(char) * ID_STRING_LEN);
   memset(s, '\0', sizeof(char) * ID_STRING_LEN);
   assert(s != NULL);

   sprintf(s, "%p", ptr);

   return s;
}

inline int min_length(char *s1, char *s2)
{
	assert(s1 != NULL);
	assert(s2 != NULL);

	int len_s1, len_s2;

	len_s1 = strlen(s1);
	len_s2 = strlen(s2);

	if (len_s1 <= len_s2) {
		return len_s1;
	} else {
		return len_s2;
	}
}

static cell_t __cell_create()
{
	cell_t cell;

	cell = (cell_t) malloc(sizeof(struct __cell));
	memset(cell, '\0', sizeof(struct __cell));
	assert(cell != NULL);

	return cell;
}


/**
 * Creates a exactly copy of a cell_t
 **/
static cell_t __cell_clone(cell_t cell)
{
	cell_t cell_clone = NULL;

	if (cell == NULL) {
		return cell_clone;
	}

	cell_clone = __cell_create();
	cell_clone->id = strdup(cell->id);
	cell_clone->data = cell->data;
	cell_clone->next = cell->next;

	return cell_clone;
}


list_t list_create()
{
	list_t list;

	list = (list_t) malloc(sizeof(struct __cell));
	memset(list, '\0', sizeof(struct __list));
	assert(list != NULL);

	return list;
}

/**
 * Insert a new data at the last position
 * For faster implementation, uses list_push
 */
void* list_add(const list_t list, char *id, void *data)
{
	assert(list != NULL);
	assert(id != NULL);

	cell_t cell = __cell_create();
	cell->id = strdup(id);
	cell->data = data;
	cell->next = NULL;

	if (list->first == NULL) {
		list->first = cell;
	} else {
		cell_t actual = list->first;
		for ( ; actual->next != NULL; actual = actual->next )
			;
		actual->next = cell;
	}

	list->size++;

	return data;
}



void* list_remove_by_id(list_t list, const char *id)
{
	assert(list != NULL);

	cell_t cell = list->first;
	cell_t prev = NULL;

	void *data;

	assert(id != NULL);

	if (list->size == 0) {
		return NULL;
	}

	while (cell != NULL) {
		if (strcmp(cell->id, id) == 0) {
			data = cell->data;
			if (prev == NULL) { // first node
				list->first = cell->next;
			} else {
				prev->next = cell->next;
			}
			list->size--;
                        free(cell->id);
			free(cell);
			return data;
		}
		prev = cell;
		cell = cell->next;
	}

	return NULL;
}


list_t list_clone(list_t list)
{
	list_t list_clone = NULL;

	if (list == NULL) {
		return list_clone;
	}

	list_clone = (list_t) malloc(sizeof(struct __list));
	list_clone->size = list->size;

	if (list->first == NULL) {
		list_clone->first = NULL;
		return list_clone;
	}

	list_clone->first = __cell_clone(list->first);

	cell_t prev = list_clone->first;
	cell_t next = list->first->next;

	while(next != NULL) {
		prev->next = __cell_clone(next);

		prev = prev->next;
		next = next->next;
	}

	return list_clone;
}

/* Une a list_b em list_a.
   list_a pode ser NULL e retornara um clone de list_b
   list_b pode ser NULL
*/
list_t list_union(list_t list_a, list_t list_b)
{
	assert(list_a != NULL);

//	fprintf(stderr, "juntando a:\n");
//	list_print(list_a, print_dummy);
//	fprintf(stderr, "juntando b:\n");
//	list_print(list_b, print_dummy);
//	fprintf(stderr, "-----------\n");

	if (list_b == NULL) {
		return list_a;
	}

	if (list_a == NULL) {
		return list_clone(list_b);
	}


	cell_t prev;

	if (list_a->size == 0) {
		list_a->first = __cell_clone(list_b->first);
		prev = list_a->first;
	} else {
		prev = list_a->first;
		while(prev->next != NULL) {
			prev = prev->next;
		}
		prev->next = __cell_clone(list_b->first);
		prev = prev->next;
	}

	cell_t next = list_b->first->next;
	while (next != NULL) {
		prev->next = __cell_clone(next);

		prev = prev->next;
		next = next->next;
	}
	list_a->size += list_b->size;

//	fprintf(stderr, "retornando :\n");
//	list_print(list_a, print_dummy);
//	fprintf(stderr, "------------:\n");

	return list_a;
}

void list_destroy(list_t *list)
{
	if (*list == NULL) {
		return;
	}

	cell_t cell = (*list)->first;
	cell_t cell_delete = NULL;

	while (cell != NULL) {
		cell_delete = cell;
		cell = cell->next;
                free(cell_delete->id);
		free(cell_delete);
	}

	free(*list);
	*list = NULL;
}

void list_destroy_all(list_t *list, int (*destroy_item)(void *))
{
	cell_t cell = (*list)->first;
	cell_t cell_delete = NULL;

	void *data;

	while (cell != NULL) {
		cell_delete = cell;
		data = cell_delete->data;
		destroy_item(&data);

		cell = cell->next;
		free(cell_delete);
	}

	free(*list);
	list = NULL;
}


/**
 * Add a item into the list at the first position.
 * Usefull to simule stacks
 */
void* list_push(const list_t list, char *id, void *data)
{
	assert(list != NULL);
	assert(id != NULL);

	cell_t cell = __cell_create();
	cell->id = strdup(id);
	cell->data = data;
	cell->next = list->first;

	list->first = cell;

	list->size++;

	return data;
}

/**
 * Get the first element of the list
 * Usefull to simule stacks
 */
cell_t list_pop(const list_t list)
{
	assert(list != NULL);

	if (list->size == 0) {
		return NULL;
	}

	cell_t cell = list->first;
	list->first = cell->next;
	list->size--;

	return cell;
}

int __list_has_next(const iterator_t iterator)
{
	if (iterator->list == NULL) {
		return 0;
	}

	if (iterator->actual == NULL) {
		if (iterator->list->first != NULL) {
			return 1;
		} else {
			return 0;
		}
	} else if (iterator->actual->next != NULL) {
		return 1;
	} else {
		return 0;
	}


}


void* __list_next(const iterator_t iterator)
{
	if (iterator->actual == NULL) {
		iterator->actual = iterator->list->first;
	} else {
		iterator->actual = iterator->actual->next;
	}

	return iterator->actual;
}


iterator_t list_iterator(const list_t list)
{
	iterator_t iterator;

	iterator = (iterator_t) malloc(sizeof(struct __iterator));
	assert(iterator != NULL);

	iterator->list = list;
	iterator->actual = NULL;
	iterator->has_next = __list_has_next;
	iterator->next = __list_next;

	return iterator;
}

/**
 * Creates a new iterator thats continues from the next position of the iterator 'parent'
 **/
iterator_t list_iterator_continues(const iterator_t iterator)
{
	iterator_t new_iterator;

	new_iterator = (iterator_t) malloc(sizeof(struct __iterator));
	assert(new_iterator != NULL);

	new_iterator->list = iterator->list;
	new_iterator->actual = iterator->actual;

	new_iterator->has_next = iterator->has_next;
	new_iterator->next = iterator->next;

	return new_iterator;

}

/* TODO: Rename to iterator_destroy */
void list_iterator_destroy(iterator_t *iterator)
{
	free(*iterator);
	*iterator = NULL;
}



iterator_t list_iterator_reset(const iterator_t iterator)
{
	iterator->actual = NULL;

	return iterator;
}


/*
 * Pesquisa uma celula dentro da lista com determinada id
 * retorna a celula que contem o dado.
 */
cell_t list_search(const list_t list, const char *id)
{
	assert(list != NULL);

	iterator_t it = list_iterator(list);
	cell_t cell = NULL;

	while (it->has_next(it)) {
		cell = it->next(it);

		if (strcmp(cell->id, id) == 0) {
			list_iterator_destroy(&it);
                        fprintf(stderr, "aaa");
			return cell;
		}
	}

	list_iterator_destroy(&it);
	return NULL;
}

/**
 * Take the pos_th element of the tree.
 * if pos is less than zero or greather of the list size, returns NULL
 **/
void *list_get_by_pos(const list_t list, const size_t pos)
{
	assert(list != NULL);

	size_t i;
	cell_t cell;

	if (pos > list->size - 1) {
		return NULL;
	}

	cell = list->first;
	for (i = 0; i < pos; i++) {
		cell = cell->next;
	}

	return cell->data;
}


list_t list_sort(const list_t list, int (*comparer)(const void *, const void *))
{
	return __list_sort(list,  comparer, __get_id);
}

list_t list_sort_by_data(const list_t list, int (*comparer)(const void *, const void *))
{
	return __list_sort(list,  comparer, __get_data);
}

/**
 * Return a NEW list sorted
 */
static list_t __list_sort(const list_t list, int (*comparer)(const void *, const void *),  void* (*getter)(cell_t))
{
	assert(list != NULL);

	int j, i;
	cell_t key;
	cell_t cell;
	cell_t *cells;   // array of the list cells
	list_t new_list; // sorted list

	if (list->size <= 1) {
		return list;
	}

	cells = (cell_t *) malloc(sizeof(cell_t) * list->size);
	assert(cells != NULL);

	cell = list->first;
	for (i = 0; (size_t) i < list->size; i++) {
		cells[i] = (cell_t) malloc(sizeof(struct __cell));
                cells[i]->data = cell->data;
                cells[i]->id = cell->id;
		cell = cell->next;
	}

	for (j = 1; (size_t) j < list->size; j++) {
		key = cells[j];
		i = j - 1;
		while ((i >= 0) && (comparer(getter(cells[i]), getter(key)) > 0)) {
	        cells[i + 1] = cells[i];
	        i = i - 1;
        }
        cells[i + 1] = key;
	}

        new_list = (list_t) malloc(sizeof(struct __list));
	new_list->size = list->size;
	new_list->first = cells[0];

	cell = new_list->first;
	for (i = 1; (size_t) i < list->size; i++) {
		cell->next = cells[i];
		cell = cells[i];
	}

	cell->next = NULL;
        free(cells);

	return new_list;
}

/*
 * Imprime todos os elementos da lista.
 *  A funcao de exibiao da lista eh passada no segundo argumento
 */
void list_print(list_t list, void (*print)(char *, void *))
{
	assert(list != NULL);

	iterator_t it = list_iterator(list);
	cell_t cell;

	while (it->has_next(it)) {
		cell = it->next(it);
		print(cell->id, cell->data);
	}

	list_iterator_destroy(&it);
}

void print_dummy(char *id, void* data)
{
	fprintf(stderr, "%s - %p\n", id, data);
}

void print_data_char(char *id, void* data)
{
	fprintf(stderr, "%s - %s\n", id, (char *) data);
}

/*
 * Retorna um ponteiro para a extensao de um arquivo.
 *  "pih_lib.c" retorna -> "c"
 *  "nome.txt"  retorn -> "txt"
 *  "nome.txt.bak" retorna -> "bak"
 */
char *get_file_extension(const char *filename)
{

	assert(filename != NULL);

	char *actual = NULL;
	int i, len = strlen(filename);

	for (i = 0; i < len; i++) {
		if (filename[i] == '.') {
			actual = (char *) filename + (sizeof(char) * (i + 1));
		}
	}

	return actual;
}


/*
 * Retorna uma copia do nome do arquivo sem a extensao.
 *  "pih_lib.c" retorna -> "pih_lib"
 *  "nome.txt"  retorna -> "nome"
 *  "nome.txt.bak retorna -> "nome"
 */
char *get_file_name(const char *filename)
{
	assert(filename != NULL);

	int i, len = strlen(filename);
	char *name = NULL;

	for (i = 0; i < len; i++) {
		if (filename[i] == '.')
			break;
	}

	name = (char *) malloc(sizeof(char) * (i + 1));
	memset(name, '\0', sizeof(char) * (i + 1));
	memcpy(name, filename, i);

	return name;
}


/*
 * Retorna uma copia do nome do arquivo sem o path inteiro
 * coisa/bla/pih.c   -> retorna "pih.c"
 * pih.c             -> retorna "pih.c"
 * a/c/              -> retorna <NAO DEFINI AINDA>
 */
char *get_file_from_dir(const char *full_path)
{
	assert(full_path != NULL);

	int i, len = strlen(full_path), delta;
	char *name = NULL;

	for (i = len - 1; i >= 0; i--) {
		if (full_path[i] == '/') {
			delta = len - i;
			name = (char *) malloc(sizeof(char) * (delta + 1));
			memset(name, '\0', sizeof(char) * (delta + 1));
			memcpy(name, full_path + i + 1, delta);
			return name;
		}
	}

	if (name == NULL) {
		return strdup(full_path);
	}

	return name;
}

/*
 * Retorna uma copia do diretorio do arquivo ou o diretorio pai do diretorio
*  "coisa/bla/pih.c -> retorna "coisa/bla/"
*  "../up/pih.c     -> retorna "../up/"
*  "bla/blu/        -> retorna "bla/"
*  "pih.c           -> retorna "./"
*/
char *get_directory(const char *full_path)
{
	assert(full_path != NULL);

	int i, len = strlen(full_path);
	char *name = NULL;

	for (i = len - 1; i >= 0; i--) {
		if (full_path[i] == '/') {
			name = (char *) malloc(sizeof(char) * (i + 1));
			memset(name, '\0', sizeof(char) * (i + 1));
			memcpy(name, full_path, i);
			return name;
		}
	}

	return  "./";
}


list_t read_file_list(char *filelist_name)
{
	FILE *listfile = NULL;
	char *tmp_name = malloc(sizeof(char) * MAX_STRING_LEN);
	char *file_name = NULL;
	int  len = 0;
	list_t files_list = list_create();

	listfile = fopen(filelist_name, "r");

	if (listfile == NULL) {
		fprintf(stderr, "file %s dont exist.\n\n", filelist_name);
		return NULL;
	}

	do {
		fscanf (listfile, "%16s\n", tmp_name);
		len = strlen(tmp_name);
		if (len > 0) {
			file_name = malloc(sizeof(char) * (len + 1));
			memcpy(file_name, tmp_name, len + 1);
			list_add(files_list, file_name, NULL);
		}
	} while (!feof(listfile));

	fclose(listfile);

	return files_list;
}


/*
 * Verifica se o identificador "name" esta na lista
 * Retorna 1 se estive e 0 se nao estiver
 */
int is_in_the_list(list_t list, char *name)
{
	assert(list != NULL);

	if (name == NULL) {
		return 0;
	}

	iterator_t iterator = list_iterator(list);
	cell_t cell = NULL;

	while (iterator->has_next(iterator)) {
		cell = iterator->next(iterator);

		if (strcmp(cell->id, name) == 0) {
			list_iterator_destroy(&iterator);
			return 1;
		}
	}

	list_iterator_destroy(&iterator);
	return 0;
}

/*
 * Cria um produto cartesiano de todos os elementos da lista
 * ou seja, todas os pares possiveis
 * A = (X, Y, Z)
 * Resultado: (XX, XY, XZ, YY, YZ, ZZ)
 */
list_t list_cartesian_product(list_t list)
{
	iterator_t iterator = list_iterator(list);
	iterator_t iterator_2 = list_iterator(list);

	list_t result = list_create();
	list_t done   = list_create();

	cell_t actual = NULL, internal = NULL;

	// \todo rewrite using list_iterator_continues
	while (iterator->has_next(iterator)) {
		actual = iterator->next(iterator);
		if (!is_in_the_list(done, actual->id)) {
			while (iterator_2->has_next(iterator_2)) {
				internal = iterator_2->next(iterator_2);

				if (!is_in_the_list(done, internal->id)) {
					list_add(result, actual->id, (void *) internal->id);
				}
			}
		}
		list_add(done, actual->id, NULL);
		list_iterator_reset(iterator_2);
	}

	list_iterator_destroy(&iterator);
	list_iterator_destroy(&iterator_2);

	return result;
}


/*
 * Le um diretorio passado pelo parametro path
 * filtros eh: id: extensao e valor eh 0 para recusar ou 1 para aceitar apenas esse
 */
list_t read_directory(const char *path, list_t filters)
{
	assert(path != NULL);

	DIR* dp;
	struct dirent *result;
	list_t list = list_create();

	dp = opendir(path);

	if (dp == NULL) {
		fprintf(stderr, "Erro ao abrir diretorio: %s\n", path);
		return NULL;
	}


	while ( (result = readdir(dp)) ) {
		if (result->d_type == DT_REG) {
			if (filters == NULL) {
				list_add(list, result->d_name, NULL);
			} else if (is_in_the_list(filters, get_file_extension(result->d_name))) {
				list_add(list, get_file_name(result->d_name), NULL);
			}
		}
	}

	closedir(dp);

	return list;
}

hash_table_t hash_table_create()
{
	hash_table_t hash_table = NULL;

	hash_table = (hash_table_t) malloc(sizeof(struct __hash_table));
	hash_table->size = HASH_TABLE_SIZE;
	hash_table->count = 0;
	hash_table->keys = list_create();
	hash_table->elems = (list_t*) malloc(sizeof(list_t) * HASH_TABLE_SIZE);
	memset(hash_table->elems, '\0', sizeof(list_t) * HASH_TABLE_SIZE);

	return hash_table;
}


static unsigned int __hash_value(hash_table_t hash_table, const char* id)
{
	assert(hash_table != NULL);

	unsigned int h, i = 0, len = strlen(id);

	for (h=0, i=0; i<len; ++i)
		h = (h<<4)^(h>>28)^id[i];

	return h % hash_table->size;
}


/**
  Armazena as referencias a informacoes numa tabela hash.
  ATENCAO: as informacoes nao sao duplicadas, eh apenas armazenado uma referencia a elas!
           caso deseja-se armazena-las, copieas preveamente
  ATENCAO 2: se for adicionado uma informacao com id X e depois outra com id Y,
             a primeira sera perdida!
 */
void* hash_table_add(hash_table_t hash_table, char *id, void *data)
{
	assert(hash_table != NULL);
	assert(id != NULL);


	unsigned int pos = __hash_value(hash_table, id);

	// Se a celula na tabelas estiver vazia, adiciona na tebela e adiciona a chave
	if (hash_table->elems[pos] == NULL) {
		hash_table->elems[pos] = list_create();
		list_add(hash_table->elems[pos], id, data);
		list_add(hash_table->keys, id, data);
		hash_table->count++;
		return data;
	} else {
		// Pesquisa se ha outra informacao com o mesmo id
		iterator_t iterator = list_iterator(hash_table->elems[pos]);
		cell_t list_cell = NULL;

		while (iterator->has_next(iterator)) {
			// Se achou a mesma id: substituir o data e atualizar na lista keys
			// e NAO adicionar a qtd de itens

			list_cell = iterator->next(iterator);
			if (strcmp(list_cell->id, id) == 0) {
				list_cell->data = data;
				iterator_t keys_iterator = list_iterator(hash_table->keys);
				cell_t keys_cell = NULL;
				while (keys_iterator->has_next(keys_iterator)) {
					keys_cell = keys_iterator->next(keys_iterator);
					if ( strcmp(keys_cell->id, id) == 0) {
						keys_cell->data = data;
						break;
					}
				}
				assert(strcmp(keys_cell->id, list_cell->id) == 0);
				assert(keys_cell->data == list_cell->data);

				list_iterator_destroy(&keys_iterator);
				list_iterator_destroy(&iterator);
				return data;
			}
		}

		list_iterator_destroy(&iterator);

		// caso nao tenha achado a mesma key, a adiciona na tabela e na lista de keys
		list_add(hash_table->elems[pos], id, data);
		list_add(hash_table->keys, id, data);
		hash_table->count++;

		return data;
	}

}

void* hash_table_remove_by_id(hash_table_t hash_table, const char *id)
{
	assert(hash_table != NULL);
	assert(id != NULL);

	unsigned int pos = __hash_value(hash_table, id);

	list_t list = hash_table->elems[pos];
	if ((list == NULL) || (list->size == 0)) {
		return NULL;
	}

	void* data = list_remove_by_id(list, id);
	if (data == NULL) {
		return NULL;
	}

	void* data_two = list_remove_by_id(hash_table->keys, id);
	assert(data == data_two);
	hash_table->count--;

	return data;
}


iterator_t hash_table_iterator(hash_table_t hash_table)
{
	iterator_t iterator;

	iterator = (iterator_t) malloc(sizeof(struct __iterator));
	iterator->list = hash_table->keys;
	iterator->actual = NULL;
	iterator->has_next = __list_has_next;
	iterator->next = __list_next;

	return iterator;
}

void hash_table_print(hash_table_t hash_table, void (*print_func)(char *, void *))
{
	assert(print_func != NULL);
	if (hash_table == NULL) {
		fprintf(stderr, "(null)");
                return;
        }

	cell_t cell = NULL;

	iterator_t iterator = NULL;
        iterator = list_iterator(hash_table->keys);

	while (iterator->has_next(iterator)) {
		cell = iterator->next(iterator);
                print_func(cell->id, cell->data);
        }


	list_iterator_destroy(&iterator);

}

void* hash_table_get(hash_table_t hash_table, char *id)
{
	int pos = __hash_value(hash_table, id);
	list_t list = hash_table->elems[pos];
	iterator_t iterator = NULL;
	cell_t cell = NULL;

	if (list == NULL) {
		return NULL;
	}

	iterator = list_iterator(list);

	while (iterator->has_next(iterator)) {
		cell = iterator->next(iterator);
		assert(cell->id != NULL);
		assert(id != NULL);
		if (strcmp(cell->id, id) == 0) {
			list_iterator_destroy(&iterator);
			return cell->data;
		}
	}

	list_iterator_destroy(&iterator);

	return NULL;
}

/**
 * Creates a copy of a hash table.
 */
hash_table_t hash_table_clone(hash_table_t hash_table)
{
	assert(hash_table != NULL);

	hash_table_t hash_table_clone = NULL;
	size_t i;

	if (hash_table == NULL) {
		return hash_table_clone;
	}

	hash_table_clone = (hash_table_t) malloc(sizeof (struct __hash_table));

	hash_table_clone->size = hash_table->size;
	hash_table_clone->count = hash_table->count;

	hash_table_clone->keys = list_clone(hash_table->keys);

	hash_table_clone->elems = (list_t*) malloc(sizeof(list_t) * hash_table->size);

	for(i = 0; i < hash_table->size; i++) {
		hash_table_clone->elems[i] = list_clone(hash_table->elems[i]);
	}

	return hash_table_clone;
}

hash_table_t hash_table_clone_all(hash_table_t hash_table, void* (*clone_item)(void *))
{
	hash_table_t hash_table_clone = NULL;

	hash_table_clone = hash_table_create();

	iterator_t iterator = hash_table_iterator(hash_table);
	cell_t cell = cell;

	while (iterator->has_next(iterator)) {
		cell = iterator->next(iterator);
		hash_table_add(hash_table_clone, cell->id, clone_item(cell->data));
	}

        list_iterator_destroy(&iterator);

	return hash_table_clone;
}

hash_table_t hash_table_destroy(hash_table_t *hash_table)
{
	assert(*hash_table != NULL);
	size_t i, total = 0;
	list_t list;

	for (i = 0; i < (*hash_table)->size; i++) {
		list = (*hash_table)->elems[i];
		total ++;
		if (list !=NULL) {
			list_destroy(&list);
		}
		free(list);
	}
	free((*hash_table)->elems);

	list = (*hash_table)->keys;
	list_destroy(&list);

	free(*hash_table);
	*hash_table = NULL;

	return *hash_table;
}

int hash_table_destroy_all(hash_table_t *hash_table, int (*destroy_item)(void *))
{
	assert(*hash_table != NULL);
	assert(destroy_item != NULL);

	size_t i, total = 0;
	list_t list;

	for (i = 0; i < (*hash_table)->size; i++) {
		list = (*hash_table)->elems[i];
		total++;
		if (list != NULL) {
		    list_destroy(&list);
		}
	}
	free((*hash_table)->elems);

	list = (*hash_table)->keys;
	list_destroy_all(&list, destroy_item);

	free(*hash_table);
	*hash_table = NULL;

	return total;
}

string_buffer_t string_buffer_create()
{
	string_buffer_t sb = (string_buffer_t) malloc(sizeof(struct __string_buffer));
	sb->strings = list_create();
	sb->total_length = 0;

	return sb;
}

string_buffer_t string_buffer_add(string_buffer_t sb, char *fmt, ...)
{
	assert(sb != NULL);
	assert(sb->strings != NULL);
	assert(fmt != NULL);

	char *string;
	va_list args;

	char *tmp = (char *) malloc(sizeof(char) * MAX_STRING_LEN);
	memset(tmp, '\0', MAX_STRING_LEN);

	va_start(args,  fmt);
	vsprintf(tmp, fmt, args);
	va_end(args);

	int total_len = strlen(tmp) + 1;

	string = (char *) malloc(sizeof(char) * total_len);
	memcpy(string, tmp, total_len);

	list_add(sb->strings, string, string);
	sb->total_length += (total_len -1);

	free(tmp);

	return sb;
}

char *string_buffer_to_string(string_buffer_t sb)
{
	assert(sb != NULL);
	assert(sb->strings != NULL);

	char* string = (char *) malloc(sizeof(char) * (sb->total_length + 1));
	memset(string, '\0', sb->total_length + 1);

	iterator_t iterator = list_iterator(sb->strings);
	cell_t cell;
	char *text;
	while(iterator->has_next(iterator)) {
		cell = iterator->next(iterator);
		text = cell->data;
		string = strcat(string, text);
                free(text);
	}

	list_iterator_destroy(&iterator);
        list_destroy(&(sb->strings));
        free(sb);

	return string;
}

int compare_doubles (const double *da, const double *db)
{
	return (*da > *db) - (*da < *db);
}

int compare_strings (const void *va, const void *vb)
{
	assert(va != NULL);
	assert(vb != NULL);

	int x = strcmp(va, vb);
        fprintf(stderr, "%d\n", x);

        return x;
}

static inline void* __get_id(cell_t c)
{
	return c->id;
}

static inline void* __get_data(cell_t c)
{
	return c->data;
}

int destroy_string(void *s)
{
	assert(s != NULL);
	char **string = (char **)s;
	free(*string);
	*string = NULL;
	return 1;
}


size_t combination(size_t l_n, size_t l_k)
{
	double n = (double) l_n;
	double k = (double) l_k;
	double sub_total = n / k;

	while ((--k > 0) && (--n > 0)) {
		sub_total*=(n/k);
	}

	return (size_t) sub_total;
}

shared_ptr_t shared_ptr_create(void *data, void* ref)
{

   shared_ptr_t shared_ptr;

   shared_ptr = (shared_ptr_t) malloc(sizeof(struct __shared_ptr));
   memset(shared_ptr, '\0', sizeof(struct __shared_ptr));
   assert(shared_ptr != NULL);

   shared_ptr->refs = list_create();

   shared_ptr_inc_ref(shared_ptr, ref);

   return shared_ptr;
}


void shared_ptr_inc_ref(shared_ptr_t shared_ptr, void *ref)
{
   assert (shared_ptr != NULL);

   if (list_search(shared_ptr->refs, ref) == NULL) {
      char *s = ptr_to_string(ref);
      list_push(shared_ptr->refs, s, ref);
      free(s);
   }
}

void shared_ptr_rem_ref(shared_ptr_t *shared_ptr, void *ref)
{
   assert (shared_ptr != NULL);

   char *s = ptr_to_string(ref);
   list_remove_by_id((*shared_ptr)->refs, s);
   free(s);

   if ((*shared_ptr)->refs->size == 0) {
      print_message(OUT_DEBUG, "Shared ptr %d does not have more references. Destroying.\n", shared_ptr);
      shared_ptr_destroy(shared_ptr);
   }
}

void shared_ptr_destroy(shared_ptr_t *shared_ptr)
{
   assert (shared_ptr != NULL);

   list_destroy(&(*shared_ptr)->refs);
   free((*shared_ptr)->ptr);
   free(*shared_ptr);
   *shared_ptr = NULL;
}

