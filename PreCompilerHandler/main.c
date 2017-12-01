#include <stdio.h>
#include "JsonOper.h"
#include "PreCompileOper.h"

int main(int argc, char **argv)
{
    getSoftWareConfInfo();
    PreCompileInfoList *begin = ExtractInfoFromJson();
    
    ExecPreCompile(begin);
    
    PreCompileInfoList *current = begin;
    while(current != NULL)
    {
        begin = begin->next;
        free(current);
        current = begin;
    }
    
	return 0;
}
