# json-cpp

Example:
```c++
#include <json.hpp>

#include <iostream>


struct Coords
{
  int x;
  int y;

  Json::Value Serialize()
  {
    return Json::StructType{
      { "x", x },
      { "y", y }
    };
  }
};


int main()
{
  Coords c { 2, 5 };

  Json j;

  j.Load(Json::StructType{
    { "coords", c.Serialize() }
  });
  j.GetData()["five"] = 5;

  std::cout << j.Serialize();

  return 0;
}
```
