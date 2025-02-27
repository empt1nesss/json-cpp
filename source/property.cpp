#include "property.hpp"


Json::Property::Property(const std::wstring &name, const Value &val) :
  m_name(name), m_value(val)
{}

Json::Property::Property(const std::string &name, const Value &val) :
  m_name(name.begin(), name.end()), m_value(val)
{}

