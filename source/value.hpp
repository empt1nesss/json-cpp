#ifndef SOURCE_VALUE_HPP
#define SOURCE_VALUE_HPP


#include "json.hpp"


class Json::Value
{
public:

  enum ERR
  {
    NotProperties,
    NotFound
  };


  Value(const Value &val);

  Value();
  Value(bool                                    val);
  Value(int64_t                                 val);
  Value(double                                  val);
  Value(const std::string                     & val);
  Value(const std::wstring                    & val);
  Value(const std::list<Value>                & val);
  Value(const std::initializer_list<Value>    & val);
  Value(const std::list<Property>             & val);
  Value(const std::initializer_list<Property> & val);

  template <typename Iterator>
  Value(Iterator it_first, Iterator it_last, bool is_list);

  template <typename T, typename ToValue>
  Value(const std::list<T> &list, ToValue to_value);

  template <typename Iterator, typename ToValue>
  Value(Iterator it_first, Iterator it_last, ToValue to_value);

  ~Value();


  bool Contains(const std::wstring &prop_name) const;

  ValueType GetType() { return type; } const

  bool                GetBool()   const { return *(bool*)value; }
  int64_t             GetInt ()   const { return *(int64_t*)value; }
  double              GetFloat()  const { return *(double*)value; }
  std::wstring        GetString() const { return *(std::wstring*)value; }
  std::list<Value>    GetList()   const { return *(std::list<Value>*)value; }
  std::list<Property> GetStruct() const { return *(std::list<Property>*)value; }


  Value&        operator=  (const Value        &val);
  Value&        operator[] (const std::wstring &prop_name);
  const Value&  operator[] (const std::wstring &prop_name) const;
  
private:

  ValueType type;
  void*     value;


  void clear();


  friend std::wstring Json::serialize_value(
    const Value        &val
  );
  friend void         Json::deserialize_value(
    const std::wstring &json_str, Value *val
  );

};


template <typename Iterator>
Json::Value::Value(Iterator it_first, Iterator it_last, bool is_list)
{
  if (is_list) {
    type  = List;
    value = new std::list<Value>;

    for (; it_first != it_last; ++it_first)
      ((std::list<Value>*)value)->push_back(*it_first);
  }
  else {
    type  = Struct;
    value = new std::list<Property>;

    for (; it_first != it_last; ++it_first)
      ((std::list<Property>*)value)->push_back(*it_first);
  }
}

template <typename T, typename ToValue>
Json::Value::Value(const std::list<T> &list, ToValue to_value)
{
  type  = List;
  value = new std::list<Value>;
  for (auto & val : list)
    ((std::list<Value>*)value)->push_back(to_value(val));
}

template <typename Iterator, typename ToValue>
Json::Value::Value(Iterator it_first, Iterator it_last, ToValue to_value)
{
  type  = List;
  value = new std::list<Value>;
  for (; it_first != it_last; ++it_first)
    ((std::list<Value>*)value)->push_back(to_value(*it_first));
}


#endif // !SOURCE_VALUE_HPP

