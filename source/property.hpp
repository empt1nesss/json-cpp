#ifndef SOURCE_PROPERTY_HPP
#define SOURCE_PROPERTY_HPP


#include "json.hpp"
#include "value.hpp"


class Json::Property
{
public:

  std::wstring Name;

  
  Property(const std::wstring &name, const Value &val);
  Property(const std::string  &name, const Value &val);


  Value&        GetData()       { return m_value; }
  const Value&  GetData() const { return m_value; }

private:

  Value m_value;

 
  friend std::wstring Json::serialize_property(
    const Property     &prop
  );
  friend void         Json::deserialize_property(
    const std::wstring &json_str, Property *prop
  );
};



#endif // !SOURCE_PROPERTY_HPP

