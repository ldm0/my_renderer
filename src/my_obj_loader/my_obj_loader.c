#include"my_obj_loader.h"

#include<stdio.h>
#include<stdlib.h>

#define MAX_LINE_LENGTH					233
#define DEFAULT_V_BUFFER_LENGTH			64
#define DEFAULT_VT_BUFFER_LENGTH		64
#define DEFAULT_VN_BUFFER_LENGTH		64
#define DEFAULT_VP_BUFFER_LENGTH		32
#define DEFAULT_F_BUFFER_LENGTH			32
#define DEFAULT_L_BUFFER_LENGTH			32
#define DEFAULT_L_INDEX_BUFFER_LENGTH	8

// For customizable client memory management
static void* (*my_obj_malloc)(size_t size) = malloc;
static void (*my_obj_free)(void* memory) = free;

/** Remove unused characters(comment empty lines etc.)
 *  \param string parse pointer of obj buffer
 *  \param obj_buffer buffer being parsed
 *  \param obj_buffer_length length of obj_buffer
 *  \return 0 means remove success, -1 means pointer come to the end
 */
static inline int remove_unuse(int *ptr, const char *obj_buffer, const int obj_buffer_length)
{
    if (*ptr >= obj_buffer_length)
        return -1;
    char ch;
    for (;;) {
        do {
            ch = obj_buffer[(*ptr)++];
            if (*ptr >= obj_buffer_length)
                return -1;
        } while (ch == ' ');
        if (ch == '#') {            // jump over comments
            do {
                ch = obj_buffer[(*ptr)++];
                if (*ptr >= obj_buffer_length)
                    return -1;
            } while (ch != '\n');
        } else if (ch != '\n') {    // jump over empty lines
            break;
        }
    }
    --(*ptr);
    return 0;
}

/** Move pointer to first character of next line
 *  \param string parse pointer of obj buffer
 *  \param obj_buffer buffer being parsed
 *  \param obj_buffer_length length of obj_buffer
 *  \return 0 means move success, -1 means pointer come to the end
 */
static inline int next_line(int *ptr, const char *obj_buffer, const int obj_buffer_length)
{
    if (*ptr >= obj_buffer_length)
        return -1;
    char ch;
    do {
        ch = obj_buffer[(*ptr)++];
        if (*ptr >= obj_buffer_length)
            return -1;
    } while (ch != '\n');
    return 0;
}

static int my_obj_get_v(
    long *ptr,
    const char *obj_buffer,
    const int obj_buffer_length,
    unsigned *v_buffer_length,
    my_obj_v **v_buffer)
{
    unsigned tmp_v_buffer_top = 0;
    unsigned tmp_v_buffer_capacity = DEFAULT_V_BUFFER_LENGTH;
    my_obj_v *tmp_v_buffer = (my_obj_v *)my_obj_malloc(tmp_v_buffer_capacity * sizeof(my_obj_v));
    if (!tmp_v_buffer)
        return -1;

    my_obj_v tmp_v;
    for (;;) {
        if (remove_unuse(ptr, obj_buffer, obj_buffer_length) == -1)
            break;
        if (sscanf(obj_buffer + *ptr, "v %f %f %f", &tmp_v.x, &tmp_v.y, &tmp_v.z) != 3)
            break;
        if (tmp_v_buffer_top >= tmp_v_buffer_capacity) {
            tmp_v_buffer_capacity += tmp_v_buffer_capacity >> 1;		// capacity multiply 1.5, for memory reuse
            my_obj_v *new_tmp_v_buffer = (my_obj_v *)my_obj_malloc(tmp_v_buffer_capacity * sizeof(my_obj_v));
            if (!new_tmp_v_buffer)
                return -1;
            for (unsigned i = 0; i < tmp_v_buffer_top; ++i)
                new_tmp_v_buffer[i] = tmp_v_buffer[i];
            my_obj_free(tmp_v_buffer);
            tmp_v_buffer = new_tmp_v_buffer;
        }
        tmp_v_buffer[tmp_v_buffer_top] = tmp_v;
        ++tmp_v_buffer_top;
        if (next_line(ptr, obj_buffer, obj_buffer_length) == -1)
            break;
    };

    *v_buffer = tmp_v_buffer;
    *v_buffer_length = tmp_v_buffer_top;
    return 0;
}

