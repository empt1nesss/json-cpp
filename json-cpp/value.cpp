#include "value.hpp"
#include "property.hpp"

#include <algorithm>


Json::Value::Value(const Value &val)
{
  m_type = val.m_type;
  switch (val.m_type)
  {
  case Bool:
    m_value = new bool(*(bool*)val.m_value);
    break;
  case Int:
    m_value = new int64_t(*(int64_t*)val.m_value);
    break;
  case Float:
    m_value = new double(*(double*)val.m_value);
    break;
  case String:
    m_value = new std::wstring(*(std::wstring*)val.m_value);
    break;
  case List:
    m_value = new ListType(*(ListType*)val.m_value);
    break;
  case Struct:
    m_value = new StructType(*(StructType*)val.m_value);
    break;
  default:
    break;
  }
}


Json::Value::Value()
{
  m_type  = Null;
  m_value = nullptr;
}

Json::Value::Value(bool val)
{
  m_type  = Bool;
  m_value = new bool(val);
}

Json::Value::Value(const char *val)
{
  std::string t(val);

  m_type  = String;
  m_value = new std::wstring(t.begin(), t.end());

}

Json::Value::Value(const std::string &val)
{
  m_type  = String;
  m_value = new std::wstring(val.begin(), val.end());
}

Json::Value::Value(const wchar_t *val)
{
  m_type  = String;
  m_value = new std::wstring(val);
}

Json::Value::Value(const std::wstring &val)
{
  m_type  = String;
  m_value = new std::wstring(val);
}

Json::Value::Value(const ListType &val)
{
  m_type  = List;
  m_value = new ListType(val);
}

Json::Value::Value(const std::initializer_list<Value> &val)
{
  m_type  = List;
  m_value = new ListType(val);
}

Json::Value::Value(const StructType &val)
{
  m_type  = Struct;
  m_value = new StructType(val);
}

Json::Value::Value(const std::initializer_list<Property> &val)
{
  m_type  = Struct;
  m_value = new StructType(val);
}


Json::Value::~Value()
{
  clear();
}


bool Json::Value::Contains(const std::wstring &prop_name) const
{
  if (m_type != Struct)
    return false;

  for (auto &prop : *(StructType*)m_value)
    if (prop.GetNameW() == prop_name)
      return true;

  return false;
}


bool Json::Value::GetBool() const
{
  if (m_type != Bool)
    throw WrongType;

  return *(bool*)m_value;
}

int64_t Json::Value::GetInt() const
{
  if (m_type != Int)
    throw WrongType;

  return *(int64_t*)m_value;
}

double Json::Value::GetFloat() const
{
  if (m_type != Float)
    throw WrongType;

  return *(double*)m_value;
}

std::string Json::Value::GetString() const
{
  if (m_type != String)
    throw WrongType;

  return Json::to_str(*(std::wstring*)m_value);
}

std::wstring Json::Value::GetStringW() const
{
  if (m_type != String)
    throw WrongType;

  return *(std::wstring*)m_value;
}

Json::ListType Json::Value::GetList() const
{
  if (m_type != List)
    throw WrongType;

  return *(ListType*)m_value;
}

Json::StructType Json::Value::GetStruct() const
{
  if (m_type != Struct)
    throw WrongType;

  return *(StructType*)m_value;
}


void Json::Value::RemoveProperty(const std::string &name)
{
  RemoveProperty(std::wstring(name.begin(), name.end()));
}

void Json::Value::RemoveProperty(const std::wstring &name)
{
  if (m_type != Struct)
    throw NotStruct;

  auto f = std::find_if(
    ((StructType*)m_value)->begin(),
    ((StructType*)m_value)->end(),
    [&](const Property &p_prop)
    {
      return p_prop.GetNameW() == name;
    }
  );
  if (f == ((StructType*)m_value)->end())
    throw NotFound;
  
  ((StructType*)m_value)->erase(f);
}


Json::Value& Json::Value::operator=(const Json::Value& val)
{
  if (m_value)
    clear();

  m_type = val.m_type;
  switch (val.m_type)
  {
  case Bool:
    m_value = new bool(*(bool*)val.m_value);
    break;
  case Int:
    m_value = new int64_t(*(int64_t*)val.m_value);
    break;
  case Float:
    m_value = new double(*(double*)val.m_value);
    break;
  case String:
    m_value = new std::wstring(*(std::wstring*)val.m_value);
    break;
  case List:
    m_value = new ListType(*(ListType*)val.m_value);
    break;
  case Struct:
    m_value = new StructType(*(StructType*)val.m_value);
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
  if (m_type != Struct)
    throw NotStruct;

  for (auto &prop : *(StructType*)m_value)
    if (prop.GetNameW() == prop_name)
      return prop.GetValue();

  ((StructType*)m_value)->push_back(Property(prop_name, Value()));
  return (((StructType*)m_value)->end() - 1)->GetValue();
}

const Json::Value& Json::Value::operator[](const std::wstring &prop_name) const
{
  if (m_type != Struct)
    throw NotStruct;

  for (auto const &prop : *(StructType*)m_value)
    if (prop.GetNameW() == prop_name)
      return prop.GetValue();
  
  throw NotFound;
}

Json::Value& Json::Value::operator[](size_t i)
{
  if (m_type != List)
    throw NotList;

  if (i >= ((ListType*)m_value)->size())
    throw NotFound;

  return *(((ListType*)m_value)->begin() + i);
}

const Json::Value& Json::Value::operator[](size_t i) const
{
  if (m_type != List)
    throw NotList;

  if (i >= ((ListType*)m_value)->size())
    throw NotFound;

  return *(((ListType*)m_value)->begin() + i);
}



void Json::Value::clear()
{
  if (m_value == nullptr) {
    m_type = Null;
    return;
  }

  switch (m_type)
  {
  case Bool:
    delete (bool*)m_value;
    break;
  case Int:
    delete (int64_t*)m_value;
    break;
  case Float:
    delete (double*)m_value;
    break;
  case String:
    ((std::wstring*)m_value)->clear();
    delete (std::wstring*)m_value;
    break;
  case List:
    delete (ListType*)m_value;
    break;
  case Struct:
    delete (StructType*)m_value;
    break;
  default:
    break;
  }
  m_type  = Null;
  m_value = nullptr;
}

