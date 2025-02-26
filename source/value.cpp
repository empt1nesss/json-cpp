#include "value.hpp"
#include "property.hpp"

#include <algorithm>


Json::Value::Value(const Value &val)
{
  type = val.type;
  switch (val.type)
  {
  case Bool:
    value = new bool(*(bool*)val.value);
    break;
  case Int:
    value = new int64_t(*(int64_t*)val.value);
    break;
  case Float:
    value = new double(*(double*)val.value);
    break;
  case String:
    value = new std::wstring(*(std::wstring*)val.value);
    break;
  case List:
    value = new ListType(*(ListType*)val.value);
    break;
  case Struct:
    value = new StructType(*(StructType*)val.value);
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

Json::Value::Value(const char *val)
{
  std::string t(val);

  type  = String;
  value = new std::wstring(t.begin(), t.end());

}

Json::Value::Value(const std::string &val)
{
  type  = String;
  value = new std::wstring(val.begin(), val.end());
}

Json::Value::Value(const wchar_t *val)
{
  type  = String;
  value = new std::wstring(val);
}

Json::Value::Value(const std::wstring &val)
{
  type  = String;
  value = new std::wstring(val);
}

Json::Value::Value(const ListType &val)
{
  type  = List;
  value = new ListType(val);
}

Json::Value::Value(const std::initializer_list<Value> &val)
{
  type  = List;
  value = new ListType(val);
}

Json::Value::Value(const StructType &val)
{
  type  = Struct;
  value = new StructType(val);
}

Json::Value::Value(const std::initializer_list<Property> &val)
{
  type  = Struct;
  value = new StructType(val);
}


Json::Value::~Value()
{
  clear();
}


bool Json::Value::Contains(const std::wstring &prop_name) const
{
  if (type != Struct)
    return false;

  for (auto &prop : *(StructType*)value)
    if (prop.Name == prop_name)
      return true;

  return false;
}


void Json::Value::RemoveProperty(const std::string &name)
{
  RemoveProperty(std::wstring(name.begin(), name.end()));
}

void Json::Value::RemoveProperty(const std::wstring &name)
{
  if (type != Struct)
    throw NotStruct;

  auto f = std::find_if(
    ((StructType*)value)->begin(),
    ((StructType*)value)->end(),
    [&](const Property &p_prop)
    {
      return p_prop.Name == name;
    }
  );
  if (f == ((StructType*)value)->end())
    throw NotFound;
  
  ((StructType*)value)->erase(f);
}


Json::Value& Json::Value::operator=(const Json::Value& val)
{
  type = val.type;
  switch (val.type)
  {
  case Bool:
    value = new bool(*(bool*)val.value);
    break;
  case Int:
    value = new int64_t(*(int64_t*)val.value);
    break;
  case Float:
    value = new double(*(double*)val.value);
    break;
  case String:
    value = new std::wstring(*(std::wstring*)val.value);
    break;
  case List:
    value = new ListType(*(ListType*)val.value);
    break;
  case Struct:
    value = new StructType(*(StructType*)val.value);
    break;
  default:
    break;
  }

  return *this;
}

Json::Value& Json::Value::operator[](const std::string &prop_name)
{
  return (*this)[std::wstring(prop_name.begin(), prop_name.end())];
}

const Json::Value& Json::Value::operator[](const std::string &prop_name) const
{
  return (*this)[std::wstring(prop_name.begin(), prop_name.end())];
}

Json::Value& Json::Value::operator[](const std::wstring &prop_name)
{
  if (type != Struct)
    throw NotStruct;

  for (auto &prop : *(StructType*)value)
    if (prop.Name == prop_name)
      return prop.GetValue();

  ((StructType*)value)->push_back(Property(prop_name, Value()));
  return (((StructType*)value)->end() - 1)->GetValue();
}

const Json::Value& Json::Value::operator[](const std::wstring &prop_name) const
{
  if (type != Struct)
    throw NotStruct;

  for (auto const &prop : *(StructType*)value)
    if (prop.Name == prop_name)
      return prop.GetValue();

  ((StructType*)value)->push_back(Property(prop_name, Value()));
  return (((StructType*)value)->end() - 1)->GetValue();
}

Json::Value& Json::Value::operator[](size_t i)
{
  if (type != List)
    throw NotList;

  return *(((ListType*)value)->begin() + i);
}

const Json::Value& Json::Value::operator[](size_t i) const
{
  if (type != List)
    throw NotList;

  return *(((ListType*)value)->begin() + i);
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
    delete (ListType*)value;
    break;
  case Struct:
    delete (StructType*)value;
    break;
  default:
    break;
  }
  type  = Null;
  value = nullptr;
}

