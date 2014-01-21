#include "SafeJson.h"
#include <stdio.h>


extern int g_inst_num;


#define Jassert(a,b) if((a) != (b)){printf("%s:%d >> Jassert fail!\n",__FILE__,__LINE__);}

#define JSONSTR "{\"a\":[1,\"2\",true],\"b\":123,\"c\":\"123\"}"
/*
{
   "a" : [ 1, "2", 3 ],
   "b" : 123,
   "c" : "123"
}
*/



//测试once宏
void testOnce(Json::Value& root)
{
//oncejson测试
	g_inst_num = 0;
	Jassert(ONCEJSON(root)["a"]["b"].asString(),"");
	Jassert(ONCEJSON(root)["a"]["b"].asCString(),NULL);
	Jassert(ONCEJSON(root)["a"]["b"].asInt(),0);
	Jassert(ONCEJSON(root)["a"]["b"].asUInt(),0u);
	Jassert(ONCEJSON(root)["a"]["b"].asDouble(),0.0);
	Jassert(g_inst_num,5);

	Jassert(ONCEJSON(root)["b"].asString(),"");
	Jassert(ONCEJSON(root)["b"].asCString(),NULL);
	Jassert(ONCEJSON(root)["b"].asInt(),123);
	Jassert(ONCEJSON(root)["b"].asUInt(),123u);
	Jassert(ONCEJSON(root)["b"].asDouble(),123.0);
	Jassert(g_inst_num,10);
	
	Jassert(ONCEJSON(root)["c"].asString(),"123");
	Jassert(ONCEJSON(root)["c"].asCString(),root["c"].asCString());
	Jassert(ONCEJSON(root)["c"].asInt(),0);
	Jassert(ONCEJSON(root)["c"].asUInt(),0u);
	Jassert(ONCEJSON(root)["c"].asDouble(),0.0);
	Jassert(g_inst_num,15);
	
//AUTOONCE测试
	//默认值测试
	Jassert(AUTOONCE(root)["a"]["b"].asString(),"");
	Jassert(AUTOONCE(root)["a"]["b"].asCString(),NULL);
	Jassert(AUTOONCE(root)["a"]["b"].asInt(),0);
	Jassert(AUTOONCE(root)["a"]["b"].asUInt(),0u);
	Jassert(AUTOONCE(root)["a"]["b"].asDouble(),0.0);
	Jassert(g_inst_num,20);
	
	//数值转字符串
	Jassert(AUTOONCE(root)["b"].asString(),"123");
	Jassert(AUTOONCE(root)["b"].asCString(),NULL);
	Jassert(AUTOONCE(root)["b"].asInt(),123);
	Jassert(AUTOONCE(root)["b"].asUInt(),123u);
	Jassert(AUTOONCE(root)["b"].asDouble(),123.0);
	Jassert(g_inst_num,25);
	
	//字符串转数值
	Jassert(AUTOONCE(root)["c"].asString(),"123");
	Jassert(AUTOONCE(root)["c"].asCString(),root["c"].asCString());
	Jassert(AUTOONCE(root)["c"].asInt(),123);
	Jassert(AUTOONCE(root)["c"].asUInt(),123u);
	Jassert(AUTOONCE(root)["c"].asDouble(),123.0);
	Jassert(g_inst_num,30);

	
}

void testDefault(Json::Value& root)
{
	g_inst_num = 0;
	
	{
	SafeJson sjson(root);
	//错误的下标默认值
	Jassert(sjson["a"]["b"].asString(),"");
	Jassert(sjson["a"]["b"].asCString(),NULL);
	Jassert(sjson["a"]["b"].asInt(),0);
	Jassert(sjson["a"]["b"].asUInt(),0u);
	Jassert(sjson["a"]["b"].asDouble(),0.0);
	
	//number->类型错误默认值
	Jassert(sjson["b"].asString(),"");
	Jassert(sjson["b"].asCString(),NULL);
	Jassert(sjson["b"].asInt(),123);
	Jassert(sjson["b"].asUInt(),123u);
	Jassert(sjson["b"].asDouble(),123.0);
	
	//string->类型错误默认值
	Jassert(sjson["c"].asString(),"123");
	Jassert(sjson["c"].asCString(),root["c"].asCString());
	Jassert(sjson["c"].asInt(),0);
	Jassert(sjson["c"].asUInt(),0u);
	Jassert(sjson["c"].asDouble(),0.0);
	
	Jassert(g_inst_num,0);
	}
	Jassert(g_inst_num,1);
	
}

