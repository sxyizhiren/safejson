1.
main.cpp是测试功能用的，
safejson.cpp是主要文件
safejson.h是头文件

2.

json文件夹是来自jsoncpp的头文件，我用的是它0.5的版本，如果版本不同，请使用对应的头文件。
使用的时候要libjson.a记得要链接进去


3.
为了跑main.cpp，我在safejson.h中开了测试宏_UNITTEST_。
如果你不想要，可以注掉。留着也没有影响


4.
使用方法
#include "SafeJson.h"
SafeJson sjson(request);
If(sjson[“a”][“b”][“c”].test(Json::stringValue))
{
 Std::string user = request[“a”][“b”][“c”].asString();
}
else
{
std::cout << sjson.getException().code << std::endl;
std::cout << sjson.getException().info << std::endl;
}
这样就能保证不回被断言。而且功能完全能够满足你的要求

