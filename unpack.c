#include <stdio.h>
#include <fcntl.h>    /* open */
#include <unistd.h>   /* read */
#include <msgpack.h>

/** defaults */
#define CHK_SIZE    8192 /* read chunk size */
#define BUF_SIZE   65536 /* streaming buffer init size */

const char *g_cmd;
const char *g_path = "";

static void usage(void);

int main(int argc, char **argv) {
  g_cmd = argv[0];
  if (argc > 2) usage();
  if (argc == 2) g_path = argv[1];

  char buf[CHK_SIZE];
  int fd;
  ssize_t rsize;
  msgpack_unpacker u;
  msgpack_unpacked obj;

  if (!strcmp(g_path, "-"))
    fd = 0;
  else if (strlen(g_path) > 0)
    fd = open(g_path, O_RDONLY);
  else
    fd = 0;

  if (fd < 0) goto readerr;

  msgpack_unpacker_init(&u, BUF_SIZE);
  msgpack_unpacked_init(&obj);

  while ((rsize = read(fd, buf, CHK_SIZE)) > 0) {
    msgpack_unpacker_reserve_buffer(&u, rsize);
    memcpy(msgpack_unpacker_buffer(&u), buf, rsize);
    msgpack_unpacker_buffer_consumed(&u, rsize);
    while (msgpack_unpacker_next(&u, &obj)) {
      msgpack_object_print(stdout, obj.data);
      putchar('\n');
    }
  }

  msgpack_unpacker_destroy(&u);
  msgpack_unpacked_destroy(&obj);
  if (fd > 0) close(fd);

  return 0;

readerr:
  fprintf(stderr, "file read error\n");
  exit(1);
}

static void usage(void) {
  fprintf(stderr, "usage: %s [file]\n", g_cmd);
  exit(1);
}
