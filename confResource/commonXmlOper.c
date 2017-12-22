/******************************************************
* Author       : fengzhimin
* Email        : 374648064@qq.com
* Filename     : commonXmlOper.c
* Descripe     : common operate xml file
******************************************************/

#include "commonXmlOper.h"

static char error_info[LOGINFO_LENGTH];
static char src_dir[DIRPATH_MAX];

bool JudgeVarUsedFromNode(xmlNodePtr cur, char *var, bool flag)
{
    while(cur != NULL)
    {
        if(!xmlStrcmp(cur->name, (const xmlChar*)"expr"))
        {
            char *varName = NULL;
            //处理mysql中_current_thd()->variables.read_buff_size情况
            varName = (char*)xmlNodeGetContent(cur);
            if(strcasecmp(varName, var) == 0)
            {
                xmlFree(varName);
                return true;
            }
            xmlFree(varName);
            xmlNodePtr temp_cur = cur->children;
            while(temp_cur != NULL)
            {
                if(!xmlStrcmp(temp_cur->name, (const xmlChar*)"name"))
                {
                    varName = (char*)xmlNodeGetContent(temp_cur);
                    //server.port <---> server.port[0]
                    if(strcasecmp(varName, var) == 0)
                    {
                        xmlFree(varName);
                        return true;
                    }
                    else if(strstr(varName, var) != NULL && varName[strlen(var)] == '[')
                    {
                        xmlFree(varName);
                        return true;
                    }
                    xmlFree(varName); 
                }
                
                temp_cur = temp_cur->next;
            }
        }
        
        if(JudgeVarUsedFromNode(cur->children, var, false))
            return true;
        if(flag)
            break;
        cur = cur->next;
    }
    
    return false;
}

bool JudgeExistChildNodeFromNode(xmlNodePtr cur, char *nodeName, bool flag)
{
    while(cur != NULL)
    {
        if(!xmlStrcmp(cur->name, (const xmlChar*)nodeName))
            return true;
        else if(JudgeExistChildNodeFromNode(cur->children, nodeName, false))
            return true;
        if(flag)
            break;
        cur = cur->next;
    }
    
    return false;
}

bool JudgeArgumentSimilar(char *funcName, char *xmlFilePath, char *arg1, char *arg2)
{
    //判断是否有重名的函数存在
    char tempSqlCommand[LINE_CHAR_MAX_NUM] = "";
    sprintf(tempSqlCommand, "select funcName from %s where funcName='%s' and sourceFile='%s'", funcScoreTableName, funcName, xmlFilePath);
    MYSQL temp_db;
    MYSQL *tempMysqlConnect = NULL;
    tempMysqlConnect = mysql_init(&temp_db);
    if(tempMysqlConnect == NULL)
    {
        Error("init mysql failure\n");
        return NULL;
    }
    if(NULL == mysql_real_connect((MYSQL *)tempMysqlConnect, bind_address, user, pass, database, port, NULL, 0))
    {
        memset(error_info, 0, LOGINFO_LENGTH);
        sprintf(error_info, "connect failed: %s\n", mysql_error(tempMysqlConnect));
        Error(error_info);
        mysql_close(tempMysqlConnect);
        return NULL;
    }
    if(mysql_real_query(tempMysqlConnect, tempSqlCommand, strlen(tempSqlCommand)) != 0)
    {
        memset(error_info, 0, LOGINFO_LENGTH);
        sprintf(error_info, "execute command failed: %s\n", mysql_error(tempMysqlConnect));
        Error(error_info);
        mysql_close(tempMysqlConnect);
        return NULL;
    }
    else
    {
        MYSQL_RES *res_ptr;
        res_ptr = mysql_store_result(tempMysqlConnect);
        int rownum = mysql_num_rows(res_ptr);
        mysql_free_result(res_ptr);
        if(rownum == 1)
        {
            //只存在一个匹配的函数，直接返回
            mysql_close(tempMysqlConnect);
            return true;
        }
    }
    mysql_close(tempMysqlConnect);
        
    int argNum1 = arg1[strlen(arg1)-2] - '0';
    int argNum2 = arg2[strlen(arg2)-2] - '0';
    if(argNum1 == argNum2)
    {
        //argument number is equal
        if(argNum1 == 0)
            return true;

        char arg1Cpy[512];
        char arg2Cpy[512];
        memset(arg1Cpy, 0, 512*sizeof(char));
        memset(arg2Cpy, 0, 512*sizeof(char));
        strcpy(arg1Cpy, arg1);
        strcpy(arg2Cpy, arg2);
        int len = strlen(arg1);
        int index = 0;
        //remove (、)、# and number char 
        for(int i = 1; i < len; i++)
        {
            if(arg1Cpy[i] == '#')
            {
                arg1Cpy[index] = '\0';
                break;
            }
            arg1Cpy[index++] = arg1Cpy[i];
        }
        len = strlen(arg2);
        index = 0;
        //remove (、)、# and number char 
        for(int i = 1; i < len; i++)
        {
            if(arg2Cpy[i] == '#')
            {
                arg2Cpy[index] = '\0';
                break;
            }
            arg2Cpy[index++] = arg2Cpy[i];
        }
        if(argNum1 == 1)
        {
            //only one argument
            if(strcasecmp(arg1Cpy, "non") == 0 || strcasecmp(arg2Cpy, "non") == 0 || strcasecmp(arg1Cpy, arg2Cpy) == 0)
                return true;
            else
                return false;
        }
        else
        {
            //more than on argument
            char (*arguType1)[MAX_SUBSTR] = (char (*)[MAX_SUBSTR])malloc(argNum1*MAX_SUBSTR);
            char (*arguType2)[MAX_SUBSTR] = (char (*)[MAX_SUBSTR])malloc(argNum2*MAX_SUBSTR);
            cutStrByLabel(arg1Cpy, '/', arguType1, argNum1);
            cutStrByLabel(arg2Cpy, '/', arguType2, argNum2);
            for(int i = 0; i < argNum1; i++)
            {
                if(strcasecmp(arguType1[i], "non") != 0 && strcasecmp(arguType2[i], "non") != 0 && strcasecmp(arguType1[i], arguType2[i]) != 0)
                {
                    free(arguType1);
                    free(arguType2);
                    return false;
                }
            }
            free(arguType1);
            free(arguType2);
            return true;
        }
    }
    
    return false;
}

//example static struct stu *stu1, *stu2, stu3;
//type = static struct stu
varType *ExtractVarDefFromNode(xmlNodePtr cur, bool flag)
{
    varType *begin, *end, *current;
    begin = end = current = NULL;
    while(cur != NULL)
    {
        if(!xmlStrcmp(cur->name, (const xmlChar*)"decl_stmt"))
        {
            if(begin == NULL)
                begin = end = malloc(sizeof(varType));
            else
                end = end->next = malloc(sizeof(varType));
            memset(end, 0, sizeof(varType));
            
            xmlNodePtr temp_cur = cur->children;
            xmlChar* attr_value = getLine(cur->last);
            char type[MAX_VARIABLE_LENGTH];
            memset(type, 0, MAX_VARIABLE_LENGTH);
            while(temp_cur != NULL)
            {
                if(!xmlStrcmp(temp_cur->name, (const xmlChar*)"decl"))
                {
                    if(temp_cur->prev == NULL)
                    {
                        xmlNodePtr temp = temp_cur->children;
                        while(temp != NULL)
                        {
                            //handle static
                            if(!xmlStrcmp(temp->name, (const xmlChar*)"specifier"))
                            {
                                char *value = (char*)xmlNodeGetContent(temp);
                                strcat(type, value);
                                xmlFree(value);
                            }
                            //struct stu *
                            else if(!xmlStrcmp(temp->name, (const xmlChar*)"type"))
                            {
                                xmlNodePtr temp_type = temp->children;
                                bool label = true;
                                while(temp_type != NULL)
                                {
                                    //handle struct stu
                                    if(!xmlStrcmp(temp_type->name, (const xmlChar*)"name"))
                                    {
                                        if(!xmlStrcmp(temp_type->children->name, (const xmlChar*)"text"))
                                        {
                                            if(strlen(type) != 0)
                                                strcat(type, " ");
                                            char *value = (char*)xmlNodeGetContent(temp_type);
                                            strcat(type, value);
                                            xmlFree(value);
                                        }
                                        else
                                        {
                                            xmlNodePtr temp_name = temp_type->children;
                                            while(temp_name != NULL)
                                            {
                                                if(strlen(type) != 0)
                                                    strcat(type, " ");
                                                char *value = (char*)xmlNodeGetContent(temp_name);
                                                strcat(type, value);
                                                xmlFree(value);
                                                temp_name = temp_name->next;
                                            }
                                        }
                                    }
                                    //handle *
                                    else if(!xmlStrcmp(temp_type->name, (const xmlChar*)"modifier"))
                                    {
                                        if(label)
                                        {
                                            strcat(end->type, type);
                                            strcat(end->type, " ");
                                            label = false;
                                        }
                                        char *value = (char*)xmlNodeGetContent(temp_type);
                                        strcat(end->type, value);
                                        xmlFree(value);
                                    }
                                    temp_type = temp_type->next;
                                }
                            }
                            //handle stu1
                            else if(!xmlStrcmp(temp->name, (const xmlChar*)"name"))
                            {
                                xmlNodePtr temp_name = temp->children;
                                if(temp_name != NULL)
                                {
                                    if(!xmlStrcmp(temp_name->name, (const xmlChar*)"text"))
                                    {
                                        char *value = (char*)xmlNodeGetContent(temp);
                                        strcat(end->varName, value);
                                        xmlFree(value);
                                    }
                                    else
                                    {
                                        char *value = (char*)xmlNodeGetContent(temp_name);
                                        strcat(end->varName, value);
                                        xmlFree(value);
                                    }
                                }
                                else
                                {
                                    char *value = (char*)xmlNodeGetContent(temp);
                                    strcat(end->varName, value);
                                    xmlFree(value);
                                }
                                end->line = StrToInt((char *)attr_value);
                                if(strlen(end->type) == 0)
                                {
                                    strcat(end->type, type);
                                }
                            }
                            temp = temp->next;
                        }
                    }
                    else
                    {
                        //handle stu3
                        end = end->next = malloc(sizeof(varType));
                        memset(end, 0, sizeof(varType));
                        strcat(end->type, type);
                        end->line = StrToInt((char *)attr_value);
                        xmlNodePtr temp_name = temp_cur->children->next;
                        while(xmlStrcmp(temp_name->name, (const xmlChar*)"name"))
                            temp_name = temp_name->next;
                        temp_name = temp_name->children;
                        if(!xmlStrcmp(temp_name->name, (const xmlChar*)"text"))
                        {
                            char *value = (char*)xmlNodeGetContent(temp_name->parent);
                            strcat(end->varName, value);
                            xmlFree(value);
                        }
                        else
                        {
                            char *value = (char*)xmlNodeGetContent(temp_name);
                            strcat(end->varName, value);
                            xmlFree(value);
                        }
                    }
                }
                else if(xmlStrcmp(temp_cur->name, (const xmlChar*)"text") && xmlStrcmp(temp_cur->name, (const xmlChar*)"position"))
                {
                    end = end->next = malloc(sizeof(varType));
                    memset(end, 0, sizeof(varType));
                    bool label = true;
                    //handle *stu2
                    while(temp_cur != NULL)
                    {
                        if(!xmlStrcmp(temp_cur->name, (const xmlChar*)"modifier"))
                        {
                            if(label)
                            {
                                strcat(end->type, type);
                                strcat(end->type, " ");
                                label = false;
                            }
                            char *value = (char*)xmlNodeGetContent(temp_cur);
                            strcat(end->type, value);
                            xmlFree(value);
                            end->line = StrToInt((char *)attr_value);
                        }
                        else if(!xmlStrcmp(temp_cur->name, (const xmlChar*)"decl"))
                        {
                            xmlNodePtr temp_name = temp_cur->children;

                            if(temp_name != NULL && !xmlStrcmp(temp_name->name, (const xmlChar*)"text"))
                            {
                                char *value = (char*)xmlNodeGetContent(temp_cur);
                                strcat(end->varName, value);
                                xmlFree(value);
                            }
                            else
                            {
                                while(temp_name != NULL)
                                {
                                    if(!xmlStrcmp(temp_name->name, (const xmlChar*)"name"))
                                    {
                                        xmlNodePtr child_name = temp_name->children;
                                        if(child_name != NULL && !xmlStrcmp(child_name->name, (const xmlChar*)"text"))
                                        {
                                            char *value = (char*)xmlNodeGetContent(temp_name);
                                            strcat(end->varName, value);
                                            xmlFree(value);
                                        }
                                        else if(child_name != NULL && !xmlStrcmp(child_name->name, (const xmlChar*)"name"))
                                        {
                                            char *value = (char*)xmlNodeGetContent(child_name);
                                            strcat(end->varName, value);
                                            xmlFree(value);
                                        }
                                        
                                        break;
                                    }
                                    
                                    temp_name = temp_name->next;
                                }
                            }
                            
                            break;
                        }
                        
                        temp_cur = temp_cur->next;
                    }
                }
                temp_cur = temp_cur->next;
            }
            
            xmlFree(attr_value);
        }
        else
        {
            current = ExtractVarDefFromNode(cur->children, false);
            if(begin != NULL)
                end->next = current;
            else
                begin = end = current;
            while(current != NULL)
            {
                end = current;
                current = current->next;
            }
        }
        
        if(flag)
            break;
        
        cur = cur->next;
    }
    //delete invaild data(for instance int main(argc, argv)int argc;char *argv[]{...})
    varType *temp_begin = NULL;
    temp_begin = current = begin;
    begin = end = NULL;
    while(current != NULL)
    {
        temp_begin = temp_begin->next;
        if(strcasecmp(current->varName, "") != 0)
        {
            if(begin == NULL)
                begin = end = current;
            else
                end = end->next = current;
        }
        else
        {
            free(current);
        }
        current = temp_begin;
    }
    
    return begin;
}

