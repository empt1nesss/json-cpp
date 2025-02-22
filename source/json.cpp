#include "json.hpp"
#include "property.hpp"

#include <wchar.h>
#include <stdexcept>
#include <cstdint>
#include <fstream>
#include <filesystem>


Json::Json() :
  m_properties(new Value(StructType()))
{}

Json::~Json()
{
  delete m_properties;
}


void Json::FromProperties(const StructType &properties)
{
  *m_properties = properties;
}

void Json::FromJsonString(const std::string &json_string)
{
  deserialize_value(std::wstring(json_string.begin(), json_string.end()), m_properties);
}

void Json::FromJsonString(const std::wstring &json_string)
{
  deserialize_value(json_string, m_properties);
}

std::string Json::Serialize() const
{
  return to_str(serialize_value(*m_properties));
}

std::wstring Json::SerializeW() const
{
  return serialize_value(*m_properties);
}

bool Json::SerializeToFile(const std::wstring &path) const
{
  std::wofstream file(std::filesystem::path(path.c_str()));
  file.imbue(std::locale("en_US.UTF-8"));
  if (!file.is_open())
    return false;

  file << SerializeW();
  file.close();
  return true;
}

bool Json::SerializeToFile(const std::string &path) const
{
  return SerializeToFile(std::wstring(path.begin(), path.end()));
}



std::wstring Json::serialize_property(const Json::Property &prop)
{
  return L"\"" + format_out(prop.Name) + L"\":" + serialize_value(prop.m_value);
}

std::wstring Json::serialize_value(const Json::Value &val)
{
  switch (val.type)
  {
  case Json::Bool:
    return *(bool*)val.value ? L"true" : L"false";
  case Json::Int:
    return std::to_wstring(*(int64_t*)val.value);
  case Json::Float:
    return std::to_wstring(*(double*)val.value);
  case Json::String: {
    return L"\"" + format_out(*(std::wstring*)val.value) + L"\"";
  }
  case Json::List: {
    std::wstring out = L"[";

    if (((ListType*)val.value)->size() != 0) {
      out += serialize_value(
        *(*(ListType*)val.value).begin()
      );
      for (
        auto it = ++((ListType*)val.value)->begin();
        it != ((ListType*)val.value)->end();
        ++it
      ) {
        out += L"," + serialize_value(*it);
      }
    }

    return out += L"]";
  }
  case Json::Struct: {
    std::wstring out = L"{";

    if (((StructType*)val.value)->size() != 0) {
      out += serialize_property(
        *(*(StructType*)val.value).begin()
      );
      for (
        auto it = ++((StructType*)val.value)->begin();
        it != ((StructType*)val.value)->end();
        ++it
      ) {
        out += L"," + serialize_property(*it);
      }
    }

    return out += L"}";
  }
  default:
    return L"null";
  }
}

void Json::deserialize_property(
  const std::wstring &json_str, Json::Property *prop
)
{
  if (prop == nullptr || json_str.empty())
    throw std::runtime_error(
      "deserialize_property was called with null argument"
    );

  std::wstring::const_iterator name_first   = json_str.end();
  std::wstring::const_iterator name_last    = json_str.end();
  std::wstring::const_iterator value_first  = json_str.end();
  std::wstring::const_iterator value_last   = json_str.end();

  bool value = false;

  for (auto it = json_str.begin(); it != json_str.end(); ++it) {
    if (*it == L'\"') {
      if (name_first == json_str.end())
        name_first = it;
      else if (name_last == json_str.end())
        name_last = it;
    }

    if (*it == L':') {
      value = true;
      continue;
    }

    if (value && *it != L' ' && *it != L'\n') {
      value_first = it;
      break;
    }
  }

  for (--value_last; value_last != json_str.begin(); --value_last)
    if (*value_last != L' ' && *value_last != L'\n') {
      ++value_last;
      break;
    }


  prop->Name = format_in(std::wstring(name_first + 1, name_last));
  deserialize_value(
    std::wstring(
      value_first, value_last
    ),
    &prop->m_value
  );
}

