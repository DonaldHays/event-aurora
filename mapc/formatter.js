"use strict";

exports.toHex = function(number, length = 2) {
  var string = number.toString(16).toUpperCase();
  while(string.length < length) {
    string = "0" + string;
  }
  
  return string;
}

exports.toBinary = function(number) {
  var string = number.toString(2);
  while(string.length < 8) {
    string = "0" + string;
  }
  return string;
}
