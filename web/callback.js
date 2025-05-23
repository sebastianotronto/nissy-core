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
  if (_validateCallbackId(id))
    _cbfl[id](UTF8ToString(arg));
},

callFunctionInt__deps: [ 'cbfl', 'validateCallbackId' ],
callFunctionInt: function(id) {
  if (_validateCallbackId(id))
    return _cbfl[id]();
  return 0;
},

});