void Json::deserialize_value(
  const std::wstring &json_str, Json::Value *val
)
{
  if (val == nullptr || json_str.empty())
    throw std::runtime_error(
      "deserialize_value was called with null argument"
    );

  val->clear();

  if (json_str == L"null") {
    *val = Json::Value();
  }
  else if (json_str == L"false") {
    *val = Json::Value(false);
  }
  else if (json_str == L"true") {
    *val = Json::Value(true);
  }
  else if (json_str[0] >= L'0' && json_str[0] <= L'9') {
    if (json_str.find(L'.') == json_str.npos) {
      *val = Json::Value((int64_t)std::stoll(json_str));
    }
    else {
      *val = Json::Value(std::stod(json_str));
    }
  }
  else if (json_str[0] == L'\"') {
    val->type = Json::String;
    val->value = new std::wstring(json_str.begin() + 1, json_str.end() - 1);
    *(std::wstring*)val->value = format_in(*(std::wstring*)val->value);
  }
  else if (json_str[0] == L'[') {
    val->type   = Json::List;
    val->value  = new ListType;
    
    bool empty = true;
    for (size_t i = 1; i < json_str.size(); ++i) {
      if (json_str[i] == ' ' || json_str[i] == '\n')
        continue;
      else if (json_str[i] == ']')
        break;
      else {
        empty = false;
        break;
      }
    }

    if (empty)
      return;

    auto val_st = json_str.end();

    bool     quote          = false;
    uint64_t square_bracket = 0;
    uint64_t curly_bracket  = 0;
    for (auto it = json_str.begin() + 1; it != json_str.end() - 1; ++it) {
      if (quote && *it == L'\\') {
        ++it;
        continue;
      }

      if (*it != L' ' && *it != L'\n' && val_st == json_str.end())
        val_st = it;

      if (*it == L'\"')
        quote = !quote;
      else if (*it == L'[')
        ++square_bracket;
      else if (*it == L']')
        --square_bracket;
      else if (*it == L'{')
        ++curly_bracket;
      else if (*it == L'}')
        --curly_bracket;
      else if (*it == L',' && !quote && square_bracket == 0 && curly_bracket == 0) {
        Json::Value _val;
        deserialize_value(std::wstring(val_st, it), &_val);
        ((ListType*)(val->value))->push_back(_val);
        val_st = json_str.end();
      }
    }

    auto val_end = json_str.end() - 1;
    for (; val_end != json_str.begin(); --val_end)
      if (*val_end == L']')
        break;

    Json::Value _val;
    deserialize_value(std::wstring(val_st, val_end), &_val);
    ((ListType*)(val->value))->push_back(_val);
  }
  else if (json_str[0] == L'{') {
    val->type   = Json::Struct;
    val->value  = new StructType;

    bool empty = true;
    for (size_t i = 1; i < json_str.size(); ++i) {
      if (json_str[i] == ' ' || json_str[i] == '\n')
        continue;
      else if (json_str[i] == '}')
        break;
      else {
        empty = false;
        break;
      }
    }

    if (empty)
      return;

    auto prop_st = json_str.end();

    bool     quote          = false;
    uint64_t square_bracket = 0;
    uint64_t curly_bracket  = 0;
    for (auto it = json_str.begin() + 1; it != json_str.end() - 1; ++it) {
      if (quote && *it == L'\\') {
        ++it;
        continue;
      }

      if (*it != L' ' && *it != L'\n' && prop_st == json_str.end())
        prop_st = it;

      if (*it == L'\"')
        quote = !quote;
      else if (*it == L'[')
        ++square_bracket;
      else if (*it == L']')
        --square_bracket;
      else if (*it == L'{')
        ++curly_bracket;
      else if (*it == L'}')
        --curly_bracket;
      else if (*it == L',' && !quote && square_bracket == 0 && curly_bracket == 0) {
        Json::Property _prop(L"", Json::Value());
        deserialize_property(std::wstring(prop_st, it), &_prop);
  
        ((StructType*)(val->value))->push_back(_prop);
        prop_st = json_str.end();
      }
    }

    auto prop_end = json_str.end() - 1;
    for (; prop_end != json_str.begin(); --prop_end)
      if (*prop_end == L'}')
        break;

    Json::Property _prop(L"", Json::Value());
    deserialize_property(std::wstring(prop_st, prop_end), &_prop);
    ((StructType*)(val->value))->push_back(_prop);
  }
}



std::wstring Json::format_in(std::wstring json_str)
{
  if (json_str.size() < 2)
    return json_str;

  for (size_t i = 0; i < json_str.size() - 1; ++i) {
    if (json_str[i] == L'\\') {
      switch (json_str[i])
    {
    case L'\"':
      json_str.erase(json_str.begin() + i);
      json_str[i] = L'\"';
      break;
    case L'\\':
      json_str.erase(json_str.begin() + i);
      json_str[i] = L'\\';
      break;
    case L'/':
      json_str.erase(json_str.begin() + i);
      json_str[i] = L'/';
      break;
    case L'\b':
      json_str.erase(json_str.begin() + i);
      json_str[i] = L'\b';
      break;
    case L'\f':
      json_str.erase(json_str.begin() + i);
      json_str[i] = L'\f';
      break;
    case L'\n':
      json_str.erase(json_str.begin() + i);
      json_str[i] = L'\n';
      break;
    case L'\r':
      json_str.erase(json_str.begin() + i);
      json_str[i] = L'\r';
      break;
    case L'\t':
      json_str.erase(json_str.begin() + i);
      json_str[i] = L'\t';
      break;
    default:
      break;
    }
    }
  }

  return json_str;
}

std::wstring Json::format_out(std::wstring str)
{
  for (size_t i = 0; i < str.size(); ++i) {
    switch (str[i])
    {
    case L'\"':
      str[i++] = L'\\';
      str.insert(str.begin() + i, L'\"');
      break;
    case L'\\':
      str[i++] = L'\\';
      str.insert(str.begin() + i, L'\\');
      break;
    case L'/':
      str[i++] = L'\\';
      str.insert(str.begin() + i, L'/');
      break;
    case L'\b':
      str[i++] = L'\\';
      str.insert(str.begin() + i, L'b');
      break;
    case L'\f':
      str[i++] = L'\\';
      str.insert(str.begin() + i, L'f');
      break;
    case L'\n':
      str[i++] = L'\\';
      str.insert(str.begin() + i, L'n');
      break;
    case L'\r':
      str[i++] = L'\\';
      str.insert(str.begin() + i, L'r');
      break;
    case L'\t':
      str[i++] = L'\\';
      str.insert(str.begin() + i, L't');
      break;
    default:
      break;
    }
  }

  return str;
}

std::string Json::to_str(const std::wstring &wstr)
{
  std::string out(wstr.size(), '\0');
  for (size_t i = 0; i < wstr.size(); ++i) {
    if (wstr[i] > 127)
      out[i] = '?';
    else
     out[i] = wstr[i];
  }

  return out;
}

