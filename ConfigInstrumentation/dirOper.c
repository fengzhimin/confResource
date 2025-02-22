/******************************************************
* Author       : fengzhimin
* Email        : 374648064@qq.com
* Filename     : dirOper.c
* Descripe     : common directory operation
******************************************************/

#include "dirOper.h"

static char error_info[LOGINFO_LENGTH];

bool createDir(char *path)
{
    if(mkdir(path, S_IRWXU|S_IRGRP|S_IXGRP|S_IROTH) < 0)
    {
         memset(error_info, 0, LOGINFO_LENGTH);
         sprintf(error_info, "create %s failed: %s.\n", path, strerror(errno));
         Error(error_info);
         return false;
    }
    
    return true;
}

bool deleteDir(char *path)
{
    DIR *dir;
    struct dirent *entry;
    char dirPath[MAX_PATH_LENGTH];

    dir = opendir(path);
    if (dir == NULL)
    {
        memset(error_info, 0, LOGINFO_LENGTH);
        sprintf(error_info, "open %s failed: %s.\n", path, strerror(errno));
        Error(error_info);
        return false;
    }

    while ((entry = readdir(dir)) != NULL)
    {
        if (strcmp(entry->d_name, ".") && strcmp(entry->d_name, ".."))
        {
            snprintf(dirPath, (size_t) MAX_PATH_LENGTH, "%s/%s", path, entry->d_name);
            if (entry->d_type == DT_DIR)
            {
                deleteDir(dirPath);
            }
            else
            {
                // delete file
                unlink(dirPath);
            }
        }
    }
    closedir(dir);

    // now we can delete the empty dir
    rmdir(path);
    
    return true;
}
