# json-cpp

## Overview

This library is a lightweight JSON tool for C++ that provides easy-to-use functions for reading, writing, and validating JSON data.

## Features

- **Parsing:** Converts JSON strings into C++ data structures.
- **Serialization:** Converts C++ data structures back into JSON format.
- **Validation:** Checks JSON data for proper syntax and structure, returning error messages when necessary.
- **Support for Complex Structures:** Handles nested objects, arrays, and various data types (e.g., strings, numbers, booleans, null).

## Requirements

- C++17 or higher
- [CMake 3.22 or higher](https://cmake.org/) (if building with CMake)

## Linking with CMake

Clone this repo to your third-party folder and add following lines to your CMakeLists.txt

```cmake
add_subdirectory(third-party/json-cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE json-cpp)
```

## Usage

Include the library header in your project and call the provided functions to parse, validate, and serialize JSON data.

### Example code

```cpp
#include <json.hpp>
#include <iostream>

int main() {
    std::string json_string = R"({"name": "John Doe", "age": 30, "is_student": false})";

    // Validate the JSON data
    std::string log;
    if (!Json::ValidateString(json_string, log)) {
        // Printing log if error occures
        std::cout << log << std::endl;
        return 1;
    }

    // Create a JSON tool instance
    Json j;

    // Parse the JSON string into an internal representation
    j.LoadFromString(json_string);

    // Serialize the object back to a JSON string
    std::string output = j.Serialize();
    std::cout << output << std::endl;

    return 0;
}
```