varType *ExtractErrorVarTypeFromNode(xmlNodePtr cur, bool flag)
{
    varType *begin, *end, *current;
    begin = end = current = NULL;
    while(cur != NULL)
    {
        if(!xmlStrcmp(cur->name, (const xmlChar*)"argument"))
        {
            xmlNodePtr temp_cur = cur->children;
            xmlChar* attr_value = NULL;//xmlGetProp(cur->last, (xmlChar*)"line");
            while(temp_cur != NULL)
            {
                if(!xmlStrcmp(temp_cur->name, (const xmlChar*)"expr"))
                {
                    xmlNodePtr temp = temp_cur->children;
                    while(temp != NULL)
                    {
                        if(!xmlStrcmp(temp->name, (const xmlChar*)"text"))
                        {
                            temp = temp->next;
                            continue;
                        }
                        if(!xmlStrcmp(temp->name, (const xmlChar*)"name"))
                        {
                            xmlNodePtr temp_node = temp->next;
                            while(temp_node != NULL)
                            {
                                if(!xmlStrcmp(temp_node->name, (const xmlChar*)"name"))
                                {
                                    if(begin == NULL)
                                        begin = end = malloc(sizeof(varType));
                                    else
                                        end = end->next = malloc(sizeof(varType));
                                    memset(end, 0, sizeof(varType));
                                    
                                    attr_value = getLine(temp_cur);
                                    end->line = StrToInt((char *)attr_value);
                                    xmlFree(attr_value);
                                    
                                    char *value = (char*)xmlNodeGetContent(temp);
                                    strcat(end->type, value);
                                    xmlFree(value);
                                    
                                    value = (char*)xmlNodeGetContent(temp_node);
                                    strcat(end->varName, value);
                                    xmlFree(value);
                                    
                                    goto next;
                                }
                                
                                temp_node = temp_node->next;
                            }
                        }
                        else 
                            break;
                        
                        temp = temp->next;
                    }
                }
                temp_cur = temp_cur->next;
            }
        }
        else
        {
            current = ExtractErrorVarTypeFromNode(cur->children, false);
            if(begin != NULL)
                end->next = current;
            else
                begin = end = current;
            while(current != NULL)
            {
                end = current;
                current = current->next;
            }
        }
        
next:
        if(flag)
            break;
        
        cur = cur->next;
    }
    
    //delete invaild data(for instance int main(argc, argv)int argc;char *argv[]{...})
    varType *temp_begin = NULL;
    temp_begin = current = begin;
    begin = end = NULL;
    while(current != NULL)
    {
        temp_begin = temp_begin->next;
        if(strcasecmp(current->varName, "") != 0)
        {
            if(begin == NULL)
                begin = end = current;
            else
                end = end->next = current;
        }
        else
        {
            free(current);
        }
        current = temp_begin;
    }
    
    if(begin != NULL)
        end->next = NULL;

    return begin;
}

//example static struct stu *stu1, *stu2, stu3;
//type = static struct stu
varType *ExtractVarTypeFromNode(xmlNodePtr cur, bool flag)
{
    varType *begin, *end, *current;
    begin = end = current = NULL;
    while(cur != NULL)
    {
        if(!xmlStrcmp(cur->name, (const xmlChar*)"decl_stmt") || !xmlStrcmp(cur->name, (const xmlChar*)"parameter"))
        {
            if(begin == NULL)
                begin = end = malloc(sizeof(varType));
            else
                end = end->next = malloc(sizeof(varType));
            memset(end, 0, sizeof(varType));
            
            xmlNodePtr temp_cur = cur->children;
            xmlChar* attr_value = NULL;//xmlGetProp(cur->last, (xmlChar*)"line");
            char type[MAX_VARIABLE_LENGTH];
            memset(type, 0, MAX_VARIABLE_LENGTH);
            while(temp_cur != NULL)
            {
                if(!xmlStrcmp(temp_cur->name, (const xmlChar*)"decl"))
                {
                    if(temp_cur->prev == NULL)
                    {
                        xmlNodePtr temp = temp_cur->children;
                        while(temp != NULL)
                        {
                            //handle static
                            if(!xmlStrcmp(temp->name, (const xmlChar*)"specifier"))
                            {
                                char *value = (char*)xmlNodeGetContent(temp);
                                strcat(type, value);
                                xmlFree(value);
                            }
                            //struct stu *
                            else if(!xmlStrcmp(temp->name, (const xmlChar*)"type"))
                            {
                                xmlNodePtr temp_type = temp->children;
                                while(temp_type != NULL)
                                {
                                    //handle struct stu
                                    if(!xmlStrcmp(temp_type->name, (const xmlChar*)"name"))
                                    {
                                        if(!xmlStrcmp(temp_type->children->name, (const xmlChar*)"text"))
                                        {
                                            char *value = (char*)xmlNodeGetContent(temp_type);
                                            strcat(type, value);
                                            xmlFree(value);
                                        }
                                        else
                                        {
                                            xmlNodePtr temp_name = temp_type->children;
                                            while(temp_name != NULL)
                                            {
                                                if(strlen(type) != 0)
                                                    strcat(type, " ");
                                                char *value = (char*)xmlNodeGetContent(temp_name);
                                                strcat(type, value);
                                                xmlFree(value);
                                                temp_name = temp_name->next;
                                            }
                                        }
                                        break;
                                    }
                                    temp_type = temp_type->next;
                                }
                            }
                            //handle stu1
                            else if(!xmlStrcmp(temp->name, (const xmlChar*)"name"))
                            {
                                xmlNodePtr temp_name = temp->children;
                                if(temp_name != NULL)
                                {
                                    if(!xmlStrcmp(temp_name->name, (const xmlChar*)"text"))
                                    {
                                        char *value = (char*)xmlNodeGetContent(temp);
                                        strcat(end->varName, value);
                                        xmlFree(value);
                                        attr_value = xmlGetProp(temp, (xmlChar*)"line");
                                    }
                                    else
                                    {
                                        char *value = (char*)xmlNodeGetContent(temp_name);
                                        strcat(end->varName, value);
                                        xmlFree(value);
                                        attr_value = xmlGetProp(temp_name, (xmlChar*)"line");
                                    }
                                }
                                else
                                {
                                    char *value = (char*)xmlNodeGetContent(temp);
                                    strcat(end->varName, value);
                                    xmlFree(value);
                                    attr_value = xmlGetProp(temp, (xmlChar*)"line");
                                }
                                end->line = StrToInt((char *)attr_value);
                                xmlFree(attr_value);
                                strcat(end->type, type);
                            }
                            temp = temp->next;
                        }
                    }
                    else
                    {
                        //handle stu3
                        end = end->next = malloc(sizeof(varType));
                        memset(end, 0, sizeof(varType));
                        strcat(end->type, type);
                        xmlNodePtr temp_name = temp_cur->children->next;
                        while(xmlStrcmp(temp_name->name, (const xmlChar*)"name"))
                            temp_name = temp_name->next;
                        temp_name = temp_name->children;
                        if(temp_name != NULL)
                        {
                            //handle array variable
                            if(!xmlStrcmp(temp_name->name, (const xmlChar*)"text"))
                            {
                                char *value = (char*)xmlNodeGetContent(temp_name->parent);
                                strcat(end->varName, value);
                                xmlFree(value);
                                attr_value = xmlGetProp(temp_name->parent, (xmlChar*)"line");
                            }
                            else
                            {
                                char *value = (char*)xmlNodeGetContent(temp_name);
                                strcat(end->varName, value);
                                xmlFree(value);
                                attr_value = xmlGetProp(temp_name, (xmlChar*)"line");
                            }
                        }
                        else
                        {
                            char *value = (char*)xmlNodeGetContent(temp_cur->children->next);
                            strcat(end->varName, value);
                            xmlFree(value);
                            attr_value = xmlGetProp(temp_cur->children->next, (xmlChar*)"line");
                        }
                        end->line = StrToInt((char *)attr_value);
                        xmlFree(attr_value);
                    }
                }
                temp_cur = temp_cur->next;
            }
        }
        else
        {
            current = ExtractVarTypeFromNode(cur->children, false);
            if(begin != NULL)
                end->next = current;
            else
                begin = end = current;
            while(current != NULL)
            {
                end = current;
                current = current->next;
            }
        }
        
        if(flag)
            break;
        
        cur = cur->next;
    }
    
    //delete invaild data(for instance int main(argc, argv)int argc;char *argv[]{...})
    varType *temp_begin = NULL;
    temp_begin = current = begin;
    begin = end = NULL;
    while(current != NULL)
    {
        temp_begin = temp_begin->next;
        if(strcasecmp(current->varName, "") != 0)
        {
            if(begin == NULL)
                begin = end = current;
            else
                end = end->next = current;
        }
        else
        {
            free(current);
        }
        current = temp_begin;
    }
    
    if(begin != NULL)
        end->next = NULL;

    return begin;
}

void ExtractGlobalVarDef(char *xmlFilePath)
{
    xmlDocPtr doc;
    xmlNodePtr cur;
    xmlKeepBlanksDefault(0);
    doc = xmlParseFile(xmlFilePath);
    if(doc == NULL )
    {
        memset(error_info, 0, LOGINFO_LENGTH);
        sprintf(error_info, "Document(%s) not parsed successfully. \n", xmlFilePath);
		Error(error_info);
        return ;
    }
    cur = xmlDocGetRootElement(doc);
    if (cur == NULL)
    {
        memset(error_info, 0, LOGINFO_LENGTH);
        sprintf(error_info, "empty document(%s). \n", xmlFilePath);
		Error(error_info);  
        xmlFreeDoc(doc);
        return ;
    }
    
    cur = cur->children;
    while (cur != NULL)
    {
        if(!xmlStrcmp(cur->name, (const xmlChar*)"decl_stmt"))
        {
            printf("global variable define info: \n");
            varType *begin = ExtractVarDef(cur);
            varType *current = begin;
            while(current != NULL)
            {
                begin = begin->next;
                printf("%s %s(%d)\n", current->type, current->varName, current->line);
                free(current);
                current = begin;
            }
        }
        cur = cur->next;
    }
      
    xmlFreeDoc(doc); 
}


