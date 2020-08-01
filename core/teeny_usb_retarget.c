/*       
 *         _______                    _    _  _____ ____  
 *        |__   __|                  | |  | |/ ____|  _ \ 
 *           | | ___  ___ _ __  _   _| |  | | (___ | |_) |
 *           | |/ _ \/ _ \ '_ \| | | | |  | |\___ \|  _ < 
 *           | |  __/  __/ | | | |_| | |__| |____) | |_) |
 *           |_|\___|\___|_| |_|\__, |\____/|_____/|____/ 
 *                               __/ |                    
 *                              |___/                     
 *
 * TeenyUSB - light weight usb stack for micro controllers
 * 
 * Copyright (c) 2020 XToolBox  - admin@xtoolbox.org
 *                         www.tusb.org
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "teeny_usb.h"
#include <errno.h>
#include <stdio.h>


#define STDIN_LEN   256
#define STDIN_MASK  (STDIN_LEN-1)
static char stdin_buf[STDIN_LEN];
static uint32_t stdin_wr;
static uint32_t stdin_rd;

WEAK void stdout_sendchar(int ch)
{
    (void)ch;
}

void stdin_recvchar(int ch)
{
    
    if(  (stdin_wr != stdin_rd)
       && ( (stdin_wr & STDIN_MASK) == (stdin_rd & STDIN_MASK) ) )
    {
        // buffer full
    }
    stdin_buf[  STDIN_MASK & stdin_wr] = (char)ch;
    stdin_wr++;
}

int stdin_getchar(void)
{
    if(stdin_wr == stdin_rd){
        return -1;
    }
    int ch = stdin_buf[  STDIN_MASK & stdin_rd];
    stdin_rd++;
    return ch;
}

#if  defined ( __GNUC__ )

#include "sys/stat.h"
#include "sys/unistd.h"

int _write(int file, char *data, int len)
{
	int i;
	if (file == STDOUT_FILENO || file == STDERR_FILENO) {
		for (i = 0; i < len; i++) {
			if (data[i] == '\n') {
				stdout_sendchar('\r');
			}
			stdout_sendchar(data[i]);
		}
		return i;
	}
	errno = EIO;
	return -1; 
}

int getchar(void)
{
    int ch = stdin_getchar();
    if(ch>0){
        if(ch == '\r'){
            stdout_sendchar('\n');
        }
        stdout_sendchar(ch);
    }
    return ch;
}

#else

int fputc(int ch, FILE* stream)
{
    if(stream == &__stdin || stream == &__stdout){
        if(ch == '\n'){
            stdout_sendchar('\r');
        }
        stdout_sendchar(ch);
    }
    return 0;
}

int fgetc (FILE * stream)
{
    if (stream == &__stdin) {
        int ch = stdin_getchar();
        if(ch>0){
            if(ch == '\r'){
                stdout_sendchar('\n');
            }
            stdout_sendchar(ch);
        }
        return ch;
    }
    return -1;
}

#endif
