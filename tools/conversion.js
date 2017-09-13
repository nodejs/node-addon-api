#! /usr/bin/env node

'use strict'

const fs = require('fs');
const path = require('path');

const args = process.argv.slice(2);
const dir = args[0];
if (!dir) {
  console.log('Usage: node ' + path.basename(__filename) + ' <target-dir>');
  process.exit(1);
}

const NodeApiVersion = require('../package.json').version;

var ConfigFileOperations = {
  'package.json': [
    [ /"nan": *"[^"]+"/g, '"node-addon-api": "' + NodeApiVersion + '"' ]
  ],
  'binding.gyp': [
    [ /\(node -e \\("|')require\(("|')nan("|')\)\\("|')\)/g, '@(node -p \\$1require(\$2node-addon-api\$3).include\\$4)' ],
    [ /("|')target_name("|'): ("|')(.+?)("|'),/g, '$1target_name$2: $3$4$5,\n      $1cflags!$1: [ $1-fno-exceptions$1 ],\n      $1cflags_cc!$1: [ $1-fno-exceptions$1 ],' ],
  ]
};

var SourceFileOperations = [
  [ /v8::Local<v8::FunctionTemplate>\s+(\w+)\s*=\s*Nan::New<FunctionTemplate>\([\w\d:]+\);(?:\w+->Reset\(\1\))?\s+\1->SetClassName\(Nan::String::New\("(\w+)"\)\);/g, 'Napi::Function $1 = DefineClass(env, "$2", {' ],
  [ /Local<FunctionTemplate>\s+(\w+)\s*=\s*Nan::New<FunctionTemplate>\([\w\d:]+\);\s+(\w+)\.Reset\((\1)\);\s+\1->SetClassName\((Nan::String::New|Nan::New<(v8::)*String>)\("(.+?)"\)\);/g, 'Napi::Function $1 = DefineClass(env, "$6", {'],
  [ /Local<FunctionTemplate>\s+(\w+)\s*=\s*Nan::New<FunctionTemplate>\([\w\d:]+\);(?:\w+->Reset\(\1\))?\s+\1->SetClassName\(Nan::String::New\("(\w+)"\)\);/g, 'Napi::Function $1 = DefineClass(env, "$2", {' ],
  [ /Nan::New<v8::FunctionTemplate>\(([\w\d:]+)\)->GetFunction\(\)/g, 'Napi::Function::New(env, $1)' ],
  [ /Nan::New<FunctionTemplate>\(([\w\d:]+)\)->GetFunction()/g, 'Napi::Function::New(env, $1);' ],
  [ /Nan::New<v8::FunctionTemplate>\(([\w\d:]+)\)/g, 'Napi::Function::New(env, $1)' ],
  [ /Nan::New<FunctionTemplate>\(([\w\d:]+)\)/g, 'Napi::Function::New(env, $1)' ],

  // FunctionTemplate to FunctionReference
  [ /Nan::Persistent<(v8::)*FunctionTemplate>/g, 'Napi::FunctionReference' ],
  [ /Nan::Persistent<(v8::)*Function>/g, 'Napi::FunctionReference' ],
  [ /v8::Local<v8::FunctionTemplate>/g, 'Napi::FunctionReference' ],
  [ /Local<FunctionTemplate>/g, 'Napi::FunctionReference' ],
  [ /v8::FunctionTemplate/g, 'Napi::FunctionReference' ],
  [ /FunctionTemplate/g, 'Napi::FunctionReference' ],


  [ /Nan::SetPrototypeMethod\(\w+, "(\w+)", (\w+)\);/g, '  InstanceMethod("$1", &$2),' ],
  [ /(?:\w+\.Reset\(\w+\);\s+)?\(target\)\.Set\("(\w+)",\s*Nan::GetFunction\((\w+)\)\);/gm,
    '});\n\n' +
    '  constructor = Napi::Persistent($2);\n' +
    '  constructor.SuppressDestruct();\n' +
    '  target.Set("$1", $2);' ],


  // TODO: Other attribute combinations
  [ /static_cast<PropertyAttribute>\(ReadOnly\s*\|\s*DontDelete\)/gm,
    'static_cast<napi_property_attributes>(napi_enumerable | napi_configurable)' ],

  [ /([\w\d:<>]+?)::Cast\((.+?)\)/g, '$2.As<$1>()' ],

  [ /\*Nan::Utf8String\(([^)]+)\)/g, '$1->As<Napi::String>().Utf8Value().c_str()' ],
  [ /Nan::Utf8String +(\w+)\(([^)]+)\)/g, 'std::string $1 = $2.As<Napi::String>()' ],
  [ /Nan::Utf8String/g, 'std::string' ],

  [ /v8::String::Utf8Value (.+?)\((.+?)\)/g, 'Napi::String $1(env, $2)' ],
  [ /String::Utf8Value (.+?)\((.+?)\)/g, 'Napi::String $1(env, $2)' ],
  [ /\.length\(\)/g, '.Length()' ],

  [ /Nan::MakeCallback\(([^,]+),[\s\\]+([^,]+),/gm, '$2.MakeCallback($1,' ],

  [ /class\s+(\w+)\s*:\s*public\s+Nan::ObjectWrap/g, 'class $1 : public Napi::ObjectWrap<$1>' ],
  [ /(\w+)\(([^\)]*)\)\s*:\s*Nan::ObjectWrap\(\)\s*(,)?/gm, '$1($2) : Napi::ObjectWrap<$1>()$3' ],

  // HandleOKCallback to OnOK
  [ /HandleOKCallback/g, 'OnOK' ],
  // HandleErrorCallback to OnError
  [ /HandleErrorCallback/g, 'OnError' ],

  // ex. .As<Function>() to .As<Napi::Object>()
  [ /\.As<v8::(Value|Boolean|String|Number|Object|Array|Symbol|External|Function)>\(\)/g, '.As<Napi::$1>()' ],
  [ /\.As<(Value|Boolean|String|Number|Object|Array|Symbol|External|Function)>\(\)/g, '.As<Napi::$1>()' ],

  // ex. Nan::New<Number>(info[0]) to Napi::Number::New(info[0])
  [ /Nan::New<(v8::)*Integer>\((.+?)\)/g, 'Napi::Number::New(env, $2)' ],
  [ /Nan::New\(([0-9\.]+)\)/g, 'Napi::Number::New(env, $1)' ],
  [ /Nan::New<(v8::)*String>\("(.+?)"\)/g, 'Napi::String::New(env, "$2")' ],
  [ /Nan::New\("(.+?)"\)/g, 'Napi::String::New(env, "$1")' ],
  [ /Nan::New<(v8::)*(.+?)>\(\)/g, 'Napi::$2::New(env)' ],
  [ /Nan::New<(.+?)>\(\)/g, 'Napi::$1::New(env)' ],
  [ /Nan::New<(v8::)*(.+?)>\(/g, 'Napi::$2::New(env, ' ],
  [ /Nan::New<(.+?)>\(/g, 'Napi::$1::New(env, ' ],
  [ /Nan::NewBuffer\(/g, 'Napi::Buffer<char>::New(env, ' ],
  // TODO: Properly handle this
  [ /Nan::New\(/g, 'Napi::New(env, ' ],

  [ /\.IsInt32\(\)/g, '.IsNumber()' ],
  [ /->IsInt32\(\)/g, '.IsNumber()' ],


  [ /(.+?)->BooleanValue\(\)/g, '$1.As<Napi::Boolean>().BooleanValue()' ],
  [ /(.+?)->Int32Value\(\)/g, '$1.As<Napi::Number>().Int32Value()' ],
  [ /(.+?)->Uint32Value\(\)/g, '$1.As<Napi::Number>().Uint32Value()' ],
  [ /(.+?)->IntegerValue\(\)/g, '$1.As<Napi::Number>().Int64Value()' ],
  [ /(.+?)->NumberValue\(\)/g, '$1.As<Napi::Number>().DoubleValue()' ],

  // ex. Nan::To<bool>(info[0]) to info[0].Value()
  [ /Nan::To<v8::(Boolean|String|Number|Object|Array|Symbol|Function)>\((.+?)\)/g, '$2.To<Napi::$1>()' ],
  [ /Nan::To<(Boolean|String|Number|Object|Array|Symbol|Function)>\((.+?)\)/g, '$2.To<Napi::$1>()' ],
  // ex. Nan::To<bool>(info[0]) to info[0].As<Napi::Boolean>().Value()
  [ /Nan::To<bool>\((.+?)\)/g, '$1.As<Napi::Boolean>().Value()' ],
  // ex. Nan::To<int>(info[0]) to info[0].As<Napi::Number>().Int32Value()
  [ /Nan::To<int>\((.+?)\)/g, '$1.As<Napi::Number>().Int32Value()' ],
  // ex. Nan::To<int32_t>(info[0]) to info[0].As<Napi::Number>().Int32Value()
  [ /Nan::To<int32_t>\((.+?)\)/g, '$1.As<Napi::Number>().Int32Value()' ],
  // ex. Nan::To<uint32_t>(info[0]) to info[0].As<Napi::Number>().Uint32Value()
  [ /Nan::To<uint32_t>\((.+?)\)/g, '$1.As<Napi::Number>().Uint32Value()' ],
  // ex. Nan::To<int64_t>(info[0]) to info[0].As<Napi::Number>().Int64Value()
  [ /Nan::To<int64_t>\((.+?)\)/g, '$1.As<Napi::Number>().Int64Value()' ],
  // ex. Nan::To<float>(info[0]) to info[0].As<Napi::Number>().FloatValue()
  [ /Nan::To<float>\((.+?)\)/g, '$1.As<Napi::Number>().FloatValue()' ],
  // ex. Nan::To<double>(info[0]) to info[0].As<Napi::Number>().DoubleValue()
  [ /Nan::To<double>\((.+?)\)/g, '$1.As<Napi::Number>().DoubleValue()' ],

  [ /Nan::New\((\w+)\)->HasInstance\((\w+)\)/g, '$2.InstanceOf($1.Value())' ],

  [ /Nan::Has\(([^,]+),\s*/gm, '($1).Has(' ],
  [ /\.Has\([\s|\\]*Nan::New<(v8::)*String>\(([^)]+)\)\)/gm, '.Has($1)' ],
  [ /\.Has\([\s|\\]*Nan::New\(([^)]+)\)\)/gm, '.Has($1)' ],

  [ /Nan::Get\(([^,]+),\s*/gm, '($1).Get(' ],
  [ /\.Get\([\s|\\]*Nan::New<(v8::)*String>\(([^)]+)\)\)/gm, '.Get($1)' ],
  [ /\.Get\([\s|\\]*Nan::New\(([^)]+)\)\)/gm, '.Get($1)' ],

  [ /Nan::Set\(([^,]+),\s*/gm, '($1).Set(' ],
  [ /\.Set\([\s|\\]*Nan::New<(v8::)*String>\(([^)]+)\)\s*,/gm, '.Set($1,' ],
  [ /\.Set\([\s|\\]*Nan::New\(([^)]+)\)\s*,/gm, '.Set($1,' ],


  // ex. node::Buffer::HasInstance(info[0]) to info[0].IsBuffer()
  [ /node::Buffer::HasInstance\((.+?)\)/g, '$1.IsBuffer()' ],
  // ex. node::Buffer::Length(info[0]) to info[0].Length()
  [ /node::Buffer::Length\((.+?)\)/g, '$1.As<Napi::Buffer<char>>().Length()' ],
  // ex. node::Buffer::Data(info[0]) to info[0].Data()
  [ /node::Buffer::Data\((.+?)\)/g, '$1.As<Napi::Buffer<char>>().Data()' ],
  [ /Nan::CopyBuffer\(/g, 'Napi::Buffer::Copy(env, ' ],

  // Nan::AsyncQueueWorker(worker)
  [ /Nan::AsyncQueueWorker\((.+)\);/g, '$1.Queue();' ],
  [ /Nan::(Undefined|Null|True|False)\(\)/g, 'env.$1()' ],

  // Nan::ThrowError(error) to Napi::Error::New(env, error).ThrowAsJavaScriptException()
  [ /return Nan::Throw(\w*?)Error\((.+?)\);/g, 'Napi::$1Error::New(env, $2).ThrowAsJavaScriptException();\n  return env.Null();' ],
  [ /Nan::Throw(\w*?)Error\((.+?)\);/g, 'Napi::$1Error::New(env, $2).ThrowAsJavaScriptException();\n  return env.Null();' ],
  // Nan::RangeError(error) to Napi::RangeError::New(env, error)
  [ /Nan::(\w*?)Error\((.+)\)/g, 'Napi::$1Error::New(env, $2)' ],

  [ /Nan::Set\((.+?),\n* *(.+?),\n* *(.+?),\n* *(.+?)\)/g, '$1.Set($2, $3, $4)' ],

  [ /Nan::(Escapable)?HandleScope\s+(\w+)\s*;/g, 'Napi::$1HandleScope $2(env);' ],
  [ /Nan::(Escapable)?HandleScope/g, 'Napi::$1HandleScope' ],
  [ /Nan::ForceSet\(([^,]+), ?/g, '$1->DefineProperty(' ],
  [ /\.ForceSet\(Napi::String::New\(env, "(\w+)"\),\s*?/g, '.DefineProperty("$1", ' ],
  // [ /Nan::GetPropertyNames\(([^,]+)\)/, '$1->GetPropertyNames()' ],
  [ /Nan::Equals\(([^,]+),/g, '$1.StrictEquals(' ],



  [ /Nan::Callback/g, 'Napi::FunctionReference' ],


  [ /Nan::Persistent<Object>/g, 'Napi::ObjectReference' ],
  [ /Nan::ADDON_REGISTER_FUNCTION_ARGS_TYPE target/g, 'Napi::Env& env, Napi::Object& target' ],

  [ /(\w+)\*\s+(\w+)\s*=\s*Nan::ObjectWrap::Unwrap<\w+>\(info\.This\(\)\);/g, '$1* $2 = this;' ],
  [ /Nan::ObjectWrap::Unwrap<(\w+)>\((.*)\);/g, '$2.Unwrap<$1>();' ],

  [ /Nan::NAN_METHOD_ARGS_TYPE/g, 'const Napi::CallbackInfo&' ],
  [ /NAN_METHOD\(([\w\d:]+?)\)/g, 'Napi::Value $1(const Napi::CallbackInfo& info)'],
  [ /static\s*NAN_GETTER\(([\w\d:]+?)\)/g, 'Napi::Value $1(const Napi::CallbackInfo& info)' ],
  [ /NAN_GETTER\(([\w\d:]+?)\)/g, 'Napi::Value $1(const Napi::CallbackInfo& info)' ],
  [ /static\s*NAN_SETTER\(([\w\d:]+?)\)/g, 'void $1(const Napi::CallbackInfo& info, const Napi::Value& value)' ],
  [ /NAN_SETTER\(([\w\d:]+?)\)/g, 'void $1(const Napi::CallbackInfo& info, const Napi::Value& value)' ],
  [ /void Init\((v8::)*Local<(v8::)*Object> exports\)/g, 'void Init(Napi::Env env, Napi::Object exports, Napi::Object module)' ],
  [ /NAN_MODULE_INIT\(([\w\d:]+?)\);/g, 'void $1(Napi::Env env, Napi::Object exports, Napi::Object module);' ],
  [ /NAN_MODULE_INIT\(([\w\d:]+?)\)/g, 'void $1(Napi::Env env, Napi::Object exports, Napi::Object module)' ],

  [ /::(Init(?:ialize)?)\(target\)/g, '::$1(env, target, module)' ],
  [ /constructor_template/g, 'constructor' ],

  [ /Nan::FunctionCallbackInfo<(v8::)*Value>\s*&\s*info\)\s*{/g, 'Napi::CallbackInfo& info) {\n  Napi::Env env = info.Env();' ],
  [ /Nan::FunctionCallbackInfo<(v8::)*Value>\s*&\s*info\);/g, 'Napi::CallbackInfo& info);' ],

  [ /info\[(\d+)\]->/g, 'info[$1].' ],
  [ /info\[([\w\d]+)\]->/g, 'info[$1].' ],
  [ /info\.This\(\)->/g, 'info.This().' ],
  [ /->Is(Object|String|Int32|Number)\(\)/g, '.Is$1()' ],
  [ /info.GetReturnValue\(\).SetUndefined\(\)/g, 'return env.Undefined()' ],
  [ /info\.GetReturnValue\(\)\.Set\(((\n|.)+?)\);/g, 'return $1;' ],


  // ex. Local<Value> to Napi::Value
  [ /v8::Local<v8::(Value|Boolean|String|Number|Object|Array|Symbol|External|Function)>/g, 'Napi::$1' ],
  [ /Local<(Value|Boolean|String|Number|Object|Array|Symbol|External|Function)>/g, 'Napi::$1' ],

  // Declare an env in helper functions that take a Napi::Value
  [ /(\w+)\(Napi::Value (\w+)(,\s*[^\()]+)?\)\s*{/g, '$1(Napi::Value $2$3) {\n  Napi::Env env = $2.Env();' ],

  // delete #include <node.h> and/or <v8.h>
  [ /#include +(<|")(?:node|nan).h("|>)/g, "#include $1napi.h$2\n#include $1uv.h$2" ],
  // NODE_MODULE to NODE_API_MODULE
  [ /NODE_MODULE/g, 'NODE_API_MODULE' ],
  [ /Nan::/g, 'Napi::' ],
  [ /nan.h/g, 'napi.h' ],

  // delete .FromJust()
  [ /\.FromJust\(\)/g, '' ],
  // delete .ToLocalCheck()
  [ /\.ToLocalChecked\(\)/g, '' ],
  [ /^.*->SetInternalFieldCount\(.*$/gm, '' ],

  // replace using node; and/or using v8; to using Napi;
  [ /using (node|v8);/g, 'using Napi;' ],
  [ /using namespace (node|Nan|v8);/g, 'using namespace Napi;' ],
  // delete using v8::Local;
  [ /using v8::Local;\n/g, '' ],
  // replace using v8::XXX; with using Napi::XXX
  [ /using v8::([A-Za-z]+);/g, 'using Napi::$1;' ],

];

var paths = listFiles(dir);
paths.forEach(function(dirEntry) {
  var filename = dirEntry.split('\\').pop().split('/').pop();

  // Check whether the file is a source file or a config file
  // then execute function accordingly
  var sourcePattern = /.+\.h|.+\.cc|.+\.cpp/;
  if (sourcePattern.test(filename)) {
    convertFile(dirEntry, SourceFileOperations);
  } else if (ConfigFileOperations[filename] != null) {
    convertFile(dirEntry, ConfigFileOperations[filename]);
  }
});

function listFiles(dir, filelist) {
  var files = fs.readdirSync(dir);
  filelist = filelist || [];
  files.forEach(function(file) {
    if (file === 'node_modules') {
      return
    }

    if (fs.statSync(path.join(dir, file)).isDirectory()) {
      filelist = listFiles(path.join(dir, file), filelist);
    } else {
      filelist.push(path.join(dir, file));
    }
  });
  return filelist;
}

function convert(content, operations) {
  for (let i = 0; i < operations.length; i ++) {
    let operation = operations[i];
    content = content.replace(operation[0], operation[1]);
  }
  return content;
}

function convertFile(fileName, operations) {
  fs.readFile(fileName, "utf-8", function (err, file) {
    if (err) throw err;

    file = convert(file, operations);

    fs.writeFile(fileName, file, function(err){
      if (err) throw err;
    });
  });
}
