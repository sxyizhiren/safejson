#include "SafeJson.h"


//是否需要覆盖原值
#define ISREWRITE(arg) ((arg) & FLAG_REWRITE)
//是否字符串和数字间自动转
#define ISSTREQNUM(arg) ((arg) & FLAG_STREQNUM)
//是否严格检查成员
#define ISSTRICTMEMBER(arg)	((arg) & FLAG_STRICTMEMBER)
//状态不对，即已经出错，并且不是覆盖模式，则执行FalseAction
#define CHECK_WRONG(FalseAction) if(m_state != JSON_RIGHT){ FalseAction ;}
//reset之后return
#define RESET_RETURN(arg) reset();return (arg)
//设置异常信息
#define SETEXCEPTION(ecode,einfo) m_exception.code=ecode;m_exception.info=einfo
//是否数字类型
#define ISNUMBERTYPE(ptrjson) ((ptrjson)->isInt() || (ptrjson)->isUInt() || (ptrjson)->isDouble())
//拼接3个字符串，但不限3个变量的类型，可以为int，string等简单类型
#define CREATESTREAM(streamname,str1,str2,str3) std::stringstream streamname;streamname << (str1) << (str2) << (str3)


#ifdef _UNITTEST_
	int g_inst_num = 0;	//测试对象释放
#endif


//构造
SafeJson::SafeJson(Json::Value& json,int param)
{
	//保存传入的值
	m_pjson = &json;
	m_param = param;
	//初始化一些值
	m_pjsonLast = m_pjson;
	setState(JSON_RIGHT,"");
}

//析构
SafeJson::~SafeJson()
{
#ifdef _UNITTEST_
	//std::cout << ">>Release SafeJson." << std::endl;
	g_inst_num ++;
#endif
}

//重置内部对象
void SafeJson::reset()
{
	//json指针指向初始的位置并恢复状态;
	m_pjsonLast = m_pjson;
	m_state=JSON_RIGHT; //只改状态不改exception
}

//取json子对象
SafeJson& SafeJson::operator[]( const char *key )
{
	CHECK_WRONG(return *this);
	if(NULL == key){return *this;}	//过滤空指针，啥也不做
	
	if(m_pjsonLast->isObject())
	{
		if(ISSTRICTMEMBER(m_param) && !(m_pjsonLast->isMember(key)))
		{
			CREATESTREAM(ss, "[\"", key, "\"]");
			setState(JSON_WRONG_NOMEMBER,ss.str());
		}
		else
		{
			m_pjsonLast = &(*m_pjsonLast)[key];
		}
	}
	else
	{
		if(!ISREWRITE(m_param))
		{
			//设置标志位，后面就可以ignore其他的读写操作了，直到reset。
			CREATESTREAM(ss, "[\"", key, "\"]");
			setState(JSON_WRONG_GETOBJECT,ss.str());
		}
		else
		{
			//覆盖掉原来的值
			(*m_pjsonLast) = Json::Value(Json::objectValue);
			m_pjsonLast = &(*m_pjsonLast)[key];
		}
	}
	
	return *this;
}

//取json子对象
SafeJson& SafeJson::operator[]( const std::string &key )
{
	return (*this)[ key.c_str() ];
}

//取json子对象
SafeJson& SafeJson::operator[](unsigned int index)
{
	CHECK_WRONG(return *this);
	
	//如果取下标不会出错
	if(m_pjsonLast->isArray())
	{
		//如果设置了严格的成员检查，并且目标下标超过了当前大小，就设置错误信息，
		//这种情况不会引起assert错误，但是业务上可能需要这种严格检查的功能
		if(ISSTRICTMEMBER(m_param) && (index >=  m_pjsonLast->size()))
		{
			//构造字符串
			CREATESTREAM(ss, "[", index, "]");
			setState(JSON_WRONG_NOMEMBER,ss.str());
		}
		else
		{
			//直接进入子对象
			m_pjsonLast = &(*m_pjsonLast)[index];
		}
	}
	else
	{
		if(!ISREWRITE(m_param))
		{
			//设置标志位，后面就可以ignore其他的读写操作了，直到reset。
			CREATESTREAM(ss, "[", index, "]");
			setState(JSON_WRONG_GETARRAY,ss.str());
		}
		else
		{
			(*m_pjsonLast) = Json::Value(Json::arrayValue);
			m_pjsonLast = &(*m_pjsonLast)[index];
		}
	}
	
	return *this;

}

