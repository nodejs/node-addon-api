module.exports.generateFileContent = function(configs) {
  const content = [];
  const inits = [];
  const exports = [];

  for (let config of configs) {
    inits.push(`Object Init${config.objectName}(Env env);`);
    exports.push(`exports.Set(\"${config.propertyName}\", Init${config.objectName}(env));`);
  }

  content.push("#include \"napi.h\"");
  content.push("using namespace Napi;");

  //content.push("Object InitName(Env env);");
  inits.forEach(init => content.push(init));

  content.push("Object Init(Env env, Object exports) {");

  //content.push("exports.Set(\"name\", InitName(env));");
  exports.forEach(exp => content.push(exp));

  content.push("return exports;");
  content.push("}");
  content.push("NODE_API_MODULE(addon, Init);");

  return Promise.resolve(content.join('\r\n'));
}
