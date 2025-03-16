#include "json.hpp"
#include "property.hpp"

#include <wchar.h>
#include <stdexcept>
#include <cstdint>
#include <fstream>
#include <filesystem>
#include <filesystem>
#include <locale>


bool Json::ValidateString(const std::string &json_string)
{
  return ValidateString(
    std::wstring(json_string.begin(), json_string.end())
  );
}

bool Json::ValidateString(const std::wstring &json_string)
{
  return validate(json_string);
}

bool Json::ValidateString(const std::string &json_string, std::string &log)
{
  return ValidateString(
    std::wstring(json_string.begin(), json_string.end()),
    log
  );
}

bool Json::ValidateString(const std::wstring &json_string, std::string &log)
{
  return validate(json_string, &log);
}

Json::ERR Json::ValidateFile(const std::filesystem::path &path)
{
  std::wstring json_str;
  if (!read_file(json_str, path))
    return ERR::BAD_PATH;

  return validate(json_str) ? Json::ERR::SUCCESS : Json::ERR::BAD_JSON;
}

Json::ERR Json::ValidateFile(const std::filesystem::path &path, std::string &log)
{
  std::wstring json_str;
  if (!read_file(json_str, path))
    return ERR::BAD_PATH;

  return validate(json_str, &log) ? Json::ERR::SUCCESS : Json::ERR::BAD_JSON;
}


Json::Json() :
  m_data(new Value())
{}

Json::~Json()
{
  delete m_data;
}


void Json::Load(const Value &val)
{
  *m_data = val;
}

Json::ERR Json::LoadFromFile(const std::filesystem::path &path)
{
  std::wstring json_str;
  if (!read_file(json_str, path))
    return ERR::BAD_PATH;

  return LoadFromString(json_str);
}

Json::ERR Json::LoadFromString(const std::string &json_string)
{
  return LoadFromString(std::wstring(json_string.begin(), json_string.end()));
}

Json::ERR Json::LoadFromString(const std::wstring &json_string)
{
  std::string validator_log;
  if (!validate(json_string, &validator_log))
    return ERR::BAD_JSON;

  auto st  = json_string.begin();
  auto end = json_string.end();

  while (
    *st == L' ' || *st == L'\t' || *st == L'\r' || *st == L'\n'
  ) {
    ++st;
  }

  while (
    *(end - 1) == L' ' || *(end - 1) == L'\t' || *(end - 1) == L'\r' || *(end - 1) == L'\n'
  ) {
    --end;
  }

  deserialize_value(std::wstring(st, end), m_data);
  return ERR::SUCCESS;
}

std::string Json::Serialize() const
{
  return to_str(serialize_value(*m_data));
}

std::wstring Json::SerializeW() const
{
  return serialize_value(*m_data);
}

bool Json::SerializeToFile(const std::filesystem::path &path) const
{
  std::wofstream file(path);
  file.imbue(std::locale("en_US.UTF-8"));
  if (!file.is_open())
    return false;

  file << SerializeW();
  file.close();
  return true;
}



std::string Json::to_str(const std::wstring &wstr)
{
  std::string out(wstr.size(), '\0');
  for (size_t i = 0; i < wstr.size(); ++i) {
    if (wstr[i] > 127)
      out[i] = '?';
    else
     out[i] = wstr[i];
  }

  return out;
}

bool Json::read_file(std::wstring &out, const std::filesystem::path &path)
{
  std::wifstream file(path);

  out.clear();
  if (!file.is_open())
    return false;

  file.imbue(std::locale("en_US.UTF-8"));
  for (std::wstring line; std::getline(file, line);)
    out += line + L'\n';

  file.close();
  return true;
}