void testrelease(Json::Value& root)
{
	g_inst_num = 0;
	//new对象测试
	SafeJson *pjson = new SafeJson(root);
	Jassert((*pjson)["b"].asString(),"");
	Jassert((*pjson)["c"].asUInt(),0u);
	delete pjson;
	Jassert(g_inst_num,1);

	
}

//因为会改变参数，所以不采用引用传入
void testFLAG_REWRITE(Json::Value root)
{
	g_inst_num = 0;
	{
	//覆盖原值
	SafeJson sjson(root,FLAG_REWRITE);
	sjson["b"][2] = "arr";
	Jassert(sjson["b"][2].asString(),"arr");	//覆盖原值
	sjson["b"]["2"]["3"]["4"] = "obj";
	Jassert(sjson["b"]["2"]["3"]["4"].asString(),"obj");	//覆盖原值
	Jassert(g_inst_num,0);
	}
	Jassert(g_inst_num,1);
}


void testFLAG_ONLYONCE(Json::Value& root)
{
	
	{
	g_inst_num = 0;
	SafeJson sjson=ONCEJSON(root);
	sjson["b"][2] = "arr";
	Jassert(g_inst_num,0);
	}
	Jassert(g_inst_num,1);
	
	{
	g_inst_num = 0;
	SafeJson sjson=ONCEJSON(root);
	sjson["b"].asString();
	Jassert(g_inst_num,0);
	}
	Jassert(g_inst_num,1);
	
	{
	g_inst_num = 0;
	SafeJson sjson=ONCEJSON(root);
	sjson["b"].asInt();
	Jassert(g_inst_num,0);
	}
	Jassert(g_inst_num,1);
	
	{
	g_inst_num = 0;
	SafeJson sjson=ONCEJSON(root);
	sjson["b"].asUInt();
	Jassert(g_inst_num,0);
	}
	Jassert(g_inst_num,1);
	
	{
	g_inst_num = 0;
	SafeJson sjson=ONCEJSON(root);
	sjson["b"].asDouble();
	Jassert(g_inst_num,0);
	}
	Jassert(g_inst_num,1);
	
	{
	g_inst_num = 0;
	SafeJson sjson=ONCEJSON(root);
	sjson["b"].asBool();
	Jassert(g_inst_num,0);
	}
	Jassert(g_inst_num,1);
	
	{
	g_inst_num = 0;
	SafeJson sjson=ONCEJSON(root);
	sjson["b"].test(Json::intValue);
	Jassert(g_inst_num,0);
	}
	Jassert(g_inst_num,1);
	
	{
	g_inst_num = 0;
	SafeJson sjson=ONCEJSON(root);
	sjson["b"].test(Json::uintValue);
	Jassert(g_inst_num,0);
	}
	Jassert(g_inst_num,1);
	
	{
	g_inst_num = 0;
	SafeJson sjson=ONCEJSON(root);
	sjson["b"].test(Json::realValue);
	Jassert(g_inst_num,0);
	}
	Jassert(g_inst_num,1);
	
	{
	g_inst_num = 0;
	SafeJson sjson=ONCEJSON(root);
	sjson["b"].test(Json::stringValue);
	Jassert(g_inst_num,0);
	}
	Jassert(g_inst_num,1);
	
	{
	g_inst_num = 0;
	SafeJson sjson=ONCEJSON(root);
	sjson["b"].test(Json::arrayValue);
	Jassert(g_inst_num,0);
	}
	Jassert(g_inst_num,1);
	
	{
	g_inst_num = 0;
	SafeJson sjson=ONCEJSON(root);
	sjson["b"].test(Json::objectValue);
	Jassert(g_inst_num,0);
	}
	Jassert(g_inst_num,1);
	
	{
	g_inst_num = 0;
	SafeJson sjson=ONCEJSON(root);
	sjson["b"].test(Json::nullValue);
	Jassert(g_inst_num,0);
	}
	Jassert(g_inst_num,1);
	
	{
	g_inst_num = 0;
	SafeJson sjson=ONCEJSON(root);
	sjson["b"].test(Json::objectValue);
	Jassert(g_inst_num,0);
	}
	Jassert(g_inst_num,1);
	
	{
	g_inst_num = 0;
	SafeJson sjson=ONCEJSON(root);
	sjson["b"].test(Json::booleanValue);
	Jassert(g_inst_num,0);
	}
	Jassert(g_inst_num,1);
	
}