void scanAssignVarFromNode(xmlNodePtr cur, bool flag)
{
    while(cur != NULL)
    {
        if(!xmlStrcmp(cur->name, (const xmlChar*)"expr"))
        {
            xmlNodePtr opt = cur->children;
            while(opt != NULL)
            {
                if(!xmlStrcmp(opt->name, (const xmlChar*)"operator"))
                { 
                    char *optName = (char*)xmlNodeGetContent(opt);
                    //handle = *= -= += operator
                    if(strstr(optName, "=") != 0)
                    {
                        xmlNodePtr name = opt->prev;
                        while(name != NULL)
                        {
                            if(!xmlStrcmp(name->name, (const xmlChar*)"name"))
                            {
                                char *varName = (char*)xmlNodeGetContent(name);
                                xmlChar* attr_value = NULL;
                                if(!xmlStrcmp(name->children->name, (const xmlChar*)"name"))
                                    attr_value = xmlGetProp(name->children, (xmlChar*)"line");
                                else
                                    attr_value = xmlGetProp(name, (xmlChar*)"line");

                                printf("%s(%s)\n", varName, attr_value);
                                xmlFree(varName);
                                xmlFree(attr_value);
                                break;
                            }
                            name = name->prev;
                        }
                    }
                    xmlFree(optName);
                }

                opt = opt->next;
            }
        }
        
        scanAssignVarFromNode(cur->children, false);
        
        if(flag)
            break;
        cur = cur->next;
    }
}

varDef *ExtractDirectInfluVarFromNode(xmlNodePtr cur, char *varName, varType *varTypeBegin, bool flag)
{
    varDef *begin = NULL;
    varDef *end = NULL;
    varDef *current = NULL;
    while(cur != NULL)
    {
        if(!xmlStrcmp(cur->name, (const xmlChar*)"expr"))
        {
            xmlNodePtr opt = cur->children;
            while(opt != NULL)
            {
                if(!xmlStrcmp(opt->name, (const xmlChar*)"operator"))
                { 
                    char *optName = (char*)xmlNodeGetContent(opt);
                    //handle frequently used = *= -= += /= |= operator
                    if(strcasecmp(optName, "=") == 0 || strcasecmp(optName, "+=") == 0 || strcasecmp(optName, "-=") == 0 ||\
                    strcasecmp(optName, "*=") == 0 || strcasecmp(optName, "/=") == 0 || strcasecmp(optName, "|=") == 0)
                    {
                        if(JudgeVarUsed(opt->parent, varName))
                        {
                            xmlNodePtr name = opt->prev;
                            while(name != NULL)
                            {
                                if(!xmlStrcmp(name->name, (const xmlChar*)"name"))
                                {
                                    char *influVarName = (char*)xmlNodeGetContent(name);
                                    //remove varName = varName + 1
                                    if(strcasecmp(influVarName, varName) != 0)
                                    {
                                        xmlChar* attr_value = getLine(name);
                                        current = begin;
                                        //whether has existence or not
                                        while(current != NULL)
                                        {
                                            if(strcasecmp(current->varName, influVarName) == 0)
                                                break;
                                            current = current->next;
                                        }
                                        if(current == NULL)
                                        {
                                            if(begin == NULL)
                                                begin = end = malloc(sizeof(varDef));
                                            else
                                                end = end->next = malloc(sizeof(varDef));
                                            memset(end, 0, sizeof(varDef));
                                            strcpy(end->varName, influVarName);
                                            varType *varTypeCurrent = varTypeBegin;
                                            //set influVarName type(local or global)
                                            while(varTypeCurrent != NULL)
                                            {
                                                //对结构体或累的成员变量进行分析，判断其是否为全局变量还是局部变量
                                                if(strstr(influVarName, varTypeCurrent->varName) != NULL)
                                                    break;
                                                varTypeCurrent = varTypeCurrent->next;
                                            }
                                            if(varTypeCurrent == NULL)
                                                end->type = true;
                                            else
                                                end->type = false;
                                            end->line = StrToInt((char *)attr_value);
                                            //printf("%s(%s)\n", influVarName, attr_value);
                                        }
                                        xmlFree(attr_value);
                                    }
                                    xmlFree(influVarName);
                                    goto next;
                                }
                                name = name->prev;
                            }
                        }
                    }
                    xmlFree(optName);
                }
next:
                opt = opt->next;
            }
        }
        else if(!xmlStrcmp(cur->name, (const xmlChar*)"decl_stmt"))
        {
            xmlNodePtr decl = cur->children;
            while(decl != NULL)
            {
                if(!xmlStrcmp(decl->name, (const xmlChar*)"decl"))
                {
                    xmlNodePtr init = decl->children;
                    while(init != NULL)
                    {
                        if(!xmlStrcmp(init->name, (const xmlChar*)"init"))
                        {
                            if(JudgeVarUsed(init, varName))
                            {
                                char *influVarName = NULL;
                                xmlChar* attr_value = NULL;
                                if(!xmlStrcmp(init->prev->children->name, (const xmlChar*)"name"))
                                {
                                    influVarName = (char*)xmlNodeGetContent(init->prev->children);
                                    attr_value = xmlGetProp(init->prev->children, (xmlChar*)"line");
                                }
                                else
                                {
                                    influVarName = (char*)xmlNodeGetContent(init->prev);
                                    attr_value = xmlGetProp(init->prev, (xmlChar*)"line");
                                }
                                if(begin == NULL)
                                    begin = end = malloc(sizeof(varDef));
                                else
                                    end = end->next = malloc(sizeof(varDef));
                                memset(end, 0, sizeof(varDef));
                                strcpy(end->varName, influVarName);
                                end->type = false;
                                end->line = StrToInt((char *)attr_value);
                                xmlFree(influVarName);
                                xmlFree(attr_value);
                                break;
                            }
                        }
                        init = init->next;
                    }
                }
                decl = decl->next;
            }
        }
        
        current = ExtractDirectInfluVarFromNode(cur->children, varName, varTypeBegin, false);
        
        if(current != NULL)
        {
            if(begin == NULL)
                begin = end = current;
            else
                end->next = current;
            while(current != NULL)
            {
                end = current;
                current = current->next;
            }
        }

        if(flag)
            break;
        cur = cur->next;
    }
    
    return begin;
}

void scanBackAssignVar(xmlNodePtr cur)
{
    if(!xmlStrcmp(cur->name, (const xmlChar*)"function"))
    {
        return ;
    }
    xmlNodePtr temp_cur = cur;
    cur = cur->next;
    while(cur != NULL)
    {
        scanAssignVar(cur);
        cur = cur->next;
    }
    
    scanBackAssignVar(temp_cur->parent);
}

char *ExtractErrorFuncArgumentType(xmlNodePtr cur)
{
    xmlNodePtr argument_list = cur;
    char *retTypeString = malloc(sizeof(char)*512);
    memset(retTypeString, 0, sizeof(char)*512);
    bool point_void = false;
    bool point = false;

    if(!xmlStrcmp(argument_list->name, (const xmlChar*)"argument_list"))
    {
        xmlNodePtr argument = argument_list->children;
        while(argument != NULL)
        {
            if(!xmlStrcmp(argument->name, (const xmlChar*)"argument"))
            {
                xmlNodePtr expr = argument->children;
                while(expr != NULL)
                {
                    if(!xmlStrcmp(expr->name, (const xmlChar*)"expr"))
                    {
                        xmlNodePtr name = expr->children;
                        while(name != NULL)
                        {
                            if(!xmlStrcmp(name->name, (const xmlChar*)"name"))
                            {
                                char *value = (char*)xmlNodeGetContent(name);
                                if(strlen(retTypeString) == 0)
                                {
                                    sprintf(retTypeString, "(%s", value);
                                }
                                else
                                {
                                    sprintf(retTypeString, "%s/%s", retTypeString, value);
                                }
                                xmlFree(value);
                                    
                                goto argumentNext;
                            }
                            name = name->next;
                        }
                    }
                    expr = expr->next;
                }
            }
        argumentNext:
            argument = argument->next;
        }
    }
    
    if(strlen(retTypeString) == 0)
        strcpy(retTypeString, "(void#0)");
    else
    {
        int charNum = getSpecCharNumFromStr(retTypeString, '/') + 1;
        if(charNum > 1)
        {
            sprintf(retTypeString, "%s#%d", retTypeString, charNum);
        }
        else
        {
            if(point_void && !point)
                sprintf(retTypeString, "%s#%d", retTypeString, 0);
            else
                sprintf(retTypeString, "%s#%d", retTypeString, charNum);
        }
        
        strcat(retTypeString, ")");
    }
    
    return retTypeString;
}

char *ExtractFuncArgumentType(xmlNodePtr cur)
{
    xmlNodePtr argument_list = cur->children;
    char *retTypeString = malloc(sizeof(char)*512);
    memset(retTypeString, 0, sizeof(char)*512);
    bool point_void = false;
    bool point = false;
    while(argument_list != NULL)
    {
        //非正确解析的函数定义中参数列表使用argument_list标签
        if(!xmlStrcmp(argument_list->name, (const xmlChar*)"parameter_list"))
        {
            xmlNodePtr parameter = argument_list->children;
            while(parameter != NULL)
            {
                if(!xmlStrcmp(parameter->name, (const xmlChar*)"parameter"))
                {
                    xmlNodePtr decl = parameter->children;
                    while(decl != NULL)
                    {
                        if(!xmlStrcmp(decl->name, (const xmlChar*)"decl"))
                        {
                            xmlNodePtr type = decl->children;
                            while(type != NULL)
                            {
                                if(!xmlStrcmp(type->name, (const xmlChar*)"type"))
                                {
                                    xmlNodePtr name = type->children;
                                    while(name != NULL)
                                    {
                                        if(!xmlStrcmp(name->name, (const xmlChar*)"name"))
                                        {
                                            if(strlen(retTypeString) == 0)
                                            {
                                                char *value = (char*)xmlNodeGetContent(name);
                                                if(strcasecmp(value, "void") == 0)
                                                {
                                                    point_void = true;
                                                    while(type != NULL)
                                                    {
                                                        
                                                        //判断函数参数是否为void *类型
                                                        if(!xmlStrcmp(type->name, (const xmlChar*)"name"))
                                                        {
                                                            point = true;
                                                            break;
                                                        }
                                                        type = type->next;
                                                    }
                                                }
                                                sprintf(retTypeString, "(%s", value);
                                                xmlFree(value);
                                            }
                                            else
                                            {
                                                char *value = (char*)xmlNodeGetContent(name);
                                                sprintf(retTypeString, "%s/%s", retTypeString, value);
                                                xmlFree(value);
                                            }
                                                
                                            goto parameterNext;
                                        }
                                        name = name->next;
                                    }
                                }
                                type = type->next;
                            }
                        }
                        decl = decl->next;
                    }
                }
            parameterNext:
                parameter = parameter->next;
            }
            
            break;
        }
        else if(!xmlStrcmp(argument_list->name, (const xmlChar*)"argument_list"))
        {
            xmlNodePtr argument = argument_list->children;
            while(argument != NULL)
            {
                if(!xmlStrcmp(argument->name, (const xmlChar*)"argument"))
                {
                    xmlNodePtr expr = argument->children;
                    while(expr != NULL)
                    {
                        if(!xmlStrcmp(expr->name, (const xmlChar*)"expr"))
                        {
                            xmlNodePtr name = expr->children;
                            while(name != NULL)
                            {
                                if(!xmlStrcmp(name->name, (const xmlChar*)"name"))
                                {
                                    char *value = (char*)xmlNodeGetContent(name);
                                    if(strlen(retTypeString) == 0)
                                    {
                                        sprintf(retTypeString, "(%s", value);
                                    }
                                    else
                                    {
                                        sprintf(retTypeString, "%s/%s", retTypeString, value);
                                    }
                                    xmlFree(value);
                                        
                                    goto argumentNext;
                                }
                                name = name->next;
                            }
                        }
                        expr = expr->next;
                    }
                }
            argumentNext:
                argument = argument->next;
            }
            
            break;
        }
        argument_list = argument_list->next;
    }
    
    if(strlen(retTypeString) == 0)
        strcpy(retTypeString, "(void#0)");
    else
    {
        int charNum = getSpecCharNumFromStr(retTypeString, '/') + 1;
        if(charNum > 1)
        {
            sprintf(retTypeString, "%s#%d", retTypeString, charNum);
        }
        else
        {
            if(point_void && !point)
                sprintf(retTypeString, "%s#%d", retTypeString, 0);
            else
                sprintf(retTypeString, "%s#%d", retTypeString, charNum);
        }
        
        strcat(retTypeString, ")");
    }
    
    return retTypeString;
}

