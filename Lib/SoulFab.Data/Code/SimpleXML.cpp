#include "SimpleXML.hpp"

#include "CommonDef.hpp"
#include "OS.hpp"

using namespace std;
using namespace SoulFab::System;
using namespace SoulFab::Base;

namespace SoulFab::Data
{
	bool BaseXMLParser::isBlank(char c)
	{
		return (c == ' ') || (c == '\t') || (c == '\r') || (c == '\n');
	}

	bool BaseXMLParser::isText(char c)
	{
		return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9');
	}

	XMLParser::XMLParser()
	{
	}

	XMLNode* XMLParser::Parse(istream& st)
	{
		this->Reset();

		char c;
		while (Read(st, c))
		{
			this->Stream(c);
		}

		if (!this->ParserStack.empty())
		{
			throw XMLParserException("");
		}

		return this->CurrentNode;
	}

	void XMLParser::Reset()
	{
		this->CurrentNode = nullptr;
		this->State = XMLParserState::NONE;

		while (!this->ParserStack.empty())
		{
			this->ParserStack.pop();
		}
	}

	bool XMLParser::Stream(char c)
	{
		bool ret = false;

		switch (this->State)
		{
		case XMLParserState::NONE:
			if (c == '<')
			{
				this->State = XMLParserState::TAG_START;
			}
			break;

		case XMLParserState::TAG_START:
			if (!BaseXMLParser::isBlank(c))
			{
				switch (c)
				{
				case '/':
					this->CloseTagParser.Reset();
					this->State = XMLParserState::CLOSE_TAG;
					break;

				case '!':
					break;

				case '?':
					break;

				default:
					if (CurrentNode)
					{
						this->ParserStack.push(CurrentNode);
					}

					this->TagParser.Reset();
					this->TagParser.Stream(c);
					this->State = XMLParserState::OPEN_TAG;
				}
			}
			break;

		case XMLParserState::OPEN_TAG:
			if (this->TagParser.Stream(c))
			{
				CurrentNode = this->TagParser.getNode();
				if (!this->ParserStack.empty())
				{
					this->ParserStack.top()->AddNode(CurrentNode);
				}

				if (this->TagParser.isClosed())
				{
					ret = this->TagEnd();
				}
				else
				{
					this->State = XMLParserState::IN_BODY;
				}
			}
			break;

		case XMLParserState::CLOSE_TAG:
			if (this->CloseTagParser.Stream(c))
			{
				if (CurrentNode && CurrentNode->getName() == this->CloseTagParser.getTagName())
				{
					ret = this->TagEnd();
				}
				else
				{
					throw XMLParserException("Close Tag Missmatch");
				}
			}

			break;

		case XMLParserState::IN_BODY:
			if (!BaseXMLParser::isBlank(c)) {
				if (c == '<')
				{
					this->State = XMLParserState::TAG_START;
				}
				else
				{
					this->TextParser.Reset();
					this->State = XMLParserState::IN_TEXT;
				}
			}
			break;

		case XMLParserState::IN_TEXT:
			if (this->TextParser.Stream(c))
			{
				if (CurrentNode)
				{
					CurrentNode->setText(this->TextParser.getText());
				}
				else
				{
				}

				if (c == '<')
				{
					this->CloseTagParser.Reset();
					this->State = XMLParserState::CLOSE_TAG;
				}
			}
			break;
		}

		return ret;
	}

	bool XMLParser::Read(istream& st, char& c)
	{
		bool ret = true;

		if (!st.eof())
		{
			st.get(c);
		}
		else
		{
			ret = false;
		}

		return ret;
	}

	bool XMLParser::TagEnd()
	{
		bool ret = this->ParserStack.empty();

		if (!ret)
		{
			this->CurrentNode = this->ParserStack.top();
			this->ParserStack.pop();
			this->State = XMLParserState::NONE;
		}

		return ret;
	}


	//------------------------------------------------------------------------------
	XMLTagParser::XMLTagParser()
		: Node(nullptr)
	{
	}
	
	void XMLTagParser::Reset()
	{
		this->Node = nullptr;
		this->Closed = false;
		this->State = TagParserState::IN_NAME;
		this->Name.clear();
	}