bool Json::validate(
  const std::wstring &json_str, std::string *log
)
{
  uint64_t ln  = 1;
  uint64_t col = 0;

  auto st  = json_str.begin();
  auto end = json_str.end();

  for (; st != json_str.end(); ++st) {
    ++col;

    bool br = false;
    switch (*st)
    {
    case L' ':                  break;
    case L'\t':                 break;
    case L'\r': col = 0;        break;
    case L'\n': col = 0;  ++ln; break;
    default:
      br = true;
      break;
    }
    if (br)
      break;
  }

  if (st == json_str.end()) {
    if (log)
      *log = "Empty json";
    return false;
  }

  while (
    *(end - 1) == L' ' || *(end - 1) == L'\t' || *(end - 1) == L'\r' || *(end - 1) == L'\n'
  ) {
    --end;
  }

  return validate_value(
    std::wstring(st, end), log, ln, col
  );
}

bool Json::validate_property(
  const std::wstring &json_str, std::string *log, uint64_t ln, uint64_t col
)
{
  const auto make_log = [](
    const std::string &msg, size_t ln, size_t col
  )
  {
    return msg + " (ln. " + std::to_string(ln) + ", col. " + std::to_string(col) + ")";
  };

  auto name_first = json_str.begin();
  auto name_last  = json_str.end();
  auto val_first  = json_str.end();
  auto val_last   = json_str.end();

  for (; name_first != json_str.end(); ++name_first) {
    if (*name_first == L'\"')
      break;
    
    switch (*name_first)
    {
    case L' ':                  break;
    case L'\t':                 break;
    case L'\r': col = 0;        break;
    case L'\n': col = 0;  ++ln; break;
    default:
      if (log)
        *log = make_log("Expected \'\"\'", ln, col);
      return false;
    }

    ++col;
  }

  if (name_first == json_str.end()) {
    if (log)
      *log = make_log("Property is empty", ln, col);
    return false;
  }


  bool esc = false;
  ++col;
  for (name_last = name_first + 1; name_last != json_str.end(); ++name_last) {
    if (*name_last == L'\"' && !esc)
      break;
    
    esc = *name_last == L'\\' && !esc;

    switch (*name_last)
    {
    case L' ':                  break;
    case L'\t':                 break;
    case L'\r': col = 0;        break;
    case L'\n': col = 0;  ++ln; break;
    default:                    break;
    }
    ++col;
  }

  if (name_last == json_str.end()) {
    if (log)
      *log = make_log("Expected \'\"\'", ln, col);
    return false;
  }

  uint64_t name_last_ln  = ln;
  uint64_t name_last_col = col;

  ++col;
  for (val_first = name_last + 1; val_first != json_str.end(); ++val_first) {
    if (*val_first == L':') {
      ++val_first;
      if (val_first == json_str.end()) {
        if (log)
          *log = make_log("Expected value", ln, col);
        return false;
      }
      ++col;
      break;
    }

    switch (*val_first)
    {
    case L' ':                  break;
    case L'\t':                 break;
    case L'\r': col = 0;        break;
    case L'\n': col = 0;  ++ln; break;
    default:
      if (log)
        *log = make_log("Expected \':\'", ln, col);
      return false;
    }

    ++col;
  }

  if (val_first == json_str.end()) {
    if (log)
      *log = make_log("Expected \':\'", name_last_ln, name_last_col + 1);
    return false;
  }

  while (
    *val_first == L' ' || *val_first == L'\t' || *val_first == L'\r' || *val_first == L'\n'
  ) {
    ++val_first;
    if (val_first == json_str.end()) {
      if (log)
        *log = make_log("Expected value", ln, col);
      return false;
    }

    switch (*val_first)
    {
    case L' ':                  break;
    case L'\t':                 break;
    case L'\r': col = 0;        break;
    case L'\n': col = 0;  ++ln; break;
    default:                    break;
    }

    ++col;
  }

  while (
    *(val_last - 1) == L' ' || *(val_last - 1) == L'\t' || *(val_last - 1) == L'\r' || *(val_last - 1) == L'\n'
  ) {
    --val_last;
  }

  return validate_value(
    std::wstring(val_first, val_last),
    log,
    ln,
    col
  );
}