char *getCalledFuncArgumentType(xmlNodePtr cur, varType *funcDefVarType)
{
    char *retTypeString = malloc(sizeof(char)*512);
    memset(retTypeString, 0, sizeof(char)*512);
    xmlNodePtr argument_list = cur->children;
    while(argument_list != NULL)
    {
        if(!xmlStrcmp(argument_list->name, (const xmlChar*)"argument_list"))
        {
            xmlNodePtr argument = argument_list->children;
            while(argument != NULL)
            {
                if(!xmlStrcmp(argument->name, (const xmlChar*)"argument"))
                {
                    xmlNodePtr expr = argument->children;
                    while(expr != NULL)
                    {
                        if(!xmlStrcmp(expr->name, (const xmlChar*)"expr"))
                        {
                            if(!xmlStrcmp(expr->last->name, (const xmlChar*)"name"))
                            {
                                //get parameter name
                                char *parameterName = (char*)xmlNodeGetContent(expr->last);
                                varType *current = funcDefVarType;
                                bool findResult = false;
                                while(current != NULL)
                                {
                                    if(strcasecmp(current->varName, parameterName) == 0)
                                    {
                                        findResult = true;
                                        if(strlen(retTypeString) == 0)
                                            sprintf(retTypeString, "(%s", current->type);
                                        else
                                        {
                                            sprintf(retTypeString, "%s/%s", retTypeString, current->type);
                                        }
                                        break;
                                    }
                                    current = current->next;
                                }
                                xmlFree(parameterName);
                                if(!findResult)
                                {
                                    //don't find variable type
                                    if(strlen(retTypeString) == 0)
                                        strcpy(retTypeString, "(non");
                                    else
                                    {
                                        sprintf(retTypeString, "%s/%s", retTypeString, "non");
                                    }
                                }
                            }
                            else
                            {
                                //function call as argument or constant as argument
                                if(strlen(retTypeString) == 0)
                                    strcpy(retTypeString, "(non");
                                else
                                {
                                    sprintf(retTypeString, "%s/%s", retTypeString, "non");
                                }
                            }
                            
                            break;
                        }
                        expr = expr->next;
                    }
                }
                argument = argument->next;
            }
            
            break;
        }
        argument_list = argument_list->next;
    }
    
    if(strlen(retTypeString) == 0)
        strcpy(retTypeString, "(void#0)");
    else
    {
        int charNum = getSpecCharNumFromStr(retTypeString, '/') + 1;
        sprintf(retTypeString, "%s#%d", retTypeString, charNum);
        strcat(retTypeString, ")");
    }
    
    return retTypeString;
}

char *getFuncName(xmlNodePtr funcNode)
{
    char *funcName = NULL;
    while(funcNode != NULL)
    {
        if(!xmlStrcmp(funcNode->name, (const xmlChar*)"name"))
        {
            funcName = (char*)xmlNodeGetContent(funcNode);
            if(strcasecmp("__attribute__", funcName) == 0)
            {
                xmlFree(funcName);
                funcName = NULL;
            }
            
            break;
        }
        funcNode = funcNode->next;
    }
    
    return funcName;
}

xmlChar *getLine(xmlNodePtr cur)
{
    if(cur == NULL)
        return NULL;
    xmlChar *line = xmlGetProp(cur, (xmlChar*)"line");
    if(line != NULL)
        return line;
    else
    {
        cur = cur->children;
        while(cur != NULL)
        {
            line = getLine(cur);
            if(line != NULL)
                return line;
            cur = cur->next;
        }
    }
    
    return line;
}

varDef *ScliceInflVarInfo(char *varName, xmlNodePtr cur, char *inflVarName, varDef *curInflVar, varType *varTypeBegin)
{
    varDef *begin = NULL;
    varDef *endInflVar = curInflVar;
    if(endInflVar != NULL)
    {
        while(endInflVar->next != NULL)
            endInflVar = endInflVar->next;
    }
    
    varDef *end = NULL;
    varDef *current = NULL;
    if(curInflVar == NULL)
        begin = end = current = ExtractDirectInfluVar(cur, varName, varTypeBegin);
    else
        begin = end = current = ExtractDirectInfluVar(cur, inflVarName, varTypeBegin);
    
    if(begin != NULL)
    {
        current = begin;
        while(current != NULL)
        {
            end = current;
            current = current->next;
        }
        varDef *varInfluCur = begin;
        while(varInfluCur != NULL)
        {
            bool isExist = false;
            varDef *tmpCurInflVar = NULL;
            for(tmpCurInflVar = curInflVar; tmpCurInflVar != NULL; tmpCurInflVar = tmpCurInflVar->next)
            {
                if(strcasecmp(varInfluCur->varName, tmpCurInflVar->varName) == 0)
                {
                    isExist = true;
                    break;
                }
            }
            if(!isExist)
            {
                if(strcasecmp(varInfluCur->varName, varName) != 0)
                {
                    if(curInflVar == NULL)
                        endInflVar = curInflVar = malloc(sizeof(varType));
                    else
                        endInflVar = endInflVar->next = malloc(sizeof(varType));
                    memset(endInflVar, 0, sizeof(varDef));
                    strcpy(endInflVar->varName, varInfluCur->varName);
                    endInflVar->line = varInfluCur->line;
                    endInflVar->type = varInfluCur->type;
                    
                    varDef *tmpBegin = ScliceInflVarInfo(varName, cur, varInfluCur->varName, curInflVar, varTypeBegin);
                    if(tmpBegin != NULL)
                    {
                        varDef *tmpCurrent = NULL;
                        for(tmpCurrent = tmpBegin; tmpCurrent != NULL; tmpCurrent = tmpCurrent->next)
                        {
                            isExist = false;
                            //只影响后的变量
                            if(tmpCurrent->line < varInfluCur->line)
                                isExist = true;
                            else
                            {
                                for(current = begin; current != NULL; current = current->next)
                                {
                                    if(strcasecmp(current->varName, tmpCurrent->varName) == 0 || strcasecmp(varName, tmpCurrent->varName) == 0)
                                    {
                                        isExist = true;
                                        break;
                                    }
                                }
                            }
                            if(!isExist)
                            {
                                end = end->next = malloc(sizeof(varDef));
                                memset(end, 0, sizeof(varDef));
                                *end = *tmpCurrent;
                                end->next = NULL;
                            }
                        }
                        tmpCurrent = tmpBegin;
                        while(tmpCurrent != NULL)
                        {
                            tmpBegin = tmpBegin->next;
                            free(tmpCurrent);
                            tmpCurrent = tmpBegin;
                        }
                    }
                }
            }

            varInfluCur = varInfluCur->next;
        }
    }
    
    if(inflVarName == NULL)
    {
        endInflVar = curInflVar;
        while(endInflVar != NULL)
        {
            curInflVar = curInflVar->next;
            free(endInflVar);
            endInflVar = curInflVar;
        }
    }
    
    return begin;
}

funcCallInfoList *ScliceErrorFromNode(char *varName, char *xmlFilePath, xmlNodePtr cur, funcInfoList *(*varScliceFunc)(varDef, xmlNodePtr , varType *, bool))
{
    funcCallInfoList *ret = NULL;
    funcCallInfoList *endFuncList = NULL;
    while (cur != NULL)
    {
        if(!xmlStrcmp(cur->name, (const xmlChar*)"decl"))
        {
            xmlNodePtr temp_cur = cur->children;
            while(temp_cur != NULL)
            {
                if(!xmlStrcmp(temp_cur->name, (const xmlChar*)"name") && temp_cur->next != NULL && !xmlStrcmp(temp_cur->next->name, (const xmlChar*)"argument_list"))
                {
                    char *value = (char*)xmlNodeGetContent(temp_cur);
                    if(strcasecmp("__attribute__", value) == 0)
                    {
                        xmlFree(value);
                        break;
                    }
                    varType *beginVarType = ExtractErrorVarType(temp_cur->next);
                    varType *currentVarType = beginVarType;
                    varDef varInfo;
                    memset(&varInfo, 0, sizeof(varDef));
                    strcpy(varInfo.varName, varName);
                    varInfo.line = 0;
                    funcInfoList *begin = NULL;
                    funcInfoList *end = NULL;
                    funcInfoList *current = NULL;
                    
                    xmlNodePtr blockNode = temp_cur->next->next;
                    while(blockNode != NULL)
                    {
                        if(!xmlStrcmp(blockNode->name, (const xmlChar*)"argument_list"))
                            break;
                        blockNode = blockNode->next;
                    }
                    
                    /*
                     * 首先获取变量varInfo变量在funcNode节点上所直接影响的代码块
                     * 直接影响分为:控制流影响和数据流影响
                     * 控制流影响: 变量varInfo作为控制条件，那么它所影响的是整个控制块
                     * 数据流影响: 变量varInfo作为函数的参数，那么它影响了该函数
                     */
                    begin = end = current = varScliceFunc(varInfo, blockNode, currentVarType, true);
                    
                    while(currentVarType != NULL)
                    {
                        beginVarType = beginVarType->next;
                        //printf("%s(%d):%s\n", current->type, current->line, current->varName);
                        free(currentVarType);
                        currentVarType = beginVarType;
                    }
                    
                    if(begin != NULL)
                    {
#if DEBUG == 1
                        memset(src_dir, 0, DIRPATH_MAX);
                        //删除开头的temp_和结尾的.xml
                        strncpy(src_dir, (char *)&(xmlFilePath[5]), strlen(xmlFilePath)-9);
                        xmlChar* funcLine = getLine(temp_cur->next);
                        printf("\033[40;36msource Path: %s\tfunction: %s(%s)\033[0m\n", src_dir, value, (char *)funcLine);
                        xmlFree(funcLine);
#endif
                        char *argumentTypeString = ExtractErrorFuncArgumentType(temp_cur->next);
                        if(argumentTypeString != NULL)
                        {
                            if(ret == NULL)
                                ret = endFuncList = malloc(sizeof(funcCallInfoList));
                            else
                                endFuncList = endFuncList->next = malloc(sizeof(funcCallInfoList));
                            memset(endFuncList, 0, sizeof(funcCallInfoList));
                            strcpy(endFuncList->info.funcName, value);
                            strncpy(endFuncList->info.sourceFile, &(xmlFilePath[5]), strlen(xmlFilePath)-9);
                            strcpy(endFuncList->info.funcArgumentType, argumentTypeString);
                            endFuncList->info.calledFuncInfo = begin;
                        }
                        else
                        {
                            Error("get function name or line error!\n");
                            return NULL;
                        }
                        
                        free(argumentTypeString);
                    }
                    
                    xmlFree(value);
                }
                
                temp_cur = temp_cur->next;
            }
        }
        
        cur = cur->next;
    }
    
    return ret;
}

