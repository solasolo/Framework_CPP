#include "XMLConfig.hpp"

#include "StringHelper.hpp"
#include "OS.hpp"

using namespace std;

using namespace SoulFab::Base;
using namespace SoulFab::Data;
using namespace SoulFab::System;

namespace SoulFab::Config
{
	XmlConfig::XmlConfig(String file)
		: Owned(true)
	{
		this->Root = XMLNode::Load(file);
	}

	XmlConfig::XmlConfig(const XMLNode* node)
		: Owned(false), Root((XMLNode*)node)
	{
	}

	XmlConfig::~XmlConfig()
	{
		if (this->Owned)
		{
			SafeDestroy(this->Root);
		}
	}

	String XmlConfig::get(string key) const
	{
		String ret;

		const XMLNode* node = this->Root;
		auto&& keys = StringHelper::Split(key, '.');
		
		int level = keys.size();
		if (level > 1)
		{
			node = this->getNode(keys, level - 1);
		}

		string k = keys[level - 1];
		if (node->ChildrenCount() == 0)
		{
			ret = node->getValue(k);
		}
		else
		{
			node = this->SubNode(node, k);
			ret = node->getValue();
		}

		return ret;
	}

	shared_ptr<IConfig> XmlConfig::getConfig(string key) const
	{
		auto&& keys = StringHelper::Split(key, '.');
		auto node = this->getNode(keys, keys.size());

		return shared_ptr<IConfig>(new XmlConfig(node));
	}

	const XMLNode* XmlConfig::SubNode(const XMLNode* node, const string key) const
	{
		XMLNode* ret = nullptr;

		auto& sub_nodes = node->getChildren();

		if (sub_nodes.size() > 0)
		{
			for (auto n : sub_nodes)
			{
				if (n->getName() == key)
				{
					ret = n;
					break;
				}
			}
		}

		return ret;
	}

	const XMLNode* XmlConfig::getNode(const vector<string>& keys, int last) const
	{
		const XMLNode* node = this->Root;

		for (int i = 0; i < last; i++)
		{
			node = this->SubNode(node, keys[i]);

			if(!node)
			{
				throw Exception("Error Keys for XML Config");
			}
		}

		return node;
	}
}
