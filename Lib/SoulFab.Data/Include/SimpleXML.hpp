#pragma once

#ifndef _SF_SIMPLE_XML_H_
#define _SF_SIMPLE_XML_H_

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <stack>

#include "CommonDef.hpp"
#include "Exception.hpp"
#include "String.hpp"

namespace SoulFab::Data
{
	class SHARED_EXPORT XMLNode
	{
		friend class XMLDoc;
	
	public:
		static XMLNode* Load(SoulFab::Base::String file);

	public:
		XMLNode();
		XMLNode(std::string name);
		XMLNode(std::string name, SoulFab::Base::String value);
		~XMLNode();

		XMLNode* AddNode(std::string name, SoulFab::Base::String value);
		void setValue(std::string name, SoulFab::Base::String value);
		void AddNode(XMLNode* node);
		void setText(SoulFab::Base::String text);

		const std::string& getName() const;
		const SoulFab::Base::String& getValue() const;

		double getFloatValue() const;
		long getIntValue() const;

		bool hasValue(std::string name) const;

		const SoulFab::Base::String& getValue(std::string name) const;
		const SoulFab::Base::String getValue(std::string name, SoulFab::Base::String def) const;

		double getFloatValue(std::string name) const;
		long getIntValue(std::string name) const;

		XMLNode& getParent() const;
		const std::vector<XMLNode*>& getChildren() const;
		
		int ChildrenCount() const;
		XMLNode* operator[](std::string& name);
		XMLNode* operator[](int index);

	private:
		std::string Name;
		SoulFab::Base::String Value;
		XMLNode* Parent;
		std::map<std::string, SoulFab::Base::String> Attributes;
		std::vector<XMLNode*> Children;

	protected:
		void Write(SoulFab::Base::StringBuffer& s, int level);
	};

	class BaseXMLParser
	{
	public:
		static bool isBlank(char c);
		static bool isText(char c);
	};

	class XMLAttributeParser
	{
	public:
		void Reset(XMLNode* node);
		bool Stream(char c);

	private:
		enum class AttributeParserState
		{
			TO_NAME,
			IN_NAME,
			TO_VALUE,
			IN_VALUE,
		};

		AttributeParserState State;
		std::string Name;
		SoulFab::Base::String Value;
		XMLNode* Node;
	};

	class XMLTagParser
	{
	public:
		XMLTagParser();

		void Reset();
		bool Stream(char c);

		XMLNode* getNode();
		bool isClosed();

	private:
		enum class TagParserState
		{
			IN_NAME,
			OUT_NAME,
			ATTRIBUTE,
			TO_END,
		} State;

		std::string Name;
		XMLAttributeParser AttributeParser;
		bool Closed;
		XMLNode* Node;
	};

	class XMLCloseParser
	{
	public:
		void Reset();
		bool Stream(char c);

		std::string getTagName();

	private:
		enum class TagParserState
		{
			TO_NAME,
			IN_NAME,
			OUT_NAME,
		} State;

		std::string Name;
	};

	class XMLTextParser 
	{
	public:
		void Reset();
		bool Stream(char c);
		
		SoulFab::Base::String getText();

	private:
		enum class TextParserState
		{
			IN_TEXT,
		} State;

		SoulFab::Base::String Text;
	};

	class XMLParser : public BaseXMLParser
	{
	public:
		XMLParser();

		XMLNode* Parse(std::istream& st);

		virtual void Reset();
		virtual bool Stream(char c);

	private:
		enum class XMLParserState
		{
			NONE,

			TAG_START,
			TAG_END,

			OPEN_TAG,
			CLOSE_TAG,

			IN_BODY,
			IN_TEXT,
		};

		XMLParserState State;
		XMLNode* CurrentNode;
		std::stack<XMLNode*> ParserStack;

		XMLTagParser TagParser;
		XMLCloseParser CloseTagParser;
		XMLTextParser TextParser;

		bool Read(std::istream& st, char& c);
		bool TagEnd();
	};

	class XMLParserException : public SoulFab::Base::Exception
	{
	public:
		XMLParserException(SoulFab::Base::String msg);
	};
}

#endif