void testFLAG_STREQNUM(Json::Value& root)
{
	g_inst_num = 0;
	{
	SafeJson sjson(root,FLAG_STREQNUM);
	//默认值测试
	Jassert(sjson["a"]["b"].asString(),"");
	Jassert(sjson["a"]["b"].asCString(),NULL);
	Jassert(sjson["a"]["b"].asInt(),0);
	Jassert(sjson["a"]["b"].asUInt(),0u);
	Jassert(sjson["a"]["b"].asDouble(),0.0);
	
	//数值转字符串
	Jassert(sjson["b"].asString(),"123");
	Jassert(sjson["b"].asCString(),NULL);
	Jassert(sjson["b"].asInt(),123);
	Jassert(sjson["b"].asUInt(),123u);
	Jassert(sjson["b"].asDouble(),123.0);
	
	//字符串转数值
	Jassert(sjson["c"].asString(),"123");
	Jassert(sjson["c"].asCString(),root["c"].asCString());
	Jassert(sjson["c"].asInt(),123);
	Jassert(sjson["c"].asUInt(),123u);
	Jassert(sjson["c"].asDouble(),123.0);
	
	Jassert(g_inst_num,0);
	}
	
	Jassert(g_inst_num,1);
}


void testFLAG_STRICTMEMBER(Json::Value& root)
{
	g_inst_num = 0;
	{
	//asXX失败
	SafeJson sjson(root,FLAG_STRICTMEMBER);
	Jassert(sjson["a"].test(Json::nullValue),true);
	Jassert(sjson["a"].test(/*等价于Json::nullValue，表示不检查值的目标类型，只检查路径*/),true);
	Jassert(sjson["a"].test(Json::arrayValue),false);
	Jassert(sjson["a"].test(Json::objectValue),false);
	Jassert(sjson["a"].test(Json::intValue),false);
	Jassert(sjson["a"].test(Json::uintValue),false);
	Jassert(sjson["a"].test(Json::realValue),false);
	Jassert(sjson["a"].test(Json::booleanValue),true);//boolean is always ok
	Jassert(sjson["a"].test(Json::stringValue),false);
	
	//下标失败
	Jassert(sjson["e"].test(Json::nullValue),false);
	Jassert(sjson["e"].test(Json::arrayValue),false);
	Jassert(sjson["e"].test(Json::objectValue),false);
	Jassert(sjson["e"].test(Json::intValue),false);
	Jassert(sjson["e"].test(Json::uintValue),false);
	Jassert(sjson["e"].test(Json::realValue),false);
	Jassert(sjson["e"].test(Json::booleanValue),false);
	Jassert(sjson["e"].test(Json::stringValue),false);
	
	Jassert(sjson["a"][3].test(Json::nullValue),false);
	Jassert(sjson["a"][3].test(Json::arrayValue),false);
	Jassert(sjson["a"][3].test(Json::objectValue),false);
	Jassert(sjson["a"][3].test(Json::intValue),false);
	Jassert(sjson["a"][3].test(Json::uintValue),false);
	Jassert(sjson["a"][3].test(Json::realValue),false);
	Jassert(sjson["a"][3].test(Json::booleanValue),false);
	Jassert(sjson["a"][3].test(Json::stringValue),false);
	
	//成功
	Jassert(sjson["a"][0u].test(Json::intValue),true);
	Jassert(sjson["a"][0u].test(Json::uintValue),true);
	Jassert(sjson["a"][0u].test(Json::realValue),true);
	Jassert(sjson["a"][1].test(Json::stringValue),true);
	Jassert(sjson["a"][2].test(Json::booleanValue),true);
	Jassert(sjson["a"][2].test(Json::stringValue),false);//虽然boolean的asString是可以的但是其asCString是不可以的，所以asString也当作不可以了

	
	Jassert(g_inst_num,0);
	}
	
	Jassert(g_inst_num,1);
}

