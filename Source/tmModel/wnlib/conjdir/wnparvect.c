#include "wnlib.h"
#include "wnasrt.h"
#include "wnmem.h"
#include "wnvect.h"
#include "wnparvect.h"

typedef struct wn_parvect_context_struct wn_parvect_context_t;
typedef struct vect_thread_context_struct *vect_thread_context,
          vect_thread_context_t;

struct vect_thread_context_struct
{
  int thread;
  double start_time;
  double end_time;
  wn_parvect_context context;
  int start_ind;
  int len;
  double sum;
};

struct wn_parvect_context_struct
{
  wn_parallel_callback par_cb;
  wn_parallel_vect_operation_func operation_func;
  void *user_data;
  int num_threads;
  int num_vars;
  vect_thread_context *thread_contexts;
  double *v1, *v2, *v3;
  double s1, s2, s3;
  double *out;
};

wn_parvect_context wn_parvect_init(wn_parallel_callback par_cb,
           int num_threads,
           int num_vars)
{
  int t, chunk_size, ind;
  wn_parvect_context context;
  vect_thread_context tcontext;

  wn_assert(num_vars >= 0);
  wn_assert(num_threads >= 1);

  context = (wn_parvect_context) wn_zalloc(sizeof(wn_parvect_context_t));

  context->num_vars = num_vars;
  context->num_threads = num_threads;
  context->par_cb = par_cb;

  context->thread_contexts = NULL;
  if (context->num_threads == 1)
    return context;

  context->thread_contexts = 
    (vect_thread_context*)wn_zalloc(sizeof(vect_thread_context) * num_threads);

  ind = 0;
  for (t = 0; t < context->num_threads; ++t)
  {
    tcontext = (vect_thread_context)wn_zalloc(sizeof(vect_thread_context_t));
    context->thread_contexts[t] = tcontext;
    tcontext->context = context;
    tcontext->thread = t;
    chunk_size = (num_vars - ind) / (num_threads - t);
    tcontext->start_ind = ind;
    tcontext->len = chunk_size;
    ind += chunk_size;
  }
  wn_assert(ind == num_vars);

  return context;
}

void wn_parvect_free(wn_parvect_context context)
{
  int t;

  if (context->thread_contexts != NULL)
  {
    for (t = 0; t < context->num_threads; ++t)
      wn_free(context->thread_contexts[t]);
    wn_free(context->thread_contexts);
  }
  wn_free(context);
}

static void *add_vect_and_scaled_vect_thread(vect_thread_context tcontext)
{
  wn_parvect_context context = tcontext->context;

  wn_add_vect_and_scaled_vect(context->out + tcontext->start_ind,
            context->v1 + tcontext->start_ind,
            context->v2 + tcontext->start_ind,
            context->s2, tcontext->len);
  return NULL;
}

void wn_add_vect_and_scaled_vect_par(wn_parvect_context context,
             double *out, double *v1, double *v2,
             double s2, int len)
{
  if (context->num_threads == 1)
  {
    wn_add_vect_and_scaled_vect(out, v1, v2, s2, len);
    return;
  }

  wn_assert(context->num_vars == len);
  context->out = out;
  context->v1 = v1;
  context->v2 = v2;
  context->s2 = s2;
  (*context->par_cb)((wn_parallel_func)add_vect_and_scaled_vect_thread,
         (wn_parallel_args)context->thread_contexts,
         "wn_add_vect_and_scaled_vect");
}

static void *add_scaled_vects_thread(vect_thread_context tcontext)
{
  wn_parvect_context context = tcontext->context;

  wn_add_scaled_vects(context->out + tcontext->start_ind,
          context->v1 + tcontext->start_ind, context->s1, 
          context->v2 + tcontext->start_ind, context->s2, 
          tcontext->len);
  return NULL;
}

void wn_add_scaled_vects_par(wn_parvect_context context, 
           double *out, double *v1, double s1, 
           double *v2, double s2, int len)
{
  if (context->num_threads == 1)
  {
    wn_add_scaled_vects(out, v1, s1, v2, s2, len);
    return;
  }

  wn_assert(context->num_vars == len);
  context->out = out;
  context->v1 = v1;
  context->v2 = v2;
  context->s1 = s1;
  context->s2 = s2;
  (*context->par_cb)((wn_parallel_func)add_scaled_vects_thread,
         (wn_parallel_args)context->thread_contexts,
         "wn_add_scaled_vects");
}

static void *add_3_scaled_vects_thread(vect_thread_context tcontext)
{
  wn_parvect_context context = tcontext->context;

  wn_add_3_scaled_vects(context->out + tcontext->start_ind,
      context->v1 + tcontext->start_ind, context->s1,
      context->v2 + tcontext->start_ind, context->s2,
      context->v3 + tcontext->start_ind, context->s3,
      tcontext->len);
  return NULL;
}

void wn_add_3_scaled_vects_par(wn_parvect_context context,
             double *out, double *v1, double s1,
             double *v2, double s2,
             double *v3, double s3, int len)
{
  if (context->num_threads == 1)
  {
    wn_add_3_scaled_vects(out, v1, s1, v2, s2, v3, s3, len);
    return;
  }

  wn_assert(context->num_vars == len);
  context->out = out;
  context->v1 = v1;
  context->v2 = v2;
  context->v3 = v3;
  context->s1 = s1;
  context->s2 = s2;
  context->s3 = s3;
  (*context->par_cb)((wn_parallel_func)add_3_scaled_vects_thread,
         (wn_parallel_args)context->thread_contexts,
         "wn_add_3_scaled_vects");
}

static void *norm2_vect_thread(vect_thread_context tcontext)
{
  wn_parvect_context context = tcontext->context;

  tcontext->sum = wn_norm2_vect(context->v1 + tcontext->start_ind, 
        tcontext->len);
  return NULL;
}