// currently w parameter of texture is ignored
static int my_obj_get_vt(
    long *ptr,
    const char *obj_buffer,
    const int obj_buffer_length,
    unsigned *vt_buffer_length,
    my_obj_vt **vt_buffer)
{
    unsigned tmp_vt_buffer_top = 0;
    unsigned tmp_vt_buffer_capacity = DEFAULT_VT_BUFFER_LENGTH;
    my_obj_vt *tmp_vt_buffer = (my_obj_vt *)my_obj_malloc(tmp_vt_buffer_capacity * sizeof(my_obj_vt));
    if (!tmp_vt_buffer)
        return -1;

    my_obj_vt tmp_vt;
    for (;;) {
        if (remove_unuse(ptr, obj_buffer, obj_buffer_length) == -1)
            break;
        if (sscanf(obj_buffer + *ptr, "vt %f %f", &tmp_vt.u, &tmp_vt.v) != 2)
            break;
        if (tmp_vt_buffer_top >= tmp_vt_buffer_capacity) {
            tmp_vt_buffer_capacity += tmp_vt_buffer_capacity >> 1;		// capacity multiply 1.5, for memory reuse
            my_obj_vt *new_tmp_vt_buffer = (my_obj_vt *)my_obj_malloc(tmp_vt_buffer_capacity * sizeof(my_obj_vt));
            if (!new_tmp_vt_buffer)
                return -1;
            for (unsigned i = 0; i < tmp_vt_buffer_top; ++i)
                new_tmp_vt_buffer[i] = tmp_vt_buffer[i];
            my_obj_free(tmp_vt_buffer);
            tmp_vt_buffer = new_tmp_vt_buffer;
        }
        tmp_vt_buffer[tmp_vt_buffer_top] = tmp_vt;
        ++tmp_vt_buffer_top;
        if (next_line(ptr, obj_buffer, obj_buffer_length) == -1)
            break;
    };

    *vt_buffer = tmp_vt_buffer;
    *vt_buffer_length = tmp_vt_buffer_top;
    return 0;
}

static int my_obj_get_vn(
    long *ptr,
    const char *obj_buffer,
    const int obj_buffer_length,
    unsigned *vn_buffer_length,
    my_obj_vn **vn_buffer)
{
    unsigned tmp_vn_buffer_top = 0;
    unsigned tmp_vn_buffer_capacity = DEFAULT_VN_BUFFER_LENGTH;
    my_obj_vn *tmp_vn_buffer = (my_obj_vn *)my_obj_malloc(tmp_vn_buffer_capacity * sizeof(my_obj_vn));
    if (!tmp_vn_buffer)
        return -1;

    my_obj_vn tmp_vn;
    for (;;) {
        if (remove_unuse(ptr, obj_buffer, obj_buffer_length) == -1)
            break;
        if (sscanf(obj_buffer + *ptr, "vn %f %f %f", &tmp_vn.x, &tmp_vn.y, &tmp_vn.z) != 3)
            break;
        if (tmp_vn_buffer_top >= tmp_vn_buffer_capacity) {
            tmp_vn_buffer_capacity += tmp_vn_buffer_capacity >> 1;		// capacity multiply 1.5, for memory reuse
            my_obj_vn *new_tmp_vn_buffer = (my_obj_vn *)my_obj_malloc(tmp_vn_buffer_capacity * sizeof(my_obj_vn));
            if (!new_tmp_vn_buffer)
                return -1;
            for (unsigned i = 0; i < tmp_vn_buffer_top; ++i)
                new_tmp_vn_buffer[i] = tmp_vn_buffer[i];
            my_obj_free(tmp_vn_buffer);
            tmp_vn_buffer = new_tmp_vn_buffer;
        }
        tmp_vn_buffer[tmp_vn_buffer_top] = tmp_vn;
        ++tmp_vn_buffer_top;
        if (next_line(ptr, obj_buffer, obj_buffer_length) == -1)
            break;
    };

    *vn_buffer = tmp_vn_buffer;
    *vn_buffer_length = tmp_vn_buffer_top;
    return 0;
}

