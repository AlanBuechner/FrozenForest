#pragma once
#include "document.h" 
#include <string>
#include <vector>

#define JSON_READ(value, data) json::Get(value, #data, data); 

namespace json
{
	bool Load(const std::string& filename, rapidjson::Document& document); 
	bool Get(const rapidjson::Value& value, const std::string& name, int& data);
	bool Get(const rapidjson::Value& value, const std::string& name, float& data);
	bool Get(const rapidjson::Value& value, const std::string& name, bool& data);
	bool Get(const rapidjson::Value& value, const std::string& name, std::string& data);
	bool Get(const rapidjson::Value& value, const std::string& name, uint32_t& data);
	bool Get(const rapidjson::Value& value, const std::string& name, uint16_t& data);
	bool Get(const rapidjson::Value& value, const std::string& name, uint8_t& data);

	bool Get(const rapidjson::Value& value, const std::string& name, std::vector<std::string>& data);
	bool Get(const rapidjson::Value& value, const std::string& name, std::vector<int>& data);
}