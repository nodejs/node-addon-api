# JSON

The `Napi::JSON` class provides methods to manage **JSON** object.

## Methods

### Parse

The `Napi::JSON::Parse` method parses a **JSON** string represented by a
`Napi::String`, constructing a value or an object described by the string.

```cpp
static Napi::Value Napi::JSON::Parse(Napi::Env env, Napi::String json_string);
```

- `[in] env`: The environment in which the API is invoked under.
- `[in] json_string`: The string to parse as **JSON**.

Returns a new instance of `Napi::Value` representing the given **JSON** string.

### Stringify

The `Napi::JSON::Stringify` method converts a `Napi::Value` object to a **JSON**
string, optionally replacing values if a replacer function is specified or
optionally including only the specified properties if a replacer array is
specified.

```cpp
static Napi::Value Napi::JSON::Stringify(Napi::Env env,
                              Napi::Object json_object,
                              Napi::Value replacer,
                              Napi::Value space);
```

- `[in] env`: The environment in which the API is invoked under.
- `[in] json_object`: The value to convert to a JSON string.
- `[in] replacer`: A function that alters the behavior of the stringification
process, or an array of String and Number objects that serve as a whitelist for
selecting/filtering the properties of the value object to be included in the JSON
string. If this value is null or not provided, all properties of the object are
included in the resulting JSON string.
- `[in] space`: A String or Number object that's used to insert white space into
the output JSON string for readability purposes.

Returns a new `Napi::String` object that is the stringified **JSON** representation
created from `Napi::Value`.