double wn_norm2_vect_par(wn_parvect_context context, double *v1, int len)
{
  int t;
  double sum = 0;

  if (context->num_threads == 1)
    return wn_norm2_vect(v1, len);

  wn_assert(context->num_vars == len);
  context->v1 = v1;
  (*context->par_cb)((wn_parallel_func)norm2_vect_thread,
         (wn_parallel_args)context->thread_contexts,
         "wn_norm2_vect");
  for (t = 0; t < context->num_threads; ++t)
    sum += context->thread_contexts[t]->sum;
  return sum;
}

static void *dot_vects_thread(vect_thread_context tcontext)
{
  wn_parvect_context context = tcontext->context;

  tcontext->sum = wn_dot_vects(context->v1 + tcontext->start_ind, 
             context->v2 + tcontext->start_ind, 
             tcontext->len);
  return NULL;
}

double wn_dot_vects_par(wn_parvect_context context, double *v1, double *v2, int len)
{
  int t;
  double sum = 0;

  if (context->num_threads == 1)
    return wn_dot_vects(v1, v2, len);

  wn_assert(context->num_vars == len);
  context->v1 = v1;
  context->v2 = v2;
  (*context->par_cb)((wn_parallel_func)dot_vects_thread,
         (wn_parallel_args)context->thread_contexts,
         "wn_dot_vects");
  for (t = 0; t < context->num_threads; ++t)
    sum += context->thread_contexts[t]->sum;
  return sum;
}

static void *multiply_vect_by_vect_thread(vect_thread_context tcontext)
{
  wn_parvect_context context = tcontext->context;

  wn_multiply_vect_by_vect(context->v1 + tcontext->start_ind, 
         context->v2 + tcontext->start_ind, tcontext->len);
  return NULL;
}

void wn_multiply_vect_by_vect_par(wn_parvect_context context, 
          double *v1, double *v2, int len)
{
  int t;
  double sum = 0;

  if (context->num_threads == 1)
  {
    wn_multiply_vect_by_vect(v1, v2, len);
    return;
  }

  wn_assert(context->num_vars == len);
  context->v1 = v1;
  context->v2 = v2;
  (*context->par_cb)((wn_parallel_func)multiply_vect_by_vect_thread,
         (wn_parallel_args)context->thread_contexts,
         "wn_multiply_vect_by_vect");
}

static void *divide_vect_by_vect_thread(vect_thread_context tcontext)
{
  wn_parvect_context context = tcontext->context;

  wn_divide_vect_by_vect(context->v1 + tcontext->start_ind, 
       context->v2 + tcontext->start_ind, tcontext->len);
  return NULL;
}

void wn_divide_vect_by_vect_par(wn_parvect_context context,
        double *v1, double *v2, int len)
{
  int t;
  double sum = 0;

  if (context->num_threads == 1)
  {
    wn_divide_vect_by_vect(v1, v2, len);
    return;
  }

  wn_assert(context->num_vars == len);
  context->v1 = v1;
  context->v2 = v2;
  (*context->par_cb)((wn_parallel_func)divide_vect_by_vect_thread,
         (wn_parallel_args)context->thread_contexts,
         "wn_divide_vect_by_vect");
}

static void *zero_vect_thread(vect_thread_context tcontext)
{
  wn_parvect_context context = tcontext->context;

  wn_zero_vect(context->v1 + tcontext->start_ind, tcontext->len);
  return NULL;
}

void wn_zero_vect_par(wn_parvect_context context, double *v1, int len)
{
  int t;

  if (context->num_threads == 1)
  {
    wn_zero_vect(v1, len);
    return;
  }

  wn_assert(context->num_vars == len);
  context->v1 = v1;
  (*context->par_cb)((wn_parallel_func)zero_vect_thread,
         (wn_parallel_args)context->thread_contexts,
         "wn_zero_vect");
}

static void *copy_vect_thread(vect_thread_context tcontext)
{
  wn_parvect_context context = tcontext->context;

  wn_copy_vect(context->v1 + tcontext->start_ind,
           context->v2 + tcontext->start_ind, tcontext->len);
  return NULL;
}

void wn_copy_vect_par(wn_parvect_context context, double *v1, double *v2,
          int len)
{
  int t;

  if (context->num_threads == 1)
  {
    wn_copy_vect(v1, v2, len);
    return;
  }

  wn_assert(context->num_vars == len);
  context->v1 = v1;
  context->v2 = v2;
  (*context->par_cb)((wn_parallel_func)copy_vect_thread,
         (wn_parallel_args)context->thread_contexts,
         "wn_copy_vect");
}

void wn_make_vect_par(wn_parvect_context context, double **v1, int len)
{
  *v1 = (double*) wn_alloc(sizeof(double) * len);
  wn_zero_vect_par(context, *v1, len);
}

static void *parallelize_vector_operation_thread(vect_thread_context tcontext)
{
  wn_parvect_context context = tcontext->context;

  (*context->operation_func)(tcontext->thread, 
           tcontext->start_ind, tcontext->len, 
           context->user_data);
  return NULL;
}

void wn_parallelize_vector_operation(wn_parvect_context context,
    wn_parallel_vect_operation_func operation_func, 
    void *user_data, int len,
    char *operation_name)
{
  if (context->num_threads == 1)
  {
    (*operation_func)(0, 0, len, user_data);
    return;
  }

  wn_assert(context->num_vars == len);
  context->operation_func = operation_func;
  context->user_data = user_data;
  (*context->par_cb)((wn_parallel_func)parallelize_vector_operation_thread,
         (wn_parallel_args)context->thread_contexts,
         operation_name);
}