void SafeJson::setState(JSON_STATE state,const std::string& actionstr)
{
	m_state=(state); 
	if(JSON_RIGHT == state)
	{
		//忽略第二个参数
		SETEXCEPTION(JSON_RIGHT,"");
	}
	else
	{
		SETEXCEPTION(state,getCurValueTag()+"->"+actionstr);
	}

}

std::string SafeJson::getasXXTag(Json::ValueType jsontype)
{
	switch(jsontype)
	{
		case Json::intValue:
			return "[asInt]";
		case Json::uintValue:
			return "[asUInt]";
		case Json::realValue:
			return "[asDouble]";
		case Json::stringValue:
			return "[asString]";
		case Json::booleanValue:
			return "[asBoolean]";
		case Json::arrayValue:
			return "[asArray]";
		case Json::objectValue:
			return "[asObject]";
		case Json::nullValue:
			return "[asNull]";
		default:
			//UNREACHABLE
			return "[asUnknown]";
			break;
		
	}
}

//查询当前的状态，正确或者异常
const JsonException& SafeJson::getException()
{
	return m_exception;
}

void SafeJson::trimRightLn(std::string& str)
{
	//find_last_of 找最后一次出现的位置，如果要找的字符串长度大于1，返回的是找到的字符串的右端的位置
	size_t  sz = str.find_last_of('\n');
    if(std::string::npos != sz)
    {
        str.erase(sz,std::string::npos);                
    }

}

std::string SafeJson::getCurValueTag()
{
	if(ISNUMBERTYPE(m_pjsonLast))
	{
		std::string tmpstr = m_pjsonLast->toStyledString();
		//删除末尾换行符
		trimRightLn(tmpstr);
		return "[Number:" + tmpstr +"]";
	}
	else if(m_pjsonLast->isArray())
	{
		return "[Array]";
	}
	else if(m_pjsonLast->isObject())
	{
		return "[Object]";
	}
	else if(m_pjsonLast->isString())
	{
		return "[String]";
		//具体字符串需要的话可以加上：m_pjsonLast->asString();但是字符串可能很长，占内存
	}
	else if(m_pjsonLast->isBool())
	{
		CREATESTREAM(ss,"[Bool:", m_pjsonLast->asBool(), "]");
		return ss.str();
	}
	else
	{	
		//isNull()
		return "[Null]";
	}
		
}

SafeJson& SafeJson::operator=( const SafeJson& other )
{
	//UNREACHABLE
	return *this;
}

//赋值
SafeJson& SafeJson::operator=( const Json::Value& other )
{
	CHECK_WRONG(RESET_RETURN(*this));
	//根结点可以被赋值，这基于jsoncpp赋值后地址不会改变，
	//根结点的地址永远不被改变,所以赋值没关系
	//assert(m_pjson != m_pjsonLast);	//不能把根节点覆盖掉(X)
	(*m_pjsonLast) = other;
	RESET_RETURN(*this);
}


//是否能自然转换
bool SafeJson::test(Json::ValueType jsontype)
{
	CHECK_WRONG(RESET_RETURN(false));
	
	if(Json::nullValue == jsontype)
	{
		//Json::nullValue表示不期望转成某种类型，只检查路径。所以直接true
		setState(JSON_RIGHT,"");
		RESET_RETURN(true);
	}
	
	if(canNatureConvert(*m_pjsonLast,jsontype))
	{
		setState(JSON_RIGHT,"");
		RESET_RETURN(true);
	}
	else
	{
		setState(JSON_WRONG_ASSOMETYPE, getasXXTag(jsontype));
		RESET_RETURN(false);
	}
}

//转成string，其他asXX的逻辑和这个相同，注释类似，就不写了
std::string SafeJson::asString()
{
	//如果前面已经出现[]取子对象错误，那么后面操作就忽略之
	//RESET_RETURN将重置参数然后返回目标值
	CHECK_WRONG(RESET_RETURN(""));
	
	//如果能够自然转换(asString())至string类型.
	if(canNatureConvert(*m_pjsonLast,Json::stringValue))
	{
		//因为RESET_RETURN会重置参数，所以m_pjsonLast这个值要暂存起来
		Json::Value	*tmp_pjson = m_pjsonLast;	
		//重置参数然后返回目标值asString()
		RESET_RETURN(tmp_pjson->asString());
	}
	else if(ISSTREQNUM(m_param) && ISNUMBERTYPE(m_pjsonLast))
	{
		//如果不能自然转换，但是设置了字符数字等价的标志，且当前正好是数值类型，那么就进行转换
		std::string tmpstr = m_pjsonLast->toStyledString();
		//删除末尾换行符
		trimRightLn(tmpstr);
		//重置参数然后返回目标值tmpstr
		RESET_RETURN(tmpstr);
	}
	else
	{
		//其他情况无法转换，就返回默认值
		RESET_RETURN("");
	}

}

