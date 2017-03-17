const include = __dirname;
const isNodeApiBuiltin = process.versions.modules >= 52;

module.exports = {
   include: include,
   isNodeApiBuiltin: isNodeApiBuiltin,
};
