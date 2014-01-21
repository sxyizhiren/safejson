/*********************************************************
	-- 金其明	18948
	jsoncpp经常由于值类型不合预期，导致程序挂掉
	为了安全的读写字段值，实现safejson类

**********************************************************/
#ifndef SAFE_JSON_INCLUDE_H
#define SAFE_JSON_INCLUDE_H

#define _UNITTEST_
#include "json/json.h"
#include <stdlib.h>
#include <iostream>
#include <sstream>
#include <cassert>

#define BITMSK(bit)				(int)(1 << (bit))

#define FLAG_REWRITE BITMSK(1)		//[]操作时，类型不同新建空对象替换原对象
//#define FLAG_ONLYONCE BITMSK(2)	//这个不用了
#define FLAG_STREQNUM BITMSK(3)		//string和number之间实现自动互转,这个标志并不能让原来test()为false的变成true。也就是test()的时候，number和string永远是认为不能互转的。不然容易在使用上出错
#define FLAG_STRICTMEMBER BITMSK(4)		//严格检查["key"]是否存在,[index]是否超过大小。虽然这两者并不会引起jsoncpp的assert

typedef struct JSONEXCEPTION
{
	int code;
	std::string info;	
}JsonException,*pJsonException;

enum JSON_STATE
{
	JSON_RIGHT,				
	JSON_WRONG_GETOBJECT,	//cannot ["key"]
	JSON_WRONG_GETARRAY,	//cannot [3]
	JSON_WRONG_ASSOMETYPE,	//cannot asXX()
	JSON_WRONG_NOMEMBER,	//object's key not exist,or array's index exceed size
};

class SafeJson
{
private:
	//不允许默认构造
	SafeJson(){}
public:
	//析构
	~SafeJson();
	/*
		构造函数，必须传入json值，
		param可选:
			FLAG_REWRITE表示打开覆盖模式，对于已经存在的字段（如obj["key"]=1），对他取下标(如obj["key"]["subkey"]=5)会重建对象，
				覆盖原字段内容(覆盖值1，变成obj["key"]["subkey"]=5)。若不加此参数，则不会覆盖，赋值语句会无效，默认不打开该参数。
			FLAG_STREQNUM
				字符串和数值之间自动互转，即"123"和123能互相根据asXX所需的类型自动转换
			FLAG_STRICTMEMBER
				严格检查["key"]是否存在,[index]是否超过大小。虽然这两者并不会引起jsoncpp的assert
	*/
	SafeJson(Json::Value& json,int param=0);
	
	/*
		取子json，返回的还是SafeJson自身.但是内部指针已经指向了子json。
		非FLAG_REWRITE模式下若json非boject类型，会置错误状态。
		FLAG_REWRITE模式若json非boject类型，则重建子对象		
	*/
	SafeJson& operator[]( const char *key );
	
	//同operator[]( const char *key );
	SafeJson& operator[]( const std::string &key );
	
	//取子json，若json非Array类型，逻辑同operator[]( const char *key );
	SafeJson& operator[](unsigned int index);
	
	//赋值
	SafeJson& operator=( const Json::Value& other );
	
	//同jsoncpp的接口
	std::string asString();	
	const char* asCString();//可能返回NULL
	bool asBool();
	double asDouble();
	int asInt();
	unsigned int asUInt();	

public:
	//下面是提供一些额外的功能函数，有需要的时候使用
	
	/*
	*  是否能自然转成目标类型，
	*  返回false可能是某个路径中对某个非object取了["key"]
	*  或者对非array取了[5]，
	*  或者asXX()时jsontype无法转换	
	*  jsontype 为值的目标类型
	*  取Json::nullValue（默认值）时，表示只检查[xx]路径的可达性，并不检查转换成某种类型
	*/	
	bool test(Json::ValueType jsontype = Json::nullValue);
	
	/*
	* 查询当前的状态，正确或者异常
	* 返回值中
	* code:取自JSON_STATE
	* info:是错误信息，格式为"[原内容]->[引起错误的操作]"
	* 原内容为:1.[Object]  2.[Array]  3.[String] 4.[Boolean:true/false]  5.[Number:123/1.23]  6.[Null]
	* 引起错误的操作：1.["key"]  2.[12]  3.[asString]  4.[asInt]  ...(其余见getasXXTag())  
	*/
	const JsonException& getException();
	
private:

	//赋值操作放私有处。禁用掉该用法
	SafeJson& operator=( const SafeJson& other );

	/**********************************************************
	*功能：能否自动转换到目标类型。
	*因为jsoncpp自带的isConvertibleTo有问题:
	*int类型的isConvertibleTo(string)返回true
	*但int类型toString()却是失败的,
	*所以不使用isConvertibleTo判断是否可转换，自己实现一个。
	json的类型：
	namespace Json {
	   enum ValueType
	   {
		  nullValue = 0, ///< 'null' value
		  intValue,      ///< signed integer value
		  uintValue,     ///< unsigned integer value
		  realValue,     ///< double value
		  stringValue,   ///< UTF-8 string value
		  booleanValue,  ///< bool value
		  arrayValue,    ///< array value (ordered list)
		  objectValue    ///< object value (collection of name/value pairs).
	   };
	}
	Json::Value& json 实施转换的目标json对象
	Json::ValueType jsontype 转换的目标类型
	***********************************************************/
	bool canNatureConvert(Json::Value& json,Json::ValueType jsontype);
	
	//必然安全的转换情况
	bool isRealSafe(Json::Value& json,Json::ValueType jsontype);
	
	//重置内部状态
	void reset();
	
	
	//下面几个函数都是为了支持异常信息的记录和获取
	/*
	*获取当前值的内容概要：
	*	[Array]		对应isArray()
	*	[Object]	对应isObject()
	*	[String]	对应isString()
	*	[Number:12345]	对应isInt() || isUInt() || isDouble()
	*	[Boolean:false]	对应isBool()
	*	[Null]
	*/
	std::string getCurValueTag();
	
	//清掉一个string的最后一个\n及其之后的内容
	void trimRightLn(std::string& str);
	
	/*
	*把枚举类型转成字符串asXX，用于表示执行某个操作
		enum ValueType
	   {
		  nullValue = 0,  [asNull]
		  intValue,       [asInt]
		  uintValue,      [asUInt]
		  realValue,      [asDouble]
		  stringValue,    [asString]
		  booleanValue,   [asBool]
		  arrayValue,     [asArray]
		  objectValue     [asObject]
	   };
	*/
	std::string getasXXTag(Json::ValueType jsontype);
	
	//设置状态和exception信息
	void setState(JSON_STATE state,const std::string& actionstr);
	
private:
	//指向初始化时的对象，用于reset时使用
	Json::Value	*m_pjson;
	
	//指向当前json对象或者json的子对象，所有操作都针对其指向的对象，重要
	Json::Value *m_pjsonLast;
	
	//内部状态，指示是否出错
	JSON_STATE m_state;
	
	//功能参数，同构造函数中的param
	int m_param;
	
	//错误信息
	JsonException m_exception;
};

//匿名对象，生命周期只有一行代码
#define ONCEJSON(json) (SafeJson(json))

//自动实现数值和字符串转换的json
#define AUTOONCE(json) (SafeJson(json,FLAG_STREQNUM))

//严格检查成员
#define STRICTONCE(json) (SafeJson(json,FLAG_STRICTMEMBER))

//重建json
#define REWTONCE(json) (SafeJson(json,FLAG_REWRITE))


#endif

