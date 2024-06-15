
#define HANDLE_NULL(ptr)                                                       \
  do {                                                                         \
    if ((ptr) == NULL) {                                                       \
      fprintf(stderr, "ERROR: Null pointer encountered at %s:%d in %s\n",      \
              __FILE__, __LINE__, __func__);                                   \
      abort();                                                                 \
    }                                                                          \
  } while (0)