static int my_obj_get_vp(
    long *ptr,
    const char *obj_buffer,
    const int obj_buffer_length,
    unsigned *vp_buffer_length,
    my_obj_vp **vp_buffer)
{
    unsigned tmp_vp_buffer_top = 0;
    unsigned tmp_vp_buffer_capacity = DEFAULT_VP_BUFFER_LENGTH;
    my_obj_vp *tmp_vp_buffer = (my_obj_vp *)my_obj_malloc(tmp_vp_buffer_capacity * sizeof(my_obj_vp));
    if (!tmp_vp_buffer)
        return -1;

    my_obj_vp tmp_vp = {0};
    for (;;) {
        if (remove_unuse(ptr, obj_buffer, obj_buffer_length) == -1)
            break;
        if (sscanf(obj_buffer + *ptr, "vp %f %f %f", &tmp_vp.u, &tmp_vp.v, &tmp_vp.w) != 3)
            break;
        if (tmp_vp_buffer_top >= tmp_vp_buffer_capacity) {
            tmp_vp_buffer_capacity += tmp_vp_buffer_capacity >> 1;		// capacity multiply 1.5, for memory reuse
            my_obj_vp *new_tmp_vp_buffer = (my_obj_vp *)my_obj_malloc(tmp_vp_buffer_capacity * sizeof(my_obj_vp));
            if (!new_tmp_vp_buffer)
                return -1;
            for (unsigned i = 0; i < tmp_vp_buffer_top; ++i)
                new_tmp_vp_buffer[i] = tmp_vp_buffer[i];
            my_obj_free(tmp_vp_buffer);
            tmp_vp_buffer = new_tmp_vp_buffer;
        }
        tmp_vp_buffer[tmp_vp_buffer_top] = tmp_vp;
        ++tmp_vp_buffer_top;
        if (next_line(ptr, obj_buffer, obj_buffer_length) == -1)
            break;
    };

    // avoid cache miss and multi thread things
    *vp_buffer = tmp_vp_buffer;
    *vp_buffer_length = tmp_vp_buffer_top;
    return 0;
}

static inline int isdigit(char ch)
{
    return (ch <= '9' && ch >= '0');
}

static inline void ws_remove(long *ptr, const char *str)
{
    while (str[*ptr] == ' ')
        ++*ptr;
}

// return -1 only when str is parsed to the end
/** Attention:
 *  1. When number is not valid, this function doesn't return error.
 *  2. This function should be used after trailing spaces are removed.
 */
static inline int get_unsigned_number(
    unsigned *result,
    long *ptr,
    const char *str,
    const int length)
{
    if (isdigit(str[*ptr])) {
        unsigned number = 0;
        do {
            number = number * 10 + (str[*ptr] - '0');
            ++(*ptr);
            if (*ptr >= length)
                return -1;
        } while (isdigit(str[*ptr]));
        *result = number;
    }
    return 0;
}

static inline int read_f_elements(
    long *ptr,
    const char *obj_buffer,
    const int obj_buffer_length,
    my_obj_f *tmp_f)
{
    if (get_unsigned_number(&(tmp_f->v1), ptr, obj_buffer, obj_buffer_length) != 0)
        return -1;
    if (obj_buffer[*ptr] == ' ') {
        ws_remove(ptr, obj_buffer);
        if (get_unsigned_number(&(tmp_f->v2), ptr, obj_buffer, obj_buffer_length) != 0)
            return -1;

        ws_remove(ptr, obj_buffer);
        if (get_unsigned_number(&(tmp_f->v3), ptr, obj_buffer, obj_buffer_length) != 0)
            return -1;
    } else if (obj_buffer[*ptr] == '/') {
        ++*ptr;
        ws_remove(ptr, obj_buffer);
        // don't return -1 when failed because no digit between two '/' is allowed
        get_unsigned_number(&(tmp_f->vt1), ptr, obj_buffer, obj_buffer_length);
        if (obj_buffer[*ptr] == ' ') {
            ws_remove(ptr, obj_buffer);
            if (get_unsigned_number(&(tmp_f->v2), ptr, obj_buffer, obj_buffer_length) != 0)
                return -1;
            ++*ptr;
            ws_remove(ptr, obj_buffer);
            if (get_unsigned_number(&(tmp_f->vt2), ptr, obj_buffer, obj_buffer_length) != 0)
                return -1;

            ws_remove(ptr, obj_buffer);
            if (get_unsigned_number(&(tmp_f->v3), ptr, obj_buffer, obj_buffer_length) != 0)
                return -1;
            ++*ptr;
            ws_remove(ptr, obj_buffer);
            if (get_unsigned_number(&(tmp_f->vt3), ptr, obj_buffer, obj_buffer_length) != 0)
                return -1;
        } else if (obj_buffer[*ptr] == '/') {
            ++*ptr;
            ws_remove(ptr, obj_buffer);
            if (get_unsigned_number(&(tmp_f->vn1), ptr, obj_buffer, obj_buffer_length) != 0)
                return -1;

            ws_remove(ptr, obj_buffer);
            if (get_unsigned_number(&(tmp_f->v2), ptr, obj_buffer, obj_buffer_length) != 0)
                return -1;
            ++*ptr;
            ws_remove(ptr, obj_buffer);
            if (get_unsigned_number(&(tmp_f->vt2), ptr, obj_buffer, obj_buffer_length) != 0)
                return -1;
            ++*ptr;
            ws_remove(ptr, obj_buffer);
            if (get_unsigned_number(&(tmp_f->vn2), ptr, obj_buffer, obj_buffer_length) != 0)
                return -1;

            ws_remove(ptr, obj_buffer);
            if (get_unsigned_number(&(tmp_f->v3), ptr, obj_buffer, obj_buffer_length) != 0)
                return -1;
            ++*ptr;
            ws_remove(ptr, obj_buffer);
            if (get_unsigned_number(&(tmp_f->vt3), ptr, obj_buffer, obj_buffer_length) != 0)
                return -1;
            ++*ptr;
            ws_remove(ptr, obj_buffer);
            if (get_unsigned_number(&(tmp_f->vn3), ptr, obj_buffer, obj_buffer_length) != 0)
                return -1;
        } else {
            return -1;
        }
    } else {
        return -1;
    }
    return 0;
}

