# syrup-path

```c
#include <syrup/path.h>
#include <limits.h>

char buffer[PATH_MAX];

sy_path_join(buffer, "/Users/my-name/", "../your-name", NULL);

size_t len, idx

len = sy_path_basename("/Users/my-name/readme.md", &idx);
memcpy(buffer, "/Users/my-name/readme.md" + idx, len); // readme.md;

```
