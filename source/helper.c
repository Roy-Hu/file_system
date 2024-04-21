#include "helper.h"
#include "log.h"

#include <comp421/yalnix.h>
#include <comp421/filesystem.h>

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/*
 * normalize the file name/dir name
 * e.g.: "/aa///a/./aa/" --> /aa/a/aa/
 * return 0 on success, Error else
 */
// int normPathname(char* pathname) {
//     if (pathname == NULL) {
//         printf( "[SERVER][ERR] Pathname is NULL\n");
//         return ERROR;
//     }

//     int len = strlen(pathname);

//     if (len == 0) {
//         printf( "[SERVER][ERR] Pathname is empty\n");
//         return ERROR;
//     }

//     int readPos = 0, writePos = 0;
//     char* temp =  (char *)malloc(MAXPATHLEN * sizeof(char));
//     memcpy(temp, pathname, strlen(pathname));

//     // Assuming the path doesn't start with unnecessary "./"
//     while (readPos < len && readPos < MAXPATHLEN) {
//         // Skip multiple slashes
//         if (pathname[readPos] == '/' && pathname[readPos + 1] == '/') {
//             readPos++;
//             continue;
//         }
//         // Skip "./"
//         if (pathname[readPos] == '.' && (pathname[readPos + 1] == '/' || pathname[readPos + 1] == '\0')) {
//             if (pathname[readPos + 1] != '\0') readPos += 2; // Skip both characters
//             else readPos++; // Only skip the dot if it's the last character

//             continue;
//         }
//         // Copy the current character to its new position
//         pathname[writePos++] = temp[readPos++];
//     }

//     // Null-terminate the cleaned path
//     pathname[writePos] = '\0';
//     free(temp);

//     return 0;
// }
int normPathname(char* pathname) {
    if (pathname == NULL) {
        TracePrintf(ERR, "[SERVER][ERR] Pathname is NULL\n");
        return ERROR;
    }

    int len = strlen(pathname);
    if (len == 0) {
        TracePrintf(ERR, "[SERVER][ERR] Pathname is empty\n");
        return ERROR;
    }

    // Return directly if path is '.' or '..'
    if (strcmp(pathname, ".") == 0 || strcmp(pathname, "..") == 0) {
        return 0; // Path is already normalized, no changes needed
    }

    char* temp = (char *)malloc(MAXPATHLEN * sizeof(char));
    if (temp == NULL) {
        TracePrintf(ERR, "[SERVER][ERR] Memory allocation failed\n");
        return ERROR;
    }

    memcpy(temp, pathname, len + 1); // Include the null terminator

    int readPos = 0, writePos = 0;
    while (readPos < len && readPos < MAXPATHLEN) {
        // Skip multiple slashes
        while (pathname[readPos] == '/' && pathname[readPos + 1] == '/') {
            readPos++;
        }
        // Handle "./"
        if (pathname[readPos] == '.' && pathname[readPos + 1] == '/' ) {
            readPos += 2; // Skip both characters
            continue;
        }
        // Handle "../" or ".." at the end
        if (pathname[readPos] == '.' && pathname[readPos + 1] == '.' && (pathname[readPos + 2] == '/' || pathname[readPos + 2] == '\0')) {
            if (writePos > 0) {  // Go back to the previous slash
                writePos--;
                while (writePos > 0 && pathname[writePos] != '/') {
                    writePos--;
                }
            }
            readPos += (pathname[readPos + 2] == '/') ? 3 : 2;
            continue;
        }
        // Copy the current character to its new position
        temp[writePos++] = pathname[readPos++];
    }

    // Null-terminate the cleaned path
    temp[writePos] = '\0';
    memcpy(pathname, temp, writePos + 1); // Copy back to original pathname including the null terminator
    free(temp);

    return 0;
}
/* get the filename before the last char */
// char* getLastName(const char* path) {
//     const char* lastSlash = strrchr(path, '/');
    
//     if (lastSlash == NULL) return strdup(path);
//     else if (*(lastSlash + 1) == '\0')  return strdup("");
//     else return strdup(lastSlash + 1);
// }
char* getLastName(const char* path) {
    // Check if path is "." or ".."
    if (strcmp(path, ".") == 0 || strcmp(path, "..") == 0) {
        return strdup(path); // Return a copy of the special case
    }

    const char* lastSlash = strrchr(path, '/');
    
    // If there is no slash, return the whole path
    if (lastSlash == NULL) {
        return strdup(path);
    }
    // If the character after the last slash is the null terminator, return empty string
    else if (*(lastSlash + 1) == '\0') {
        return strdup("");
    }
    // Return the substring after the last slash
    else {
        return strdup(lastSlash + 1);
    }
}

/*
 * Add a directory entry given the inum and name
 *
 * 
 */

/* set the newly created directory/file name */
void setdirName(struct dir_entry* entry, char* filename) {
    memset(entry->name, '\0', DIRNAMELEN);
    int len = strlen(filename);
    if(len > DIRNAMELEN){
        memcpy(entry->name, filename, DIRNAMELEN);
    }
    else {
        memcpy(entry->name, filename, len);
    }
}

bool cmpDirName(struct dir_entry* entry, char* eName) {    
    unsigned int len = 0;
    for (len = 0; len < DIRNAMELEN; len++) {
        if (entry->name[len] == '\0') {
            break;
        }
    }
    
    if (strlen(eName) != len) return false;
        
    return strncmp(entry->name, eName, len) == 0;
}