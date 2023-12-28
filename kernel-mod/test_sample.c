#include <assert.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

enum {
  ADD,
  SUB,
  MUL = 3,
  DIV,
  MOD,
};

int main(int argc, char *argv[]) {
  int fd;
  int nums[3];

  if (argc != 4) {
    printf("Usage: %s num1 op(+,-,x,/,%) num2\n", argv[0]);
    return -1;
  }

  nums[0] = atoi(argv[1]);
  nums[1] = atoi(argv[3]);

  fd = open("/dev/ioctl_demo", O_RDONLY);

  if (fd < 0) {
    perror("Failed to open the device");
    return -1;
  }

  switch (argv[2][0]) {
  case '+':
    ioctl(fd, ADD, nums);
    assert(nums[0] + nums[1] == nums[2]);
    break;
  case '-':
    ioctl(fd, SUB, nums);
    assert(nums[0] - nums[1] == nums[2]);
    break;
  case 'x':
    ioctl(fd, MUL, nums);
    assert(nums[0] * nums[1] == nums[2]);
    break;
  case '/':
    if (nums[1] == 0) {
      printf("num2 cannot be zero!\n");
      goto out;
    }
    ioctl(fd, DIV, nums);
    assert(nums[0] / nums[1] == nums[2]);
    break;
  case '%':
    if (nums[1] == 0) {
      printf("num2 cannot be zero!\n");
      goto out;
    }
    ioctl(fd, MOD, nums);
    assert(nums[0] % nums[1] == nums[2]);
    break;
  default:
    printf("unsupported op: %s\n", argv[2]);
    break;
  }
  printf("%d %s %d = %d\n", nums[0], argv[2], nums[1], nums[2]);

out:
  close(fd);

  return 0;
}