#ifndef JSON_HPP
#define JSON_HPP


#include <string>
#include <vector>
#include <initializer_list>


class Json
{
public:

  enum ValueType
  {
    Null,
    Bool,
    Int,
    Float,
    String,
    List,
    Struct
  };

  class Property;
  class Value;

  typedef std::vector<Property> StructType;
  typedef std::vector<Value>    ListType;

  Json();
  ~Json();


  void FromProperties(const StructType   &properties);
  void FromJsonString(const std::string  &json_string);
  void FromJsonString(const std::wstring &json_string);

  std::string   Serialize      ()                         const;
  std::wstring  SerializeW     ()                         const;
  bool          SerializeToFile(const std::wstring &path) const;
  bool          SerializeToFile(const std::string  &path) const;

  Value&        GetData()       { return *m_properties; }
  const Value&  GetData() const { return *m_properties; }

private:

  Value *m_properties;

  static std::wstring format_in (std::wstring json_str);
  static std::wstring format_out(std::wstring str);

  static std::wstring serialize_property(
    const Json::Property &prop
  );
  static std::wstring serialize_value(
    const Json::Value &val
  );
  static void deserialize_property(
    const std::wstring &json_str, Json::Property *prop
  );
  static void deserialize_value(
    const std::wstring &json_str, Json::Value *val
  );

  static std::string to_str(const std::wstring &wstr);

};


#endif // !JSON_HPP

