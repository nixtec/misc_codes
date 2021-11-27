/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or 
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */


#include <stdlib.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <asm/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/hiddev.h>


int main (int argc, char **argv) {

  int fd = -1;
  struct hiddev_devinfo device_info;

  /* ioctl() requires a file descriptor, so we check we got one, and then open it */
  if (argc != 2) {
    fprintf(stderr, "usage: %s hiddevice - probably /dev/usb/hiddev0\n", argv[0]);
    exit(1);
  }
  if ((fd = open(argv[1], O_RDONLY)) < 0) {
    perror("hiddev open");
    exit(1);
  }

  /* suck out some device information */
  ioctl(fd, HIDIOCGDEVINFO, &device_info);

  /* the HIDIOCGDEVINFO ioctl() returns hiddev_devinfo
   * structure - see <linux/hiddev.h> 
   * So we work through the various elements, displaying 
   * each of them 
   */
  printf("vendor 0x%04hx product 0x%04hx version 0x%04hx ",
          device_info.vendor, device_info.product, device_info.version);
  printf("has %i application%s ", device_info.num_applications,
         (device_info.num_applications==1?"":"s"));
  printf("and is on bus: %d devnum: %d ifnum: %d\n",
         device_info.busnum, device_info.devnum, device_info.ifnum);

  close(fd);

  exit(0);
}