	bool XMLTagParser::Stream(char c)
	{
		bool ret = false;

		switch (this->State)
		{
		case TagParserState::IN_NAME:
			if (BaseXMLParser::isText(c)) 
			{
				this->Name += c;
			}
			else if(BaseXMLParser::isBlank(c) || c == '>')
			{
				this->Node = new XMLNode(this->Name);

				if (c == '>')
				{
					ret = true;
				}
				else
				{
					this->State = TagParserState::OUT_NAME;
				}
			}
			else
			{
				throw XMLParserException("Invalid Character in Tag Name");
			}
			break;

		case TagParserState::OUT_NAME:
			if (c == '>')
			{
				ret = true;
			}
			else if(BaseXMLParser::isText(c))
			{
				this->AttributeParser.Reset(this->Node);
				this->AttributeParser.Stream(c);
				this->State = TagParserState::ATTRIBUTE;
			}
			break;

		case TagParserState::ATTRIBUTE:
			if (this->AttributeParser.Stream(c))
			{
				if (c == '>')
				{
					ret = true;
				}
				else if(c == '/')
				{
					this->State = TagParserState::TO_END;
				}
			}
			break;

		case TagParserState::TO_END:
			if (c == '>')
			{
				this->Closed = true;
				ret = true;
			}

			break;
		}

		return ret;
	}

	XMLNode* XMLTagParser::getNode()
	{
		return this->Node;
	}

	bool XMLTagParser::isClosed()
	{
		return this->Closed;
	}

	//------------------------------------------------------------------------------
	void XMLAttributeParser::Reset(XMLNode* node)
	{
		this->Node = node;
		this->State = AttributeParserState::IN_NAME;
	}

	bool XMLAttributeParser::Stream(char c)
	{
		bool ret = false;

		switch (this->State)
		{
		case AttributeParserState::IN_NAME:
			if (BaseXMLParser::isText(c))
			{
				this->Name += c;
			}
			else if (c == '=')
			{
				this->State = AttributeParserState::TO_VALUE;
			}
			else if (!BaseXMLParser::isBlank(c))
			{
				throw XMLParserException("Invalid Character in Attribute Name");
			}
			break;

		case AttributeParserState::TO_VALUE:
			if (c == '"')
			{
				this->State = AttributeParserState::IN_VALUE;
			} 
			else if (!BaseXMLParser::isBlank(c))
			{
				throw XMLParserException("Invalid Character in Attribute Name");
			}
			break;

		case AttributeParserState::IN_VALUE:
			if (c == '"')
			{
				this->Node->setValue(this->Name, this->Value);

				this->Name.clear();
				this->Value.Clear();
				this->State = AttributeParserState::TO_NAME;

			}
			else
			{
				this->Value += c;
			}
			
			break;

		case AttributeParserState::TO_NAME:
			if (c == '/' || c == '>')
			{
				ret = true;
			}
			else if (BaseXMLParser::isText(c))
			{
				this->Name += c;
				this->State = AttributeParserState::IN_NAME;
			}
			break;
		}


		return ret;
	}

	//------------------------------------------------------------------------------
	void XMLCloseParser::Reset()
	{
		this->State = TagParserState::TO_NAME;
		this->Name.clear();
	}

	bool XMLCloseParser::Stream(char c)
	{
		bool ret = false;

		switch (this->State)
		{
		case  TagParserState::TO_NAME:
			if (c == '/')
			{
				this->State = TagParserState::IN_NAME;
			}
			else if (BaseXMLParser::isText(c))
			{
				this->Name += c;
				this->State = TagParserState::IN_NAME;
			}
			else if (!BaseXMLParser::isBlank(c))
			{
				throw XMLParserException("Invalid Character in Close Tag Name");
			}
			break;

		case TagParserState::IN_NAME:
			if (BaseXMLParser::isText(c))
			{
				this->Name += c;
			}
			else if (c == '>')
			{
				ret = true;
			}
			else if (BaseXMLParser::isBlank(c))
			{
				this->State = TagParserState::OUT_NAME;
			}
			else
			{
				throw XMLParserException("Invalid Character in Close Tag Name");
			}
			break;

		case TagParserState::OUT_NAME:
			if (c == '>')
			{
				ret = true;
			}
			else if (!BaseXMLParser::isBlank(c))
			{
				throw XMLParserException("Invalid Character in Close Tag Name");
			}
			break;
		}

		return ret;
	}

	string XMLCloseParser::getTagName()
	{
		return this->Name;
	}

