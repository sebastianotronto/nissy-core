addToLibrary({

cbfl: [],

addCallbackFunction__deps: [ 'cbfl' ],
addCallbackFunction: function(f) {
  _cbfl.push(f)
  return _cbfl.length - 1
},

callFunction__deps: [ 'cbfl' ],
callFunction: function(id, arg) {
  _cbfl[id](UTF8ToString(arg))
},

callFunctionInt__deps: [ 'cbfl' ],
callFunctionInt: function(id) {
  return _cbfl[id]()
},

});
