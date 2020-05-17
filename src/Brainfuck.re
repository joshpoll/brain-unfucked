type instr =
  /* > */
  | IncP
  /* < */
  | DecP
  /* + */
  | Inc
  /* - */
  | Dec
  /* . */
  | Out
  /* , */
  | In
  /* [ */
  | JmpFwdZ
  /* ] */
  | JmpBwdNZ;

/* TODO: */
type byte = int;

type config = {
  dataPtr: int,
  instrPtr: int,
  data: array(byte),
  instrs: array(instr),
  input: list(byte),
  output: list(byte),
};

type program = {
  numCells: int,
  instrs: array(instr),
  input: list(byte),
};

type value = {
  data: array(byte),
  output: list(byte),
};

let step = ({dataPtr, instrPtr, data, instrs, input, output} as c: config): option(config) =>
  if (instrPtr >= Array.length(instrs)) {
    None;
  } else {
    switch (instrs[instrPtr]) {
    | IncP => Some({...c, dataPtr: dataPtr + 1, instrPtr: instrPtr + 1})
    | DecP => Some({...c, dataPtr: dataPtr - 1, instrPtr: instrPtr + 1})
    | Inc =>
      let newData = Array.copy(data);
      newData[dataPtr] = newData[dataPtr] + 1;
      Some({...c, data: newData, instrPtr: instrPtr + 1});
    | Dec =>
      let newData = Array.copy(data);
      newData[dataPtr] = newData[dataPtr] - 1;
      Some({...c, data: newData, instrPtr: instrPtr - 1});
    | Out => Some({...c, output: [data[dataPtr], ...output], instrPtr: instrPtr + 1})
    | In =>
      let newData = Array.copy(data);
      switch (input) {
      | [] => None
      | [i, ...input] =>
        newData[dataPtr] = i;
        Some({...c, data: newData, input, instrPtr: instrPtr + 1});
      };
    | JmpFwdZ =>
      if (data[dataPtr] == 0) {
        /* jump forward to instr after next ] */
        let ptr = ref(instrPtr);
        while (ptr^ < Array.length(instrs) && instrs[ptr^] != JmpBwdNZ) {
          ptr := ptr^ + 1;
        };
        ptr := ptr^ + 1;
        if (ptr^ >= Array.length(instrs)) {
          None;
        } else {
          Some({...c, instrPtr: ptr^});
        };
      } else {
        Some({...c, instrPtr: instrPtr + 1});
      }
    | JmpBwdNZ =>
      if (data[dataPtr] != 0) {
        /* jump backward to instr after prev [ */
        let ptr = ref(instrPtr);
        while (ptr^ >= 0 && instrs[ptr^] != JmpFwdZ) {
          ptr := ptr^ - 1;
        };
        if (instrs[ptr^] != JmpFwdZ) {
          None;
        } else {
          ptr := ptr^ + 1;
          Some({...c, instrPtr: ptr^});
        };
      } else {
        Some({...c, instrPtr: instrPtr + 1});
      }
    };
  };

let inject = ({numCells, instrs, input}: program): config => {
  dataPtr: 0,
  instrPtr: 0,
  data: Array.make(numCells, 0),
  instrs,
  input,
  output: [],
};

/* program is always terminated with None when we exceed instruction list */
let isFinal = (c: config): bool => false;

let rec iterateMaybeAux = (f, x) =>
  switch (x) {
  | None => []
  | Some(x) =>
    let fx = f(x);
    [x, ...iterateMaybeAux(f, fx)];
  };

let advance = step;

let rec takeWhileInclusive = (p, l) =>
  switch (l) {
  | [] => []
  | [x, ...xs] => [
      x,
      ...if (p(x)) {
           takeWhileInclusive(p, xs);
         } else {
           [];
         },
    ]
  };

let iterateMaybe = (f: 'a => option('a), x: 'a): list('a) => iterateMaybeAux(f, Some(x));

let rec iterateMaybeSideEffect = (f: 'a => option(('a, 'b)), x: 'a): (list('a), list('b)) =>
  switch (f(x)) {
  | None => ([x], [])
  | Some((a, b)) =>
    let (als, bls) = iterateMaybeSideEffect(f, a);
    ([x, ...als], [b, ...bls]);
  };

let interpretTrace = (p: program): list(config) => {
  let states = iterateMaybe(step, inject(p));
  // Js.log2("rules", rules |> Array.of_list);
  // let (actualRules, flow) = rules |> List.split;
  // Js.log2("rules", List.combine(actualRules, List.map(MS.toArray, flow)) |> Array.of_list);
  // List.combine(rules @ [("", MS.empty)], );
  takeWhileInclusive(c => !isFinal(c), states);
};

let interpret = (p: program): value =>
  switch (interpretTrace(p) |> List.rev |> List.hd) {
  | {data, output} => {data, output}
  };