void testException(Json::Value& root)
{
	g_inst_num = 0;
	{

	SafeJson sjson(root);
	//asXX
	Jassert(sjson["a"].test(Json::stringValue),false);	
	Jassert(sjson.getException().code,JSON_WRONG_ASSOMETYPE);
	Jassert(sjson.getException().info,"[Array]->[asString]");
	
	Jassert(sjson["a"].test(Json::intValue),false);	
	Jassert(sjson.getException().code,JSON_WRONG_ASSOMETYPE);
	Jassert(sjson.getException().info,"[Array]->[asInt]");
	
		Jassert(sjson["a"].test(Json::uintValue),false);	
	Jassert(sjson.getException().code,JSON_WRONG_ASSOMETYPE);
	Jassert(sjson.getException().info,"[Array]->[asUInt]");
	
		Jassert(sjson["a"].test(Json::realValue),false);	
	Jassert(sjson.getException().code,JSON_WRONG_ASSOMETYPE);
	Jassert(sjson.getException().info,"[Array]->[asDouble]");
	
		Jassert(sjson["a"].test(Json::objectValue),false);	
	Jassert(sjson.getException().code,JSON_WRONG_ASSOMETYPE);
	Jassert(sjson.getException().info,"[Array]->[asObject]");
	
		Jassert(sjson["a"].test(Json::arrayValue),false);	
	Jassert(sjson.getException().code,JSON_WRONG_ASSOMETYPE);
	Jassert(sjson.getException().info,"[Array]->[asArray]");
	
		Jassert(sjson["a"].test(Json::nullValue),true);//null表示只检查路径	
	Jassert(sjson.getException().code,JSON_RIGHT);
	Jassert(sjson.getException().info,"");
	
	//not strict
	Jassert(sjson["e"].test(Json::arrayValue),true);	//空对象可自然转
	Jassert(sjson["e"].test(Json::objectValue),true);	//空对象可自然转
	Jassert(sjson["e"].test(Json::stringValue),true);
	Jassert(sjson["e"].test(Json::booleanValue),true);
	Jassert(sjson["e"].test(Json::uintValue),true);
	//can repeat
	Jassert(sjson["f"]["g"].test(Json::uintValue),true);
	root["f"]["g"].asString();
	Jassert(sjson["f"]["g"].test(Json::uintValue),true);	//asXX之后可重复判定
	
	
	//["key"]
		Jassert(sjson["b"]["bbbbb"].test(Json::stringValue),false);	
	Jassert(sjson.getException().code,JSON_WRONG_GETOBJECT);
	Jassert(sjson.getException().info,"[Number:123]->[\"bbbbb\"]");//number可以得到具体值
	
	//[index]
		Jassert(sjson[6].test(Json::stringValue),false);	
	Jassert(sjson.getException().code,JSON_WRONG_GETARRAY);
	Jassert(sjson.getException().info,"[Object]->[6]");
	
	
	//strict
	SafeJson sjson1(root,FLAG_STRICTMEMBER);
	//根对象没有字符下标
	Jassert(sjson1["h"].test(Json::arrayValue),false);	
	Jassert(sjson1.getException().code,JSON_WRONG_NOMEMBER);
	Jassert(sjson1.getException().info,"[Object]->[\"h\"]");
	Jassert(sjson1["h"].test(Json::objectValue),false);	
	Jassert(sjson1.getException().code,JSON_WRONG_NOMEMBER);
	Jassert(sjson1.getException().info,"[Object]->[\"h\"]");
	
	//有子对象，类型错误
	Jassert(sjson1["a"][0u].test(Json::stringValue),false);
	Jassert(sjson1.getException().code,JSON_WRONG_ASSOMETYPE);
	Jassert(sjson1.getException().info,"[Number:1]->[asString]");
	
	//0下标（NULL）被忽略
	//同时测试成功时的exception对不对
	char *pc=NULL;
		Jassert(sjson1["a"][pc][1].test(Json::stringValue),true);//
	Jassert(sjson1.getException().code,JSON_RIGHT);
	Jassert(sjson1.getException().info,"");
	
	//数组下标过大
	Jassert(sjson1["a"][3].test(Json::stringValue),false);
	Jassert(sjson1.getException().code,JSON_WRONG_NOMEMBER);
	Jassert(sjson1.getException().info,"[Array]->[3]");
	
	//对象字符下标错误
		Jassert(sjson1["a"]["3"].test(Json::stringValue),false);
	Jassert(sjson1.getException().code,JSON_WRONG_GETOBJECT);
	Jassert(sjson1.getException().info,"[Array]->[\"3\"]");
	

	
	Jassert(g_inst_num,0);
	}
	Jassert(g_inst_num,2);
}