	//------------------------------------------------------------------------------
	void XMLTextParser::Reset()
	{
		this->State = TextParserState::IN_TEXT;
		this->Text.Clear();
	}

	bool XMLTextParser::Stream(char c)
	{
		bool ret = false;

		switch (this->State)
		{
		case TextParserState::IN_TEXT:
			if (BaseXMLParser::isText(c))
			{
				this->Text += c;
			}
			else if (c == '<')
			{
				ret = true;
			}
			break;
		}

		return ret;
	}

	String XMLTextParser::getText()
	{
		return this->Text;
	}

	//------------------------------------------------------------------------------
	XMLNode* XMLNode::Load(String file)
	{
		XMLNode* ret = nullptr;

		string os_file = OS::OSPath(file);

		ifstream fs;
		fs.open(os_file, ios::in);
		if (fs)
		{
			XMLParser parser;
			ret = parser.Parse(fs);
		}
		else
		{
			throw Exception("File [" + file + "] Open Error");
		}

		return ret;
	}
	
	XMLNode::XMLNode()
		: Attributes(), Children()
	{
	}

	XMLNode::XMLNode(string name)
		: Name(name), Value(""), Attributes(), Children()
	{
	}

	XMLNode::XMLNode(string name, String value)
		: Name(name), Value(value), Attributes(), Children()
	{
	}

	XMLNode::~XMLNode()
	{
		for (auto n : this->Children)
		{
			if (n) delete n;
		}
	}

	const vector<XMLNode*>& XMLNode::getChildren() const
	{
		return this->Children;
	}

	void XMLNode::AddNode(XMLNode* node)
	{
		this->Children.push_back(node);
	}

	XMLNode* XMLNode::AddNode(string name, String value)
	{
		XMLNode* node = new XMLNode(name, value);
		node->Parent = this;

		this->Children.push_back(node);

		return node;
	}

	void XMLNode::setValue(string name, String value)
	{
		this->Attributes[name] = value;
	}

	void XMLNode::setText(String text)
	{
		this->Value = text;
	}

	const string& XMLNode::getName() const 
	{
		return Name;
	}

	const String& XMLNode::getValue() const
	{
		return Value;
	}

	double XMLNode::getFloatValue() const 
	{
		const String& value = this->getValue();

		return value.ToFloat();
	}

	long XMLNode::getIntValue() const
	{
		const String& value = this->getValue();

		return value.ToInt();
	}

	XMLNode& XMLNode::getParent() const
	{
		return *Parent;
	}

	bool XMLNode::hasValue(string name) const
	{
		return this->Attributes.find(name) != this->Attributes.end();
	}

	const String& XMLNode::getValue(string name) const
	{
		auto it = this->Attributes.find(name);
		if (it == this->Attributes.end())
		{
			throw Exception("No Attribute [" + String(name) + "]");
		}

		return it->second;;
	}

	const String XMLNode::getValue(string name, String def) const
	{
		String ret;

		auto it = this->Attributes.find(name);
		if (it != this->Attributes.end())
		{
			ret = it->second;
		}
		else
		{
			ret = def;
		}

		return ret;
	}

	double XMLNode::getFloatValue(string name) const
	{
		const String& value = this->getValue(name);

		return value.ToFloat();
	}

	long XMLNode::getIntValue(string name) const
	{
		const String& value = this->getValue(name);

		return value.ToInt();
	}

	int XMLNode::ChildrenCount() const
	{
		return this->Children.size();
	}

	XMLNode* XMLNode::operator[](string& name)
	{
		XMLNode* ret = nullptr;

		int len = this->Children.size();
		for (int i = 0; i < len; i++)
		{
			XMLNode* node = this->Children[i];
			if (node->getName() == name) { ret = node; };
			break;
		}

		return ret;
	}

	XMLNode* XMLNode::operator[](int index)
	{
		XMLNode* ret = nullptr;

		if (index < this->Children.size())
		{
			ret = (this->Children)[index];
		}

		return ret;
	}

	void XMLNode::Write(StringBuffer& s, int level)
	{
		for (int i = 0; i < level; i++)
		{
			s << '\t';
		}

		s << "<" << this->Name << ">";

		if (this->Children.size() == 0)
		{
			s << this->Value;
		}
		else
		{
		}

		s << "</" << Name << ">" << "\r\n";
	}

	//------------------------------------------------------------------------------
	XMLParserException::XMLParserException(String msg)
		: Exception(msg)
	{
	}
}