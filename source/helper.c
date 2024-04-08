#include "helper.h"

#include <comp421/yalnix.h>
#include <comp421/filesystem.h>

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/*
 * normalize the file name/dir name
 * e.g.: "/aa///a/./aa/" --> /aa/a/aa/
 *
 */
int normPathname(char* pathname) {
    if (pathname == NULL) {
        TracePrintf( 1, "[SERVER][ERR] Pathname is NULL\n");
        return ERROR;
    }

    int len = strlen(pathname);

    if (len == 0) {
        TracePrintf( 1, "[SERVER][ERR] Pathname is empty\n");
        return ERROR;
    }

    int readPos = 0, writePos = 0;
    char* temp =  (char *)malloc(MAXPATHLEN * sizeof(char));
    memcpy(temp, pathname, strlen(pathname));

    // Assuming the path doesn't start with unnecessary "./"
    while (readPos < len && readPos < MAXPATHLEN) {
        // Skip multiple slashes
        if (pathname[readPos] == '/' && pathname[readPos + 1] == '/') {
            readPos++;
            continue;
        }
        // Skip "./"
        if (pathname[readPos] == '.' && (pathname[readPos + 1] == '/' || pathname[readPos + 1] == '\0')) {
            if (pathname[readPos + 1] != '\0') {
                readPos += 2; // Skip both characters
            } else {
                readPos++; // Only skip the dot if it's the last character
            }
            continue;
        }
        // Copy the current character to its new position
        pathname[writePos++] = temp[readPos++];
    }

    // Null-terminate the cleaned path
    pathname[writePos] = '\0';
    free(temp);

    return 0;
}

/* get the filename before the last char */
char* getLastName(const char* path) {
    const char* lastSlash = strrchr(path, '/');
    if (lastSlash == NULL) {
        // No slash found, the path itself is a filename or an empty string.
        return strdup(path); // Duplicate the whole path as the filename.
    } else if (*(lastSlash + 1) == '\0') {
        // The path ends with a slash. There is no filename to extract.
        return strdup(""); // Return an empty string to indicate no filename.
    } else {
        // Extract and return the filename after the last slash.
        return strdup(lastSlash + 1);
    }
}