funcCallInfoList *ScliceFromNode(char *varName, char *xmlFilePath, xmlNodePtr cur, funcInfoList *(*varScliceFunc)(varDef, xmlNodePtr , varType *, bool))
{
    funcCallInfoList *ret = NULL;
    funcCallInfoList *endFuncList = NULL;
    while (cur != NULL)
    {
        if(!xmlStrcmp(cur->name, (const xmlChar*)"function") || \
            (!xmlStrcmp(cur->name, (const xmlChar*)"extern") && cur->children != NULL && !xmlStrcmp(cur->last->name, (const xmlChar*)"function")) || \
            (!xmlStrcmp(cur->parent->name, (const xmlChar*)"block") && !xmlStrcmp(cur->name, (const xmlChar*)"decl_stmt") && cur->children != NULL && \
            !xmlStrcmp(cur->last->name, (const xmlChar*)"decl")))
        {
            xmlNodePtr funcNode;
            if(!xmlStrcmp(cur->name, (const xmlChar*)"function"))
                funcNode = cur;
            else
                funcNode = cur->last;
            
            varType *beginVarType = ExtractVarType(funcNode);
            varType *currentVarType = beginVarType;
            varDef varInfo;
            memset(&varInfo, 0, sizeof(varDef));
            strcpy(varInfo.varName, varName);
            varInfo.line = 0;
            funcInfoList *begin = NULL;
            funcInfoList *end = NULL;
            funcInfoList *current = NULL;
            
            xmlNodePtr blockNode = funcNode->children;
            while(blockNode != NULL)
            {
                if(!xmlStrcmp(blockNode->name, (const xmlChar*)"block"))
                    break;
                blockNode = blockNode->next;
            }
            /*
             * 首先获取变量varInfo变量在funcNode节点上所直接影响的代码块
             * 直接影响分为:控制流影响和数据流影响
             * 控制流影响: 变量varInfo作为控制条件，那么它所影响的是整个控制块
             * 数据流影响: 变量varInfo作为函数的参数，那么它影响了该函数
             */
            begin = end = current = varScliceFunc(varInfo, blockNode, currentVarType, true);
            while(current != NULL)
            {
                end = current;
                current = current->next;
            }
            
            /*
             * 获取varName变量在funcNode节点上通过数据传播所直接或间接影响的变量
             * 数据传播: 将变量的值通过一定的运算后赋值给另一个变量
             * 直接影响: 例如a变量通过运算后直接赋值给了b
             * 间接影响: 例如a变量直接影响了b，b变量直接影响了c，那么a是间接影响了c
             */
            varDef *varInfluence = ScliceInflVar(varName, blockNode, currentVarType);
            if(varInfluence != NULL)
            {
                varDef *varInfluCur = varInfluence;
                while(varInfluCur != NULL)
                {
#if DEBUG == 1
                    printf("\033[40;34m%s influence %s(%d)\033[0m\n", varName, varInfluCur->varName, varInfluCur->line);
#endif
                    //对影响的变量继续进行切片分析
                    current = varScliceFunc(*varInfluCur, blockNode, currentVarType, true);
                    if(current != NULL)
                    {
                        if(begin == NULL)
                        {
                            begin = end = current;
                        }
                        else if(current != NULL)
                        {
                            funcInfoList *tempBegin = NULL;
                            funcInfoList *tempCurrent = current;
                            while(tempCurrent != NULL)
                            {
                                tempBegin = begin;
                                while(tempBegin != NULL)
                                {
                                    if(strcasecmp(tempBegin->info.funcName, tempCurrent->info.funcName) == 0 && \
                                        tempBegin->info.calledLine == tempCurrent->info.calledLine)
                                    {
                                        if(tempCurrent->prev == NULL)
                                        {
                                            //第一个被影响的函数已经存在了
                                            current = current->next;
                                            //有且仅有一个函数
                                            if(current != NULL)
                                                current->prev = NULL;
                                            free(tempCurrent);
                                            tempCurrent = current;
                                        }
                                        else if(tempCurrent->next != NULL)
                                        {
                                            //中间的某个被影响的函数已经存在了
                                            funcInfoList *temp = tempCurrent;
                                            tempCurrent->prev->next = tempCurrent->next;
                                            tempCurrent->next->prev = tempCurrent->prev;
                                            tempCurrent = temp->next;
                                            free(temp);
                                        }
                                        else
                                        {
                                            //最后一个被影响的函数已经存在了
                                            tempCurrent->prev->next = NULL;
                                            free(tempCurrent);
                                            tempCurrent = NULL;
                                        }
                                        
                                        break;
                                    }
                                    
                                    tempBegin = tempBegin->next;
                                }
                                
                                if(tempBegin == NULL)
                                    tempCurrent = tempCurrent->next;
                            }
                            if(current != NULL)
                            {
                                end->next = current;
                                current->prev = end;
                                end = end->next;
                            }
                        }
                        //更新end变量
                        while(current != NULL)
                        {
                            end = current;
                            current = current->next;
                        }
                    }
                    varInfluence = varInfluence->next;
                    free(varInfluCur);
                    varInfluCur = varInfluence;
                }
            }
            while(currentVarType != NULL)
            {
                beginVarType = beginVarType->next;
                //printf("%s(%d):%s\n", current->type, current->line, current->varName);
                free(currentVarType);
                currentVarType = beginVarType;
            }
            
            if(begin != NULL)
            {
                
                char *funcName = getFuncName(funcNode->children);
                char *argumentTypeString = ExtractFuncArgumentType(funcNode);
#if DEBUG == 1
                memset(src_dir, 0, DIRPATH_MAX);
                //删除开头的temp_和结尾的.xml
                strncpy(src_dir, (char *)&(xmlFilePath[5]), strlen(xmlFilePath)-9);
                xmlChar* funcLine = getLine(funcNode->children);
                printf("\033[40;36msource Path: %s\tfunction: %s(%s)\033[0m\n", src_dir, funcName, (char *)funcLine);
                xmlFree(funcLine);
#endif
                if(argumentTypeString != NULL && funcName != NULL)
                {
                    if(ret == NULL)
                        ret = endFuncList = malloc(sizeof(funcCallInfoList));
                    else
                        endFuncList = endFuncList->next = malloc(sizeof(funcCallInfoList));
                    memset(endFuncList, 0, sizeof(funcCallInfoList));
                    strcpy(endFuncList->info.funcName, funcName);
                    strncpy(endFuncList->info.sourceFile, &(xmlFilePath[5]), strlen(xmlFilePath)-9);
                    strcpy(endFuncList->info.funcArgumentType, argumentTypeString);
                    endFuncList->info.calledFuncInfo = begin;
                    xmlFree(funcName);
                }
                else
                {
                    Error("get function name or line error!\n");
                    return NULL;
                }
                
                free(argumentTypeString);
            }
        }
        else if(!xmlStrcmp(cur->name, (const xmlChar*)"extern") && cur->children != NULL)
        {
            //handle extern "C"
            xmlNodePtr children = cur->children;
            while(children != NULL)
            {
                if(!xmlStrcmp(children->name, (const xmlChar*)"block"))
                {
                    if(ret == NULL)
                        ret = endFuncList = ScliceFromNode(varName, xmlFilePath, children->children, varScliceFunc);
                    else
                        endFuncList = ScliceFromNode(varName, xmlFilePath, children->children, varScliceFunc);
                    if(ret != NULL)
                    {
                        while(endFuncList->next != NULL)
                        {
                            endFuncList = endFuncList->next;
                        }
                    }
                }
                else if(!xmlStrcmp(children->name, (const xmlChar*)"decl_stmt"))
                {
                    //处理解析错误的C++函数
                    if(ret == NULL)
                        ret = endFuncList = ScliceErrorFromNode(varName, xmlFilePath, children->children, varScliceFunc);
                    else
                        endFuncList->next = ScliceErrorFromNode(varName, xmlFilePath, children->children, varScliceFunc);
                    if(ret != NULL)
                    {
                        while(endFuncList->next != NULL)
                        {
                            endFuncList = endFuncList->next;
                        }
                    }
                }
                
                children = children->next;
            }
        }
        cur = cur->next;
    }
    
    return ret;
}

funcCallInfoList *Sclice(char *varName, char *xmlFilePath, funcInfoList *(*varScliceFunc)(varDef, xmlNodePtr , varType *, bool ))
{
    funcCallInfoList *ret = NULL;
    xmlDocPtr doc;
    xmlNodePtr cur;
    xmlKeepBlanksDefault(0);
    doc = xmlParseFile(xmlFilePath);
    if(doc == NULL )
    {
        memset(error_info, 0, LOGINFO_LENGTH);
        sprintf(error_info, "Document(%s) not parsed successfully. \n", xmlFilePath);
		Error(error_info);
        return NULL;
    }
    cur = xmlDocGetRootElement(doc);
    if (cur == NULL)
    {
        memset(error_info, 0, LOGINFO_LENGTH);
        sprintf(error_info, "empty document(%s). \n", xmlFilePath);
		Error(error_info);  
        xmlFreeDoc(doc);
        return NULL;
    }
    
    cur = cur->children;
    
    ret = ScliceFromNode(varName, xmlFilePath, cur, varScliceFunc);
    
    xmlFreeDoc(doc);
    
    return ret;
}

char *getParaNameByIndexFromParaList(xmlNodePtr parameterListNode, int index)
{
    if(xmlStrcmp(parameterListNode->name, (const xmlChar*)"parameter_list") \
    && xmlStrcmp(parameterListNode->name, (const xmlChar*)"argument_list"))
        return NULL;
    xmlNodePtr parameterNode = parameterListNode->children;
    while(parameterNode != NULL)
    {
        if(!xmlStrcmp(parameterNode->name, (const xmlChar*)"parameter"))
        {
            if(index-- == 0)
            {
                xmlNodePtr decl = parameterNode->children;
                while(decl != NULL)
                {
                    if(!xmlStrcmp(decl->name, (const xmlChar*)"decl"))
                    {
                        xmlNodePtr name = decl->children;
                        while(name != NULL)
                        {
                            if(!xmlStrcmp(name->name, (const xmlChar*)"name"))
                                return (char *)xmlNodeGetContent(name);
                            name = name->next;
                        }
                    }
                    decl = decl->next;
                }
            }
        }
        else if(!xmlStrcmp(parameterNode->name, (const xmlChar*)"argument"))
        {
            if(index-- == 0)
            {
                xmlNodePtr expr = parameterNode->children;
                while(expr != NULL)
                {
                    if(!xmlStrcmp(expr->name, (const xmlChar*)"expr"))
                    {
                        xmlNodePtr name = expr->last;
                        while(name != NULL)
                        {
                            if(!xmlStrcmp(name->name, (const xmlChar*)"name"))
                                return (char *)xmlNodeGetContent(name);
                            name = name->prev;
                        }
                    }
                    expr = expr->next;
                }
            }
        }
        
        parameterNode = parameterNode->next;
    }
    
    return NULL;
}

