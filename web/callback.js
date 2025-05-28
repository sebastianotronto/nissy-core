addToLibrary({

cbfl: [],

validateCallbackId__deps: [ 'cbfl' ],
validateCallbackId: function(i) {
  if (i < 0) {
    console.log("--- WARNING ---");
    console.log("Trying to access callback function of invalid id " + i);
    console.log("--- WARNING ---");
    return false;
  }

  if (i >= _cbfl.length) {
    console.log("--- WARNING ---");
    console.log("Trying to access callback function " + i + ", but only "
      + _cbfl.length + " have been registered. This may be caused by a "
      + "call outside of the main thread.");
    console.log("--- WARNING ---");
    return false;
  }

  return true;
},

addCallbackFunction__deps: [ 'cbfl' ],
addCallbackFunction: function(f) {
  _cbfl.push(f)
  return _cbfl.length - 1
},

callFunction__deps: [ 'cbfl', 'validateCallbackId' ],
callFunction: function(id, arg) {
  // This is a workaround related to usign WASM64
  // JavaScript's UTF8ToString expects a pointer argument, which for JS is
  // of type "number", but WASM64 is passing a BigInt. See also:
  // https://github.com/emscripten-core/emscripten/issues/21541
  // (but I could not make the suggested solution work in this case).
  // TODO: check if there is a better workaround.
  const non64_arg = Number(arg);
  if (_validateCallbackId(id))
    _cbfl[id](UTF8ToString(non64_arg));
},

callFunctionInt__deps: [ 'cbfl', 'validateCallbackId' ],
callFunctionInt: function(id) {
  if (_validateCallbackId(id))
    return _cbfl[id]();
  return 0;
},

});
