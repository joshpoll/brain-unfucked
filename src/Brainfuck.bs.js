'use strict';

var List = require("bs-platform/lib/js/list.js");
var $$Array = require("bs-platform/lib/js/array.js");
var Curry = require("bs-platform/lib/js/curry.js");
var Caml_array = require("bs-platform/lib/js/caml_array.js");
var Caml_option = require("bs-platform/lib/js/caml_option.js");

function step(c) {
  var instrs = c.instrs;
  var instrPtr = c.instrPtr;
  if (instrPtr >= instrs.length) {
    return ;
  }
  var input = c.input;
  var data = c.data;
  var dataPtr = c.dataPtr;
  var match = Caml_array.caml_array_get(instrs, instrPtr);
  switch (match) {
    case /* IncP */0 :
        return {
                dataPtr: dataPtr + 1 | 0,
                instrPtr: instrPtr + 1 | 0,
                data: c.data,
                instrs: c.instrs,
                input: c.input,
                output: c.output
              };
    case /* DecP */1 :
        return {
                dataPtr: dataPtr - 1 | 0,
                instrPtr: instrPtr + 1 | 0,
                data: c.data,
                instrs: c.instrs,
                input: c.input,
                output: c.output
              };
    case /* Inc */2 :
        var newData = $$Array.copy(data);
        Caml_array.caml_array_set(newData, dataPtr, Caml_array.caml_array_get(newData, dataPtr) + 1 | 0);
        return {
                dataPtr: c.dataPtr,
                instrPtr: instrPtr + 1 | 0,
                data: newData,
                instrs: c.instrs,
                input: c.input,
                output: c.output
              };
    case /* Dec */3 :
        var newData$1 = $$Array.copy(data);
        Caml_array.caml_array_set(newData$1, dataPtr, Caml_array.caml_array_get(newData$1, dataPtr) - 1 | 0);
        return {
                dataPtr: c.dataPtr,
                instrPtr: instrPtr - 1 | 0,
                data: newData$1,
                instrs: c.instrs,
                input: c.input,
                output: c.output
              };
    case /* Out */4 :
        return {
                dataPtr: c.dataPtr,
                instrPtr: instrPtr + 1 | 0,
                data: c.data,
                instrs: c.instrs,
                input: c.input,
                output: /* :: */[
                  Caml_array.caml_array_get(data, dataPtr),
                  c.output
                ]
              };
    case /* In */5 :
        var newData$2 = $$Array.copy(data);
        if (input) {
          Caml_array.caml_array_set(newData$2, dataPtr, input[0]);
          return {
                  dataPtr: c.dataPtr,
                  instrPtr: instrPtr + 1 | 0,
                  data: newData$2,
                  instrs: c.instrs,
                  input: input[1],
                  output: c.output
                };
        } else {
          return ;
        }
    case /* JmpFwdZ */6 :
        if (Caml_array.caml_array_get(data, dataPtr) !== 0) {
          return {
                  dataPtr: c.dataPtr,
                  instrPtr: instrPtr + 1 | 0,
                  data: c.data,
                  instrs: c.instrs,
                  input: c.input,
                  output: c.output
                };
        }
        var ptr = instrPtr;
        while(ptr < instrs.length && Caml_array.caml_array_get(instrs, ptr) !== /* JmpBwdNZ */7) {
          ptr = ptr + 1 | 0;
        };
        ptr = ptr + 1 | 0;
        if (ptr >= instrs.length) {
          return ;
        } else {
          return {
                  dataPtr: c.dataPtr,
                  instrPtr: ptr,
                  data: c.data,
                  instrs: c.instrs,
                  input: c.input,
                  output: c.output
                };
        }
    case /* JmpBwdNZ */7 :
        if (Caml_array.caml_array_get(data, dataPtr) === 0) {
          return {
                  dataPtr: c.dataPtr,
                  instrPtr: instrPtr + 1 | 0,
                  data: c.data,
                  instrs: c.instrs,
                  input: c.input,
                  output: c.output
                };
        }
        var ptr$1 = instrPtr;
        while(ptr$1 >= 0 && Caml_array.caml_array_get(instrs, ptr$1) !== /* JmpFwdZ */6) {
          ptr$1 = ptr$1 - 1 | 0;
        };
        if (Caml_array.caml_array_get(instrs, ptr$1) !== /* JmpFwdZ */6) {
          return ;
        }
        ptr$1 = ptr$1 + 1 | 0;
        return {
                dataPtr: c.dataPtr,
                instrPtr: ptr$1,
                data: c.data,
                instrs: c.instrs,
                input: c.input,
                output: c.output
              };
    
  }
}

function inject(param) {
  return {
          dataPtr: 0,
          instrPtr: 0,
          data: Caml_array.caml_make_vect(param.numCells, 0),
          instrs: param.instrs,
          input: param.input,
          output: /* [] */0
        };
}

function isFinal(c) {
  return false;
}

function iterateMaybeAux(f, x) {
  if (x === undefined) {
    return /* [] */0;
  }
  var x$1 = Caml_option.valFromOption(x);
  var fx = Curry._1(f, x$1);
  return /* :: */[
          x$1,
          iterateMaybeAux(f, fx)
        ];
}

function takeWhileInclusive(p, l) {
  if (!l) {
    return /* [] */0;
  }
  var x = l[0];
  return /* :: */[
          x,
          Curry._1(p, x) ? takeWhileInclusive(p, l[1]) : /* [] */0
        ];
}

function iterateMaybe(f, x) {
  return iterateMaybeAux(f, Caml_option.some(x));
}

function iterateMaybeSideEffect(f, x) {
  var match = Curry._1(f, x);
  if (match === undefined) {
    return /* tuple */[
            /* :: */[
              x,
              /* [] */0
            ],
            /* [] */0
          ];
  }
  var match$1 = iterateMaybeSideEffect(f, match[0]);
  return /* tuple */[
          /* :: */[
            x,
            match$1[0]
          ],
          /* :: */[
            match[1],
            match$1[1]
          ]
        ];
}

function interpretTrace(p) {
  var x = inject(p);
  var states = iterateMaybeAux(step, Caml_option.some(x));
  return takeWhileInclusive((function (c) {
                return true;
              }), states);
}

function interpret(p) {
  var match = List.hd(List.rev(interpretTrace(p)));
  return {
          data: match.data,
          output: match.output
        };
}

var advance = step;

exports.step = step;
exports.inject = inject;
exports.isFinal = isFinal;
exports.iterateMaybeAux = iterateMaybeAux;
exports.advance = advance;
exports.takeWhileInclusive = takeWhileInclusive;
exports.iterateMaybe = iterateMaybe;
exports.iterateMaybeSideEffect = iterateMaybeSideEffect;
exports.interpretTrace = interpretTrace;
exports.interpret = interpret;
/* No side effect */