char *getErrorParaNameByIndexFromNode(xmlNodePtr cur, int index, char *funcName, char *xmlFilePath, char *funcArgumentType)
{
    char *ret = NULL;
    while (cur != NULL)
    {
        if(!xmlStrcmp(cur->name, (const xmlChar*)"decl"))
        {
            xmlNodePtr temp_cur = cur->children;
            while(temp_cur != NULL)
            {
                if(!xmlStrcmp(temp_cur->name, (const xmlChar*)"name") && temp_cur->next != NULL && !xmlStrcmp(temp_cur->next->name, (const xmlChar*)"argument_list"))
                {
                    char *value = (char*)xmlNodeGetContent(temp_cur);
                    if(strcasecmp("__attribute__", value) == 0)
                    {
                        xmlFree(value);
                        return ret;
                    }
                    if(strcasecmp(funcName, value) == 0)
                    {
                        char *argumentTypeString = ExtractErrorFuncArgumentType(temp_cur->next);
                        char sourcePath[512] = "";
                        //删除开头的temp_和结尾的.xml
                        strncpy(sourcePath, (char *)&(xmlFilePath[5]), strlen(xmlFilePath)-9);
                        if(JudgeArgumentSimilar(funcName, sourcePath, argumentTypeString, funcArgumentType))
                        {
                            //函数名和参数格式都匹配的函数
                            free(argumentTypeString);
                            while(temp_cur != NULL)
                            {
                                if(!xmlStrcmp(temp_cur->name, (const xmlChar*)"parameter_list") ||
                                    !xmlStrcmp(temp_cur->name, (const xmlChar*)"argument_list"))
                                {
                                    //get specific position parameter name
                                    char *paraName = getParaNameByIndexFromParaList(temp_cur, index);
                                    //handle the (const char* fmt, ...) parameter
                                    if(paraName != NULL)
                                    {
                                        int len = sizeof(char)*(strlen(paraName)+1);
                                        ret = malloc(len);
                                        memset(ret, 0, len);
                                        strcpy(ret, paraName);
                                        xmlFree(paraName);
                                        xmlFree(value);
                                        
                                        return ret;
                                    }
                                    break;
                                }
                                temp_cur = temp_cur->next;
                            }
                        }
                        else
                        {
                            free(argumentTypeString);
                            memset(error_info, 0, LOGINFO_LENGTH);
                            sprintf(error_info, "%s: function: %s has more than one define!\n", xmlFilePath, funcName);
                            Warning(error_info);
                        }
                    }
                    xmlFree(value);
                }
                
                temp_cur = temp_cur->next;
            }
        }
        
        cur = cur->next;
    }
    
    return ret;
}

char *getParaNameByIndexFromNode(xmlNodePtr cur, int index, char *funcName, char *xmlFilePath, char *funcArgumentType)
{
    char *ret = NULL;
    while (cur != NULL)
    {
        if(!xmlStrcmp(cur->name, (const xmlChar*)"function") || \
            (!xmlStrcmp(cur->name, (const xmlChar*)"extern") && cur->children != NULL && !xmlStrcmp(cur->last->name, (const xmlChar*)"function")) || \
            (!xmlStrcmp(cur->parent->name, (const xmlChar*)"block") && !xmlStrcmp(cur->name, (const xmlChar*)"decl_stmt") && cur->children != NULL && \
            !xmlStrcmp(cur->last->name, (const xmlChar*)"decl")))
        {
            xmlNodePtr funcNode;
            if(!xmlStrcmp(cur->name, (const xmlChar*)"function"))
                funcNode = cur;
            else
                funcNode = cur->last;
            xmlNodePtr temp_cur = funcNode->children;
            while(temp_cur != NULL)
            {
                if(!xmlStrcmp(temp_cur->name, (const xmlChar*)"name"))
                {
                    char *value = (char*)xmlNodeGetContent(temp_cur);
                    if(strcasecmp(funcName, value) == 0)
                    {
                        //get function argument type string
                        char *argumentTypeString = ExtractFuncArgumentType(funcNode);
                        char sourcePath[512] = "";
                        //删除开头的temp_和结尾的.xml
                        strncpy(sourcePath, (char *)&(xmlFilePath[5]), strlen(xmlFilePath)-9);
                        if(JudgeArgumentSimilar(funcName, sourcePath, argumentTypeString, funcArgumentType))
                        {
                            //函数名和参数格式都匹配的函数
                            free(argumentTypeString);
                            while(temp_cur != NULL)
                            {
                                if(!xmlStrcmp(temp_cur->name, (const xmlChar*)"parameter_list") ||
                                    !xmlStrcmp(temp_cur->name, (const xmlChar*)"argument_list"))
                                {
                                    //get specific position parameter name
                                    char *paraName = getParaNameByIndexFromParaList(temp_cur, index);
                                    //handle the (const char* fmt, ...) parameter
                                    if(paraName != NULL)
                                    {
                                        int len = sizeof(char)*(strlen(paraName)+1);
                                        ret = malloc(len);
                                        memset(ret, 0, len);
                                        strcpy(ret, paraName);
                                        xmlFree(paraName);
                                        xmlFree(value);
                                        
                                        return ret;
                                    }
                                    break;
                                }
                                temp_cur = temp_cur->next;
                            }
                        }
                        else
                        {
                            free(argumentTypeString);
                            memset(error_info, 0, LOGINFO_LENGTH);
                            sprintf(error_info, "%s: function: %s has more than one define!\n", xmlFilePath, funcName);
                            Warning(error_info);
                        }
                    }
                    xmlFree(value);
                }
                temp_cur = temp_cur->next;
            }
        }
        else if(!xmlStrcmp(cur->name, (const xmlChar*)"extern") && cur->children != NULL)
        {
            //handle extern "C"
            xmlNodePtr children = cur->children;
            while(children != NULL)
            {
                if(!xmlStrcmp(children->name, (const xmlChar*)"block"))
                {
                    ret = getParaNameByIndexFromNode(children->children, index, funcName, xmlFilePath, funcArgumentType);
                    if(ret != NULL)
                        return ret;
                }
                else if(!xmlStrcmp(children->name, (const xmlChar*)"decl_stmt"))
                {
                    //处理解析错误的C++函数
                    ret = getErrorParaNameByIndexFromNode(children->children, index, funcName, xmlFilePath, funcArgumentType);
                    if(ret != NULL)
                        return ret;
                }
                
                children = children->next;
            }
        }
        
        cur = cur->next;
    }
    
    return ret;
}

char *getParaNameByIndex(int index, char *funcName, char *xmlFilePath, char *funcArgumentType)
{
    char *ret = NULL;
    xmlDocPtr doc;
    xmlNodePtr cur;
    xmlKeepBlanksDefault(0);
    doc = xmlParseFile(xmlFilePath);
    if(doc == NULL )
    {
        memset(error_info, 0, LOGINFO_LENGTH);
        sprintf(error_info, "Document(%s) not parsed successfully. \n", xmlFilePath);
		Error(error_info);
        return ret;
    }
    cur = xmlDocGetRootElement(doc);
    if (cur == NULL)
    {
        memset(error_info, 0, LOGINFO_LENGTH);
        sprintf(error_info, "empty document(%s). \n", xmlFilePath);
		Error(error_info);  
        xmlFreeDoc(doc);
        return ret;
    }
    
    cur = cur->children;
    
    ret = getParaNameByIndexFromNode(cur, index, funcName, xmlFilePath, funcArgumentType);
      
    xmlFreeDoc(doc);
    
    return ret;
}

int getArguPosition(char *paraName, xmlNodePtr paraListNode)
{
    int ret = 0;
    if(xmlStrcmp(paraListNode->name, (const xmlChar*)"argument_list"))
        return -1;
    xmlNodePtr argument = paraListNode->children;
    while(argument != NULL)
    {
        if(!xmlStrcmp(argument->name, (const xmlChar*)"argument"))
        {
            //这里只是忽略的查找某个参数是否包含要被查找的字符串+
            char *value = (char *)xmlNodeGetContent(argument);
            if(strstr(value, paraName) != NULL)
            {
                xmlFree(value);
                return ret;
            }
            xmlFree(value);
            ret++;
        }
        
        argument = argument->next;
    }
    
    return -1;
}

varDirectInflFuncList *getErrorVarInfluFuncFromNode(xmlNodePtr cur, char *varName, char *funcName, char *xmlFilePath, char *funcArgumentType, \
    varDirectInflFuncList *(*DirectInflFunc)(char *, xmlNodePtr, varType *, bool))
{
    varDirectInflFuncList *begin = NULL;
    while (cur != NULL)
    {
        if(!xmlStrcmp(cur->name, (const xmlChar*)"decl"))
        {
            xmlNodePtr temp_cur = cur->children;
            while(temp_cur != NULL)
            {
                if(!xmlStrcmp(temp_cur->name, (const xmlChar*)"name") && temp_cur->next != NULL && !xmlStrcmp(temp_cur->next->name, (const xmlChar*)"argument_list"))
                {
                    char *value = (char*)xmlNodeGetContent(temp_cur);
                    if(strcasecmp("__attribute__", value) == 0)
                    {
                        xmlFree(value);
                        return begin;
                    }
                    if(strcasecmp(funcName, value) == 0)
                    {
                        char *argumentTypeString = ExtractErrorFuncArgumentType(temp_cur->next);
                        char sourcePath[512] = "";
                        //删除开头的temp_和结尾的.xml
                        strncpy(sourcePath, (char *)&(xmlFilePath[5]), strlen(xmlFilePath)-9);
                        if(JudgeArgumentSimilar(funcName, sourcePath, argumentTypeString, funcArgumentType))
                        {
                            //函数名和参数格式都匹配的函数
                            free(argumentTypeString);
                            //获取函数参数定义的变量信息
                            varType *beginVarType = ExtractErrorVarType(temp_cur->next);
                            varType *endVarType = beginVarType;
                            if(endVarType != NULL)
                            {
                                while(endVarType->next != NULL)
                                {
                                    endVarType = endVarType->next;
                                }
                            }
                            
                            xmlNodePtr blockNode = temp_cur->next->next;
                            while(blockNode != NULL)
                            {
                                if(!xmlStrcmp(blockNode->name, (const xmlChar*)"argument_list"))
                                    break;
                                blockNode = blockNode->next;
                            }
                            
                            //获取函数block块中定义的变量信息
                            if(endVarType != NULL)
                                endVarType->next = ExtractErrorVarType(blockNode);
                            else
                                beginVarType = ExtractErrorVarType(blockNode);
                            
                            varType *currentVarType = beginVarType;
                            
                            if(judgeCSrcXmlFile(xmlFilePath))
                            {
                                begin = DirectInflFunc(varName, blockNode, NULL, true);
                            }
                            else
                            {
                                begin = DirectInflFunc(varName, blockNode, currentVarType, true);
                            }
                            
                            while(currentVarType != NULL)
                            {
                                beginVarType = beginVarType->next;
                                free(currentVarType);
                                currentVarType = beginVarType;
                            }
                            
                            xmlFree(value);
                            return begin;
                        }
                        else
                        {
                            free(argumentTypeString);
                            memset(error_info, 0, LOGINFO_LENGTH);
                            sprintf(error_info, "%s: function: %s has more than one define!\n", xmlFilePath, funcName);
                            Warning(error_info);
                        }
                    }
                    xmlFree(value);
                }
                
                temp_cur = temp_cur->next;
            }
        }
        
        cur = cur->next;
    }
    
    return begin;
}

