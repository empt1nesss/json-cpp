#include "value.hpp"
#include "property.hpp"


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
  type  = Bool;
  value = new bool(val);
}

Json::Value::Value(const std::string &val)
{
  type  = String;
  value = new std::wstring(val.begin(), val.end());
}

Json::Value::Value(const std::wstring &val)
{
  type  = String;
  value = new std::wstring(val);
}

Json::Value::Value(const std::list<Value> &val)
{
  type  = List;
  value = new std::list<Value>(val);
}

Json::Value::Value(const std::initializer_list<Value> &val)
{
  type  = List;
  value = new std::list<Value>(val);
}

Json::Value::Value(const std::list<Property> & val)
{
  type  = Struct;
  value = new std::list<Property>(val);
}

Json::Value::Value(const std::initializer_list<Property> &val)
{
  type  = Struct;
  value = new std::list<Property>(val);
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

