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
            xmlNodePtr temp_cur = cur->children;
            char *varName = NULL;
            while(temp_cur != NULL)
            {
                if(!xmlStrcmp(temp_cur->name, (const xmlChar*)"name"))
                {
                    varName = (char*)xmlNodeGetContent(temp_cur);
                    //server.port <---> server.port[0]
                    if(strcasecmp(varName, var) == 0)
                        return true;
                    else if(strstr(varName, var) != NULL && varName[strlen(var)] == '[')
                        return true;
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
            xmlChar* attr_value = xmlGetProp(cur->last, (xmlChar*)"line");
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
                                strcat(type, (char*)xmlNodeGetContent(temp));
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
                                            strcat(type, (char*)xmlNodeGetContent(temp_type));
                                        }
                                        else
                                        {
                                            xmlNodePtr temp_name = temp_type->children;
                                            while(temp_name != NULL)
                                            {
                                                if(strlen(type) != 0)
                                                    strcat(type, " ");
                                                strcat(type, (char*)xmlNodeGetContent(temp_name));
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
                                        strcat(end->type, (char*)xmlNodeGetContent(temp_type));
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
                                        strcat(end->varName, (char*)xmlNodeGetContent(temp));
                                    else
                                        strcat(end->varName, (char*)xmlNodeGetContent(temp_name));
                                }
                                else
                                    strcat(end->varName, (char*)xmlNodeGetContent(temp));
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
                            strcat(end->varName, (char*)xmlNodeGetContent(temp_name->parent));
                        else
                            strcat(end->varName, (char*)xmlNodeGetContent(temp_name));
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
                            strcat(end->type, (char*)xmlNodeGetContent(temp_cur));
                            end->line = StrToInt((char *)attr_value);
                        }
                        else if(!xmlStrcmp(temp_cur->name, (const xmlChar*)"decl"))
                        {
                            xmlNodePtr temp_name = temp_cur->children;

                            if(temp_name != NULL && !xmlStrcmp(temp_name->name, (const xmlChar*)"text"))
                                strcat(end->varName, (char*)xmlNodeGetContent(temp_cur));
                            else
                            {
                                while(temp_name != NULL)
                                {
                                    if(!xmlStrcmp(temp_name->name, (const xmlChar*)"name"))
                                    {
                                        xmlNodePtr child_name = temp_name->children;
                                        if(child_name != NULL && !xmlStrcmp(child_name->name, (const xmlChar*)"text"))
                                            strcat(end->varName, (char*)xmlNodeGetContent(temp_name));
                                        else if(child_name != NULL && !xmlStrcmp(child_name->name, (const xmlChar*)"name"))
                                            strcat(end->varName, (char*)xmlNodeGetContent(child_name));
                                        
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
                                strcat(type, (char*)xmlNodeGetContent(temp));
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
                                            strcat(type, (char*)xmlNodeGetContent(temp_type));
                                        }
                                        else
                                        {
                                            xmlNodePtr temp_name = temp_type->children;
                                            while(temp_name != NULL)
                                            {
                                                if(strlen(type) != 0)
                                                    strcat(type, " ");
                                                strcat(type, (char*)xmlNodeGetContent(temp_name));
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
                                        strcat(end->varName, (char*)xmlNodeGetContent(temp));
                                        attr_value = xmlGetProp(temp, (xmlChar*)"line");
                                    }
                                    else
                                    {
                                        strcat(end->varName, (char*)xmlNodeGetContent(temp_name));
                                        attr_value = xmlGetProp(temp_name, (xmlChar*)"line");
                                    }
                                }
                                else
                                {
                                    strcat(end->varName, (char*)xmlNodeGetContent(temp));
                                    attr_value = xmlGetProp(temp, (xmlChar*)"line");
                                }
                                end->line = StrToInt((char *)attr_value);
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
                                strcat(end->varName, (char*)xmlNodeGetContent(temp_name->parent));
                                attr_value = xmlGetProp(temp_name->parent, (xmlChar*)"line");
                            }
                            else
                            {
                                strcat(end->varName, (char*)xmlNodeGetContent(temp_name));
                                attr_value = xmlGetProp(temp_name, (xmlChar*)"line");
                            }
                        }
                        else
                        {
                            strcat(end->varName, (char*)xmlNodeGetContent(temp_cur->children->next));
                            attr_value = xmlGetProp(temp_cur->children->next, (xmlChar*)"line");
                        }
                        end->line = StrToInt((char *)attr_value);
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
		RecordLog(error_info);
        return ;
    }
    cur = xmlDocGetRootElement(doc);
    if (cur == NULL)
    {
        memset(error_info, 0, LOGINFO_LENGTH);
        sprintf(error_info, "empty document(%s). \n", xmlFilePath);
		RecordLog(error_info);  
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
                                break;
                            }
                            name = name->prev;
                        }
                    }
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
                                        xmlChar* attr_value = NULL;
                                        if(!xmlStrcmp(name->children->name, (const xmlChar*)"name"))
                                            attr_value = xmlGetProp(name->children, (xmlChar*)"line");
                                        else
                                            attr_value = xmlGetProp(name, (xmlChar*)"line");
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
                                        }
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
                                    goto next;
                                }
                                name = name->prev;
                            }
                            //}
                            //influName = influName->next;
                        }
                    }
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
                            /*xmlNodePtr expr = init->children;
                            while(expr != NULL)
                            {
                                if(!xmlStrcmp(expr->name, (const xmlChar*)"expr"))
                                {
                                    xmlNodePtr name = expr->children;
                                    while(name != NULL)
                                    {
                                        
                                        bool isInfluence = false;
                                        
                                        if(!xmlStrcmp(name->name, (const xmlChar*)"name"))
                                        {
                                            char *var = (char*)xmlNodeGetContent(name);
                                            if(strcasecmp(varName, var) == 0)
                                            {
                                                isInfluence = true;
                                            }
                                            else if(strstr(var, varName) != NULL && var[strlen(varName)] == '[')
                                                isInfluence = true;
                                        }
                                        else if(!xmlStrcmp(name->name, (const xmlChar*)"call") && JudgeVarUsed(name, varName))
                                        {
                                            isInfluence = true;
                                        }*/
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
                                break;
                            }
                            /*
                                        name = name->next;
                                    }
                                }
                                expr = expr->next;
                            }*/
                        }
                        init = init->next;
                    }
                }
                decl = decl->next;
            }
        }
        
        current = ExtractDirectInfluVarFromNode(cur->children, varName, varTypeBegin, false);
        
        if(begin == NULL)
            begin = end = current;
        else
            end->next = current;
        while(current != NULL)
        {
            end = current;
            current = current->next;
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

char *ExtractFuncArgumentType(xmlNodePtr cur)
{
    xmlNodePtr argument_list = cur->children;
    char *retTypeString = malloc(sizeof(char)*512);
    memset(retTypeString, 0, sizeof(char)*512);
    while(argument_list != NULL)
    {
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
                                                if(strcasecmp((char*)xmlNodeGetContent(name), "void") == 0)
                                                    sprintf(retTypeString, "(%s", (char*)xmlNodeGetContent(name));
                                                else
                                                    sprintf(retTypeString, "(%s", (char*)xmlNodeGetContent(name));
                                                
                                            }
                                            else
                                            {
                                                sprintf(retTypeString, "%s/%s", retTypeString, (char*)xmlNodeGetContent(name));
                                            }
                                                
                                            goto next;
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
            next:
                parameter = parameter->next;
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
    if(inflVarName == NULL)
        begin = end = current = ExtractDirectInfluVar(cur, varName, varTypeBegin);
    else
        begin = end = current = ExtractDirectInfluVar(cur, inflVarName, varTypeBegin);
        
    if(begin != NULL)
    {
        current = begin;
        while(current != NULL)
        {xmlChar *getLine(xmlNodePtr cur);
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

funcInfo *Sclice(char *varName, char *xmlFilePath, funcCallList *(*varScliceFunc)(varDef, xmlNodePtr , varType *, bool ))
{
    funcCallList *begin = NULL;
    funcCallList *end = NULL;
    funcCallList *current = NULL;
    funcInfo *ret = NULL;
    funcInfo *curFuncList = NULL;
    xmlDocPtr doc;
    xmlNodePtr cur;
    xmlKeepBlanksDefault(0);
    doc = xmlParseFile(xmlFilePath);
    if(doc == NULL )
    {
        memset(error_info, 0, LOGINFO_LENGTH);
        sprintf(error_info, "Document(%s) not parsed successfully. \n", xmlFilePath);
		RecordLog(error_info);
        return NULL;
    }
    cur = xmlDocGetRootElement(doc);
    if (cur == NULL)
    {
        memset(error_info, 0, LOGINFO_LENGTH);
        sprintf(error_info, "empty document(%s). \n", xmlFilePath);
		RecordLog(error_info);  
        xmlFreeDoc(doc);
        return NULL;
    }
    
    cur = cur->children;
    while (cur != NULL)
    {
        if(!xmlStrcmp(cur->name, (const xmlChar*)"function") || \
            (!xmlStrcmp(cur->name, (const xmlChar*)"extern") && cur->children != NULL && !xmlStrcmp(cur->children->name, (const xmlChar*)"function")) || \
            (!xmlStrcmp(cur->name, (const xmlChar*)"decl_stmt") && cur->children != NULL && !xmlStrcmp(cur->last->name, (const xmlChar*)"decl")))
        {
            xmlNodePtr funcNode;
            if(!xmlStrcmp(cur->name, (const xmlChar*)"function"))
                funcNode = cur;
            else
                funcNode = cur->children;
            varType *beginVarType = ExtractVarType(funcNode);
            varType *currentVarType = beginVarType;
            varDef varInfo;
            memset(&varInfo, 0, sizeof(varDef));
            strcpy(varInfo.varName, varName);
            varInfo.line = 0;
            current = varScliceFunc(varInfo, funcNode, currentVarType, true);
            if(begin == NULL)
                begin = end = current;
            else if(current != NULL)
                end = end->next = current;
            while(current != NULL)
            {
                end = current;
                current = current->next;
            }
            //influence variable
            varDef *varInfluence = ScliceInflVar(varName, funcNode, currentVarType);
            if(varInfluence != NULL)
            {
                varDef *varInfluCur = varInfluence;
                while(varInfluCur != NULL)
                {
                    current = varScliceFunc(*varInfluCur, funcNode, currentVarType, true);
                    if(begin == NULL)
                        begin = end = current;
                    else if(current != NULL)
                        end = end->next = current;
                    while(current != NULL)
                    {
                        end = current;
                        current = current->next;
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
        }
        cur = cur->next;
    }
      
    xmlFreeDoc(doc);
    
    current = begin;
    while(current != NULL)
    {
        
        if(ret == NULL)
            ret = curFuncList = malloc(sizeof(funcInfo));
        else
            curFuncList = curFuncList->next = malloc(sizeof(funcInfo));
        memset(curFuncList, 0, sizeof(funcInfo));
        strcpy(curFuncList->funcName, current->funcName);
        curFuncList->type = current->type;
        if(curFuncList->type == 'S')
        {
            strcpy(curFuncList->sourceFile, current->sourceFile);
            strcpy(curFuncList->argumentType, current->argumentType);
            strcpy(curFuncList->funcType, current->funcType);
        }
        
        begin = begin->next;
        free(current);
        current = begin;
    }
    return ret;
}

funcInfo *ScliceDebug(char *varName, char *xmlFilePath, funcCallList *(*varScliceFunc)(varDef, xmlNodePtr , varType *, bool ))
{
    funcCallList *begin = NULL;
    funcCallList *end = NULL;
    funcCallList *current = NULL;
    funcInfo *ret = NULL;
    funcInfo *curFuncList = NULL;
    xmlDocPtr doc;
    xmlNodePtr cur;
    xmlKeepBlanksDefault(0);
    doc = xmlParseFile(xmlFilePath);
    if(doc == NULL )
    {
        memset(error_info, 0, LOGINFO_LENGTH);
        sprintf(error_info, "Document(%s) not parsed successfully. \n", xmlFilePath);
		RecordLog(error_info);
        return NULL;
    }
    cur = xmlDocGetRootElement(doc);
    if (cur == NULL)
    {
        memset(error_info, 0, LOGINFO_LENGTH);
        sprintf(error_info, "empty document(%s). \n", xmlFilePath);
		RecordLog(error_info);  
        xmlFreeDoc(doc);
        return NULL;
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
            xmlChar* attr_value = NULL;
            while(temp_cur != NULL)
            {
                if(!xmlStrcmp(temp_cur->name, (const xmlChar*)"name"))
                {
                    attr_value = getLine(temp_cur);
                    break;
                }
                temp_cur = temp_cur->next;
            }
            
            varType *beginVarType = ExtractVarType(funcNode);
            varType *currentVarType = beginVarType;

            bool isInfluence = false;
            varDef varInfo;
            memset(&varInfo, 0, sizeof(varDef));
            strcpy(varInfo.varName, varName);
            varInfo.line = 0;
            current = varScliceFunc(varInfo, funcNode, currentVarType, true);
            if(begin == NULL)
                begin = end = current;
            else if(current != NULL)
                end = end->next = current;
            while(current != NULL)
            {
                isInfluence = true;
                end = current;
                current = current->next;
            }
            varDef *varInfluence = ScliceInflVar(varName, funcNode, currentVarType);
            if(varInfluence != NULL)
            {
                isInfluence = true;
                varDef *varInfluCur = varInfluence;
                while(varInfluCur != NULL)
                {
                    printf("\033[40;34m%s influence %s(%d)\033[0m\n", varName, varInfluCur->varName, varInfluCur->line);
                    current = varScliceFunc(*varInfluCur, funcNode, currentVarType, true);
                    if(begin == NULL)
                        begin = end = current;
                    else if(current != NULL)
                        end = end->next = current;
                    while(current != NULL)
                    {
                        end = current;
                        current = current->next;
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
            if(isInfluence)
            {
                memset(src_dir, 0, DIRPATH_MAX);
                //删除开头的temp_和结尾的.xml
                strncpy(src_dir, (char *)&(xmlFilePath[5]), strlen(xmlFilePath)-9);
                printf("\033[40;36msource Path: %s\tfunction: %s(%s)\033[0m\n", src_dir, (char*)xmlNodeGetContent(temp_cur), attr_value);
            }
        }
        cur = cur->next;
    }
      
    xmlFreeDoc(doc);
    current = begin;
    while(current != NULL)
    {
        if(ret == NULL)
            ret = curFuncList = malloc(sizeof(funcInfo));
        else
            curFuncList = curFuncList->next = malloc(sizeof(funcInfo));
        memset(curFuncList, 0, sizeof(funcInfo));
        strcpy(curFuncList->funcName, current->funcName);
        curFuncList->type = current->type;
        curFuncList->calledLine = current->calledLine;
        if(curFuncList->type == 'S')
        {
            strcpy(curFuncList->sourceFile, current->sourceFile);
            strcpy(curFuncList->argumentType, current->argumentType);
            strcpy(curFuncList->funcType, current->funcType);
        }
        begin = begin->next;
        free(current);
        current = begin;
    }
    return ret;
}

confVarDefValue judgeVarDefValueModel(char *varName, xmlNodePtr expr)
{
    confVarDefValue ret;
    ret.defValue = -1;
    if(xmlStrcmp(expr->name, (const xmlChar*)"expr"))
        return ret;
    xmlNodePtr exprChildren = expr->children;
    char *operatorContent = NULL;
    while(exprChildren != NULL)
    {
        if(!xmlStrcmp(exprChildren->name, (const xmlChar*)"operator"))
        {
            operatorContent = (char *)xmlNodeGetContent(exprChildren);
            if(operatorContent[0] == '<' || operatorContent[0] == '>')
            {
                xmlNodePtr prev = exprChildren->prev;
                xmlNodePtr next = exprChildren->next;
                if(prev != NULL && next != NULL)
                {
                    if(!xmlStrcmp(prev->name, (const xmlChar*)"name") && \
                    !xmlStrcmp(next->name, (const xmlChar*)"literal") && \
                    !xmlStrcmp(xmlGetProp(next, (xmlChar*)"type"), (const xmlChar*)"number"))
                    {
                        /*
                         * varaible-name Operator default-value
                         * example: varName < 100
                         */
                        if(strcasecmp((char *)xmlNodeGetContent(prev), varName) == 0)
                        {
                            //judge maximum value or minimum value
                            if(operatorContent[0] == '<')
                                ret.type = false;
                            else
                                ret.type = true;
                            ret.defValue = StrToInt((char *)xmlNodeGetContent(next));
                            
                            return ret;
                        }
                    }
                    else if(!xmlStrcmp(next->name, (const xmlChar*)"name") && \
                    !xmlStrcmp(prev->name, (const xmlChar*)"literal") &&\
                    !xmlStrcmp(xmlGetProp(prev, (xmlChar*)"type"), (const xmlChar*)"number"))
                    {
                        /*
                         * default-value Operator varaible-name
                         * example: 100 > varName
                         */
                        if(strcasecmp((char *)xmlNodeGetContent(next), varName) == 0)
                        {
                            //judge maximum value or minimum value
                            if(operatorContent[0] == '<')
                                ret.type = true;
                            else
                                ret.type = false;
                            ret.defValue = StrToInt((char *)xmlNodeGetContent(prev));
                            
                            return ret;
                        }  
                    }
                }
            }
        }
        exprChildren = exprChildren->next;
    }
    
    return ret;
}
confVarDefValue getVarDefValueFromNode(char *varName, xmlNodePtr funcNode, bool flag)
{
    confVarDefValue ret;
    ret.defValue = -1;
#if DEBUG == 1
    int currentLine = 0;
#endif
    while(funcNode != NULL)
    {
        if(!xmlStrcmp(funcNode->name, (const xmlChar*)"if"))
        {
#if DEBUG == 1
            currentLine = StrToInt((char *)xmlGetProp(funcNode, (xmlChar*)"line"));
#endif
            xmlNodePtr condition = funcNode->children;
            while(condition != NULL)
            {
                if(!xmlStrcmp(condition->name, (const xmlChar*)"condition"))
                {
                    xmlNodePtr conditionExpr = condition->children;
                    while(conditionExpr != NULL)
                    {
                        if(!xmlStrcmp(conditionExpr->name, (const xmlChar*)"expr"))
                        {
                            ret = judgeVarDefValueModel(varName, conditionExpr);
                            if(ret.defValue != -1)
                            {
#if DEBUG == 1
                                printf("default value line: %d\n", currentLine);
#endif
                                return ret;
                            }
                            else
                                goto next;
                        }
                        conditionExpr = conditionExpr->next;
                    }
                }
                condition = condition->next;
            }
        }
next:
        ret = getVarDefValueFromNode(varName, funcNode->children, false);
        if(ret.defValue != -1)
            return ret;
    
        if(flag)
            break;
        
        funcNode = funcNode->next;
    }
    
    return ret;
}

char *getParaNameByIndex(xmlNodePtr parameterListNode, int index)
{
    if(xmlStrcmp(parameterListNode->name, (const xmlChar*)"parameter_list"))
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
        
        parameterNode = parameterNode->next;
    }
    
    return NULL;
}

<<<<<<< HEAD
int getArguPosition(char *paraName, xmlNodePtr paraListNode)
=======
int getParaPosition(char *paraName, xmlNodePtr paraListNode)
>>>>>>> 65a04831e26563b7c2393cc25205d4a8b9ea6c85
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
            if(strstr((char *)xmlNodeGetContent(argument), paraName) != NULL)
                return ret;
            ret++;
        }
        
        argument = argument->next;
    }
    
    return -1;
}

confVarDefValue ExtractSpeciParaDefValue(int paraIndex, char *funcName, char *xmlFilePath, \
    varDirectInflFunc *(*DirectInflFunc)(char *, xmlNodePtr, varType *, bool))
{
    confVarDefValue ret;
    ret.defValue = -1;
    xmlDocPtr doc;
    xmlNodePtr cur;
    xmlKeepBlanksDefault(0);
    doc = xmlParseFile(xmlFilePath);
    if(doc == NULL )
    {
        memset(error_info, 0, LOGINFO_LENGTH);
        sprintf(error_info, "Document(%s) not parsed successfully. \n", xmlFilePath);
		RecordLog(error_info);
        return ret;
    }
    cur = xmlDocGetRootElement(doc);
    if (cur == NULL)
    {
        memset(error_info, 0, LOGINFO_LENGTH);
        sprintf(error_info, "empty document(%s). \n", xmlFilePath);
		RecordLog(error_info);  
        xmlFreeDoc(doc);
        return ret;
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
                    if(strcasecmp(funcName, (char*)xmlNodeGetContent(temp_cur)) == 0)
                    {
                        while(temp_cur != NULL)
                        {
                            if(!xmlStrcmp(temp_cur->name, (const xmlChar*)"parameter_list"))
                            {
                                //get specific position parameter name
                                char *paraName = getParaNameByIndex(temp_cur, paraIndex);
                                //handle the (const char* fmt, ...) parameter
                                if(paraName != NULL)
                                {
                                    ret = getVarDefValue(paraName, funcNode);
                                    if(ret.defValue == -1)
                                    {
                                        varDirectInflFunc *begin = NULL;
                                        varDirectInflFunc *current = NULL;
                                        if(judgeCSrcXmlFile(xmlFilePath))
                                        {
                                            begin = current = DirectInflFunc(paraName, funcNode, NULL, true);
                                        }
                                        else
                                        {
                                            varType *beginVarType = ExtractVarType(funcNode);
                                            varType *currentVarType = beginVarType;
                                            begin = current = DirectInflFunc(paraName, funcNode, currentVarType, true);
                                            while(currentVarType != NULL)
                                            {
                                                beginVarType = beginVarType->next;
                                                free(currentVarType);
                                                currentVarType = beginVarType;
                                            }
                                        }
                                        
                                        while(current != NULL)
                                        {
                                            ret = ExtractSpeciParaDefValue(current->index, current->funcName, current->xmlFilePath, DirectInflFunc);
                                            if(ret.defValue != -1)
                                                break;
                                            current = current->next;
                                        }
                                        current = begin;
                                        while(current != NULL)
                                        {
                                            begin = begin->next;
                                            free(current);
                                            current = begin;
                                        }
                                    }
                                    xmlFreeDoc(doc);
                                    return ret;
                                }
                                else
                                {
                                    //direct break program
                                    break;
                                }
                            }
                            temp_cur = temp_cur->next;
                        }
                    }
                }
                temp_cur = temp_cur->next;
            }
        }
        cur = cur->next;
    }
      
    xmlFreeDoc(doc);
    
    return ret;
}