static int my_obj_get_f(
    long *ptr,
    const char *obj_buffer,
    const int obj_buffer_length,
    unsigned *f_buffer_length,
    my_obj_f **f_buffer)
{
    unsigned tmp_f_buffer_top = 0;
    unsigned tmp_f_buffer_capacity = DEFAULT_F_BUFFER_LENGTH;
    my_obj_f *tmp_f_buffer = (my_obj_f *)my_obj_malloc(tmp_f_buffer_capacity * sizeof(my_obj_f));
    if (!tmp_f_buffer)
        return -1;

    for (;;) {
        my_obj_f tmp_f = {
            -1, -1, -1,
            -1, -1, -1,
            -1, -1, -1, };
        char str[MAX_LINE_LENGTH] = {0};
        if (remove_unuse(ptr, obj_buffer, obj_buffer_length) == -1)
            break;
        if (obj_buffer[*ptr] != 'f' || obj_buffer[1 + *ptr] != ' ')
            break;
        *ptr += 2;  // Jump over "f "
        read_f_elements(ptr, obj_buffer, obj_buffer_length, &tmp_f);
        if (tmp_f_buffer_top >= tmp_f_buffer_capacity) {
            tmp_f_buffer_capacity += tmp_f_buffer_capacity >> 1;		// capacity multiply 1.5, for memory reuse
            my_obj_f *new_tmp_f_buffer = (my_obj_f *)my_obj_malloc(tmp_f_buffer_capacity * sizeof(my_obj_f));
            if (!new_tmp_f_buffer)
                return -1;
            for (unsigned i = 0; i < tmp_f_buffer_top; ++i)
                new_tmp_f_buffer[i] = tmp_f_buffer[i];
            my_obj_free(tmp_f_buffer);
            tmp_f_buffer = new_tmp_f_buffer;
        }
        tmp_f_buffer[tmp_f_buffer_top] = tmp_f;
        ++tmp_f_buffer_top;
        if (next_line(ptr, obj_buffer, obj_buffer_length) == -1)
            break;
    };

    *f_buffer = tmp_f_buffer;
    *f_buffer_length = tmp_f_buffer_top;
    return 0;
}

