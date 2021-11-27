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
  unsigned int yalv;
  int appl;
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

 /* Now that we have the number of applications (in the
  * device_info.num_applications field), 
  * we can retrieve them using the HIDIOCAPPLICATION ioctl()
  * applications are indexed from 0..{num_applications-1}
  */
  for (yalv = 0; yalv < device_info.num_applications; yalv++) {
    appl = ioctl(fd, HIDIOCAPPLICATION, yalv);
    if (appl > 0) {
	printf("Application %i is 0x%x ", yalv, appl);
	/* The magic values come from various usage table specs */
	switch ( appl >> 16)
	    {
	    case 0x01 :
		printf("(Generic Desktop Page)\n");
		break;
	    case 0x0c :
		printf("(Consumer Product Page)\n");
		break;
	    case 0x80 :
		printf("(USB Monitor Page)\n");
		break;
	    case 0x81 :
		printf("(USB Enumerated Values Page)\n");
		break;
	    case 0x82 :
		printf("(VESA Virtual Controls Page)\n");
		break;
	    case 0x83 :
		printf("(Reserved Monitor Page)\n");
		break;
	    case 0x84 :
		printf("(Power Device Page)\n");
		break;
	    case 0x85 :
		printf("(Battery System Page)\n");
		break;
	    case 0x86 :
	    case 0x87 :
		printf("(Reserved Power Device Page)\n");
		break;
	    default :
		printf("(Unknown page - needs to be added)\n");
	    }
    }
  }

  close(fd);

  exit(0);
}
