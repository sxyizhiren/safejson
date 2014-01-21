//
//  "$Id$"
//
//  Copyright (c)1992-2012, ZheJiang Dahua Technology Stock CO.LTD.
//  All Rights Reserved.
//
//	Description:	Json 扩展，支持 WIN32 DLL
//	Revisions:		Year-Month-Day  SVN-Author  Modification
//

#ifndef DAHUA_JSON_EXTERNSION_H__
#define DAHUA_JSON_EXTERNSION_H__


#include <stddef.h>
#include "forwards.h"


namespace Json {


////////////////////////////////////////////////////////////////////////////////

/// FastWriter封装类，支持 WIN32 DLL
class JSON_API FastWriterWrapper
{
	FastWriterWrapper(FastWriterWrapper const&);
	FastWriterWrapper& operator=(FastWriterWrapper const&);

public:
	/// 构造函数
	FastWriterWrapper(Json::Value const& root, size_t reserved = 1024);

	/// 析构函数
	~FastWriterWrapper();

	/// 转换后的字符串首地址
	const char* data() const;

	/// 转换后的字符串长度
	size_t size() const;

private:
	struct FastWriterWrapperInternal;
	FastWriterWrapperInternal* m_internal;
};

////////////////////////////////////////////////////////////////////////////////

/// StyledWriter封装类，支持 WIN32 DLL
class JSON_API StyledWriterWrapper
{
	StyledWriterWrapper(StyledWriterWrapper const&);
	StyledWriterWrapper& operator=(StyledWriterWrapper const&);

public:
	/// 构造函数
	StyledWriterWrapper(Json::Value const& root, size_t reserved = 1024);

	/// 析构函数
	~StyledWriterWrapper();

	/// 转换后的字符串首地址
	const char* data() const;

	/// 转换后的字符串长度
	size_t size() const;

private:
	struct StyledWriterWrapperInternal;
	StyledWriterWrapperInternal* m_internal;
};


////////////////////////////////////////////////////////////////////////////////

/// Reader封装类，支持 WIN32 DLL
class JSON_API ReaderWrapper
{
	ReaderWrapper(ReaderWrapper const&);
	ReaderWrapper& operator=(ReaderWrapper const&);

public:
	/// 构造函数
	ReaderWrapper();

	/// 析构函数
	~ReaderWrapper();

	/// \brief Read a Value from a <a HREF="http://www.json.org">JSON</a> document.
	/// \param document UTF-8 encoded string containing the document to read.
	/// \param root [out] Contains the root value of the document if it was
	///             successfully parsed.
	/// \param collectComments \c true to collect comment and allow writing them back during
	///                        serialization, \c false to discard comments.
	///                        This parameter is ignored if Features::allowComments_
	///                        is \c false.
	/// \return \c true if the document was successfully parsed, \c false if an error occurred.
	bool parse(const char *beginDoc, const char *endDoc,
			Value &root,
			bool collectComments = true);

	/// \brief Returns a user friendly string that list errors in the parsed document.
	/// \return Formatted error message with the list of errors with their location in
	///         the parsed document. An empty string is returned if no error occurred
	///         during parsing.
	///         返回的指针在重新调用 parse 之后失效
	const char* getFormattedErrorMessages() const;

private:
	struct ReaderWrapperInternal;
	ReaderWrapperInternal* m_internal;
};

////////////////////////////////////////////////////////////////////////////////

/// 取成员列表助手类
class JSON_API MemberNames
{
	MemberNames(MemberNames const&);
	MemberNames& operator=(MemberNames const&);

public:
	/// 构造函数
	MemberNames(Json::Value const&);

	/// 析构函数
	~MemberNames();

	/// 取成员个数
	size_t count() const;

	/// 取成员名称
	char const* operator[](size_t index) const;

private:
	struct MemberNamesInternal;
	MemberNamesInternal* m_internal;
};

} // namespace Json

#endif // DAHUA_JSON_EXTERNSION_H__
