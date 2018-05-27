
#include <errno.h>
#include <limits.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <syrup/path.h>
#include <sys/stat.h>
#include <unistd.h>

static size_t last_index(const char *segment, char c) {
  int i = strlen(segment);
  while (i >= 0) {
    if (segment[--i] == c)
      return i;
  }
  return -1;
}

static size_t last_lindex(const char *path, size_t size, char c) {
  int i = size;
  while (i >= 0) {
    if (path[--i] == c)
      return i;
  }
  return -1;
}

static int path_exists(const char *filename) {
  struct stat buffer;
  return (stat(filename, &buffer) == 0) ? 1 : 0;
}

static int normalize(const char *segment, int strip_first, int *idx) {
  int c_ln = 0;
  if (segment == NULL || (c_ln = strlen(segment)) == 0)
    return 0;

  int i = 0;
  while (segment[i] == SY_PATH_SEPARATOR && i < c_ln)
    i++;

  if (!strip_first && segment[0] == SY_PATH_SEPARATOR)
    i--;

  if (strncmp(segment + i, "./", 2) == 0) {
    segment = segment + i + 2;
    c_ln -= 2;
    *idx = i + 2;
  } else {
    *idx = i;
  }

  if (segment[c_ln - 1] == SY_PATH_SEPARATOR) {
    int l = c_ln;
    while (segment[--l] == SY_PATH_SEPARATOR && l > 0)
      c_ln--;
  }

  return c_ln - i;
}

static const char *resolve(const char *buffer, size_t size, const char *path,
                           int *idx, int *cidx) {
  size_t ln = size;
  size_t i = 0;
  while (strncmp(path + i, "../", 3) == 0 && ln > 0) {
    i += 3;
    ln -= 3;
    int i = last_lindex(buffer, ln, SY_PATH_SEPARATOR);
    if (i > -1) {
      ln = i + 1;
    }
  }
  *idx = ln;
  *cidx = strlen(path) - i;
  return path + i;
}

char *sy_path_join(char *buffer, ...) {
  va_list ap;
  int c_ln = 0, c_idx = 0, i = 0, ln = 0, c = 0;

  if (buffer == NULL) {
    buffer = malloc(sizeof(char) * PATH_MAX);
    c = 1;
  }
  const char *current = NULL;
  va_start(ap, buffer);
  while ((current = va_arg(ap, char *))) {

    c_ln = normalize(current, i++ > 0 ? 1 : 0, &c_idx);
    // Empty string
    if (!c_ln)
      continue;
    else if (ln + c_ln >= PATH_MAX) {
      errno = EFBIG;
      goto fail;
    } else if (!ln) {
      memcpy(buffer + ln, current + c_idx, c_ln);
    } else if (strncmp(current + c_idx, "..", 2) == 0) {

      current = resolve(buffer, ln, current + c_idx, &ln, &c_ln);
      memcpy(buffer + ln, current + c_idx, c_ln);
    } else {
      memcpy(ln == 0 ? buffer : buffer + (ln++), "/", 1);
      memcpy(buffer + ln, current + c_idx, c_ln);
    }

    ln += c_ln;
  }

  buffer[ln] = '\0';

  va_end(ap);

  return buffer;

fail:
  va_end(ap);
  if (c)
    free(buffer);
  return NULL;
}

char *sy_path_join_array(char *buffer, const char **paths) {
  int c_ln = 0, c_idx = 0, i = 0, ln = 0, c = 0;

  if (buffer == NULL) {
    buffer = malloc(sizeof(char) * PATH_MAX);
    c = 1;
  }

  while (*paths) {
    const char *current = *paths++;
    c_ln = normalize(current, i++ > 0 ? 1 : 0, &c_idx);

    // Empty string
    if (!c_ln)
      continue;
    else if (ln + c_ln >= PATH_MAX) {
      errno = EFBIG;
      goto fail;
    } else if (!ln) {
      memcpy(buffer + ln, current + c_idx, c_ln);
    } else if (strncmp(current + c_idx, "..", 2) == 0) {
      current = resolve(buffer, ln, current + c_idx, &ln, &c_ln);
      memcpy(buffer + ln, current + c_idx, c_ln);
    } else if (strncmp(current + c_idx, "./", 2) == 0) {
      current = resolve(buffer, ln, current + c_idx + 2, &ln, &c_ln);
      memcpy(buffer + ln, current + c_idx, c_ln);
    } else {
      memcpy(ln == 0 ? buffer : buffer + (ln++), "/", 1);
      memcpy(buffer + ln, current + c_idx, c_ln);
    }

    ln += c_ln;
  }

  buffer[ln] = '\0';

  return buffer;

fail:
  if (c)
    free(buffer);
  return NULL;
}

int sy_path_is_abs(const char *path) {
  if (path == NULL)
    return 0;
  if (strlen(path) == 0)
    return 0;
  return path[0] == SY_PATH_SEPARATOR ? 1 : 0;
}

char *sy_path_abs(const char *path, char *buffer, size_t maxlen) {
  int c = 0;
  if (buffer == NULL) {
    buffer = malloc(sizeof(char) * PATH_MAX);
    c = 1;
  }

  char cwdBuf[PATH_MAX];
  if (!(getcwd(cwdBuf, PATH_MAX))) {
    goto fail;
  }
  if (!(sy_path_join(buffer, cwdBuf, path, NULL))) {
    goto fail;
  }

  if (!path_exists(buffer)) {
    goto fail;
  }

  return buffer;

fail:
  if (c)
    free(buffer);

  return NULL;
}

size_t sy_path_base(const char *path, size_t *idx) {
  int strl = strlen(path);
  int counter = strl;
  int len = strl;
  *idx = 0;
  while (counter-- >= 0) {
    if (path[counter] == SY_PATH_SEPARATOR) {
      *idx = counter + 1;
      len = strl - *idx;
      break;
    }
  }
  return len;
}

size_t sy_path_dir(const char *path) {
  int ret = last_index(path, SY_PATH_SEPARATOR);
  return ret == 0 ? 1 : ret + 1;
}

size_t sy_path_ext(const char *path, size_t *idx) {
  size_t _idx = -1;
  size_t len = sy_path_base(path, &_idx);

  if (len == 0) {
    return 0;
  }
  const char *p = path + _idx;
  for (int i = 0; i < len; i++) {
    if (i != 0 && p[i] == '.') {
      *idx = _idx + i;
      break;
    }
  }

  if (*idx == -1)
    return 0;

  return strlen(path) - (*idx);
}

char *sy_path_resolve(const char *path, char *buffer) {

  int c = 0;
  if (buffer == NULL) {
    buffer = malloc(sizeof(char) * PATH_MAX);
    c = 1;
  }

  char cwdBuf[PATH_MAX];
  if (!(getcwd(cwdBuf, PATH_MAX))) {
    goto fail;
  }
  if (!(sy_path_join(buffer, cwdBuf, path, NULL))) {
    goto fail;
  }

  return buffer;

fail:
  if (c)
    free(buffer);

  return NULL;
}