#pragma once
#include <stddef.h>
#define SY_PATH_SEPARATOR '/'

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Concat 1 or more path together into buffer.
 * @params {char *} buffer
 * @params {char **} paths
 * @return {int}
 */
char *sy_path_join(char *buffer, ...);
char *sy_path_join_array(char *buffer, const char **paths);
int sy_path_is_abs(const char *path);
char *sy_path_abs(const char *path, char *buffer, size_t maxlen);

size_t sy_path_base(const char *path, size_t *idx);
size_t sy_path_dir(const char *path);
size_t sy_path_ext(const char *path, size_t *idx);
char *sy_path_resolve(const char *path, char *buffer);

#ifdef __cplusplus
}
#endif