varDirectInflFuncList *getVarInfluFuncFromNode(xmlNodePtr cur, char *varName, char *funcName, char *xmlFilePath, char *funcArgumentType, \
    varDirectInflFuncList *(*DirectInflFunc)(char *, xmlNodePtr, varType *, bool))
{
    varDirectInflFuncList *begin = NULL;
    varDirectInflFuncList *end = NULL;
    varDirectInflFuncList *current = NULL;
    while (cur != NULL)
    {
        if(!xmlStrcmp(cur->name, (const xmlChar*)"function") || \
            (!xmlStrcmp(cur->name, (const xmlChar*)"extern") && cur->children != NULL && !xmlStrcmp(cur->last->name, (const xmlChar*)"function")) || \
            (!xmlStrcmp(cur->parent->name, (const xmlChar*)"block") && !xmlStrcmp(cur->name, (const xmlChar*)"decl_stmt") && cur->children != NULL && \
            !xmlStrcmp(cur->last->name, (const xmlChar*)"decl")))
        {
            xmlNodePtr funcNode;
            if(!xmlStrcmp(cur->name, (const xmlChar*)"function"))
                funcNode = cur;
            else
                funcNode = cur->last;
            xmlNodePtr temp_cur = funcNode->children;
            while(temp_cur != NULL)
            {
                if(!xmlStrcmp(temp_cur->name, (const xmlChar*)"name"))
                {
                    char *value = (char*)xmlNodeGetContent(temp_cur);
                    if(strcasecmp(funcName, value) == 0)
                    {
                        //get function argument type string
                        char *argumentTypeString = ExtractFuncArgumentType(funcNode);
                        char sourcePath[512] = "";
                        //删除开头的temp_和结尾的.xml
                        strncpy(sourcePath, (char *)&(xmlFilePath[5]), strlen(xmlFilePath)-9);
                        if(JudgeArgumentSimilar(funcName, sourcePath, argumentTypeString, funcArgumentType))
                        {
                            //函数名和参数格式都匹配的函数
                            free(argumentTypeString);
                            varType *beginVarType = ExtractVarType(funcNode);
                            varType *currentVarType = beginVarType;
                            
                            xmlNodePtr blockNode = funcNode->children;
                            while(blockNode != NULL)
                            {
                                if(!xmlStrcmp(blockNode->name, (const xmlChar*)"block"))
                                    break;
                                blockNode = blockNode->next;
                            }
                            if(judgeCSrcXmlFile(xmlFilePath))
                            {
                                begin = current = DirectInflFunc(varName, blockNode, NULL, true);
                            }
                            else
                            {
                                begin = current = DirectInflFunc(varName, blockNode, currentVarType, true);
                            }
                            while(current != NULL)
                            {
                                end = current;
                                current = current->next;
                            }
                            //influence variable
                            varDef *varInfluence = ScliceInflVar(varName, blockNode, currentVarType);
                            if(varInfluence != NULL)
                            {
                                varDef *varInfluCur = varInfluence;
                                while(varInfluCur != NULL)
                                {
                                    if(judgeCSrcXmlFile(xmlFilePath))
                                    {
                                        current = DirectInflFunc(varInfluCur->varName, blockNode, NULL, true);
                                    }
                                    else
                                    {
                                        current = DirectInflFunc(varInfluCur->varName, blockNode, currentVarType, true);
                                    }
                                    if(current != NULL)
                                    {
                                        if(begin == NULL)
                                        {
                                            begin = end = current;
                                        }
                                        else
                                        {
                                            end->next = current;
                                        }
                                        while(current != NULL)
                                        {
                                            end = current;
                                            current = current->next;
                                        }
                                    }
                                    varInfluence = varInfluence->next;
                                    free(varInfluCur);
                                    varInfluCur = varInfluence;
                                }
                            }
                            
                            while(currentVarType != NULL)
                            {
                                beginVarType = beginVarType->next;
                                free(currentVarType);
                                currentVarType = beginVarType;
                            }
                            
                            xmlFree(value);
                            return begin;
                        }
                        else
                        {
                            free(argumentTypeString);
                            memset(error_info, 0, LOGINFO_LENGTH);
                            sprintf(error_info, "%s: function: %s has more than one define!\n", xmlFilePath, funcName);
                            Warning(error_info);
                        }
                    }
                    xmlFree(value);
                }
                temp_cur = temp_cur->next;
            }
        }
        else if(!xmlStrcmp(cur->name, (const xmlChar*)"extern") && cur->children != NULL)
        {
            //handle extern "C"
            xmlNodePtr children = cur->children;
            while(children != NULL)
            {
                if(!xmlStrcmp(children->name, (const xmlChar*)"block"))
                {
                    begin = getVarInfluFuncFromNode(children->children, varName, funcName, xmlFilePath, funcArgumentType, DirectInflFunc);
                    if(begin != NULL)
                        return begin;
                }
                else if(!xmlStrcmp(children->name, (const xmlChar*)"decl_stmt"))
                {
                    //处理解析错误的C++函数
                    begin = getErrorVarInfluFuncFromNode(children->children, varName, funcName, xmlFilePath, funcArgumentType, DirectInflFunc);
                    if(begin != NULL)
                        return begin;
                }
                
                children = children->next;
            }
        }
        
        cur = cur->next;
    }
    
    return begin;
}

varDirectInflFuncList *getVarInfluFunc(char *varName, char *funcName, char *xmlFilePath, char *funcArgumentType, \
    varDirectInflFuncList *(*DirectInflFunc)(char *, xmlNodePtr, varType *, bool))
{
    varDirectInflFuncList *begin = NULL;
    xmlDocPtr doc;
    xmlNodePtr cur;
    xmlKeepBlanksDefault(0);
    doc = xmlParseFile(xmlFilePath);
    if(doc == NULL )
    {
        memset(error_info, 0, LOGINFO_LENGTH);
        sprintf(error_info, "Document(%s) not parsed successfully. \n", xmlFilePath);
		Error(error_info);
        return begin;
    }
    cur = xmlDocGetRootElement(doc);
    if (cur == NULL)
    {
        memset(error_info, 0, LOGINFO_LENGTH);
        sprintf(error_info, "empty document(%s). \n", xmlFilePath);
		Error(error_info);  
        xmlFreeDoc(doc);
        return begin;
    }
    
    cur = cur->children;
    
    begin = getVarInfluFuncFromNode(cur, varName, funcName, xmlFilePath, funcArgumentType, DirectInflFunc);
    
    xmlFreeDoc(doc);
    
    return begin;
}

varDef *getVarInfluVarInfo(char *varName, char *funcName, char *xmlFilePath, char *funcArgumentType)
{
    varDef *begin = NULL;
    xmlDocPtr doc;
    xmlNodePtr cur;
    xmlKeepBlanksDefault(0);
    doc = xmlParseFile(xmlFilePath);
    if(doc == NULL )
    {
        memset(error_info, 0, LOGINFO_LENGTH);
        sprintf(error_info, "Document(%s) not parsed successfully. \n", xmlFilePath);
		Error(error_info);
        return begin;
    }
    cur = xmlDocGetRootElement(doc);
    if (cur == NULL)
    {
        memset(error_info, 0, LOGINFO_LENGTH);
        sprintf(error_info, "empty document(%s). \n", xmlFilePath);
		Error(error_info);  
        xmlFreeDoc(doc);
        return begin;
    }
    
    cur = cur->children;
    while (cur != NULL)
    {
        if(!xmlStrcmp(cur->name, (const xmlChar*)"function") || \
            (!xmlStrcmp(cur->name, (const xmlChar*)"extern") && cur->children != NULL && !xmlStrcmp(cur->last->name, (const xmlChar*)"function")) || \
            (!xmlStrcmp(cur->name, (const xmlChar*)"decl_stmt") && cur->children != NULL && !xmlStrcmp(cur->last->name, (const xmlChar*)"decl")))
        {
            xmlNodePtr funcNode;
            if(!xmlStrcmp(cur->name, (const xmlChar*)"function"))
                funcNode = cur;
            else
                funcNode = cur->last;
            xmlNodePtr temp_cur = funcNode->children;
            while(temp_cur != NULL)
            {
                if(!xmlStrcmp(temp_cur->name, (const xmlChar*)"name"))
                {
                    char *value = (char*)xmlNodeGetContent(temp_cur);
                    if(strcasecmp(funcName, value) == 0)
                    {
                        //get function argument type string
                        char *argumentTypeString = ExtractFuncArgumentType(funcNode);
                        char sourcePath[512] = "";
                        //删除开头的temp_和结尾的.xml
                        strncpy(sourcePath, (char *)&(xmlFilePath[5]), strlen(xmlFilePath)-9);
                        if(JudgeArgumentSimilar(funcName, sourcePath, argumentTypeString, funcArgumentType))
                        {
                            //函数名和参数格式都匹配的函数
                            free(argumentTypeString);
                            varType *beginVarType = ExtractVarType(funcNode);
                            varType *currentVarType = beginVarType;
                            //influence variable
                            begin = ScliceInflVar(varName, funcNode, currentVarType);
                            
                            while(currentVarType != NULL)
                            {
                                beginVarType = beginVarType->next;
                                free(currentVarType);
                                currentVarType = beginVarType;
                            }
                        }
                        else
                        {
                            free(argumentTypeString);
                            memset(error_info, 0, LOGINFO_LENGTH);
                            sprintf(error_info, "%s: function: %s has more than one define!\n", xmlFilePath, funcName);
                            Warning(error_info);
                        }
                    }
                    xmlFree(value);
                }
                temp_cur = temp_cur->next;
            }
        }
        cur = cur->next;
    }
      
    xmlFreeDoc(doc);
    
    return begin;
}

xmlNodePtr getSpeciCalledFuncNode(xmlNodePtr cur, char *calledFuncName, int calledLine)
{
    while(cur != NULL)
    {
        if(!xmlStrcmp(cur->name, (const xmlChar*)"call"))
        {
            if(cur->children->last != NULL)
            {
                xmlChar* attr_value = getLine(cur->children);
                int line = StrToInt((char *)attr_value);
                xmlFree(attr_value);
                char *callFuncName = NULL;
                if(xmlStrcmp(cur->children->last->name, (const xmlChar*)"position"))
                {
                    callFuncName = (char*)xmlNodeGetContent(cur->children->last);
                }
                else
                {
                    callFuncName = (char*)xmlNodeGetContent(cur->children);
                }
                //判断该节点是否为检查节点
                if(strcasecmp(callFuncName, calledFuncName) == 0 && line == calledLine)
                {
                    xmlFree(callFuncName);
                    return cur;
                }
                xmlFree(callFuncName);
            }
        }
        
        xmlNodePtr retNode = getSpeciCalledFuncNode(cur->children, calledFuncName, calledLine);
        if(retNode != NULL)
            return retNode;

        cur = cur->next;
    }
    
    return NULL;
}

