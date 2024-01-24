#include <stdio.h>
#include <json-c/json.h>
#include <json-c/json_object.h>
#include <unistd.h>

int main()
{
    /* 新进json对象 */
    struct json_object * jsonObj = json_object_new_object();
    if (jsonObj == NULL)
    {
        perror("new json object error");
        exit(-1);
    }

    struct json_object * wayValue = json_object_new_int64(32);
    if (wayValue == NULL)
    {
        perror("new json object error");
        exit(-1);
    }

    int ret = json_object_object_add(jsonObj, "way", wayValue);
    if (ret < 0)
    {
        /* 释放内存 .....*/

    }

    struct json_object * nameValue = json_object_new_string("zhangsan");

    ret = json_object_object_add(jsonObj, "name", nameValue);
    if (ret < 0)
    {

    }

    const char * ptr = json_object_to_json_string(jsonObj);

    printf("ptr : %s\n", ptr);

    /* 将字符串转换成json对象 */
    struct json_object * ageObj = json_tokener_parse(ptr);

    struct json_object * keyValue = json_object_object_get(ageObj, "age");
    

    int64_t value = json_object_get_int64(keyValue);

    // const char * value = json_object_to_json_string(keyValue);
    printf("value:%ld\n", value);

    return 0;
}