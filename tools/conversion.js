const fs = require('fs');

var args = process.argv.slice(2);
var dir = args[0];
var paths = listFiles(dir);

var operations = [
  // ex. String::Utf8Value str(info[0]) to Napi::String str(env, info[0])
  [/v8::String::Utf8Value (.+?)\((.+?)\)/g, 'Napi::String $1(env, $2)'],
  [/String::Utf8Value (.+?)\((.+?)\)/g, 'Napi::String $1(env, $2)'],
  [/\.length\(\)/g, '.Length()'],
  // FunctionTemplate to FunctionReference
  [/v8::FunctionTemplate/g, 'Napi::FunctionReference'],
  [/FunctionTemplate/g, 'Napi::FunctionReference'],
  // HandleOKCallback to OnOK
  [/HandleOKCallback/g, 'OnOK'],
  // HandleErrorCallback to OnError
  [/HandleErrorCallback/g, 'OnError'],
  // ex. Local<Value> to Napi::Value
  [/v8::Local<v8::(\w+?)>/g, 'Napi::$1'],
  [/Local<(\w+?)>/g, 'Napi::$1'],
  // ex. Nan::New<Number>(info[0]) to Napi::Number::New(info[0])
  [/Nan::New\(([0-9\.]+)\)/g, 'Napi::Number::New(env, $1)'],
  [/Nan::New\("(.+?)"\)/g, 'Napi::String::New(env, "$1")'],
  [/Nan::New<(.+?)>\(\)/g, 'Napi::$1::New(env)'],
  [/Nan::New<(.+?)>\(/g, 'Napi::$1::New(env, '],
  [/Nan::NewBuffer\(/g, 'Napi::Buffer<char>::New(env, '],
  // TODO: Properly handle this
  [/Nan::New\(/g, 'Napi::New(env, '],
  // ex. Nan::To<bool>(info[0]) to info[0]->Value()
  [/Nan::To<bool>\((.+?)\)/g, '$1.As<Napi::Boolean>().Value()'],
  // ex. Nan::To<int>(info[0]) to info[0]->Int32Value()
  [/Nan::To<int>\((.+?)\)/g, '$1.As<Napi::Number>().Int32Value()'],
  // ex. Nan::To<int32_t>(info[0]) to info[0]->Int32Value()
  [/Nan::To<int32_t>\((.+?)\)/g, '$1.As<Napi::Number>().Int32Value()'],
  // ex. Nan::To<uint32_t>(info[0]) to info[0]->Uint32Value()
  [/Nan::To<uint32_t>\((.+?)\)/g, '$1.As<Napi::Number>().Uint32Value()'],
  // ex. Nan::To<int64_t>(info[0]) to info[0]->Int64Value()
  [/Nan::To<int64_t>\((.+?)\)/g, '$1.As<Napi::Number>().Int64Value()'],
  // ex. Nan::To<float>(info[0]) to info[0]->FloatValue()
  [/Nan::To<float>\((.+?)\)/g, '$1.As<Napi::Number>().FloatValue()'],
  // ex. Nan::To<double>(info[0]) to info[0]->DoubleValue()
  [/Nan::To<double>\((.+?)\)/g, '$1.As<Napi::Number>().DoubleValue()'],
  // ex. node::Buffer::HasInstance(info[0]) to info[0]->IsBuffer()
  [/node::Buffer::HasInstance\((.+?)\)/g, '$1.IsBuffer()'],
  // ex. node::Buffer::Length(info[0]) to info[0]->Length()
  [/node::Buffer::Length\((.+?)\)/g, '$1.As<Napi::Buffer<char>>().Length()'],
  // ex. node::Buffer::Data(info[0]) to info[0]->Data()
  [/node::Buffer::Data\((.+?)\)/g, '$1.As<Napi::Buffer<char>>().Data()'],
  // Nan::AsyncQueueWorker(worker)
  [/Nan::AsyncQueueWorker\((.+)\);/g, '$1.Queue();'],
  // Nan::ThrowError(error) to Napi::Error::New(env, error).ThrowAsJavaScriptException()
  [/Nan::Throw(\w*?)Error\((.+?)\);/g, 'Napi::$1Error::New(env, $2).ThrowAsJavaScriptException();'],
  // Nan::RangeError(error) to Napi::RangeError::New(env, error)
  [/Nan::(\w*?)Error\((.+)\)/g, 'Napi::$1Error::New(env, $2)'],

  [/Nan::Set\((.+?),\n* *(.+?),\n* *(.+?),\n* *(.+?)\)/g, '$1.Set($2, $3, $4)'],
  [/info\.GetReturnValue\(\)\.Set\(((\n|.)+?)\);/g, 'return $1;'],

  [/Nan::HandleScope (.+?);/g, 'Napi::HandleScope $1(env);'],
  [/Nan::EscapableHandleScope (.+?)/g, 'Napi::EscapableHandleScope $1(env)'],
  [/Nan::Callback/g, 'Napi::Function'],
  [/Nan::Persistent<FunctionTemplate>/g, 'Napi::FunctionReference'],
  [/Nan::ADDON_REGISTER_FUNCTION_ARGS_TYPE target/g, 'Napi::Env& env, Napi::Object& target'],
  // direct replacement
  [/NAN_METHOD\((\w+?)\)/g, 'Napi::Value $1(const Napi::CallbackInfo& info)'],
  [/NAN_MODULE_INIT\((\w+?)\)/g, 'void $1(Napi::Env env, Napi::Object exports, Napi::Object module)'],
  [/Nan::/g, 'Napi::'],
  [/nan.h/g, 'napi.h'],
  // delete using v8::XXX;
  [/using v8;/g, 'using Napi;'],
  // delete using v8::XXX;
  [/using v8::Local;\n/g, ''],
  // delete using v8::XXX;
  [/using v8::([A-Za-z]+);/g, 'using Napi::$1;'],

  // delete .FromJust()
  [/\.FromJust\(\)/g, ''],
  // delete .ToLocalCheck()
  [/\.ToLocalChecked\(\)/g, ''],
  // delete v8::
  [/v8::/g, ''],
  // delete #include <v8.h>
  [/#include <v8.h>\n/g, ''],
  // NODE_MODULE to NODE_API_MODULE
  [/NODE_MODULE/g, 'NODE_API_MODULE']
];

paths.forEach(function(path) {
  convertFile(path, operations);
});

function listFiles(dir, filelist) {
  var path = path || require('path');
  files = fs.readdirSync(dir);
  filelist = filelist || [];
  files.forEach(function(file) {
    if (fs.statSync(path.join(dir, file)).isDirectory()) {
      filelist = listFiles(path.join(dir, file), fs, filelist);
    }
    else {
      filelist.push(path.join(dir, file));
    }
  });
  return filelist;
};

function convert(string, operations) {
  operations.forEach(function(operation) {
    string = string.replace(operation[0], operation[1]);
  });
  return string;
};

function convertFile(path, operations) {
  fs.readFile(path, "utf-8", function (err, file) {
    if (err) throw err;
    // console.log(file);

    file = convert(file, operations);

    fs.writeFile(path, file, function(err){
      if (err) throw err;
    });
  });
};
