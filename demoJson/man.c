#include <stdio.h>
#include <json-c/json.h>
#include <json-c/json_object.h>

int main()
{
    /* 新进json对象 */
    struct json_object * jsonObj = json_object_new_object();

    struct json_object * value1 = json_object_new_int64(32);

    json_object_object_add(jsonObj, "age", value1);

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