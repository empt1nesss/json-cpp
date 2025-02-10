#include "json.hpp"

#include <wchar.h>
#include <stdexcept>
#include <cstdint>



std::wstring format_in(std::wstring json_str)
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

std::wstring format_out(std::wstring str)
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



Json::Json() :
  m_properties(std::list<Property>())
{}


void Json::FromProperties(const std::list<Property> &properties)
{
  m_properties = properties;
}

void Json::FromJsonString(const std::wstring &json_string)
{
  deserialize_value(json_string, &m_properties);
}


std::wstring Json::Serialize() const
{
  return serialize_value(m_properties);
}



Json::Value::Value(const Value &val)
{
  type = val.type;
  switch (val.type)
  {
  case Bool:
    value = new bool;
    *(bool*)value = *(bool*)val.value;
    break;
  case Int:
    value = new int64_t;
    *(int64_t*)value = *(int64_t*)val.value;
    break;
  case Float:
    value = new double;
    *(double*)value = *(double*)val.value;
    break;
  case String:
    value = new std::wstring;
    *(std::wstring*)value = *(std::wstring*)val.value;
    break;
  case List:
    value = new std::list<Value>;
    *(std::list<Value>*)value = *(std::list<Value>*)val.value;
    break;
  case Struct:
    value = new std::list<Property>;
    *(std::list<Property>*)value = *(std::list<Property>*)val.value;
    break;
  default:
    break;
  }
}


Json::Value::Value()
{
  type  = Null;
  value = nullptr;
}

Json::Value::Value(bool val)
{
  type          = Bool;
  value         = new bool;
  *(bool*)value = val;
}

Json::Value::Value(int64_t val)
{
  type              = Int;
  value             = new int64_t;
  *(int64_t*)value  = val;
}

Json::Value::Value(double val)
{
  type            = Float;
  value           = new double;
  *(double*)value = val;
}

Json::Value::Value(const wchar_t *val)
{
  type  = String;
  value = new std::wstring;
  if (val != nullptr)
    *(std::wstring*)value = val;
}

Json::Value::Value(const std::wstring &val)
{
  type                  = String;
  value                 = new std::wstring;
  *(std::wstring*)value = val;
}

Json::Value::Value(const std::list<Value> &val)
{
  type                        = List;
  value                       = new std::list<Value>;
  *(std::list<Value>*)value = val;
}

Json::Value::Value(const std::initializer_list<Value> &val)
{
  type                        = List;
  value                       = new std::list<Value>;
  *(std::list<Value>*)value = val;
}

Json::Value::Value(const std::list<Property> & val)
{
  type                            = Struct;
  value                           = new std::list<Property>;
  *(std::list<Property>*)value  = val;
}

Json::Value::Value(const std::initializer_list<Property> &val)
{
  type                            = Struct;
  value                           = new std::list<Property>;
  *(std::list<Property>*)value  = val;
}


Json::Value::~Value()
{
  if (value == nullptr)
    return;

  switch (type)
  {
  case Bool:
    delete (bool*)value;
    break;
  case Int:
    delete (int64_t*)value;
    break;
  case Float:
    delete (double*)value;
    break;
  case String:
    ((std::wstring*)value)->clear();
    delete (std::wstring*)value;
    break;
  case List:
    ((std::list<Value>*)value)->clear();
    delete (std::list<Value>*)value;
    break;
  case Struct:
    ((std::list<Property>*)value)->clear();
    delete (std::list<Property>*)value;
    break;
  default:
    break;
  }
}


bool Json::Value::Contains(const std::wstring &prop_name) const
{
  if (type != Struct)
    return false;

  for (auto &prop : *(std::list<Property>*)value)
    if (prop.Name == prop_name)
      return true;

  return false;
}


