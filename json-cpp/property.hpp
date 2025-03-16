#ifndef SOURCE_PROPERTY_HPP
#define SOURCE_PROPERTY_HPP


#include "json.hpp"
#include "value.hpp"


class Json::Property
{
public:
  
  Property(const std::wstring &name, const Value &val);
  Property(const std::string  &name, const Value &val);

  std::string  GetName () const { return Json::to_str(m_name); }
  std::wstring GetNameW() const { return m_name; }

  void SetName(const std::string  &name) { m_name = std::wstring(name.begin(), name.end()); }
  void SetName(const std::wstring &name) { m_name = name; }

  Value&        GetValue()       { return m_value; }
  const Value&  GetValue() const { return m_value; }

private:

  std::wstring m_name;
  Value        m_value;

  friend bool         Json::validate_property(
    const std::wstring &json_str, std::string *log, uint64_t ln, uint64_t col
  );
  friend std::wstring Json::serialize_property(
    const Property     &prop
  );
  friend void         Json::deserialize_property(
    const std::wstring &json_str, Property *prop
  );
};



#endif // !SOURCE_PROPERTY_HPP

