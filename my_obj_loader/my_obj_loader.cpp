#include"my_obj_loader.h"

#include<stdio.h>

#define MAX_LINE_LENGTH				233
#define DEFAULT_V_BUFFER_LENGTH		64
#define DEFAULT_VT_BUFFER_LENGTH	64
#define DEFAULT_VN_BUFFER_LENGTH	64
#define DEFAULT_VP_BUFFER_LENGTH	32
#define DEFAULT_F_BUFFER_LENGTH		32
#define DEFAULT_L_BUFFER_LENGTH		32

// For customizable client memory management
static void* (*my_obj_malloc)(size_t size);
static void (*my_obj_free)(void* memory);

void my_obj_set_allocator(
	void* (*func_malloc)(size_t), 
	void* (*func_realloc)(void *, size_t), 
	void (*func_free)(void *))
{
	my_obj_malloc = func_malloc;
	my_obj_free = func_free;
}

int my_obj_get_elements(
	char* file_name,
	my_obj_elements *elements)
{
	FILE *obj_fs = 0;
	if (fopen_s(&obj_fs, file_name, "r") != 0)
		goto error;
	if (my_obj_get_v(obj_fs, &(elements->num_v), &(elements->v_buffer)) != 0)
		goto error;
	if (my_obj_get_vt(obj_fs, &(elements->num_vt), &(elements->vt_buffer)) != 0)
		goto error;
	if (my_obj_get_vn(obj_fs, &(elements->num_vn), &(elements->vn_buffer)) != 0)
		goto error;
	if (my_obj_get_vp(obj_fs, &(elements->num_vp), &(elements->vp_buffer)) != 0)
		goto error;
	if (my_obj_get_f(obj_fs, &(elements->num_f), &(elements->f_buffer)) != 0)
		goto error;
	if (my_obj_get_l(obj_fs, &(elements->num_l), &(elements->l_buffer)) != 0)
		goto error;
	return 0;
error:
	if (obj_fs)
		fclose(obj_fs);
	return -1;
}

int my_obj_get_v(
	FILE *obj_file,
	unsigned *v_buffer_length,
	my_obj_v **v_buffer)
{
	unsigned tmp_v_buffer_top = 0;
	unsigned tmp_v_buffer_capacity = DEFAULT_V_BUFFER_LENGTH;
	my_obj_v *tmp_v_buffer = (my_obj_v *)my_obj_malloc(tmp_v_buffer_capacity * sizeof(my_obj_v));
	if (!tmp_v_buffer)
		return -1;

	my_obj_v tmp_v;
	while (fscanf_s(obj_file, "v %f %f %f", &tmp_v.x, &tmp_v.y, &tmp_v.z) == 3) {
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
	};

	// avoid cache miss and multi thread things
	*v_buffer = tmp_v_buffer;
	*v_buffer_length = tmp_v_buffer_top;
	return 0;
}

// currently w parameter of texture is ignored
int my_obj_get_vt(
	FILE *obj_file,
	unsigned *vt_buffer_length,
	my_obj_vt **vt_buffer)
{
	unsigned tmp_vt_buffer_top = 0;
	unsigned tmp_vt_buffer_capacity = DEFAULT_VT_BUFFER_LENGTH;
	my_obj_vt *tmp_vt_buffer = (my_obj_vt *)my_obj_malloc(tmp_vt_buffer_capacity * sizeof(my_obj_vt));
	if (!tmp_vt_buffer)
		return -1;

	my_obj_vt tmp_vt;
	while (fscanf_s(obj_file, "vt %f %f", &tmp_vt.u, &tmp_vt.v) == 2) {
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
	};

	// avoid cache miss and multi thread things
	*vt_buffer = tmp_vt_buffer;
	*vt_buffer_length = tmp_vt_buffer_top;
	return 0;
}

int my_obj_get_vn(
	FILE *obj_file,
	unsigned *vn_buffer_length,
	my_obj_vn **vn_buffer)
{
	unsigned tmp_vn_buffer_top = 0;
	unsigned tmp_vn_buffer_capacity = DEFAULT_VN_BUFFER_LENGTH;
	my_obj_vn *tmp_vn_buffer = (my_obj_vn *)my_obj_malloc(tmp_vn_buffer_capacity * sizeof(my_obj_vn));
	if (!tmp_vn_buffer)
		return -1;

	my_obj_vn tmp_vn;
	while (fscanf_s(obj_file, "vn %f %f %f", &tmp_vn.x, &tmp_vn.x, &tmp_vn.x) == 3) {
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
	};

	// avoid cache miss and multi thread things
	*vn_buffer = tmp_vn_buffer;
	*vn_buffer_length = tmp_vn_buffer_top;
	return 0;
}