static int my_obj_get_l(
    long *ptr,
    const char *obj_buffer,
    const int obj_buffer_length,
    unsigned *l_buffer_length,
    my_obj_l **l_buffer)
{
    unsigned tmp_l_buffer_top = 0;
    unsigned tmp_l_buffer_capacity = DEFAULT_L_BUFFER_LENGTH;
    my_obj_l *tmp_l_buffer = (my_obj_l *)my_obj_malloc(tmp_l_buffer_capacity * sizeof(my_obj_l));
    if (!tmp_l_buffer)
        return -1;

    my_obj_l tmp_l;
    for (;;) {
        tmp_l.size = 0;
        unsigned tmp_val;
        if (remove_unuse(ptr, obj_buffer, obj_buffer_length) == -1)
            break;
        // because at least there is a unsigned after l
        int offset = 0;
        if (sscanf(obj_buffer + *ptr, "l %u%n", &tmp_val, &offset) != 1)
            break;
        *ptr += offset;
        unsigned tmp_l_index_capacity = DEFAULT_L_INDEX_BUFFER_LENGTH;
        tmp_l.v = (unsigned *)my_obj_malloc(tmp_l_index_capacity * sizeof(unsigned));
        if (!tmp_l.v)
            return -1;
        for (;;) {
            if (sscanf(obj_buffer + *ptr, "%u%n", &tmp_val, &offset) != 1)
                break;
            *ptr += offset;

            tmp_l.v[tmp_l.size++] = tmp_val;
            if (tmp_l.size >= tmp_l_index_capacity) {
                tmp_l_index_capacity += tmp_l_index_capacity >> 1;
                unsigned *new_v = (unsigned *)my_obj_malloc(tmp_l_index_capacity * sizeof(unsigned));
                if (!new_v)
                    return -1;
                unsigned length = tmp_l.size;
                for (unsigned i = 0; i < length; ++i)
                    new_v[i] = tmp_l.v[i];
                my_obj_free(tmp_l.v);
                tmp_l.v = new_v;
            }
        }

        if (tmp_l_buffer_top >= tmp_l_buffer_capacity) {
            tmp_l_buffer_capacity += tmp_l_buffer_capacity >> 1;		// capacity multiply 1.5, for memory reuse
            my_obj_l *new_tmp_l_buffer = (my_obj_l *)my_obj_malloc(tmp_l_buffer_capacity * sizeof(my_obj_l));
            if (!new_tmp_l_buffer)
                return -1;
            for (unsigned i = 0; i < tmp_l_buffer_top; ++i)
                new_tmp_l_buffer[i] = tmp_l_buffer[i];
            my_obj_free(tmp_l_buffer);
            tmp_l_buffer = new_tmp_l_buffer;
        }
        tmp_l_buffer[tmp_l_buffer_top] = tmp_l;
        ++tmp_l_buffer_top;
        if (next_line(ptr, obj_buffer, obj_buffer_length) == -1)
            break;
    };

    *l_buffer = tmp_l_buffer;
    *l_buffer_length = tmp_l_buffer_top;
    return 0;
}

void my_obj_set_allocator(
    void* (*func_malloc)(size_t),
    void (*func_free)(void *))
{
    my_obj_malloc = func_malloc;
    my_obj_free = func_free;
}

int my_obj_get_mesh(
    const char* file_name,
    my_obj_elements *elements)
{
    FILE *obj_fs = NULL;
    char *obj_buffer = NULL;

    obj_fs = fopen(file_name, "r");
    if (obj_fs == NULL)
        goto error;
    if (fseek(obj_fs, 0, SEEK_END) == -1)
        goto error;
    long obj_buffer_length = ftell(obj_fs);
    obj_buffer = (char *)my_obj_malloc(obj_buffer_length * sizeof(char));
    if (obj_buffer == NULL)
        goto error;
    rewind(obj_fs);
    if (fread(obj_buffer, 1, obj_buffer_length, obj_fs) != obj_buffer_length * sizeof(char))
        goto error;
    fclose(obj_fs);
    obj_fs = NULL;

    int parse_ptr = 0;
    if (my_obj_get_v(&parse_ptr, obj_buffer, obj_buffer_length, &(elements->num_v), &(elements->v_buffer)) != 0)
        goto error;
    if (my_obj_get_vt(&parse_ptr, obj_buffer, obj_buffer_length, &(elements->num_vt), &(elements->vt_buffer)) != 0)
        goto error;
    if (my_obj_get_vn(&parse_ptr, obj_buffer, obj_buffer_length, &(elements->num_vn), &(elements->vn_buffer)) != 0)
        goto error;
    if (my_obj_get_vp(&parse_ptr, obj_buffer, obj_buffer_length, &(elements->num_vp), &(elements->vp_buffer)) != 0)
        goto error;
    if (my_obj_get_f(&parse_ptr, obj_buffer, obj_buffer_length, &(elements->num_f), &(elements->f_buffer)) != 0)
        goto error;
    if (my_obj_get_l(&parse_ptr, obj_buffer, obj_buffer_length, &(elements->num_l), &(elements->l_buffer)) != 0)
        goto error;
    free(obj_buffer);
    return 0;
error:
    if (obj_fs)
        fclose(obj_fs);
    if (obj_buffer)
        my_obj_free(obj_buffer);
    return -1;
}

void my_obj_free_mesh(my_obj_elements * elements)
{
    my_obj_free(elements->v_buffer);
    my_obj_free(elements->vt_buffer);
    my_obj_free(elements->vn_buffer);
    my_obj_free(elements->vp_buffer);
    my_obj_free(elements->f_buffer);
    unsigned num_l_buffer = elements->num_l;
    for (unsigned i = 0; i < num_l_buffer; ++i)
        my_obj_free(elements->l_buffer[i].v);
    my_obj_free(elements->l_buffer);
}