//转成char*可能返回NULL!
const char* SafeJson::asCString()
{
	CHECK_WRONG(RESET_RETURN(NULL));
	
	//如果能自然转换
	if(canNatureConvert(*m_pjsonLast,Json::stringValue))
	{
		Json::Value	*tmp_pjson = m_pjsonLast;
		RESET_RETURN(tmp_pjson->asCString());
	}
	else
	{
		//asCString无法实现数值互转，asString才行		
		RESET_RETURN(NULL);
	}
}

//转成bool
bool SafeJson::asBool()
{
	CHECK_WRONG(RESET_RETURN(false));
	
	//it is always safe
	if(canNatureConvert(*m_pjsonLast,Json::booleanValue))
	{
		Json::Value	*tmp_pjson = m_pjsonLast;
		RESET_RETURN(tmp_pjson->asBool());
	}
	else
	{
		//UNREACHABLE
		RESET_RETURN(false);
	}
	

}

//转成double
double SafeJson::asDouble()
{
	CHECK_WRONG(RESET_RETURN(0.0));
	
	if(canNatureConvert(*m_pjsonLast,Json::realValue))
	{
		Json::Value	*tmp_pjson = m_pjsonLast;
		RESET_RETURN(tmp_pjson->asDouble());
	}
	else if(ISSTREQNUM(m_param) && m_pjsonLast->isString())
	{
		//字符串转成数值
		std::string tmpstr = m_pjsonLast->asString();
		double tmpret = strtod(tmpstr.c_str(),NULL);
		RESET_RETURN(tmpret);
	}
	else
	{
		RESET_RETURN(0.0);
	}
}

//转成int
int SafeJson::asInt()
{
	CHECK_WRONG(RESET_RETURN(0));
	
	if(canNatureConvert(*m_pjsonLast,Json::intValue))
	{
		Json::Value	*tmp_pjson = m_pjsonLast;
		RESET_RETURN(tmp_pjson->asInt());
	}
	else if(ISSTREQNUM(m_param) && m_pjsonLast->isString())
	{
		//字符串转成数值
		std::string tmpstr = m_pjsonLast->asString();
		int tmpret = atoi(tmpstr.c_str());
		RESET_RETURN(tmpret);
	}
	else
	{
		RESET_RETURN(0);
	}
	

}

//转成uint
unsigned int SafeJson::asUInt()
{
	CHECK_WRONG(RESET_RETURN(0u));
	
	if(canNatureConvert(*m_pjsonLast,Json::uintValue))
	{
		Json::Value	*tmp_pjson = m_pjsonLast;
		RESET_RETURN(tmp_pjson->asUInt());
	}
	else if(ISSTREQNUM(m_param) && m_pjsonLast->isString())
	{
		//字符串转成数值
		std::string tmpstr = m_pjsonLast->asString();
		unsigned int tmpret = atoi(tmpstr.c_str());
		RESET_RETURN(tmpret);
	}
	else
	{
		RESET_RETURN(0u);
	}

}

//是否可转
bool SafeJson::canNatureConvert(Json::Value& json,Json::ValueType jsontype)
{
	if (isRealSafe(json,jsontype))
	{
		return true;
	}
	switch (jsontype)
	{
		case Json::intValue:
		case Json::uintValue:
		case Json::realValue:
			return json.isInt()
				|| json.isUInt()
				|| json.isDouble();
		case Json::stringValue:
			//boolean类型可以asString()，但是不能asCString()，所以统一把boolean类型当作不能转String
			return json.isString();
		case Json::booleanValue:
			//isRealSafe中已经包含了
			return true;	
		case Json::nullValue:
		case Json::arrayValue:
		case Json::objectValue:
			//objectValue这种是不能转过去的也不存在asObject接口
		default:			
			return false;			
	
	}
	//UNREACHABLE
	return false;
}


/*
必然可转的情况
*/
bool SafeJson::isRealSafe(Json::Value& json,Json::ValueType jsontype)
{
	return (json.isNull()) || (Json::booleanValue == jsontype);
}



