#include <stdio.h>

#include <comp421/yalnix.h>
#include <comp421/iolib.h>
#include <string.h>

int
main()
{
    int fd;
    int nch;
    int status;
    char ch[90000];
    char *article;
    
    article = "current lookup inode becomes the inode for that directory (if that directory itself was found), and processing \
                of the following component of the pathname proceeds with the inode for that directory as the current lookup\
                inode.\
                A pathname, when presented as an argument to a Yalnix file system call is represented as a normal\
                C-style string, terminated by a null character. The maximum length of the entire pathname, including the\
                null character, is limited to MAXPATHNAMELEN characters. This limit of MAXPATHNAMELEN characters\
                applies only to the length of the pathname string when presented as an argument to a Yalnix file system\
                call. The fact of whether this pathname is an absolute pathname or a relative pathname, or the possible\
                presence of symbolic links encountered while processing this pathname, do not count against that limit of\
                MAXPATHNAMELEN characters. The limit of MAXPATHNAMELEN characters literally applies only to the\
                argument of the call itself.\
                Within each directory, two special directory entries must exist (created by the MkDir call):\
                • “.” (dot) : This directory entry has the name “.” and the inode number of the directory within which\
                it is contained.\
                • “..” (dot dot) : This directory entry has the name “..” and the inode number of the parent directory\
                of the directory within which it is contained. In the root directory, the “..” entry instead has the\
                same inode number as “.” (the inode number of the root directory, which is defined as ROOTINODE\
                in comp421/filesystem.h).\
                The “.” and “..” entries are created in a directory when it is created (by the MkDir request) as the first\
                two entries in the new directory. These two directory entries subsequently cannot be explicitly deleted, but\
                are automatically deleted along with the rest of the directory on a successful RmDir request. The “.” and\
                “..” entries must be included in the nlink count in the inode of the directory to which each points.\
                2.5 Symbolic Links\
                NOTE: Support for symbolic links in this project is optional and will not affect your grade. Symbolic\
                links are defined here for those who want to experiment with them in their implementation, and to give a\
                clear example of how symbolic links fit into a “real” file system. You are strongly encouraged—but not\
                required—to implement support for symbolic links in your file system.\
                The Yalnix file system can support symbolic links, as in the Unix file system. A symbolic link to some\
                other file is represented in the Yalnix file system by an inode of type INODE_SYMLINK; the format of this\
                file is otherwise the same as an INODE_REGULAR file. However, the contents of this file (the data stored\
                in the data blocks hanging off of this inode) is interpreted by the file system as the name of the file to which\
                this symbolic link is linked. Note that the length of this name is the entire length of the data in the file, as\
                given by the size field in the inode, and the name as recorded here is not terminated by a null ()\
                character.\
                The file name to which a symbolic link points may be either an absolute pathname or a relative pathname.\
                If a relative pathname, it is interpreted relative to the directory in which this symbolic link file\
                itself occurs; that is, the processing of the symbolic link target begins with the current lookup inode (see\
                Section 2.4) being the inode of the directory in which the symbolic link itself was found. For example, consider\
                the pathname “/a/b/c”, where “b” within this pathname is a symbolic link to the relative pathname\
                “x/y”. Since the target of the symbolic link “b” in this example is a relative pathname, “x/y”, the search\
                for “x” when, for example, attempting to Open the name “/a/b/c”, begins in the same directory in which\
                the name “b” itself was found. Thus, attempting to Open the file “/a/b/c” is ultimately “equivalent” to\
                attempting to Open the name “/a/x/y/c”. If, instead, “b” within the pathname “/a/b/c” is a symbolic\
                link to the absolute pathname “/p/q”, then attempting to open the file “/a/b/c” is then “equivalent” to\
                attempting to open the name “/p/q/c” (but see the note below).The combined effect of attempting to Open this original pathname and encountering these symbolic links \
                during processing that Open attempt is “equivalent” to attempting to Open the pathname “/f/g/h/j” (but\
                see the note below).\
                Note that to process a pathname such as in these examples, in which you might encounter one or\
                more symbolic links during processing that pathname, you should not attempt to build up the complete\
                pathname that the original name is “equivalent” to. Rather, as with any pathname, you should process\
                each component of the pathname one at a time. If you encounter a symbolic link, you should make a\
                recursive call to your pathname lookup procedure, attempting to lookup the target of that symbolic link,\
                before resuming processing what remains of the name you were processing when you encountered that\
                symbolic link. That is why I wrote “equivalent” in quotes above, when saying that attempting to Open\
                one pathname is “equivalent” to attempting to Open some other pathname. Attempting to Open the first\
                pathname is not literally the same as attempting to Open the second, but the final effect in terms of which\
                file ends up being Opened is the same. Processing a symbolic link as part of processing a single pathname\
                in this way is referred to as a symbolic link traversal.\
                When creating a symbolic link, it is not an error if the target of the new symbolic link does not exist.\
                Indeed, when creating a symbolic link, the target is simply recorded as-is, without any processing, except\
                that it is an error to attempt to create a symbolic link to an empty pathname (a null string). Furthermore, if\
                the target of an existing symbolic link is later deleted, this is not an error. In both cases, such a “dangling”\
                symbolic link is not a problem; however, if such a dangling symbolic link is encountered while processing\
                some other pathname (such as during an Open operation), an error would be returned upon encountering\
                the dangling symbolic link.\
                Also, note that, whereas it is an error to attempt to create a hard (regular) link to to a directory, creating\
                a symbolic link to a directory is allowed; this is not an error.\
                Within the complete processing of a single pathname passed as an argument to any Yalnix file system\
                operation, the maximum number of symbolic link traversals that may be performed is limited to\
                MAXSYMLINKS symbolic link traversals (defined in comp421/filesystem.h). If, in processing any\
                single pathname that was an argument to any Yalnix file system operation, you would need to traverse more\
                than MAXSYMLINKS symbolic links, you should terminate processing of that pathname and instead return\
                an error as the result of that Yalnix file system operation.\
                Lastly, note the following special exception to handling a symbolic link when looking up a pathname,\
                with respect to the last component of that pathname: If the last component of the pathname is the name\
                of a symbolic link, then that symbolic link must not be traversed unless the pathname is being looked up\
                for an Open, Create, or ChDir file system operation (see the definition of the file system operations in\
                Section 4.2). For example, if the last component of the pathname passed to an Unlink operation is the\
                name of a symbolic link, then the symbolic link itself should be removed, but if the last component of the\
                pathname passed to an Open operation is the name of a symbolic link, then the file to which the symbolic\
                link refers should be opened, not the symbolic link itself.\
                               Note that to process a pathname such as in these examples, in which you might encounter one or\
                more symbolic links during processing that pathname, you should not attempt to build up the complete\
                pathname that the original name is “equivalent” to. Rather, as with any pathname, you should process\
                each component of the pathname one at a time. If you encounter a symbolic link, you should make a\
                recursive call to your pathname lookup procedure, attempting to lookup the target of that symbolic link,\
                before resuming processing what remains of the name you were processing when you encountered that\
                symbolic link. That is why I wrote “equivalent” in quotes above, when saying that attempting to Open\
                one pathname is “equivalent” to attempting to Open some other pathname. Attempting to Open the first\
                pathname is not literally the same as attempting to Open the second, but the final effect in terms of which\
                file ends up being Opened is the same. Processing a symbolic link as part of processing a single pathname\
                in this way is referred to as a symbolic link traversal.\
                When creating a symbolic link, it is not an error if the target of the new symbolic link does not exist.\
                Indeed, when creating a symbolic link, the target is simply recorded as-is, without any processing, except\
                that it is an error to attempt to create a symbolic link to an empty pathname (a null string). Furthermore, if\
                the target of an existing symbolic link is later deleted, this is not an error. In both cases, such a “dangling”\
                symbolic link is not a problem; however, if such a dangling symbolic link is encountered while processing\
                some other pathname (such as during an Open operation), an error would be returned upon encountering\
                the dangling symbolic link.\
               more symbolic links during processing that pathname, you should not attempt to build up the complete\
                pathname that the original name is “equivalent” to. Rather, as with any pathname, you should process\
                each component of the pathname one at a time. If you encounter a symbolic link, you should make a\
                recursive call to your pathname lookup procedure, attempting to lookup the target of that symbolic link,\
                before resuming processing what remains of the name you were processing when you encountered that\
                symbolic link. That is why I wrote “equivalent” in quotes above, when saying that attempting to Open\
                one pathname is “equivalent” to attempting to Open some other pathname. Attempting to Open the first\
                pathname is not literally the same as attempting to Open the second, but the final effect in terms of which\
                file ends up being Opened is the same. Processing a symbolic link as part of processing a single pathname\
                in this way is referred to as a symbolic link traversal.\
                When creating a symbolic link, it is not an error if the target of the new symbolic link does not exist.\
                Indeed, when creating a symbolic link, the target is simply recorded as-is, without any processing, except\
                that it is an error to attempt to create a symbolic link to an empty pathname (a null string). Furthermore, if\
                the target of an existing symbolic link is later deleted, this is not an error. In both cases, such a “dangling”\
                symbolic link is not a problem; however, if such a dangling symbolic link is encountered while processing\
                some other pathname (such as during an Open operation), an error would be returned upon encountering\
                the dangling symbolic link.\
                Also, note that, whereas it is an error to attempt to create a hard (regular) link to to a directory, creating\
                a symbolic link to a directory is allowed; this is not an error.\
                Within the complete processing of a single pathname passed as an argument to any Yalnix file system\
                operation, the maximum number of symbolic link traversals that may be performed is limited to\
                MAXSYMLINKS symbolic link traversals (defined in comp421/filesystem.h). If, in processing any\
                single pathname that was an argument to any Yalnix file system operation, you would need to traverse more\
                than MAXSYMLINKS symbolic links, you should terminate processing of that pathname and instead return\
                an error as the result of that Yalnix file system operation.\
                Lastly, note the following special exception to handling a symbolic link when looking up a pathname,\
                with respect to the last component of that pathname: If the last component of the pathname is the name\
                of a symbolic link, then that symbolic link must not be traversed unless the pathname is being looked up\
                for an Open, Create, or ChDir file system operation (see the definition of the file system operations in\
                              null character, is limited to MAXPATHNAMELEN characters. This limit of MAXPATHNAMELEN characters\
                applies only to the length of the pathname string when presented as an argument to a Yalnix file system\
                call. The fact of whether this pathname is an absolute pathname or a relative pathname, or the possible\
                presence of symbolic links encountered while processing this pathname, do not count against that limit of\
                MAXPATHNAMELEN characters. The limit of MAXPATHNAMELEN characters literally applies only to the\
                argument of the call itself.\
                Within each directory, two special directory entries must exist (created by the MkDir call):\
                • “.” (dot) : This directory entry has the name “.” and the inode number of the directory within which\
                it is contained.\
                • “..” (dot dot) : This directory entry has the name “..” and the inode number of the parent directory\
                of the directory within which it is contained. In the root directory, the “..” entry instead has the\
                same inode number as “.” (the inode number of the root directory, which is defined as ROOTINODE\
                in comp421/filesystem.h).\
                The “.” and “..” entries are created in a directory when it is created (by the MkDir request) as the first\
                two entries in the new directory. These two directory entries subsequently cannot be explicitly deleted, but\
                are automatically deleted along with the rest of the directory on a successful RmDir request. The “.” and\
                “..” entries must be included in the nlink count in the inode of the directory to which each points.\
                2.5 Symbolic Links\
                NOTE: Support for symbolic links in this project is optional and will not affect your grade. Symbolic\
                links are defined here for those who want to experiment with them in their implementation, and to give a\
                clear example of how symbolic links fit into a “real” file system. You are strongly encouraged—but not\
                required—to implement support for symbolic links in your file system.\
                The Yalnix file system can support symbolic links, as in the Unix file system. A symbolic link to some\
                other file is represented in the Yalnix file system by an inode of type INODE_SYMLINK; the format of this\
                file is otherwise the same as an INODE_REGULAR file. However, the contents of this file (the data stored\
                in the data blocks hanging off of this inode) is interpreted by the file system as the name of the file to which\
                this symbolic link is linked. Note that the length of this name is the entire length of the data in the file, as\
                given by the size field in the inode, and the name as recorded here is not terminated by a null ()\
                character.\
                The file name to which a symbolic link points may be either an absolute pathname or a relative pathname.\
                If a relative pathname, it is interpreted relative to the directory in which this symbolic link file\
                itself occurs; that is, the processing of the symbolic link target begins with the current lookup inode (see\
                Section 2.4) being the inode of the directory in which the symbolic link itself was found. For example, consider\
                the pathname “/a/b/c”, where “b” within this pathname is a symbolic link to the relative pathname\
                “x/y”. Since the target of the symbolic link “b” in this example is a relative pathname, “x/y”, the search\
                for “x” when, for example, attempting to Open the name “/a/b/c”, begins in the same directory in which\
                the name “b” itself was found. Thus, attempting to Open the file “/a/b/c” is ultimately “equivalent” to\
                attempting to Open the name “/a/x/y/c”. If, instead, “b” within the pathname “/a/b/c” is a symbolic\
                link to the absolute pathname “/p/q”, then attempting to open the file “/a/b/c” is then “equivalent” to\
                attempting to open the name “/p/q/c” (but see the note below).The combined effect of attempting to Open this original pathname and encountering these symbolic links \
                during processing that Open attempt is “equivalent” to attempting to Open the pathname “/f/g/h/j” (but\
                see the note below).\
                Note that to process a pathname such as in these examples, in which you might encounter one or\
                more symbolic links during processing that pathname, you should not attempt to build up the complete\
                pathname that the original name is “equivalent” to. Rather, as with any pathname, you should process\
                each component of the pathname one at a time. If you encounter a symbolic link, you should make a\
                recursive call to your pathname lookup procedure, attempting to lookup the target of that symbolic link,\
                before resuming processing what remains of the name you were processing when you encountered that\
                symbolic link. That is why I wrote “equivalent” in quotes above, when saying that attempting to Open\
                one pathname is “equivalent” to attempting to Open some other pathname. Attempting to Open the first\
                pathname is not literally the same as attempting to Open the second, but the final effect in terms of which\
                file ends up being Opened is the same. Processing a symbolic link as part of processing a single pathname\
                in this way is referred to as a symbolic link traversal.\
                When creating a symbolic link, it is not an error if the target of the new symbolic link does not exist.\
                Indeed, when creating a symbolic link, the target is simply recorded as-is, without any processing, except\
                that it is an error to attempt to create a symbolic link to an empty pathname (a null string). Furthermore, if\
                the target of an existing symbolic link is later deleted, this is not an error. In both cases, such a “dangling”\
                symbolic link is not a problem; however, if such a dangling symbolic link is encountered while processing\
                some other pathname (such as during an Open operation), an error would be returned upon encountering\
                the dangling symbolic link.\
                Also, note that, whereas it is an error to attempt to create a hard (regular) link to to a directory, creating\
                a symbolic link to a directory is allowed; this is not an error.\
                Within the complete processing of a single pathname passed as an argument to any Yalnix file system\
                operation, the maximum number of symbolic link traversals that may be performed is limited to\
                MAXSYMLINKS symbolic link traversals (defined in comp421/filesystem.h). If, in processing any\
                single pathname that was an argument to any Yalnix file system operation, you would need to traverse more\
                than MAXSYMLINKS symbolic links, you should terminate processing of that pathname and instead return\
                an error as the result of that Yalnix file system operation.\
                Lastly, note the following special exception to handling a symbolic link when looking up a pathname,\
                with respect to the last component of that pathname: If the last component of the pathname is the name\
                of a symbolic link, then that symbolic link must not be traversed unless the pathname is being looked up\
                for an Open, Create, or ChDir file system operation (see the definition of the file system operations in\
                Section 4.2). For example, if the last component of the pathname passed to an Unlink operation is the\
                name of a symbolic link, then the symbolic link itself should be removed, but if the last component of the\
                pathname passed to an Open operation is the name of a symbolic link, then the file to which the symbolic\
                link refers should be opened, not the symbolic link itself.\
                               Note that to process a pathname such as in these examples, in which you might encounter one or\
                more symbolic links during processing that pathname, you should not attempt to build up the complete\
                pathname that the original name is “equivalent” to. Rather, as with any pathname, you should process\
                each component of the pathname one at a time. If you encounter a symbolic link, you should make a\
                recursive call to your pathname lookup procedure, attempting to lookup the target of that symbolic link,\
                before resuming processing what remains of the name you were processing when you encountered that\
                symbolic link. That is why I wrote “equivalent” in quotes above, when saying that attempting to Open\
                one pathname is “equivalent” to attempting to Open some other pathname. Attempting to Open the first\
                pathname is not literally the same as attempting to Open the second, but the final effect in terms of which\
                file ends up being Opened is the same. Processing a symbolic link as part of processing a single pathname\
                in this way is referred to as a symbolic link traversal.\
                When creating a symbolic link, it is not an error if the target of the new symbolic link does not exist.\
                Indeed, when creating a symbolic link, the target is simply recorded as-is, without any processing, except\
                that it is an error to attempt to create a symbolic link to an empty pathname (a null string). Furthermore, if\
                the target of an existing symbolic link is later deleted, this is not an error. In both cases, such a “dangling”\
                symbolic link is not a problem; however, if such a dangling symbolic link is encountered while processing\
                some other pathname (such as during an Open operation), an error would be returned upon encountering\
                the dangling symbolic link.\
                Also, note that, whereas it is an error to attempt to create a hard (regular) link to to a directory, creating\
                a symbolic link to a directory is allowed; this is not an error.\
                Within the complete processing of a single pathname passed as an argument to any Yalnix file system\
                operation, the maximum number of symbolic link traversals that may be performed is limited to\
                MAXSYMLINKS symbolic link traversals (defined in comp421/filesystem.h). If, in processing any\
                single pathname that was an argument to any Yalnix file system operation, you would need to traverse more\
                than MAXSYMLINKS symbolic links, you should terminate processing of that pathname and instead return\
                an error as the result of that Yalnix file system operation.\
                Lastly, note the following special exception to handling a symbolic link when looking up a pathname,\
                with respect to the last component of that pathname: If the last component of the pathname is the name\
                of a symbolic link, then that symbolic link must not be traversed unless the pathname is being looked up\
                for an Open, Create, or ChDir file system operation (see the definition of the file system operations in\
                Section 4.2). For example, if the last component of the pathname passed to an Unlink operation is the\
                name of a symbolic link, then the symbolic link itself should be removed, but if the last component of the\
                Also, note that, whereas it is an error to attempt to create a hard (regular) link to to a directory, creating\
                a symbolic link to a directory is allowed; this is not an error.\
                Within the complete processing of a single pathname passed as an argument to any Yalnix file system\
                operation, the maximum number of symbolic link traversals that may be performed is limited to\
                MAXSYMLINKS symbolic link traversals (defined in comp421/filesystem.h). If, in processing any\
                     A pathname, when presented as an argument to a Yalnix file system call is represented as a normal\
                C-style string, terminated by a null character. The maximum length of the entire pathname, including the\
                null character, is limited to MAXPATHNAMELEN characters. This limit of MAXPATHNAMELEN characters\
                applies only to the length of the pathname string when presented as an argument to a Yalnix file system\
                call. The fact of whether this pathname is an absolute pathname or a relative pathname, or the possible\
                presence of symbolic links encountered while processing this pathname, do not count against that limit of\
                MAXPATHNAMELEN characters. The limit of MAXPATHNAMELEN characters literally applies only to the\
                argument of the call itself.\
                Within each directory, two special directory entries must exist (created by the MkDir call):\
                • “.” (dot) : This directory entry has the name “.” and the inode number of the directory within which\
                it is contained.\
                • “..” (dot dot) : This directory entry has the name “..” and the inode number of the parent directory\
                of the directory within which it is contained. In the root directory, the “..” entry instead has the\
                same inode number as “.” (the inode number of the root directory, which is defined as ROOTINODE\
                in comp421/filesystem.h).\
                The “.” and “..” entries are created in a directory when it is created (by the MkDir request) as the first\
                two entries in the new directory. These two directory entries subsequently cannot be explicitly deleted, but\
                are automatically deleted along with the rest of the directory on a successful RmDir request. The “.” and\
                “..” entries must be included in the nlink count in the inode of the directory to which each points.\
                2.5 Symbolic Links\
                NOTE: Support for symbolic links in this project is optional and will not affect your grade. Symbolic\
                links are defined here for those who want to experiment with them in their implementation, and to give a\
                clear example of how symbolic links fit into a “real” file system. You are strongly encouraged—but not\
                required—to implement support for symbolic links in your file system.\
                The Yalnix file system can support symbolic links, as in the Unix file system. A symbolic link to some\
                other file is represented in the Yalnix file system by an inode of type INODE_SYMLINK; the format of this\
                file is otherwise the same as an INODE_REGULAR file. However, the contents of this file (the data stored\
                in the data blocks hanging off of this inode) is interpreted by the file system as the name of the file to which\
                this symbolic link is linked. Note that the length of this name is the entire length of the data in the file, as\
                given by the size field in the inode, and the name as recorded here is not terminated by a null ()\
                character.\
                The file name to which a symbolic link points may be either an absolute pathname or a relative pathname.\
                If a relative pathname, it is interpreted relative to the directory in which this symbolic link file\
                itself occurs; that is, the processing of the symbolic link target begins with the current lookup inode (see\
                Section 2.4) being the inode of the directory in which the symbolic link itself was found. For example, consider\
                the pathname “/a/b/c”, where “b” within this pathname is a symbolic link to the relative pathname\
                “x/y”. Since the target of the symbolic link “b” in this example is a relative pathname, “x/y”, the search\
                for “x” when, for example, attempting to Open the name “/a/b/c”, begins in the same directory in which\
                the name “b” itself was found. Thus, attempting to Open the file “/a/b/c” is ultimately “equivalent” to\
                attempting to Open the name “/a/x/y/c”. If, instead, “b” within the pathname “/a/b/c” is a symbolic\
                link to the absolute pathname “/p/q”, then attempting to open the file “/a/b/c” is then “equivalent” to\
                attempting to open the name “/p/q/c” (but see the note below).The combined effect of attempting to Open this original pathname and encountering these symbolic links \
                during processing that Open attempt is “equivalent” to attempting to Open the pathname “/f/g/h/j” (but\
                see the note below).\
                Note that to process a pathname such as in these examples, in which you might encounter one or\
                more symbolic links during processing that pathname, you should not attempt to build up the complete\
                pathname that the original name is “equivalent” to. Rather, as with any pathname, you should process\
                each component of the pathname one at a time. If you encounter a symbolic link, you should make a\
                recursive call to your pathname lookup procedure, attempting to lookup the target of that symbolic link,\
                before resuming processing what remains of the name you were processing when you encountered that\
                symbolic link. That is why I wrote “equivalent” in quotes above, when saying that attempting to Open\
                one pathname is “equivalent” to attempting to Open some other pathname. Attempting to Open the first\
                pathname is not literally the same as attempting to Open the second, but the final effect in terms of which\
                file ends up being Opened is the same. Processing a symbolic link as part of processing a single pathname\
                in this way is referred to as a symbolic link traversal.\
                When creating a symbolic link, it is not an error if the target of the new symbolic link does not exist.\
                Indeed, when creating a symbolic link, the target is simply recorded as-is, without any processing, except\
                that it is an error to attempt to create a symbolic link to an empty pathname (a null string). Furthermore, if\
                the target of an existing symbolic link is later deleted, this is not an error. In both cases, such a “dangling”\
                symbolic link is not a problem; however, if such a dangling symbolic link is encountered while processing\
                some other pathname (such as during an Open operation), an error would be returned upon encountering\
                the dangling symbolic link.\
                Also, note that, whereas it is an error to attempt to create a hard (regular) link to to a directory, creating\
                a symbolic link to a directory is allowed; this is not an error.\
                Within the complete processing of a single pathname passed as an argument to any Yalnix file system\
                operation, the maximum number of symbolic link traversals that may be performed is limited to\
                MAXSYMLINKS symbolic link traversals (defined in comp421/filesystem.h). If, in processing any\
                single pathname that was an argument to any Yalnix file system operation, you would need to traverse more\
                than MAXSYMLINKS symbolic links, you should terminate processing of that pathname and instead return\
                an error as the result of that Yalnix file system operation.\
                Lastly, note the following special exception to handling a symbolic link when looking up a pathname,\
                with respect to the last component of that pathname: If the last component of the pathname is the name\
                of a symbolic link, then that symbolic link must not be traversed unless the pathname is being looked up\
                for an Open, Create, or ChDir file system operation (see the definition of the file system operations in\
                Section 4.2). For example, if the last component of the pathname passed to an Unlink operation is the\
                name of a symbolic link, then the symbolic link itself should be removed, but if the last component of the\
                pathname passed to an Open operation is the name of a symbolic link, then the file to which the symbolic\
                link refers should be opened, not the symbolic link itself.\
                               Note that to process a pathname such as in these examples, in which you might encounter one or\
                more symbolic links during processing that pathname, you should not attempt to build up the complete\
                pathname that the original name is “equivalent” to. Rather, as with any pathname, you should process\
                each component of the pathname one at a time. If you encounter a symbolic link, you should make a\
                recursive call to your pathname lookup procedure, attempting to lookup the target of that symbolic link,\
                before resuming processing what remains of the name you were processing when you encountered that\
                symbolic link. That is why I wrote “equivalent” in quotes above, when saying that attempting to Open\
                one pathname is “equivalent” to attempting to Open some other pathname. Attempting to Open the first\
                pathname is not literally the same as attempting to Open the second, but the final effect in terms of which\
                file ends up being Opened is the same. Processing a symbolic link as part of processing a single pathname\
                in this way is referred to as a symbolic link traversal.\
                When creating a symbolic link, it is not an error if the target of the new symbolic link does not exist.\
                Indeed, when creating a symbolic link, the target is simply recorded as-is, without any processing, except\
                that it is an error to attempt to create a symbolic link to an empty pathname (a null string). Furthermore, if\
                the target of an existing symbolic link is later deleted, this is not an error. In both cases, such a “dangling”\
                symbolic link is not a problem; however, if such a dangling symbolic link is encountered while processing\
                some other pathname (such as during an Open operation), an error would be returned upon encountering\
                the dangling symbolic link.\
               more symbolic links during processing that pathname, you should not attempt to build up the complete\
                pathname that the original name is “equivalent” to. Rather, as with any pathname, you should process\
                each component of the pathname one at a time. If you encounter a symbolic link, you should make a\
                recursive call to your pathname lookup procedure, attempting to lookup the target of that symbolic link,\
                before resuming processing what remains of the name you were processing when you encountered that\
                symbolic link. That is why I wrote “equivalent” in quotes above, when saying that attempting to Open\
                one pathname is “equivalent” to attempting to Open some other pathname. Attempting to Open the first\
                pathname is not literally the same as attempting to Open the second, but the final effect in terms of which\
                file ends up being Opened is the same. Processing a symbolic link as part of processing a single pathname\
                in this way is referred to as a symbolic link traversal.\
                When creating a symbolic link, it is not an error if the target of the new symbolic link does not exist.\
                Indeed, when creating a symbolic link, the target is simply recorded as-is, without any processing, except\
                that it is an error to attempt to create a symbolic link to an empty pathname (a null string). Furthermore, if\
                the target of an existing symbolic link is later deleted, this is not an error. In both cases, such a “dangling”\
                symbolic link is not a problem; however, if such a dangling symbolic link is encountered while processing\
                some other pathname (such as during an Open operation), an error would be returned upon encountering\
                the dangling symbolic link.\
                Also, note that, whereas it is an error to attempt to create a hard (regular) link to to a directory, creating\
                a symbolic link to a directory is allowed; this is not an error.\
                Within the complete processing of a single pathname passed as an argument to any Yalnix file system\
                operation, the maximum number of symbolic link traversals that may be performed is limited to\
                MAXSYMLINKS symbolic link traversals (defined in comp421/filesystem.h). If, in processing any\
                single pathname that was an argument to any Yalnix file system operation, you would need to traverse more\
                than MAXSYMLINKS symbolic links, you should terminate processing of that pathname and instead return\
                an error as the result of that Yalnix file system operation.\
                Lastly, note the following special exception to handling a symbolic link when looking up a pathname,\
                with respect to the last component of that pathname: If the last component of the pathname is the name\
                of a symbolic link, then that symbolic link must not be traversed unless the pathname is being looked up\
                for an Open, Create, or ChDir file system operation (see the definition of the file system operations in\
                              null character, is limited to MAXPATHNAMELEN characters. This limit of MAXPATHNAMELEN characters\
                applies only to the length of the pathname string when presented as an argument to a Yalnix file system\
                call. The fact of whether this pathname is an absolute pathname or a relative pathname, or the possible\
                presence of symbolic links encountered while processing this pathname, do not count against that limit of\
                MAXPATHNAMELEN characters. The limit of MAXPATHNAMELEN characters literally applies only to the\
                argument of the call itself.\
                Within each directory, two special directory entries must exist (created by the MkDir call):\
                • “.” (dot) : This directory entry has the name “.” and the inode number of the directory within which\
                it is contained.\
                • “..” (dot dot) : This directory entry has the name “..” and the inode number of the parent directory\
                of the directory within which it is contained. In the root directory, the “..” entry instead has the\
                same inode number as “.” (the inode number of the root directory, which is defined as ROOTINODE\
                in comp421/filesystem.h).\
                The “.” and “..” entries are created in a directory when it is created (by the MkDir request) as the first\
                two entries in the new directory. These two directory entries subsequently cannot be explicitly deleted, but\
                are automatically deleted along with the rest of the directory on a successful RmDir request. The “.” and\
                “..” entries must be included in the nlink count in the inode of the directory to which each points.\
                2.5 Symbolic Links\
                NOTE: Support for symbolic links in this project is optional and will not affect your grade. Symbolic\
                links are defined here for those who want to experiment with them in their implementation, and to give a\
                clear example of how symbolic links fit into a “real” file system. You are strongly encouraged—but not\
                required—to implement support for symbolic links in your file system.\
                The Yalnix file system can support symbolic links, as in the Unix file system. A symbolic link to some\
                other file is represented in the Yalnix file system by an inode of type INODE_SYMLINK; the format of this\
                file is otherwise the same as an INODE_REGULAR file. However, the contents of this file (the data stored\
                in the data blocks hanging off of this inode) is interpreted by the file system as the name of the file to which\
                this symbolic link is linked. Note that the length of this name is the entire length of the data in the file, as\
                given by the size field in the inode, and the name as recorded here is not terminated by a null ()\
                character.\
                The file name to which a symbolic link points may be either an absolute pathname or a relative pathname.\
                If a relative pathname, it is interpreted relative to the directory in which this symbolic link file\
                itself occurs; that is, the processing of the symbolic link target begins with the current lookup inode (see\
                Section 2.4) being the inode of the directory in which the symbolic link itself was found. For example, consider\
                the pathname “/a/b/c”, where “b” within this pathname is a symbolic link to the relative pathname\
                “x/y”. Since the target of the symbolic link “b” in this example is a relative pathname, “x/y”, the search\
                for “x” when, for example, attempting to Open the name “/a/b/c”, begins in the same directory in which\
                the name “b” itself was found. Thus, attempting to Open the file “/a/b/c” is ultimately “equivalent” to\
                attempting to Open the name “/a/x/y/c”. If, instead, “b” within the pathname “/a/b/c” is a symbolic\
                link to the absolute pathname “/p/q”, then attempting to open the file “/a/b/c” is then “equivalent” to\
                attempting to open the name “/p/q/c” (but see the note below).The combined effect of attempting to Open this original pathname and encountering these symbolic links \
                during processing that Open attempt is “equivalent” to attempting to Open the pathname “/f/g/h/j” (but\
                see the note below).\
                Note that to process a pathname such as in these examples, in which you might encounter one or\
                more symbolic links during processing that pathname, you should not attempt to build up the complete\
                pathname that the original name is “equivalent” to. Rather, as with any pathname, you should process\
                each component of the pathname one at a time. If you encounter a symbolic link, you should make a\
                recursive call to your pathname lookup procedure, attempting to lookup the target of that symbolic link,\
                before resuming processing what remains of the name you were processing when you encountered that\
                symbolic link. That is why I wrote “equivalent” in quotes above, when saying that attempting to Open\
                one pathname is “equivalent” to attempting to Open some other pathname. Attempting to Open the first\
                pathname is not literally the same as attempting to Open the second, but the final effect in terms of which\
                file ends up being Opened is the same. Processing a symbolic link as part of processing a single pathname\
                in this way is referred to as a symbolic link traversal.\
                When creating a symbolic link, it is not an error if the target of the new symbolic link does not exist.\
                Indeed, when creating a symbolic link, the target is simply recorded as-is, without any processing, except\
                that it is an error to attempt to create a symbolic link to an empty pathname (a null string). Furthermore, if\
                the target of an existing symbolic link is later deleted, this is not an error. In both cases, such a “dangling”\
                symbolic link is not a problem; however, if such a dangling symbolic link is encountered while processing\
                some other pathname (such as during an Open operation), an error would be returned upon encountering\
                the dangling symbolic link.\
                Also, note that, whereas it is an error to attempt to create a hard (regular) link to to a directory, creating\
                a symbolic link to a directory is allowed; this is not an error.\
                Within the complete processing of a single pathname passed as an argument to any Yalnix file system\
                operation, the maximum number of symbolic link traversals that may be performed is limited to\
                MAXSYMLINKS symbolic link traversals (defined in comp421/filesystem.h). If, in processing any\
                single pathname that was an argument to any Yalnix file system operation, you would need to traverse more\
                than MAXSYMLINKS symbolic links, you should terminate processing of that pathname and instead return\
                an error as the result of that Yalnix file system operation.\
                Lastly, note the following special exception to handling a symbolic link when looking up a pathname,\
                with respect to the last component of that pathname: If the last component of the pathname is the name\
                of a symbolic link, then that symbolic link must not be traversed unless the pathname is being looked up\
                for an Open, Create, or ChDir file system operation (see the definition of the file system operations in\
                Section 4.2). For example, if the last component of the pathname passed to an Unlink operation is the\
                name of a symbolic link, then the symbolic link itself should be removed, but if the last component of the\
                pathname passed to an Open operation is the name of a symbolic link, then the file to which the symbolic\
                link refers should be opened, not the symbolic link itself.\
                               Note that to process a pathname such as in these examples, in which you might encounter one or\
                more symbolic links during processing that pathname, you should not attempt to build up the complete\
                pathname that the original name is “equivalent” to. Rather, as with any pathname, you should process\
                each component of the pathname one at a time. If you encounter a symbolic link, you should make a\
                recursive call to your pathname lookup procedure, attempting to lookup the target of that symbolic link,\
                before resuming processing what remains of the name you were processing when you encountered that\
                symbolic link. That is why I wrote “equivalent” in quotes above, when saying that attempting to Open\
                one pathname is “equivalent” to attempting to Open some other pathname. Attempting to Open the first\
                pathname is not literally the same as attempting to Open the second, but the final effect in terms of which\
                file ends up being Opened is the same. Processing a symbolic link as part of processing a single pathname\
                in this way is referred to as a symbolic link traversal.\
                When creating a symbolic link, it is not an error if the target of the new symbolic link does not exist.\
                Indeed, when creating a symbolic link, the target is simply recorded as-is, without any processing, except\
                that it is an error to attempt to create a symbolic link to an empty pathname (a null string). Furthermore, if\
                the target of an existing symbolic link is later deleted, this is not an error. In both cases, such a “dangling”\
                symbolic link is not a problem; however, if such a dangling symbolic link is encountered while processing\
                some other pathname (such as during an Open operation), an error would be returned upon encountering\
                the dangling symbolic link.\
                Also, note that, whereas it is an error to attempt to create a hard (regular) link to to a directory, creating\
                a symbolic link to a directory is allowed; this is not an error.\
                Within the complete processing of a single pathname passed as an argument to any Yalnix file system\
                operation, the maximum number of symbolic link traversals that may be performed is limited to\
                MAXSYMLINKS symbolic link traversals (defined in comp421/filesystem.h). If, in processing any\
                single pathname that was an argument to any Yalnix file system operation, you would need to traverse more\
                than MAXSYMLINKS symbolic links, you should terminate processing of that pathname and instead return\
                an error as the result of that Yalnix file system operation.\
                Lastly, note the following special exception to handling a symbolic link when looking up a pathname,\
                with respect to the last component of that pathname: If the last component of the pathname is the name\
                of a symbolic link, then that symbolic link must not be traversed unless the pathname is being looked up\
                for an Open, Create, or ChDir file system operation (see the definition of the file system operations in\
                Section 4.2). For example, if the last component of the pathname passed to an Unlink operation is the\
                name of a symbolic link, then the symbolic link itself should be removed, but if the last component of the\
                Also, note that, whereas it is an error to attempt to create a hard (regular) link to to a directory, creating\
                a symbolic link to a directory is allowed; this is not an error.\
                Within the complete processing of a single pathname passed as an argument to any Yalnix file system\
                operation, the maximum number of symbolic link traversals that may be performed is limite\
                single pathname that was an argument to any Yalnix file system operation, you would need to traverse more\
                than MAXSYMLINKS symbolic links, you should terminate processing of that pathname and instead return\
                an error as the result of that Yalnix file system operation.\
                Lastly, note the following special exception to handling a symbolic link when looking up a pathname,\
                with respect to the last component of that pathname: If the last component of the pathname is the name\
                of a symbolic link, then that symbolic link must not be traversed unless the pathname is being looked up\
                for an Open, Create, or ChDir file system operation (see the definition of the file system operations in\
                              null character, is limited to MAXPATHNAMELEN characters. This limit of MAXPATHNAMELEN characters\
                applies only to the length of the pathname string when presented as an argument to a Yalnix file system\
                call. The fact of whether this pathname is an absolute pathname or a relative pathname, or the possible\
                presence of symbolic links encountered while processing this pathname, do not count against that limit of\
                MAXPATHNAMELEN characters. The limit of MAXPATHNAMELEN characters literally applies only to the\
                argument of the call itself.\
                Within each directory, two special directory entries must exist (created by the MkDir call):\
                • “.” (dot) : This directory entry has the name “.” and the inode number of the directory within which\
                it is contained.\
                • “..” (dot dot) : This directory entry has the name “..” and the inode number of the parent directory\
                of the directory within which it is contained. In the root directory, the “..” entry instead has the\
                same inode number as “.” (the inode number of the root directory, which is defined as ROOTINODE\
                in comp421/filesystem.h).\
                The “.” and “..” entries are created in a directory when it is created (by the MkDir request) as the first\
                two entries in the new directory. These two directory entries subsequently cannot be explicitly deleted, but\
                are automatically deleted along with the rest of the directory on a successful RmDir request. The “.” and\
                “..” entries must be included in the nlink count in the inode of the directory to which each points.\
                2.5 Symbolic Links\
                NOTE: Support for symbolic links in this project is optional and will not affect your grade. Symbolic\
                links are defined here for those who want to experiment with them in their implementation, and to give a\
                clear example of how symbolic links fit into a “real” file system. You are strongly encouraged—but not\
                required—to implement support for symbolic links in your file system.\
                The Yalnix file system can support symbolic links, as in the Unix file system. A symbolic link to some\
                other file is represented in the Yalnix file system by an inode of type INODE_SYMLINK; the format of this\
                file is otherwise the same as an INODE_REGULAR file. However, the contents of this file (the data stored\
                in the data blocks hanging off of this inode) is interpreted by the file system as the name of the file to which\
                this symbolic link is linked. Note that the length of this name is the entire length of the data in the file, as\
                given by the size field in the inode, and the name as recorded here is not terminated by a null ()\
                character.\
                The file name to which a symbolic link points may be either an absolute pathname or a relative pathname.\
                If a relative pathname, it is interpreted relative to the directory in which this symbolic link file\
                itself occurs; that is, the processing of the symbolic link target begins with the current lookup inode (see\
                Section 2.4) being the inode of the directory in which the symbolic link itself was found. For example, consider\
                the pathname “/a/b/c”, where “b” within this pathname is a symbolic link to the relative pathname\
                “x/y”. Since the target of the symbolic link “b” in this example is a relative pathname, “x/y”, the search\
                for “x” when, for example, attempting to Open the name “/a/b/c”, begins in the same directory in which\
                the name “b” itself was found. Thus, attempting to Open the file “/a/b/c” is ultimately “equivalent” to\
                attempting to Open the name “/a/x/y/c”. If, instead, “b” within the pathname “/a/b/c” is a symbolic\
                link to the absolute pathname “/p/q”, then attempting to open the file “/a/b/c” is then “equivalent” to\
                attempting to open the name “/p/q/c” (but see the note below).The combined effect of attempting to Open this original pathname and encountering these symbolic links \
                during processing that Open attempt is “equivalent” to attempting to Open the pathname “/f/g/h/j” (but\
                see the note below).\
                Note that to process a pathname such as in these examples, in which you might encounter one or\
                more symbolic links during processing that pathname, you should not attempt to build up the complete\
                pathname that the original name is “equivalent” to. Rather, as with any pathname, you should process\
                each component of the pathname one at a time. If you encounter a symbolic link, you should make a\
                recursive call to your pathname lookup procedure, attempting to lookup the target of that symbolic link,\
                before resuming processing what remains of the name you were processing when you encountered that\
                symbolic link. That is why I wrote “equivalent” in quotes above, when saying that attempting to Open\
                one pathname is “equivalent” to attempting to Open some other pathname. Attempting to Open the first\
                pathname is not literally the same as attempting to Open the second, but the final effect in terms of which\
                file ends up being Opened is the same. Processing a symbolic link as part of processing a single pathname\
                in this way is referred to as a symbolic link traversal.\
                When creating a symbolic link, it is not an error if the target of the new symbolic link does not exist.\
                Indeed, when creating a symbolic link, the target is simply recorded as-is, without any processing, except\
                that it is an error to attempt to create a symbolic link to an empty pathname (a null string). Furthermore, if\
                the target of an existing symbolic link is later deleted, this is not an error. In both cases, such a “dangling”\
                symbolic link is not a problem; however, if such a dangling symbolic link is encountered while processing\
                some other pathname (such as during an Open operation), an error would be returned upon encountering\
                the dangling symbolic link.\
                Also, note that, whereas it is an error to attempt to create a hard (regular) link to to a directory, creating\
                a symbolic link to a directory is allowed; this is not an error.\
                Within the complete processing of a single pathname passed as an argument to any Yalnix file system\
                operation, the maximum number of symbolic link traversals that may be performed is limited to\
                MAXSYMLINKS symbolic link traversals (defined in comp421/filesystem.h). If, in processing any\
                single pathname that was an argument to any Yalnix file system operation, you would need to traverse more\
                than MAXSYMLINKS symbolic links, you should terminate processing of that pathname and instead return\
                an error as the result of that Yalnix file system operation.\
                Lastly, note the following special exception to handling a symbolic link when looking up a pathname,\
                with respect to the last component of that pathname: If the last component of the pathname is the name\
                of a symbolic link, then that symbolic link must not be traversed unless the pathname is being looked up\
                for an Open, Create, or ChDir file system operation (see the definition of the file system operations in\
                Section 4.2). For example, if the last component of the pathname passed to an Unlink operation is the\
                name of a symbolic link, then the symbolic link itself should be removed, but if the last component of the\
                pathname passed to an Open operation is the name of a symbolic link, then the file to which the symbolic\
                link refers should be opened, not the symbolic link itself.\
                               Note that to process a pathname such as in these examples, in which you might encounter one or\
                more symbolic links during processing that pathname, you should not attempt to build up the complete\
                pathname that the original name is “equivalent” to. Rather, as with any pathname, you should process\
                each component of the pathname one at a time. If you encounter a symbolic link, you should make a\
                recursive call to your pathname lookup procedure, attempting to lookup the target of that symbolic link,\
                before resuming processing what remains of the name you were processing when you encountered that\
                symbolic link. That is why I wrote “equivalent” in quotes above, when saying that attempting to Open\
                one pathname is “equivalent” to attempting to Open some other pathname. Attempting to Open the first\
                pathname is not literally the same as attempting to Open the second, but the final effect in terms of which\
                file ends up being Opened is the same. Processing a symbolic link as part of processing a single pathname\
                in this way is referred to as a symbolic link traversal.\
                When creating a symbolic link, it is not an error if the target of the new symbolic link does not exist.\
                Indeed, when creating a symbolic link, the target is simply recorded as-is, without any processing, except\
                that it is an error to attempt to create a symbolic link to an empty pathname (a null string). Furthermore, if\
                the target of an existing symbolic link is later deleted, this is not an error. In both cases, such a “dangling”\
                symbolic link is not a problem; however, if such a dangling symbolic link is encountered while processing\
                some other pathname (such as during an Open operation), an error would be returned upon encountering\
                the dangling symbolic link.\
                               inode.\
                A pathname, when presented as an argument to a Yalnix file system call is represented as a normal\
                C-style string, terminated by a null character. The maximum length of the entire pathname, including the\
                null character, is limited to MAXPATHNAMELEN characters. This limit of MAXPATHNAMELEN characters\
                applies only to the length of the pathname string when presented as an argument to a Yalnix file system\
                call. The fact of whether this pathname is an absolute pathname or a relative pathname, or the possible\
                presence of symbolic links encountered while processing this pathname, do not count against that limit of\
                MAXPATHNAMELEN characters. The limit of MAXPATHNAMELEN characters literally applies only to the\
                argument of the call itself.\
                Within each directory, two special directory entries must exist (created by the MkDir call):\
                • “.” (dot) : This directory entry has the name “.” and the inode number of the directory within which\
                it is contained.\
                • “..” (dot dot) : This directory entry has the name “..” and the inode number of the parent directory\
                of the directory within which it is contained. In the root directory, the “..” entry instead has the\
                same inode number as “.” (the inode number of the root directory, which is defined as ROOTINODE\
                in comp421/filesystem.h).\
                The “.” and “..” entries are created in a directory when it is created (by the MkDir request) as the first\
                two entries in the new directory. These two directory entries subsequently cannot be explicitly deleted, but\
                are automatically deleted along with the rest of the directory on a successful RmDir request. The “.” and\
                “..” entries must be included in the nlink count in the inode of the directory to which each points.\
                2.5 Symbolic Links\
                NOTE: Support for symbolic links in this project is optional and will not affect your grade. Symbolic\
                links are defined here for those who want to experiment with them in their implementation, and to give a\
                clear example of how symbolic links fit into a “real” file system. You are strongly encouraged—but not\
                required—to implement support for symbolic links in your file system.\
                The Yalnix file system can support symbolic links, as in the Unix file system. A symbolic link to some\
                other file is represented in the Yalnix file system by an inode of type INODE_SYMLINK; the format of this\
                file is otherwise the same as an INODE_REGULAR file. However, the contents of this file (the data stored\
                in the data blocks hanging off of this inode) is interpreted by the file system as the name of the file to which\
                this symbolic link is linked. Note that the length of this name is the entire length of the data in the file, as\
                given by the size field in the inode, and the name as recorded here is not terminated by a null ()\
                character.\
                The file name to which a symbolic link points may be either an absolute pathname or a relative pathname.\
                If a relative pathname, it is interpreted relative to the directory in which this symbolic link file\
                itself occurs; that is, the processing of the symbolic link target begins with the current lookup inode (see\
                Section 2.4) being the inode of the directory in which the symbolic link itself was found. For example, consider\
                the pathname “/a/b/c”, where “b” within this pathname is a symbolic link to the relative pathname\
                “x/y”. Since the target of the symbolic link “b” in this example is a relative pathname, “x/y”, the search\
                for “x” when, for example, attempting to Open the name “/a/b/c”, begins in the same directory in which\
                the name “b” itself was found. Thus, attempting to Open the file “/a/b/c” is ultimately “equivalent” to\
                attempting to Open the name “/a/x/y/c”. If, instead, “b” within the pathname “/a/b/c” is a symbolic\
                link to the absolute pathname “/p/q”, then attempting to open the file “/a/b/c” is then “equivalent” to\
                attempting to open the name “/p/q/c” (but see the note below).The combined effect of attempting to Open this original pathname and encountering these symbolic links \
                during processing that Open attempt is “equivalent” to attempting to Open the pathname “/f/g/h/j” (but\
                see the note below).\
                Note that to process a pathname such as in these examples, in which you might encounter one or\
                more symbolic links during processing that pathname, you should not attempt to build up the complete\
                pathname that the original name is “equivalent” to. Rather, as with any pathname, you should process\
                each component of the pathname one at a time. If you encounter a symbolic link, you should make a\
                recursive call to your pathname lookup procedure, attempting to lookup the target of that symbolic link,\
                before resuming processing what remains of the name you were processing when you encountered that\
                symbolic link. That is why I wrote “equivalent” in quotes above, when saying that attempting to Open\
                one pathname is “equivalent” to attempting to Open some other pathname. Attempting to Open the first\
                pathname is not literally the same as attempting to Open the second, but the final effect in terms of which\
                file ends up being Opened is the same. Processing a symbolic link as part of processing a single pathname\
                in this way is referred to as a symbolic link traversal.\
                When creating a symbolic link, it is not an error if the target of the new symbolic link does not exist.\
                Indeed, when creating a symbolic link, the target is simply recorded as-is, without any processing, except\
                that it is an error to attempt to create a symbolic link to an empty pathname (a null string). Furthermore, if\
                the target of an existing symbolic link is later deleted, this is not an error. In both cases, such a “dangling”\
                symbolic link is not a problem; however, if such a dangling symbolic link is encountered while processing\
                some other pathname (such as during an Open operation), an error would be returned upon encountering\
                the dangling symbolic link.\
                Also, note that, whereas it is an error to attempt to create a hard (regular) link to to a directory, creating\
                a symbolic link to a directory is allowed; this is not an error.\
                Within the complete processing of a single pathname passed as an argument to any Yalnix file system\
                operation, the maximum number of symbolic link traversals that may be performed is limited to\
                MAXSYMLINKS symbolic link traversals (defined in comp421/filesystem.h). If, in processing any\
                single pathname that was an argument to any Yalnix file system operation, you would need to traverse more\
                than MAXSYMLINKS symbolic links, you should terminate processing of that pathname and instead return\
                an error as the result of that Yalnix file system operation.\
                Lastly, note the following special exception to handling a symbolic link when looking up a pathname,\
                with respect to the last component of that pathname: If the last component of the pathname is the name\
                of a symbolic link, then that symbolic link must not be traversed unless the pathname is being looked up\
                for an Open, Create, or ChDir file system operation (see the definition of the file system operations in\
                Section 4.2). For example, if the last component of the pathname passed to an Unlink operation is the\
                name of a symbolic link, then the symbolic link itself should be removed, but if the last component of the\
                pathname passed to an Open operation is the name of a symbolic link, then the file to which the symbolic\
                link refers should be opened, not the symbolic link itself.\
                               Note that to process a pathname such as in these examples, in which you might encounter one or\
                more symbolic links during processing that pathname, you should not attempt to build up the complete\
                pathname that the original name is “equivalent” to. Rather, as with any pathname, you should process\
                each component of the pathname one at a time. If you encounter a symbolic link, you should make a\
                recursive call to your pathname lookup procedure, attempting to lookup the target of that symbolic link,\
                before resuming processing what remains of the name you were processing when you encountered that\
                symbolic link. That is why I wrote “equivalent” in quotes above, when saying that attempting to Open\
                one pathname is “equivalent” to attempting to Open some other pathname. Attempting to Open the first\
                pathname is not literally the same as attempting to Open the second, but the final effect in terms of which\
                file ends up being Opened is the same. Processing a symbolic link as part of processing a single pathname\
                in this way is referred to as a symbolic link traversal.\
                When creating a symbolic link, it is not an error if the target of the new symbolic link does not exist.\
                Indeed, when creating a symbolic link, the target is simply recorded as-is, without any processing, except\
                that it is an error to attempt to create a symbolic link to an empty pathname (a null string). Furthermore, if\
                the target of an existing symbolic link is later deleted, this is not an error. In both cases, such a “dangling”\
                symbolic link is not a problem; however, if such a dangling symbolic link is encountered while processing\
                some other pathname (such as during an Open operation), an error would be returned upon encountering\
                the dangling symbolic link.\
               more symbolic links during processing that pathname, you should not attempt to build up the complete\
                pathname that the original name is “equivalent” to. Rather, as with any pathname, you should process\
                each component of the pathname one at a time. If you encounter a symbolic link, you should make a\
                recursive call to your pathname lookup procedure, attempting to lookup the target of that symbolic link,\
                before resuming processing what remains of the name you were processing when you encountered that\
                symbolic link. That is why I wrote “equivalent” in quotes above, when saying that attempting to Open\
                one pathname is “equivalent” to attempting to Open some other pathname. Attempting to Open the first\
                pathname is not literally the same as attempting to Open the second, but the final effect in terms of which\
                file ends up being Opened is the same. Processing a symbolic link as part of processing a single pathname\
                in this way is referred to as a symbolic link traversal.\
                When creating a symbolic link, it is not an error if the target of the new symbolic link does not exist.\
                Indeed, when creating a symbolic link, the target is simply recorded as-is, without any processing, except\
                that it is an error to attempt to create a symbolic link to an empty pathname (a null string). Furthermore, if\
                the target of an existing symbolic link is later deleted, this is not an error. In both cases, such a “dangling”\
                symbolic link is not a problem; however, if such a dangling symbolic link is encountered while processing\
                some other pathname (such as during an Open operation), an error would be returned upon encountering\
                the dangling symbolic link.\
                Also, note that, whereas it is an error to attempt to create a hard (regular) link to to a directory, creating\
                a symbolic link to a directory is allowed; this is not an error.\
                Within the complete processing of a single pathname passed as an argument to any Yalnix file system\
                operation, the maximum number of symbolic link traversals that may be performed is limited to\
                MAXSYMLINKS symbolic link traversals (defined in comp421/filesystem.h). If, in processing any\
                single pathname that was an argument to any Yalnix file system operation, you would need to traverse more\
                than MAXSYMLINKS symbolic links, you should terminate processing of that pathname and instead return\
                an error as the result of that Yalnix file system operation.\
                Lastly, note the following special exception to handling a symbolic link when looking up a pathname,\
                with respect to the last component of that pathname: If the last component of the pathname is the name\
                of a symbolic link, then that symbolic link must not be traversed unless the pathname is being looked up\
                for an Open, Create, or ChDir file system operation (see the definition of the file system operations in\
                              null character, is limited to MAXPATHNAMELEN characters. This limit of MAXPATHNAMELEN characters\
                applies only to the length of the pathname string when presented as an argument to a Yalnix file system\
                call. The fact of whether this pathname is an absolute pathname or a relative pathname, or the possible\
                presence of symbolic links encountered while processing this pathname, do not count against that limit of\
                MAXPATHNAMELEN characters. The limit of MAXPATHNAMELEN characters literally applies only to the\
                argument of the call itself.\
                Within each directory, two special directory entries must exist (created by the MkDir call):\
                • “.” (dot) : This directory entry has the name “.” and the inode number of the directory within which\
                it is contained.\
                • “..” (dot dot) : This directory entry has the name “..” and the inode number of the parent directory\
                of the directory within which it is contained. In the root directory, the “..” entry instead has the\
                same inode number as “.” (the inode number of the root directory, which is defined as ROOTINODE\
                in comp421/filesystem.h).\
                The “.” and “..” entries are created in a directory when it is created (by the MkDir request) as the first\
                two entries in the new directory. These two directory entries subsequently cannot be explicitly deleted, but\
                are automatically deleted along with the rest of the directory on a successful RmDir request. The “.” and\
                “..” entries must be included in the nlink count in the inode of the directory to which each points.\
                2.5 Symbolic Links\
                NOTE: Support for symbolic links in this project is optional and will not affect your grade. Symbolic\
                links are defined here for those who want to experiment with them in their implementation, and to give a\
                clear example of how symbolic links fit into a “real” file system. You are strongly encouraged—but not\
                required—to implement support for symbolic links in your file system.\
                The Yalnix file system can support symbolic links, as in the Unix file system. A symbolic link to some\
                other file is represented in the Yalnix file system by an inode of type INODE_SYMLINK; the format of this\
                file is otherwise the same as an INODE_REGULAR file. However, the contents of this file (the data stored\
                in the data blocks hanging off of this inode) is interpreted by the file system as the name of the file to which\
                this symbolic link is linked. Note that the length of this name is the entire length of the data in the file, as\
                given by the size field in the inode, and the name as recorded here is not terminated by a null ()\
                character.\
                The file name to which a symbolic link points may be either an absolute pathname or a relative pathname.\
                If a relative pathname, it is interpreted relative to the directory in which this symbolic link file\
                itself occurs; that is, the processing of the symbolic link target begins with the current lookup inode (see\
                Section 2.4) being the inode of the directory in which the symbolic link itself was found. For example, consider\
                the pathname “/a/b/c”, where “b” within this pathname is a symbolic link to the relative pathname\
                “x/y”. Since the target of the symbolic link “b” in this example is a relative pathname, “x/y”, the search\
                for “x” when, for example, attempting to Open the name “/a/b/c”, begins in the same directory in which\
                the name “b” itself was found. Thus, attempting to Open the file “/a/b/c” is ultimately “equivalent” to\
                attempting to Open the name “/a/x/y/c”. If, instead, “b” within the pathname “/a/b/c” is a symbolic\
                link to the absolute pathname “/p/q”, then attempting to open the file “/a/b/c” is then “equivalent” to\
                attempting to open the name “/p/q/c” (but see the note below).The combined effect of attempting to Open this original pathname and encountering these symbolic links \
                during processing that Open attempt is “equivalent” to attempting to Open the pathname “/f/g/h/j” (but\
                see the note below).\
                Note that to process a pathname such as in these examples, in which you might encounter one or\
                more symbolic links during processing that pathname, you should not attempt to build up the complete\
                pathname that the original name is “equivalent” to. Rather, as with any pathname, you should process\
                each component of the pathname one at a time. If you encounter a symbolic link, you should make a\
                recursive call to your pathname lookup procedure, attempting to lookup the target of that symbolic link,\
                before resuming processing what remains of the name you were processing when you encountered that\
                symbolic link. That is why I wrote “equivalent” in quotes above, when saying that attempting to Open\
                one pathname is “equivalent” to attempting to Open some other pathname. Attempting to Open the first\
                pathname is not literally the same as attempting to Open the second, but the final effect in terms of which\
                file ends up being Opened is the same. Processing a symbolic link as part of processing a single pathname\
                in this way is referred to as a symbolic link traversal.\
                When creating a symbolic link, it is not an error if the target of the new symbolic link does not exist.\
                Indeed, when creating a symbolic link, the target is simply recorded as-is, without any processing, except\
                that it is an error to attempt to create a symbolic link to an empty pathname (a null string). Furthermore, if\
                the target of an existing symbolic link is later deleted, this is not an error. In both cases, such a “dangling”\
                symbolic link is not a problem; however, if such a dangling symbolic link is encountered while processing\
                some other pathname (such as during an Open operation), an error would be returned upon encountering\
                the dangling symbolic link.\
                Also, note that, whereas it is an error to attempt to create a hard (regular) link to to a directory, creating\
                a symbolic link to a directory is allowed; this is not an error.\
                Within the complete processing of a single pathname passed as an argument to any Yalnix file system\
                operation, the maximum number of symbolic link traversals that may be performed is limited to\
                MAXSYMLINKS symbolic link traversals (defined in comp421/filesystem.h). If, in processing any\
                single pathname that was an argument to any Yalnix file system operation, you would need to traverse more\
                than MAXSYMLINKS symbolic links, you should terminate processing of that pathname and instead return\
                an error as the result of that Yalnix file system operation.\
                Lastly, note the following special exception to handling a symbolic link when looking up a pathname,\
                with respect to the last component of that pathname: If the last component of the pathname is the name\
                of a symbolic link, then that symbolic link must not be traversed unless the pathname is being looked up\
                for an Open, Create, or ChDir file system operation (see the definition of the file system operations in\
                Section 4.2). For example, if the last component of the pathname passed to an Unlink operation is the\
                name of a symbolic link, then the symbolic link itself should be removed, but if the last component of the\
                pathname passed to an Open operation is the name of a symbolic link, then the file to which the symbolic\
                link refers should be opened, not the symbolic link itself.\
                               Note that to process a pathname such as in these examples, in which you might encounter one or\
                more symbolic links during processing that pathname, you should not attempt to build up the complete\
                pathname that the original name is “equivalent” to. Rather, as with any pathname, you should process\
                each component of the pathname one at a time. If you encounter a symbolic link, you should make a\
                recursive call to your pathname lookup procedure, attempting to lookup the target of that symbolic link,\
                before resuming processing what remains of the name you were processing when you encountered that\
                symbolic link. That is why I wrote “equivalent” in quotes above, when saying that attempting to Open\
                one pathname is “equivalent” to attempting to Open some other pathname. Attempting to Open the first\
                pathname is not literally the same as attempting to Open the second, but the final effect in terms of which\
                file ends up being Opened is the same. Processing a symbolic link as part of processing a single pathname\
                in this way is referred to as a symbolic link traversal.\
                When creating a symbolic link, it is not an error if the target of the new symbolic link does not exist.\
                Indeed, when creating a symbolic link, the target is simply recorded as-is, without any processing, except\
                that it is an error to attempt to create a symbolic link to an empty pathname (a null string). Furthermore, if\
                the target of an existing symbolic link is later deleted, this is not an error. In both cases, such a “dangling”\
                symbolic link is not a problem; however, if such a dangling symbolic link is encountered while processing\
                some other pathname (such as during an Open operation), an error would be returned upon encountering\
                the dangling symbolic link.\
                Also, note that, whereas it is an error to attempt to create a hard (regular) link to to a directory, creating\
                a symbolic link to a directory is allowed; this is not an error.\
                Within the complete processing of a single pathname passed as an argument to any Yalnix file system\
                operation, the maximum number of symbolic link traversals that may be performed is limited to\
                MAXSYMLINKS symbolic link traversals (defined in comp421/filesystem.h). If, in processing any\
                single pathname that was an argument to any Yalnix file system operation, you would need to traverse more\
                than MAXSYMLINKS symbolic links, you should terminate processing of that pathname and instead return\
                an error as the result of that Yalnix file system operation.\
                Lastly, note the following special exception to handling a symbolic link when looking up a pathname,\
                with respect to the last component of that pathname: If the last component of the pathname is the name\
                of a symbolic link, then that symbolic link must not be traversed unless the pathname is being looked up\
                for an Open, Create, or ChDir file system operation (see the definition of the file system operations in\
                Section 4.2). For example, if the last component of the pathname passed to an Unlink operation is the\
                name of a symbolic link, then the symbolic link itself should be removed, but if the last component of the\
                Also, note that, whereas it is an error to attempt to create a hard (regular) link to to a directory, creating\
                a symbolic link to a directory is allowed; this is not an error.\
                Within the complete processing of a single pathname passed as an argument to any Yalnix file system\
                operation, the maximum number of symbolic link traversals that may be performed is limited to\
                MAXSYMLINKS symbolic link traversals (defined in comp421/filesystem.h). If, in processing any\
                single pathname that was an argument to any Yalnix file system operation, you would need to traverse more\
                than MAXSYMLINKS symbolic links, you should terminate processing of that pathname and instead return\
                an error as the result of that Yalnix file system operation.\
                Lastly, note the following special exception to handling a symbolic link when looking up a pathname,\
                with respect to the last component of that pathname: If the last component of the pathname is the name\
                of a symbolic link, then that symbolic link must not be traversed unless the pathname is being looked up\
                for an Open, Create, or ChDir file system operation (see the definition of the file system operations in\
                              null character, is limited to MAXPATHNAMELEN characters. This limit of MAXPATHNAMELEN characters\
                applies only to the length of the pathname string when presented as an argument to a Yalnix file system\
                call. The fact of whether this pathname is an absolute pathname or a relative pathname, or the possible\
                presence of symbolic links encountered while processing this pathname, do not count against that limit of\
                MAXPATHNAMELEN characters. The limit of MAXPATHNAMELEN characters literally applies only to the\
                argument of the call itself.\
                Within each directory, two special directory entries must exist (created by the MkDir call):\
                • “.” (dot) : This directory entry has the name “.” and the inode number of the directory within which\
                it is contained.\
                • “..” (dot dot) : This directory entry has the name “..” and the inode number of the parent directory\
                of the directory within which it is contained. In the root directory, the “..” entry instead has the\
                same inode number as “.” (the inode number of the root directory, which is defined as ROOTINODE\
                in comp421/filesystem.h).\
                The “.” and “..” entries are created in a directory when it is created (by the MkDir request) as the first\
                two entries in the new directory. These two directory entries subsequently cannot be explicitly deleted, but\
                are automatically deleted along with the rest of the directory on a successful RmDir request. The “.” and\
                “..” entries must be included in the nlink count in the inode of the directory to which each points.\
                2.5 Symbolic Links\
                NOTE: Support for symbolic links in this project is optional and will not affect your grade. Symbolic\
                links are defined here for those who want to experiment with them in their implementation, and to give a\
                clear example of how symbolic links fit into a “real” file system. You are strongly encouraged—but not\
                required—to implement support for symbolic links in your file system.\
                The Yalnix file system can support symbolic links, as in the Unix file system. A symbolic link to some\
                other file is represented in the Yalnix file system by an inode of type INODE_SYMLINK; the format of this\
                file is otherwise the same as an INODE_REGULAR file. However, the contents of this file (the data stored\
                in the data blocks hanging off of this inode) is interpreted by the file system as the name of the file to which\
                this symbolic link is linked. Note that the length of this name is the entire length of the data in the file, as\
                given by the size field in the inode, and the name as recorded here is not terminated by a null ()\
                character.\
                The file name to which a symbolic link points may be either an absolute pathname or a relative pathname.\
                If a relative pathname, it is interpreted relative to the directory in which this symbolic link file\
                itself occurs; that is, the processing of the symbolic link target begins with the current lookup inode (see\
                Section 2.4) being the inode of the directory in which the symbolic link itself was found. For example, consider\
                the pathname “/a/b/c”, where “b” within this pathname is a symbolic link to the relative pathname\
                “x/y”. Since the target of the symbolic link “b” in this example is a relative pathname, “x/y”, the search\
                for “x” when, for example, attempting to Open the name “/a/b/c”, begins in the same directory in which\
                the name “b” itself was found. Thus, attempting to Open the file “/a/b/c” is ultimately “equivalent” to\
                attempting to Open the name “/a/x/y/c”. If, instead, “b” within the pathname “/a/b/c” is a symbolic\
                link to the absolute pathname “/p/q”, then attempting to open the file “/a/b/c” is then “equivalent” to\
                attempting to open the name “/p/q/c” (but see the note below).The combined effect of attempting to Open this original pathname and encountering these symbolic links \
                during processing that Open attempt is “equivalent” to attempting to Open the pathname “/f/g/h/j” (but\
                see the note below).\
                Note that to process a pathname such as in these examples, in which you might encounter one or\
                more symbolic links during processing that pathname, you should not attempt to build up the complete\
                pathname that the original name is “equivalent” to. Rather, as with any pathname, you should process\
                each component of the pathname one at a time. If you encounter a symbolic link, you should make a\
                recursive call to your pathname lookup procedure, attempting to lookup the target of that symbolic link,\
                before resuming processing what remains of the name you were processing when you encountered that\
                symbolic link. That is why I wrote “equivalent” in quotes above, when saying that attempting to Open\
                one pathname is “equivalent” to attempting to Open some other pathname. Attempting to Open the first\
                pathname is not literally the same as attempting to Open the second, but the final effect in terms of which\
                file ends up being Opened is the same. Processing a symbolic link as part of processing a single pathname\
                in this way is referred to as a symbolic link traversal.\
                When creating a symbolic link, it is not an error if the target of the new symbolic link does not exist.\
                Indeed, when creating a symbolic link, the target is simply recorded as-is, without any processing, except\
                that it is an error to attempt to create a symbolic link to an empty pathname (a null string). Furthermore, if\
                the target of an existing symbolic link is later deleted, this is not an error. In both cases, such a “dangling”\
                symbolic link is not a problem; however, if such a dangling symbolic link is encountered while processing\
                some other pathname (such as during an Open operation), an error would be returned upon encountering\
                the dangling symbolic link.\
                Also, note that, whereas it is an error to attempt to create a hard (regular) link to to a directory, creating\
                a symbolic link to a directory is allowed; this is not an error.\
                Within the complete processing of a single pathname passed as an argument to any Yalnix file system\
                operation, the maximum number of symbolic link traversals that may be performed is limited to\
                MAXSYMLINKS symbolic link traversals (defined in comp421/filesystem.h). If, in processing any\
                single pathname that was an argument to any Yalnix file system operation, you would need to traverse more\
                than MAXSYMLINKS symbolic links, you should terminate processing of that pathname and instead return\
                an error as the result of that Yalnix file system operation.\
                Lastly, note the following special exception to handling a symbolic link when looking up a pathname,\
                with respect to the last component of that pathname: If the last component of the pathname is the name\
                of a symbolic link, then that symbolic link must not be traversed unless the pathname is being looked up\
                for an Open, Create, or ChDir file system operation (see the definition of the file system operations in\
                Section 4.2). For example, if the last component of the pathname passed to an Unlink operation is the\
                name of a symbolic link, then the symbolic link itself should be removed, but if the last component of the\
                pathname passed to an Open operation is the name of a symbolic link, then the file to which the symbolic\
                link refers should be opened, not the symbolic link itself.\
                               Note that to process a pathname such as in these examples, in which you might encounter one or\
                more symbolic links during processing that pathname, you should not attempt to build up the complete\
                pathname that the original name is “equivalent” to. Rather, as with any pathname, you should process\
                each component of the pathname one at a time. If you encounter a symbolic link, you should make a\
                recursive call to your pathname lookup procedure, attempting to lookup the target of that symbolic link,\
                before resuming processing what remains of the name you were processing when you encountered that\
                symbolic link. That is why I wrote “equivalent” in quotes above, when saying that attempting to Open\
                one pathname is “equivalent” to attempting to Open some other pathname. Attempting to Open the first\
                pathname is not literally the same as attempting to Open the second, but the final effect in terms of which\
                file ends up being Opened is the same. Processing a symbolic link as part of processing a single pathname\
                in this way is referred to as a symbolic link traversal.\
                When creating a symbolic link, it is not an error if the target of the new symbolic link does not exist.\
                Indeed, when creating a symbolic link, the target is simply recorded as-is, without any processing, except\
                that it is an error to attempt to create a symbolic link to an empty pathname (a null string). Furthermore, if\
                the target of an existing symbolic link is later deleted, this is not an error. In both cases, such a “dangling”\
                symbolic link is not a problem; however, if such a dangling symbolic link is encountered while processing\
                some other pathname (such as during an Open operation), an error would be returned upon encountering\
                the dangling symbolic link.\
                Also, note that, whereas it is an error to attempt to create a hard (regular) link to to a directory, creating\
                a symbolic link to a directory is allowed; this is not an error.\
                Within the complete processing of a single pathname passed as an argument to any Yalnix file system\
                operation, the maximum number of symbolic link traversals that may be performed is limited to\
                MAXSYMLINKS symbolic link traversals (defined in comp421/filesystem.h). If, in processing any\
                single pathname that was an argument to any Yalnix file system operation, you would need to traverse more\
                than MAXSYMLINKS symbolic links, you should terminate processing of that pathname and instead return\
                an error as the result of that Yalnix file system operation.\
                Lastly, note the following special exception to handling a symbolic link when looking up a pathname,\
                with respect to the last component of that pathname: If the last component of the pathname is the name\
                of a symbolic link, then that symbolic link must not be traversed unless the pathname is being looked up\
                for an Open, Create, or ChDir file system operation (see the definition of the file system operations in\
                Section 4.2). For example, if the last component of the pathname passed to an Unlink operation is the\
                name of a symbolic link, then the symbolic link itself should be removed, but if the last component of the\
                pathname passed to an Open operation is the name of a symbolic link, then the file to which the symbolic";

    printf("article len %d\n", strlen(article));
    fd = Create("/xxxxxx");
    printf("Create fd %d\n", fd);

    nch = Write(fd, article, 90000);
    printf("Write nch %d\n", nch);

    status = Close(fd);
    printf("Close status %d\n", status);

    Sync();

    fd = Open("/xxxxxx");
    printf("Open fd %d\n", fd);

	nch = Read(fd, &ch, 90000);
	printf("Read nch %d\n", nch);
    // printf("ch %s \n", ch);

    status = Close(fd);
    printf("Close status %d\n", status);

    Shutdown();
}