loopExprList *getCalledFuncLoopInfo(char *funcName, char *xmlFilePath, char *funcArgumentType, char *calledFuncName, int calledLine)
{
    loopExprList *begin = NULL;
    loopExprList *end = NULL;
    xmlDocPtr doc;
    xmlNodePtr cur;
    xmlKeepBlanksDefault(0);
    doc = xmlParseFile(xmlFilePath);
    if(doc == NULL )
    {
        memset(error_info, 0, LOGINFO_LENGTH);
        sprintf(error_info, "Document(%s) not parsed successfully. \n", xmlFilePath);
		Error(error_info);
        return begin;
    }
    cur = xmlDocGetRootElement(doc);
    if (cur == NULL)
    {
        memset(error_info, 0, LOGINFO_LENGTH);
        sprintf(error_info, "empty document(%s). \n", xmlFilePath);
		Error(error_info);  
        xmlFreeDoc(doc);
        return begin;
    }
    
    cur = cur->children;
    while (cur != NULL)
    {
        if(!xmlStrcmp(cur->name, (const xmlChar*)"function") || \
            (!xmlStrcmp(cur->name, (const xmlChar*)"extern") && cur->children != NULL && !xmlStrcmp(cur->last->name, (const xmlChar*)"function")) || \
            (!xmlStrcmp(cur->name, (const xmlChar*)"decl_stmt") && cur->children != NULL && !xmlStrcmp(cur->last->name, (const xmlChar*)"decl")))
        {
            xmlNodePtr funcNode;
            if(!xmlStrcmp(cur->name, (const xmlChar*)"function"))
                funcNode = cur;
            else
                funcNode = cur->last;
            xmlNodePtr temp_cur = funcNode->children;
            while(temp_cur != NULL)
            {
                if(!xmlStrcmp(temp_cur->name, (const xmlChar*)"name"))
                {
                    char *value = (char*)xmlNodeGetContent(temp_cur);
                    if(strcasecmp(funcName, value) == 0)
                    {
                        //get function argument type string
                        char *argumentTypeString = ExtractFuncArgumentType(funcNode);
                        char sourcePath[512] = "";
                        //删除开头的temp_和结尾的.xml
                        strncpy(sourcePath, (char *)&(xmlFilePath[5]), strlen(xmlFilePath)-9);
                        if(JudgeArgumentSimilar(funcName, sourcePath, argumentTypeString, funcArgumentType))
                        {
                            //函数名和参数格式都匹配的函数
                            free(argumentTypeString);
                            //获取被调用函数的节点
                            xmlNodePtr tempNode = getSpeciCalledFuncNode(funcNode, calledFuncName, calledLine);
                            if(tempNode != NULL)
                            {
                                xmlNodePtr parentNode = tempNode->parent;
                                while(parentNode != NULL)
                                {
                                    loopExpr tempLoopExpr;
                                    tempLoopExpr.type = -1;
                                    if(!xmlStrcmp(parentNode->name, (const xmlChar*)"for"))
                                    {
                                        xmlNodePtr childrenNode = parentNode->children;
                                        while(childrenNode != NULL)
                                        {
                                            if(!xmlStrcmp(childrenNode->name, (const xmlChar*)"control"))
                                            {
                                                tempLoopExpr.type = 0;
                                                char *value = (char*)xmlNodeGetContent(childrenNode);
                                                strcpy(tempLoopExpr.loopExpr, value);
                                                xmlFree(value);
                                                break;
                                            }
                                            childrenNode = childrenNode->next;
                                        }
                                    }
                                    else if(!xmlStrcmp(parentNode->name, (const xmlChar*)"do"))
                                    {
                                        xmlNodePtr childrenNode = parentNode->children;
                                        while(childrenNode != NULL)
                                        {
                                            if(!xmlStrcmp(childrenNode->name, (const xmlChar*)"condition"))
                                            {
                                                tempLoopExpr.type = 2;
                                                char *value = (char*)xmlNodeGetContent(childrenNode);
                                                strcpy(tempLoopExpr.loopExpr, value);
                                                xmlFree(value);
                                                break;
                                            }
                                            childrenNode = childrenNode->next;
                                        }
                                    }
                                    else if(!xmlStrcmp(parentNode->name, (const xmlChar*)"while"))
                                    {
                                        xmlNodePtr childrenNode = parentNode->children;
                                        while(childrenNode != NULL)
                                        {
                                            if(!xmlStrcmp(childrenNode->name, (const xmlChar*)"condition"))
                                            {
                                                tempLoopExpr.type = 1;
                                                char *value = (char*)xmlNodeGetContent(childrenNode);
                                                strcpy(tempLoopExpr.loopExpr, value);
                                                xmlFree(value);
                                                break;
                                            }
                                            childrenNode = childrenNode->next;
                                        }
                                    }
                                    
                                    if(tempLoopExpr.type != -1)
                                    {
                                        if(begin == NULL)
                                            begin = end = malloc(sizeof(loopExprList));
                                        else
                                            end = end->next = malloc(sizeof(loopExprList));
                                        
                                        memset(end, 0, sizeof(loopExprList));
                                        end->expr = tempLoopExpr;
                                    }
                                    parentNode = parentNode->parent;
                                }
                            }
                        }
                        else
                        {
                            free(argumentTypeString);
                            memset(error_info, 0, LOGINFO_LENGTH);
                            sprintf(error_info, "%s: function: %s has more than one define!\n", xmlFilePath, funcName);
                            Warning(error_info);
                        }
                    }
                    xmlFree(value);
                }
                temp_cur = temp_cur->next;
            }
        }
        cur = cur->next;
    }
      
    xmlFreeDoc(doc);
    
    return begin;
}

loopCountInfoList *getCalledFuncLoopCountInfo(char *funcName, char *xmlFilePath, char *funcArgumentType, char *calledFuncName, int calledLine)
{
    loopCountInfoList *begin = NULL;
    loopCountInfoList *end = NULL;
    xmlDocPtr doc;
    xmlNodePtr cur;
    xmlKeepBlanksDefault(0);
    doc = xmlParseFile(xmlFilePath);
    if(doc == NULL )
    {
        memset(error_info, 0, LOGINFO_LENGTH);
        sprintf(error_info, "Document(%s) not parsed successfully. \n", xmlFilePath);
		Error(error_info);
        return begin;
    }
    cur = xmlDocGetRootElement(doc);
    if (cur == NULL)
    {
        memset(error_info, 0, LOGINFO_LENGTH);
        sprintf(error_info, "empty document(%s). \n", xmlFilePath);
		Error(error_info);  
        xmlFreeDoc(doc);
        return begin;
    }
    
    cur = cur->children;
    while (cur != NULL)
    {
        if(!xmlStrcmp(cur->name, (const xmlChar*)"function") || \
            (!xmlStrcmp(cur->name, (const xmlChar*)"extern") && cur->children != NULL && !xmlStrcmp(cur->last->name, (const xmlChar*)"function")) || \
            (!xmlStrcmp(cur->name, (const xmlChar*)"decl_stmt") && cur->children != NULL && !xmlStrcmp(cur->last->name, (const xmlChar*)"decl")))
        {
            xmlNodePtr funcNode;
            if(!xmlStrcmp(cur->name, (const xmlChar*)"function"))
                funcNode = cur;
            else
                funcNode = cur->last;
            xmlNodePtr temp_cur = funcNode->children;
            while(temp_cur != NULL)
            {
                if(!xmlStrcmp(temp_cur->name, (const xmlChar*)"name"))
                {
                    char *value = (char*)xmlNodeGetContent(temp_cur);
                    if(strcasecmp(funcName, value) == 0)
                    {
                        //get function argument type string
                        char *argumentTypeString = ExtractFuncArgumentType(funcNode);
                        char sourcePath[512] = "";
                        //删除开头的temp_和结尾的.xml
                        strncpy(sourcePath, (char *)&(xmlFilePath[5]), strlen(xmlFilePath)-9);
                        if(JudgeArgumentSimilar(funcName, sourcePath, argumentTypeString, funcArgumentType))
                        {
                            //函数名和参数格式都匹配的函数
                            free(argumentTypeString);
                            //获取被调用函数的节点
                            xmlNodePtr tempNode = getSpeciCalledFuncNode(funcNode, calledFuncName, calledLine);
                            if(tempNode != NULL)
                            {
                                xmlNodePtr parentNode = tempNode->parent;
                                while(parentNode != NULL)
                                {
                                    loopCountInfo tempLoopCount;
                                    tempLoopCount.type = -1;
                                    tempLoopCount.count = -1;
                                    strcpy(tempLoopCount.funcName, funcName);
                                    //删除结尾的.xml
                                    strncpy(tempLoopCount.sourcePath, xmlFilePath, strlen(xmlFilePath)-4);
                                    char strCount[5] = "";
                                    if(!xmlStrcmp(parentNode->name, (const xmlChar*)"for"))
                                    {
                                        xmlNodePtr childrenNode = parentNode->children;
                                        while(childrenNode != NULL)
                                        {
                                            if(!xmlStrcmp(childrenNode->name, (const xmlChar*)"block"))
                                            {
                                                xmlNodePtr countNode = childrenNode->last;
                                                while(countNode != NULL)
                                                {
                                                    //不等于text节点
                                                    if(xmlStrcmp(countNode->name, (const xmlChar*)"text"))
                                                    {
                                                        char *countContent = (char*)xmlNodeGetContent(countNode);
                                                        if(strstr(countContent, "count") != NULL && \
                                                            strstr(countContent, "++") != NULL)
                                                        {
                                                            tempLoopCount.type = 0;
                                                            strncpy(strCount, &(countContent[5]), strlen(countContent)-7);
                                                            tempLoopCount.count = StrToInt(strCount);
                                                            xmlFree(countContent);
                                                            goto next;
                                                        }
                                                        xmlFree(countContent);
                                                    }
                                                    
                                                    countNode = countNode->prev;
                                                }
                                                
                                            }
                                            childrenNode = childrenNode->next;
                                        }
                                    }
                                    else if(!xmlStrcmp(parentNode->name, (const xmlChar*)"do"))
                                    {
                                        xmlNodePtr childrenNode = parentNode->children;
                                        while(childrenNode != NULL)
                                        {
                                            if(!xmlStrcmp(childrenNode->name, (const xmlChar*)"block"))
                                            {
                                                xmlNodePtr countNode = childrenNode->last;
                                                while(countNode != NULL)
                                                {
                                                    //不等于text节点
                                                    if(xmlStrcmp(countNode->name, (const xmlChar*)"text"))
                                                    {
                                                        char *countContent = (char*)xmlNodeGetContent(countNode);
                                                        if(strstr(countContent, "count") != NULL && \
                                                            strstr(countContent, "++") != NULL)
                                                        {
                                                            tempLoopCount.type = 2;
                                                            strncpy(strCount, &(countContent[5]), strlen(countContent)-7);
                                                            tempLoopCount.count = StrToInt(strCount);
                                                            xmlFree(countContent);
                                                            goto next;
                                                        }
                                                        xmlFree(countContent);
                                                    }
                                                    
                                                    countNode = countNode->prev;
                                                }
                                            }
                                            childrenNode = childrenNode->next;
                                        }
                                    }
                                    else if(!xmlStrcmp(parentNode->name, (const xmlChar*)"while"))
                                    {
                                        xmlNodePtr childrenNode = parentNode->children;
                                        while(childrenNode != NULL)
                                        {
                                            if(!xmlStrcmp(childrenNode->name, (const xmlChar*)"condition"))
                                            {
                                                xmlNodePtr countNode = childrenNode->last;
                                                while(countNode != NULL)
                                                {
                                                    //不等于text节点
                                                    if(xmlStrcmp(countNode->name, (const xmlChar*)"block"))
                                                    {
                                                        char *countContent = (char*)xmlNodeGetContent(countNode);
                                                        if(strstr(countContent, "count") != NULL && \
                                                            strstr(countContent, "++") != NULL)
                                                        {
                                                            tempLoopCount.type = 1;
                                                            strncpy(strCount, &(countContent[5]), strlen(countContent)-7);
                                                            tempLoopCount.count = StrToInt(strCount);
                                                            xmlFree(countContent);
                                                            goto next;
                                                        }
                                                        xmlFree(countContent);
                                                    }
                                                    
                                                    countNode = countNode->prev;
                                                }
                                            }
                                            childrenNode = childrenNode->next;
                                        }
                                    }

next:
                                    if(tempLoopCount.type != -1 && tempLoopCount.count != -1)
                                    {
                                        if(begin == NULL)
                                            begin = end = malloc(sizeof(loopCountInfoList));
                                        else
                                            end = end->next = malloc(sizeof(loopCountInfoList));
                                        
                                        memset(end, 0, sizeof(loopCountInfoList));
                                        end->info = tempLoopCount;
                                    }
                                    parentNode = parentNode->parent;
                                }
                            }
                        }
                        else
                        {
                            free(argumentTypeString);
                            memset(error_info, 0, LOGINFO_LENGTH);
                            sprintf(error_info, "%s: function: %s has more than one define!\n", xmlFilePath, funcName);
                            Warning(error_info);
                        }
                    }
                    xmlFree(value);
                }
                temp_cur = temp_cur->next;
            }
        }
        cur = cur->next;
    }
      
    xmlFreeDoc(doc);
    
    return begin;
}