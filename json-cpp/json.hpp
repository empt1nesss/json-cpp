#ifndef JSON_HPP
#define JSON_HPP


#include <string>
#include <vector>
#include <filesystem>


class Json
{
public:

  enum class ERR
  {
    SUCCESS = 0,
    BAD_PATH,
    BAD_JSON
  };

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


  static bool ValidateString(
    const std::string &json_string
  );
  static bool ValidateString(
    const std::wstring &json_string
  );
  static bool ValidateString(
    const std::string &json_string, std::string &log
  );
  static bool ValidateString(
    const std::wstring &json_string, std::string &log
  );
  static Json::ERR ValidateFile(
    const std::filesystem::path &path
  );
  static Json::ERR ValidateFile(
    const std::filesystem::path &path, std::string &log
  );


  Json();
  ~Json();


  void Load          (const Value                 &val);
  ERR  LoadFromFile  (const std::filesystem::path &path);
  ERR  LoadFromString(const std::string           &json_string);
  ERR  LoadFromString(const std::wstring          &json_string);

  std::string   Serialize      ()                                  const;
  std::wstring  SerializeW     ()                                  const;
  bool          SerializeToFile(const std::filesystem::path &path) const;

  Value&        GetData()       { return *m_data; }
  const Value&  GetData() const { return *m_data; }

private:

  Value *m_data;

  static std::string to_str(
    const std::wstring &wstr
  );
  static bool read_file(
    std::wstring &out, const std::filesystem::path &path
  );
  static bool validate(
    const std::wstring &json_str, std::string *log=nullptr
  );
  static bool validate_property(
    const std::wstring &json_str, std::string *log, uint64_t ln, uint64_t col
  );
  static bool validate_value(
    const std::wstring &json_str, std::string *log, uint64_t ln, uint64_t col
  );

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

};


#endif // !JSON_HPP

