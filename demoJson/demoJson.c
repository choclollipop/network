#include <stdio.h>
#include <json-c/json.h>
#include <json-c/json_object.h>

int main()
{
    struct json_object * jsonObj = json_object_new_object();
    if (jsonObj == NULL)
    {
        perror("new jsonObject error");
        exit(-1);
    }

    struct json_object * nameValue = json_object_new_string("ruoob");
    if (nameValue == NULL)
    {
        free(jsonObj);
        jsonObj = NULL;
        perror("new nameValue error");
        exit(-1);
    }

    int ret = json_object_object_add(jsonObj, "name", nameValue);
    if (ret < 0)
    {
        free(jsonObj);
        jsonObj = NULL;
        free(nameValue);
        nameValue = NULL;
        perror("add nameValue error");
        exit(-1);
    }

    struct json_object * alexaValue = json_object_new_int64(10000);
    if (alexaValue == NULL)
    {
        free(jsonObj);
        jsonObj = NULL;
        perror("new alexaValue error");
        exit(-1);
    }

    ret = json_object_object_add(jsonObj, "alexa", alexaValue);
    if (ret < 0)
    {
        free(jsonObj);
        jsonObj = NULL;
        free(alexaValue);
        alexaValue = NULL;
        perror("add alexaValue error");
        exit(-1);
    }

    /* 数组 */
    struct json_object * array = json_object_new_array();
    if (array == NULL)
    {
        free(jsonObj);
        jsonObj = NULL;
        perror("new array error");
        exit(-1);
    }

    struct json_object * Value1 = json_object_new_string("jingdong");
    struct json_object * Value2 = json_object_new_string("pingduoduo");
    struct json_object * Value3 = json_object_new_string("Taobao");

    json_object_array_put_idx(array, 0, Value1);
    json_object_array_put_idx(array, 1, Value2);
    json_object_array_put_idx(array, 2, Value3);

    ret = json_object_object_add(jsonObj, "shooping", array);
    if (ret < 0)
    {
        free(jsonObj);
        jsonObj = NULL;
        free(array);
        array = NULL;
        perror("add array error");
        exit(-1);
    }

    struct json_object * siteObj = json_object_new_object();
    if (siteObj == NULL)
    {
        free(jsonObj);
        jsonObj = NULL;
        perror("new siteObj error");
        exit(-1);
    }

    struct json_object * site1 = json_object_new_string("www.runoob.com");

    json_object_object_add(siteObj, "site1", site1);


    struct json_object * site2 = json_object_new_string("m.runoob.com");

    json_object_object_add(siteObj, "site2", site2);

    json_object_object_add(jsonObj, "sites", siteObj);


    const char * ptr = json_object_to_json_string(jsonObj);

    printf("ptr : %s\n", ptr);

    return 0;
}