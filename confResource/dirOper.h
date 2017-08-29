#ifndef __DIROPER_H__
#define __DIROPER_H__

#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include "logOper.h"

/*********************************
 * func: create a directory
 * return: true = success    false = failure
 * @para path: directory path
**********************************/
bool createDir(char *path);

/**********************************
 * func: delete a directory
 * return: true = success    false = failure
 * @para path: directory path
**********************************/
bool deleteDir(char *path);

#endif