bool Json::validate_value(
  const std::wstring &json_str, std::string *log, uint64_t ln, uint64_t col
)
{
  const auto make_log = [](
    const std::string &msg, size_t ln, size_t col
  )
  {
    return msg + " (ln. " + std::to_string(ln) + ", col. " + std::to_string(col) + ")";
  };

  const auto type_error = [&]()
  {
    if (log)
      *log = make_log("Unknown type", ln, col);
    return false;
  };


  if (json_str == L"null") {
    return true;
  }
  else if (json_str == L"false") {
    return true;
  }
  else if (json_str == L"true") {
    return true;
  }
  else if (
    (json_str[0] >= L'0' && json_str[0] <= L'9') || json_str[0] == L'-' || json_str[0] == L'.'
  ) {
    auto is_int = [](const std::wstring &str, bool is_signed)
    {
      size_t i = 0;

      if (str.empty() || str == L"-")
        return false;

      if (is_signed && str[0] == L'-')
        ++i;

      for (; i < str.size(); ++i)
        if (str[i] < L'0' || str[i] > L'9')
          return false;

      return true;
    };

    auto is_float = [&](const std::wstring &str)
    {
      if (str.empty() || str == L".")
        return false;

      auto point = str.find(L'.');
      if (point == str.npos)
        point = str.size();

      if (point > 0) {
        if (
          !is_int(
            std::wstring(str.begin(), str.begin() + point),
            true
          )
        ) {
          return false;
        }
      }
      if (point < json_str.size() - 1) {
        if (
          !is_int(
            std::wstring(str.begin() + point + 1, str.end()),
            false
          )
        ) {
          return false;
        }
      }

      return true;
    };


    auto fi_e = json_str.find(L'e');
    if (fi_e == json_str.npos)
      fi_e = json_str.size();

    if (fi_e > 0) {
      if (
        !is_float(std::wstring(json_str.begin(), json_str.begin() + fi_e))
      ) {
        return type_error();
      }
    }
    if (fi_e < json_str.size() - 1) {
      if (
        !is_int(
          std::wstring(json_str.begin() + fi_e + 1, json_str.end()),
          true
        )
      ) {
        return type_error();
      }
    }

    return true;
  }
  else if (json_str[0] == L'\"') {
    bool quote = true;
    bool esc   = false;

    for (size_t i = 1; i < json_str.size(); ++i) {
      if (!esc && json_str[i] == L'\"')
        quote = !quote;
      else if (quote)
        esc = json_str[i] == L'\\' && !esc;
      else {
        if (
          json_str[i] != L' ' && json_str[i] != L'\t' && json_str[i] != L'\r' && json_str[i] != L'\n'
        ) {
          return type_error();
        }
      }
    }

    if (quote)
      return type_error();

    return true;
  }
  else if (json_str[0] == L'[' || json_str[0] == L'{') {
    uint64_t curly_br_c  = 0;
    uint64_t square_br_c = 0;
    uint64_t last_comma  = 0;
    uint64_t ln_comma    = ln;
    uint64_t col_comma   = col;
    bool     str         = false;
    bool     esc         = false;

    ++col;
    for (size_t i = 1; i < json_str.size(); ++i) {
      if (!str) {
        if (json_str[i] == L'{')
          ++curly_br_c;
        else if (json_str[i] == L'}')
          --curly_br_c;
        else if (json_str[i] == L'[')
          ++square_br_c;
        else if (json_str[i] == L']')
          --square_br_c;
        else if (curly_br_c == 0 && square_br_c == 0 && json_str[i] == L',') {
          auto val_st  = json_str.begin() + last_comma + 1;
          auto val_end = json_str.begin() + i;

          uint64_t val_ln  = ln_comma;
          uint64_t val_col = col_comma + 1;

          ln_comma  = ln;
          col_comma = col;

          last_comma = i;

          while (
            *val_st == L' ' || *val_st == L'\t' || *val_st == L'\r' || *val_st == L'\n'
          ) {
            switch (*val_st)
            {
            case L' ':                          break;
            case L'\t':                         break;
            case L'\r': val_col = 0;            break;
            case L'\n': val_col = 0;  ++val_ln; break;
            default:                            break;
            }

            ++val_st;
            ++val_col;
          }
          while (
            *(val_end - 1) == L' ' || *(val_end - 1) == L'\t' || *(val_end - 1) == L'\r' || *(val_end - 1) == L'\n'
          ) {
            --val_end;
          }

          if (json_str[0] == L'[') {
            if (
              !validate_value(
                std::wstring(val_st, val_end),
                log,
                val_ln,
                val_col
              )
            ) {
              return false;
            }
          }
          else {
            if (
              !validate_property(
                std::wstring(val_st, val_end),
                log,
                val_ln,
                val_col
              )
            ) {
              return false;
            }
          }
        }
      }

      if (!esc && json_str[i] == L'\"')
        str = !str;
      else if (str)
        esc = json_str[i] == L'\\' && !esc;

      switch (json_str[i])
      {
      case L'\t':                 break;
      case L'\r': col = 0;        break;
      case L'\n': col = 0;  ++ln; break;
      default:                    break;
      }

      ++col;
    }

    if (json_str[0] == L'[') {
      if (*(json_str.end() - 1) != L']') {
        if (log)
          *log = make_log("Expected ']'", ln, col);
        return false;
      }
    }
    else {
      if (*(json_str.end() - 1) != L'}') {
        if (log)
          *log = make_log("Expected '}'", ln, col);
        return false;
      }
    }

    bool is_comma = json_str[last_comma] == L',';
    auto val_st   = json_str.begin() + last_comma + 1;
    auto val_end  = json_str.end() - 1;

    uint64_t val_ln  = ln_comma;
    uint64_t val_col = col_comma + 1;

    while (
      *val_st == L' ' || *val_st == L'\t' || *val_st == L'\r' || *val_st == L'\n'
    ) {
      switch (*val_st)
      {
      case L' ':                          break;
      case L'\t':                         break;
      case L'\r': val_col = 0;            break;
      case L'\n': val_col = 0;  ++val_ln; break;
      default:                            break;
      }

      ++val_st;
      ++val_col;
    }
    while (
      *(val_end - 1) == L' ' || *(val_end - 1) == L'\t' || *(val_end - 1) == L'\r' || *(val_end - 1) == L'\n'
    ) {
      --val_end;
    }

    if (
      val_end == json_str.begin() + last_comma + 1 &&
      val_st  == json_str.end() - 1
    ) {
      if (is_comma) {
        if (log) {
          if (json_str[0] == L'[') {
            *log = make_log(
              "Expected value", val_ln, val_col
            );
          }
          else {
            *log = make_log(
              "Expected property", val_ln, val_col
            );
          }
        }
        return false;
      }

      return true;
    }

    if (json_str[0] == L'[') {
      return validate_value(
        std::wstring(val_st, val_end),
        log,
        val_ln,
        val_col
      );
    }
    
    return validate_property(
      std::wstring(val_st, val_end),
      log,
      val_ln,
      val_col
    );
  }

  return type_error();
}