//这个要最后测试
void testrootnode(Json::Value& root)
{
	SafeJson sjson(root);
	sjson = 123;
	//assert error
}


void testDouble(Json::Value root)
{
	root["b"] = 123.99;
	g_inst_num = 0;
		
	{
	SafeJson sjson(root);

	Jassert(sjson["b"].asString(),"");
	Jassert(sjson["b"].asCString(),(const char*)NULL);
	Jassert(sjson["b"].asInt(),123);
	Jassert(sjson["b"].asUInt(),123u);
	Jassert(sjson["b"].asDouble(),123.99);

	SafeJson sjson1(root,FLAG_STREQNUM);

	Jassert(sjson1["b"].asString(),"123.990");
	Jassert(sjson1["b"].asCString(),(const char*)NULL);
	Jassert(sjson1["b"].asInt(),123);
	Jassert(sjson1["b"].asUInt(),123u);
	Jassert(sjson1["b"].asDouble(),123.99);
	Jassert(g_inst_num,0);
	}
	Jassert(g_inst_num,2);


}


int main()
{
	Json::Reader reader;
	Json::Value root;
	bool parsingSuccessful = reader.parse( JSONSTR, root );
	if ( !parsingSuccessful )
	{
		printf("Failed to parse %s\n",reader.getFormatedErrorMessages().c_str());
		return 1;
	}

	
	printf("=>%s\n",root.toStyledString().c_str());
	
	printf("====>testOnce\n");
	testOnce(root);
	
	printf("====>testDefault\n");
	testDefault(root);
	
	printf("====>testrelease\n");
	testrelease(root);
	
	printf("====>testFLAG_REWRITE\n");
	testFLAG_REWRITE(root);
	
	printf("====>testFLAG_ONLYONCE\n");
	testFLAG_ONLYONCE(root);
	
	printf("====>testFLAG_STREQNUM\n");
	testFLAG_STREQNUM(root);
	
	printf("====>testFLAG_STRICTMEMBER\n");
	testFLAG_STRICTMEMBER(root);
	
	printf("====>testException\n");
	testException(root);
	
	printf("====>testDouble\n");
	testDouble(root);
	/*
	Json::Value jv;
	jv["a"]["b"]["c"] = 123;
	Json::Value *pjv=&jv["a"]["b"]["c"];	
	std::cout << pjv << std::endl;
	
	SafeJson sj(jv,FLAG_REWRITE);
	sj["a"]["b"]["c"]["d"] = 3;
	pjv=&jv["a"]["b"]["c"];	
	std::cout <<jv["a"]["b"]["c"]["d"].asInt() << std::endl;
	std::cout << pjv <<std::endl;
	*/
	return 0;
   
}



   
