#ifndef _MYFUNCS_H_
#define _MYFUNCS_H_
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <openssl/sha.h>

#include <gtk/gtk.h>

#ifdef _WIN32
#include <windows.h>
#include <wincon.h>
#endif	/* !_WIN32 */

void buildLicense(GtkWidget *widget);

#endif