Json::Value& Json::Value::operator=(const Json::Value& val)
{
  type = val.type;
  switch (val.type)
  {
  case Bool:
    value = new bool;
    *(bool*)((Property*)value) = *(bool*)val.value;
    break;
  case Int:
    value = new int64_t;
    *(int64_t*)((Property*)value) = *(int64_t*)val.value;
    break;
  case Float:
    value = new double;
    *(double*)((Property*)value) = *(double*)val.value;
    break;
  case String:
    value = new std::wstring;
    *(std::wstring*)((Property*)value) = *(std::wstring*)val.value;
    break;
  case List:
    value = new std::list<Value>;
    *(std::list<Value>*)((Value*)value) = *(std::list<Value>*)val.value;
    break;
  case Struct:
    value = new std::list<Property>;
    *(std::list<Property>*)((Property*)value) = *(std::list<Property>*)val.value;
    break;
  default:
    break;
  }

  return *this;
}

Json::Value& Json::Value::operator[](const std::wstring &prop_name)
{
  if (type != Struct)
    throw NotProperties;

  for (auto &prop : *(std::list<Property>*)value)
    if (prop.Name == prop_name)
      return prop.GetData();

  throw NotFound;
}

const Json::Value& Json::Value::operator[](const std::wstring &prop_name) const
{
  if (type != Struct)
    throw NotProperties;

  for (auto const &prop : *(std::list<Property>*)value)
    if (prop.Name == prop_name)
      return prop.GetData();

  throw NotFound;
}


void Json::Value::clear()
{
  if (value == nullptr) {
    type = Null;
    return;
  }

  switch (type)
  {
  case Bool:
    delete (bool*)value;
    break;
  case Int:
    delete (int64_t*)value;
    break;
  case Float:
    delete (double*)value;
    break;
  case String:
    ((std::wstring*)value)->clear();
    delete (std::wstring*)value;
    break;
  case List:
    ((std::list<Value>*)value)->clear();
    delete (std::list<Value>*)value;
    break;
  case Struct:
    ((std::list<Property>*)value)->clear();
    delete (std::list<Property>*)value;
    break;
  default:
    break;
  }
  type = Null;
}



Json::Property::Property(const std::wstring &name, const Value &val) :
  Name(name), m_value(val)
{}



std::wstring serialize_property(const Json::Property &prop)
{
  return L"\"" + format_out(prop.Name) + L"\":" + serialize_value(prop.m_value);
}

std::wstring serialize_value(const Json::Value &val)
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

    if (((std::list<Json::Value>*)val.value)->size() != 0) {
      out += serialize_value(
        *(*(std::list<Json::Value>*)val.value).begin()
      );
      for (
        auto it = ++((std::list<Json::Value>*)val.value)->begin();
        it != ((std::list<Json::Value>*)val.value)->end();
        ++it
      ) {
        out += L"," + serialize_value(*it);
      }
    }

    return out += L"]";
  }
  case Json::Struct: {
    std::wstring out = L"{";

    if (((std::list<Json::Property>*)val.value)->size() != 0) {
      out += serialize_property(
        *(*(std::list<Json::Property>*)val.value).begin()
      );
      for (
        auto it = ++((std::list<Json::Property>*)val.value)->begin();
        it != ((std::list<Json::Property>*)val.value)->end();
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

void deserialize_property(
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

void deserialize_value(
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
    val->value  = new std::list<Json::Value>;
    
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
        ((std::list<Json::Value>*)(val->value))->push_back(_val);
        val_st = json_str.end();
      }
    }

    auto val_end = json_str.end() - 1;
    for (; val_end != json_str.begin(); --val_end)
      if (*val_end == L']')
        break;

    Json::Value _val;
    deserialize_value(std::wstring(val_st, val_end), &_val);
    ((std::list<Json::Value>*)(val->value))->push_back(_val);
  }
  else if (json_str[0] == L'{') {
    val->type   = Json::Struct;
    val->value  = new std::list<Json::Property>;

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
  
        ((std::list<Json::Property>*)(val->value))->push_back(_prop);
        prop_st = json_str.end();
      }
    }

    auto prop_end = json_str.end() - 1;
    for (; prop_end != json_str.begin(); --prop_end)
      if (*prop_end == L'}')
        break;

    Json::Property _prop(L"", Json::Value());
    deserialize_property(std::wstring(prop_st, prop_end), &_prop);
    ((std::list<Json::Property>*)(val->value))->push_back(_prop);
  }
}
