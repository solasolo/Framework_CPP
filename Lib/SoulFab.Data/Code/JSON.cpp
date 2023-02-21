#include "JSON.hpp"

using namespace std;

namespace SoulFab::Data
{
	template<>
	bool Value::GetAs()
	{
		return value_ == "true" ? true : false;
	}

	template<>
	string Value::GetAs()
	{
		return value_;
	}

	template<>
	void Value::Set(string v)
	{
		std::ostringstream oss;
		if (nokey_) {
			oss << "\"" << v << "\"";
		}
		else {
			oss << "\"" << value_ << "\"" << ":" << "\"" << v << "\"";
		}
		value_ = oss.str();
	}

	template<>
	void Value::Set(const char* v)
	{
		Set(string(v));
	}

	template<>
	void Value::Set(bool v)
	{
		ostringstream oss;
		string val = v == true ? "true" : "false";
		if (nokey_) {
			oss << val;
		}
		else {
			oss << "\"" << value_ << "\"" << ":" << val;
		}
		value_ = oss.str();
	}


	bool JsonParser::ParseArray(string json, vector<string>& vo) {
		json = Trims(json, '[', ']');
		string tokens;

		for (size_t i = 0; i < json.size(); ++i) {
			char c = json[i];

			if (isspace(c) || c == '\"') continue;

			if (c == ':' || c == ',' || c == '{') 
			{
				if (!tokens.empty()) 
				{
					vo.push_back(tokens);
					tokens.clear();
				}
				
				if (c == ',') continue;
				
				int offset = 0;
				char nextc = c;
				for (; c != '{';) 
				{
					nextc = json[++i];
					if (isspace(nextc)) continue;
					break;
				}

				if (nextc == '{') 
				{
					tokens = FetchObject(json, i, offset);
				}
				else if (nextc == '[') 
				{
					tokens = FetchArray(json, i, offset);
				}

				i += offset;
			}

			tokens.push_back(c);
		}

		if (!tokens.empty()) {
			vo.push_back(tokens);
		}

		return true;
	}


	bool JsonParser::ParseObject(string json) {
		auto LastValidChar = [&](int index)->char {
			for (int i = index - 1; i >= 0; --i) {
				if (isspace(json[i])) continue;
				char tmp = json[i];
				return tmp;
			}
			return '\0';
		};

		json = Trims(json, '{', '}');

		for (size_t i = 0; i < json.size(); ++i) {
			char nextc = json[i];
			if (isspace(nextc)) continue;

			string tokens;
			int offset = 0;
			if (nextc == '{') {
				tokens = this->FetchObject(json, i, offset);
			}
			else if (nextc == '[') {
				tokens = this->FetchArray(json, i, offset);
			}
			else if (nextc == '\"') {
				tokens = FetchString(json, i, offset);
			}
			else if (isdigit(nextc) && LastValidChar(i) == ':')
			{
				tokens = this->FetchNumber(json, i, offset);
			}
			else {
				continue;
			}
			keyval_.push_back(tokens);
			i += offset;
		}

		if (keyval_.size() == 0)
		{
			keyval_.push_back(json);
		}

		return true;
	}

	string JsonParser::Trims(string s, char lc, char rc)
	{
		string ss = s;
		
		if (s.find(lc) != string::npos && s.find(rc) != string::npos) 
		{
			size_t b = s.find_first_of(lc);
			size_t e = s.find_last_of(rc);
			ss = s.substr(b + 1, e - b - 1);
		}
		
		return ss;
	}

	int JsonParser::GetFirstNotSpaceChar(string& s, int cur)
	{
		for (size_t i = cur; i < s.size(); i++) 
		{
			if (isspace(s[i])) continue;
			return i - cur;
		}
		
		return 0;
	}

	string JsonParser::FetchArray(string inputstr, int inpos, int& offset)
	{
		int tokencount = 0;
		string objstr;
		size_t i = inpos + GetFirstNotSpaceChar(inputstr, inpos);
		for (; i < inputstr.size(); i++) {
			char c = inputstr[i];
			if (c == '[') {
				++tokencount;
			}
			if (c == ']') {
				--tokencount;
			}
			objstr.push_back(c);
			if (tokencount == 0) {
				break;
			}
		}
		offset = i - inpos;
		return objstr;
	}

	string JsonParser::FetchObject(string inputstr, int inpos, int& offset)
	{
		int tokencount = 0;
		string objstr;
		size_t i = inpos + GetFirstNotSpaceChar(inputstr, inpos);
		for (; i < inputstr.size(); i++) {
			char c = inputstr[i];
			if (c == '{') {
				++tokencount;
			}
			if (c == '}') {
				--tokencount;
			}
			objstr.push_back(c);
			if (tokencount == 0) {
				break;
			}
		}
		offset = i - inpos;
		return objstr;
	}

	string JsonParser::FetchString(string inputstr, int inpos, int& offset)
	{
		int tokencount = 0;
		string objstr;
		size_t i = inpos + GetFirstNotSpaceChar(inputstr, inpos);
		for (; i < inputstr.size(); i++) {
			char c = inputstr[i];
			if (c == '\"') {
				++tokencount;
			}
			objstr.push_back(c);
			if (tokencount % 2 == 0 && (c == ',' || c == ':')) {
				break;
			}
		}
		offset = i - inpos;

		return Trims(objstr, '\"', '\"');
	}

	string JsonParser::FetchNumber(string inputstr, int inpos, int& offset)
	{
		string objstr;

		size_t i = inpos + GetFirstNotSpaceChar(inputstr, inpos);
		for (; i < inputstr.size(); i++) {
			char c = inputstr[i];
			if (c == ',') {
				break;
			}
			objstr.push_back(c);
		}
		offset = i - inpos;

		return objstr;
	}




	template<>
	xarray TinyJson::Get(string key)
	{
		string val = Get<string>(key);
		JsonParser p;
		vector<string> vo;
		p.ParseArray(val, vo);
		xarray vals(vo);
		return vals;
	}

	ostream& operator << (ostream& os, TinyJson& ob)
	{
		os << ob.WriteJson();
		return os;
	}

	string TinyJson::WriteJson(int type)
	{
		string prefix = type == 1 ? "{" : "[";
		string suffix = type == 1 ? "}" : "]";
		if (type == 0) {
			prefix = "";
			suffix = "";
		}
		ostringstream oss;
		oss << prefix;
		int i = 0;
		int size = Items_.size();
		string seq = ",";
		for (; i < size; ++i) {
			Value& v = Items_[i];
			oss << v.value() << seq;
		}
		string jsonstring = oss.str();
		if (jsonstring.back() == ',') {
			jsonstring = jsonstring.substr(0, jsonstring.find_last_of(','));
		}

		jsonstring += suffix;
		return jsonstring;
	}

	template<>
	void Value::Set(TinyJson v) {
		ostringstream oss;

		if (v.sub_type_ == 1) {
			oss << "\"" << value_ << "\"" << ":" << v.WriteJson(2);
		}
		else {
			if (nokey_) {
				oss << v;
			}
			else {
				oss << "\"" << value_ << "\"" << ":" << v;
			}
		}

		value_ = oss.str();
	}
}