#pragma once

#ifndef _SF_JSON_H_
#define _SF_JSON_H_

#include <string>
#include <sstream>
#include <map>
#include <vector>
#include <algorithm>
#include <iostream>

namespace SoulFab::Data
{
	class Value
	{
	public:
		Value() {
			value_.clear();
			nokey_ = false;
		}
		Value(std::string val) : value_(val) {
			if (value_ == "") {
				value_.clear();
				nokey_ = true;
			}
			else {
				nokey_ = false;
			}
		}
		~Value() {}

	public:
		std::string value() { return value_; }
		template<typename R>
		R GetAs() {
			istringstream iss(value_);
			R v;
			iss >> v;
			return v;
		}


		template<typename V>
		void Set(V v) {
			ostringstream oss;
			if (nokey_) {
				oss << v;
			}
			else {
				oss << "\"" << value_ << "\"" << ":" << v;
			}
			value_ = oss.str();
		}

		template<typename T>
		void Push(T& v) {
			ostringstream oss;
			if (v.get_nokey()) {
				oss << v.WriteJson(0);
			}
			else {
				oss << v.WriteJson(1);
			}
			value_ = oss.str();
		}

	private:
		std::string value_;
		bool nokey_;
	};

	template<typename T>
	class ValueArray : public T
	{
	public:
		ValueArray() {}
		ValueArray(std::vector<std::string> vo) { vo_ = vo; }

		bool Enter(int i) {
			std::string obj = vo_[i];
			return this->ReadJson(obj);
		}

		int Count() { return vo_.size(); }

	private:
		std::vector<std::string> vo_;
	};


	class JsonParser
	{
	public:
		JsonParser() {}
		~JsonParser() {}

	public:
		bool ParseArray(std::string json, std::vector<std::string>& vo);
		bool ParseObject(std::string json);
		std::vector<std::string> GetKeyVal() {
			return keyval_;
		}

	protected:
		std::string Trims(std::string s, char lc, char rc);
		int GetFirstNotSpaceChar(std::string& s, int cur);
		
		std::string FetchArray(std::string inputstr, int inpos, int& offset);
		std::string FetchObject(std::string inputstr, int inpos, int& offset);
		std::string FetchString(std::string inputstr, int inpos, int& offset);
		std::string FetchNumber(std::string inputstr, int inpos, int& offset);

	private:
		std::vector<char> token_;
		std::vector<std::string> keyval_;
	};


	class TinyJson;
	typedef ValueArray<TinyJson> xarray;
	typedef ValueArray<TinyJson> xobject;

	class TinyJson
	{
		friend class ValueArray<TinyJson>;

	public:
		TinyJson() {
			nokey_ = false;
		}
		~TinyJson() {}

	public:
		// read
		bool ReadJson(std::string json) 
		{
			JsonParser p;
			p.ParseObject(json);

			KeyVal_ = p.GetKeyVal();
			return true;
		}

		template<typename R>
		R Get(std::string key, R defVal) {
			auto itr = find(KeyVal_.begin(), KeyVal_.end(), key);
			if (itr == KeyVal_.end()) {
				return defVal;
			}
			return Value(*(++itr)).GetAs<R>();
		}

		template<typename R>
		R Get(std::string key) {
			return Get(key, R());
		}

		template<typename R>
		R Get() {
			return Value(KeyVal_[0]).GetAs<R>();
		}

		// write
		Value& operator[](std::string k) {
			Items_.push_back(Value(k));
			Value& v = Items_[Items_.size() - 1];
			if (k == "") {
				nokey_ = true;
			}
			return v;
		}

		void Push(TinyJson item) {
			Items_.push_back(Value(""));
			Value& v = Items_[Items_.size() - 1];
			nokey_ = true;
			v.Push(item);
			sub_type_ = 1;
		}

		bool get_nokey() {
			return nokey_;
		}

		std::string WriteJson() {
			return WriteJson(1);
		}

		// 0: none  1: object  2: array
		std::string WriteJson(int type);

	public:
		int sub_type_;

	private:
		std::vector<std::string> KeyVal_;
		std::vector<Value> Items_;
		bool nokey_;
	};

	

} 

#endif  