std::wstring Json::format_in(std::wstring json_str)
{
  if (json_str.size() < 2)
    return json_str;

  for (size_t i = 0; i < json_str.size() - 1; ++i) {
    if (json_str[i] == L'\\') {
      switch (json_str[i + 1])
      {
      case L'\"':
        json_str.erase(json_str.begin() + i);
        break;
      case L'\\':
        json_str.erase(json_str.begin() + i);
        break;
      case L'/':
        json_str.erase(json_str.begin() + i);
        break;
      case L'b':
        json_str.erase(json_str.begin() + i);
        json_str[i] = L'\b';
        break;
      case L'f':
        json_str.erase(json_str.begin() + i);
        json_str[i] = L'\f';
        break;
      case L'n':
        json_str.erase(json_str.begin() + i);
        json_str[i] = L'\n';
        break;
      case L'r':
        json_str.erase(json_str.begin() + i);
        json_str[i] = L'\r';
        break;
      case L't':
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

std::wstring Json::format_out(std::wstring str)
{
  for (size_t i = 0; i < str.size(); ++i) {
    switch (str[i])
    {
    case L'\"':
    case L'\\':
    case L'/':
      str.insert(str.begin() + i + 1, str[i]);
      str[i++] = L'\\';
      break;
    case L'\b':
      str.insert(str.begin() + i + 1, L'b');
      str[i++] = L'\\';
      break;
    case L'\f':
      str.insert(str.begin() + i + 1, L'f');
      str[i++] = L'\\';
      break;
    case L'\n':
      str.insert(str.begin() + i + 1, L'n');
      str[i++] = L'\\';
      break;
    case L'\r':
      str.insert(str.begin() + i + 1, L'r');
      str[i++] = L'\\';
      break;
    case L'\t':
      str.insert(str.begin() + i + 1, L't');
      str[i++] = L'\\';
      break;
      
    default:
      break;
    }
  }

  return str;
}

std::wstring Json::serialize_property(const Json::Property &prop)
{
  return L"\"" + format_out(prop.GetNameW()) + L"\":" + serialize_value(prop.m_value);
}

std::wstring Json::serialize_value(const Json::Value &val)
{
  switch (val.m_type)
  {
  case Json::Bool:
    return *(bool*)val.m_value ? L"true" : L"false";
  case Json::Int:
    return std::to_wstring(*(int64_t*)val.m_value);
  case Json::Float:
    return std::to_wstring(*(double*)val.m_value);
  case Json::String: {
    return L"\"" + format_out(*(std::wstring*)val.m_value) + L"\"";
  }
  case Json::List: {
    std::wstring out = L"[";

    if (((ListType*)val.m_value)->size() != 0) {
      out += serialize_value(
        *(*(ListType*)val.m_value).begin()
      );
      for (
        auto it = ++((ListType*)val.m_value)->begin();
        it != ((ListType*)val.m_value)->end();
        ++it
      ) {
        out += L"," + serialize_value(*it);
      }
    }

    return out += L"]";
  }
  case Json::Struct: {
    std::wstring out = L"{";

    if (((StructType*)val.m_value)->size() != 0) {
      out += serialize_property(
        *(*(StructType*)val.m_value).begin()
      );
      for (
        auto it = ++((StructType*)val.m_value)->begin();
        it != ((StructType*)val.m_value)->end();
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

void Json::deserialize_property(
  const std::wstring &json_str, Json::Property *prop
)
{
  if (prop == nullptr || json_str.empty())
    throw std::runtime_error(
      "deserialize_property was called with null argument"
    );

  auto name_first   = json_str.begin();
  auto name_last    = json_str.end();
  auto val_first    = json_str.end();
  auto val_last     = json_str.end();

  bool esc = false;

  for (; name_first != json_str.end(); ++name_first)
    if (*name_first == L'\"')
      break;
  
  for (name_last = name_first + 1; name_last != json_str.end(); ++name_last) {
    if (*name_last == L'\"' && !esc)
      break;

    esc = *name_last == L'\\' && !esc;
  }

  for (val_first = name_last + 1; val_first != json_str.end(); ++val_first)
    if (*val_first == L':')
      break;
  
  ++val_first;
  while (
    *val_first == L' ' || *val_first == L'\t' || *val_first == L'\r' || *val_first == L'\n'
  ) {
    ++val_first;
  }

  while (
    *(val_last - 1) == L' ' || *(val_last - 1) == L'\t' || *(val_last - 1) == L'\r' || *(val_last - 1) == L'\n'
  ) {
    --val_last;
  }


  prop->SetName(format_in(std::wstring(name_first + 1, name_last)));
  deserialize_value(
    std::wstring(val_first, val_last), &prop->m_value
  );
}

void Json::deserialize_value(
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
  else if (
    (json_str[0] >= L'0' && json_str[0] <= L'9') || json_str[0] == L'-' || json_str[0] == L'.'
  ) {
    if (json_str.find(L'.') == json_str.npos) {
      *val = Json::Value((int64_t)std::stoll(json_str));
    }
    else {
      *val = Json::Value(std::stod(json_str));
    }
  }
  else if (json_str[0] == L'\"') {
    size_t       st  = 1,    end = json_str.size();
    bool         esc = false;
    std::wstring res;

    for (size_t i = 1; i < json_str.size(); ++i) {
      if (!esc && json_str[i] == L'\"') {
        if (end == json_str.size()) {
          end = i;
          res += std::wstring(json_str.begin() + st, json_str.begin() + end);
        }
        else {
          end = json_str.size();
          st = i + 1;
        }
      }

      esc = json_str[i] == L'\\' && !esc;
    }

    val->m_type  = Json::String;
    val->m_value = new std::wstring(format_in(res));
  }
  else if (json_str[0] == L'[' || json_str[0] == L'{') {
    uint64_t square_br_c = 0;
    uint64_t curly_br_c  = 0;
    uint64_t last_comma  = 0;
    bool     str         = false;
    bool     esc         = false;

    if (json_str[0] == L'[') {
      val->m_type   = Json::List;
      val->m_value  = new ListType;
    }
    else {
      val->m_type   = Json::Struct;
      val->m_value  = new StructType;
    }

    for (size_t i = 1; i < json_str.size(); ++i) {
      if (!str) {
        if (json_str[i] == L'[')
          ++square_br_c;
        else if (json_str[i] == L']')
          --square_br_c;
        else if (json_str[i] == L'{')
          ++curly_br_c;
        else if (json_str[i] == L'}')
          --curly_br_c;
        else if (square_br_c == 0 && curly_br_c == 0 && json_str[i] == L',') {
          auto val_st  = json_str.begin() + last_comma + 1;
          auto val_end = json_str.begin() + i;

          last_comma = i;

          while (
            *val_st == L' ' || *val_st == L'\t' || *val_st == L'\r' || *val_st == L'\n'
          ) {
            ++val_st;
          }
          while (
            *(val_end - 1) == L' ' || *(val_end - 1) == L'\t' || *(val_end - 1) == L'\r' || *(val_end - 1) == L'\n'
          ) {
            --val_end;
          }

          if (json_str[0] == L'[') {
            Json::Value _val;
            deserialize_value(
              std::wstring(val_st, val_end),
              &_val
            );
            ((ListType*)(val->m_value))->push_back(_val);
          }
          else {
            Json::Property _prop(L"", Json::Value());
            deserialize_property(
              std::wstring(val_st, val_end),
              &_prop
            );
            ((StructType*)(val->m_value))->push_back(_prop);
          }
        }
      }

      if (!esc && json_str[i] == L'\"')
        str = !str;

      if (str)
        esc = json_str[i] == L'\\' && !esc;
    }

    auto val_st   = json_str.begin() + last_comma + 1;
    auto val_end  = json_str.end() - 1;

    while (
      *val_st == L' ' || *val_st == L'\t' || *val_st == L'\r' || *val_st == L'\n'
    ) {
      ++val_st;
    }
    while (
      *(val_end - 1) == L' ' || *(val_end - 1) == L'\t' || *(val_end - 1) == L'\r' || *(val_end - 1) == L'\n'
    ) {
      --val_end;
    }

    if (
      val_end == json_str.begin() + last_comma + 1 &&
      val_st  == json_str.end() - 1
    ) {
      return;
    }

    if (json_str[0] == L'[') {
      Json::Value _val;
      deserialize_value(
        std::wstring(val_st, val_end),
        &_val
      );
      ((ListType*)(val->m_value))->push_back(_val);
    }
    else {
      Json::Property _prop(L"", Json::Value());
      deserialize_property(
        std::wstring(val_st, val_end),
        &_prop
      );
      ((StructType*)(val->m_value))->push_back(_prop);
    }
  }
}
