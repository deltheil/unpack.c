#include <stdio.h>
#include <fcntl.h>    /* open */
#include <sys/stat.h> /* fstat */
#include <unistd.h>   /* read */
#include <msgpack.h>

const char *g_cmd;
const char *g_path = "";

static void usage(void);
static char *read_stdin(int *size);
static char *read_file(const char *path, int *size);

int main(int argc, char **argv) {
  if (argc > 2) usage();
  g_cmd = argv[0];
  if (argc == 2) g_path = argv[1];

  int size;
  char *buf;
  msgpack_unpacker u;
  msgpack_unpacked obj;

  if (!strcmp(g_path, "-"))
    buf = read_stdin(&size);
  else if (strlen(g_path) > 0)
    buf = read_file(g_path, &size);
  else
    buf = read_stdin(&size);

  if (!buf) goto readerr;

  msgpack_unpacker_init(&u, size);
  msgpack_unpacked_init(&obj);

  msgpack_unpacker_reserve_buffer(&u, size);
  memcpy(msgpack_unpacker_buffer(&u), buf, size);
  msgpack_unpacker_buffer_consumed(&u, size);

  while (msgpack_unpacker_next(&u, &obj)) {
    msgpack_object_print(stdout, obj.data);
    putchar('\n');
  }

  msgpack_unpacker_destroy(&u);
  msgpack_unpacked_destroy(&obj);
  free(buf);

  return 0;

readerr:
  fprintf(stderr, "file read error\n");
  exit(1);
}

static void usage(void) {
  fprintf(stderr, "usage: %s [file]\n", g_cmd);
  exit(1);
}

static char *read_stdin(int *size) {
  msgpack_sbuffer sbuf;
  msgpack_sbuffer_init(&sbuf);
  char buf[8192];
  int rsize;
  while ((rsize = read(0, buf, sizeof(buf))) > 0)
    msgpack_sbuffer_write(&sbuf, buf, rsize);
  *size = sbuf.size;
  return sbuf.data;
}

static char *read_file(const char *path, int *size) {
  if (!path) return read_stdin(size);
  int fd, len, rsize;
  char *buf = NULL, *wp;
  fd = open(path, O_RDONLY);
  if (fd < 0) goto done;
  struct stat sbuf;
  if (fstat(fd, &sbuf) == -1) goto done;
  len = sbuf.st_size;
  buf = malloc(len);
  wp = buf;
  while ((rsize = read(fd, wp, len)) > 0) {
    wp += rsize;
    len -= rsize;
  }
  *size = sbuf.st_size;
done:
  if (fd > 0) close(fd);
  return buf;
}
