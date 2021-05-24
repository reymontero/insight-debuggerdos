/*****************************************************************************
* mkhelp.c -- creates help file for Insight 1.20.
* Copyright (c) 2007, Oleg O. Chukaev
*
* This program is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public License
* as published by the Free Software Foundation; either version 2
* of the License, or (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
* 
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
* 02111-1307, USA.
*****************************************************************************/

#include <stdio.h>
#include "mkhelp.h"

int main (int argc, char *argv[]) {

	FILE *in, *out;
	int i;
	char str[1000], c, color;

	if (argc < 3) {
		printf ("mkhelp: error: filename(s) not specified\n");
		printf ("usage: mkhelp <in-file> <out-file>\n\n");
		return (1);
	}

	in = fopen (argv[1], "rt");
	if (in == NULL) {
		printf ("mkhelp: error: can't open input file\n\n");
		return (2);
	}

	out = fopen (argv[2], "wb");
	if (out == NULL) {
		printf ("mkhelp: error: can't open output file\n\n");
		return (3);
	}

	color = COL_TEXT;
	while (fgets (str, 800, in)) {
		i = 0;
		while (str[i] != '\n' && str[i] != '\0') {
			c = str[i++];
			switch (c) {
			case '(':
				color = COL_BOLD; break;
			case '[':
				color = COL_ITALIC; break;
			case '<':
				color = COL_FRAME; break;
			case '{':
				color = COL_KEY; break;
			case ')':
			case ']':
			case '>':
			case '}':
				color = COL_TEXT; break;
			case '\\':
				if (str[i+1] != '\0' && str[i+1] != '\n') {
					c = str[i++];
				}
				if (c == '0') {
					c = 0x1b;  /* left arrow */
				} else if (c == '1') {
					c = 0x1a;  /* right arrow */
				}
				/* FALLTHROUGH */
			default:
				fprintf (out, "%c%c", c, color);
			}
		}
	}

	fclose (in);
	fclose (out);

	return (0);
}