int my_obj_get_vp(
	FILE *obj_file,
	unsigned *vp_buffer_length,
	my_obj_vp **vp_buffer)
{
	unsigned tmp_vp_buffer_top = 0;
	unsigned tmp_vp_buffer_capacity = DEFAULT_VP_BUFFER_LENGTH;
	my_obj_vp *tmp_vp_buffer = (my_obj_vp *)my_obj_malloc(tmp_vp_buffer_capacity * sizeof(my_obj_vp));
	if (!tmp_vp_buffer)
		return -1;

	my_obj_vp tmp_vp = {0};
	while (fscanf_s(obj_file, "vp %f %f %f", &tmp_vp.u, &tmp_vp.u, &tmp_vp.u) == 2) {
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
	};

	// avoid cache miss and multi thread things
	*vp_buffer = tmp_vp_buffer;
	*vp_buffer_length = tmp_vp_buffer_top;
	return 0;
}

static inline int isdigit(char *ch)
{
	return (*ch <= '9' && *ch >= '0');
}

static inline int get_unsigned_number(unsigned *_result, char* str, int* top)
{
	if (str[*top] == 0 && str[*top] == ' ' && str[*top] == '\\')
		return 0;
	int result = 0;
	do {
		if (!isdigit(&str[*top]))
			return -1;
		result = result * 10 + (str[*top] - '0');
		++*top;
	} while (str[*top] != 0 && str[*top] != ' ' && str[*top] != '\\');
	*_result = result;
	return 0;
}

static inline int read_f_elements(my_obj_f *tmp_f, char *str)
{
	int top = 0;
	if (get_unsigned_number(&(tmp_f->v1), str, &top) != 0)
		return -1;
	if (str[top] == ' ') {
		++top;
		if (get_unsigned_number(&(tmp_f->v2), str, &top) != 0)
			return -1;
		++top;
		if (get_unsigned_number(&(tmp_f->v3), str, &top) != 0)
			return -1;
	} else if (str[top] == '\\') {
		++top;
		if (get_unsigned_number(&(tmp_f->vt1), str, &top) != 0)
			return -1;
		if (str[top] == ' ') {
			++top;
			if (get_unsigned_number(&(tmp_f->v2), str, &top) != 0)
				return -1;
			++top;
			if (get_unsigned_number(&(tmp_f->vt2), str, &top) != 0)
				return -1;
			++top;
			if (get_unsigned_number(&(tmp_f->v3), str, &top) != 0)
				return -1;
			++top;
			if (get_unsigned_number(&(tmp_f->vt3), str, &top) != 0)
				return -1;
		} else if (str[top] == '\\') {
			++top;
			if (get_unsigned_number(&(tmp_f->vn1), str, &top) != 0)
				return -1;
			++top;
			if (get_unsigned_number(&(tmp_f->v2), str, &top) != 0)
				return -1;
			++top;
			if (get_unsigned_number(&(tmp_f->vt2), str, &top) != 0)
				return -1;
			++top;
			if (get_unsigned_number(&(tmp_f->vn2), str, &top) != 0)
				return -1;
			++top;
			if (get_unsigned_number(&(tmp_f->v3), str, &top) != 0)
				return -1;
			++top;
			if (get_unsigned_number(&(tmp_f->vt3), str, &top) != 0)
				return -1;
			++top;
			if (get_unsigned_number(&(tmp_f->vn3), str, &top) != 0)
				return -1;
		} else /*if (str[top] == 0)*/ {
			return -1;
		}
	} else /*if (str[top] == 0)*/ {
		return -1;
	}
	return 0;
}

int my_obj_get_f(
	FILE *obj_file,
	unsigned *f_buffer_length,
	my_obj_f **f_buffer)
{
	unsigned tmp_f_buffer_top = 0;
	unsigned tmp_f_buffer_capacity = DEFAULT_V_BUFFER_LENGTH;
	my_obj_f *tmp_f_buffer = (my_obj_f *)my_obj_malloc(tmp_f_buffer_capacity * sizeof(my_obj_f));
	if (!tmp_f_buffer)
		return -1;

	for (;;) {
		my_obj_f tmp_f = {
			-1, -1, -1,
			-1, -1, -1,
			-1, -1, -1,};
		char str[MAX_LINE_LENGTH] = {0};
		if (fscanf_s(obj_file, "f %[^\n]", str, MAX_LINE_LENGTH) != 1)
			break;
		if (read_f_elements(&tmp_f, str) != 0)
			break;
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
	};

	// avoid cache miss and multi thread things
	*f_buffer = tmp_f_buffer;
	*f_buffer_length = tmp_f_buffer_top;
	return 0;
}

int my_obj_get_l(
	FILE *obj_file,
	unsigned *l_buffer_length,
	my_obj_l **l_buffer)
{
}
