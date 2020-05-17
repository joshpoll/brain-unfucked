// Entry point

[@bs.val] external document: Js.t({..}) = "document";

// We're using raw DOM manipulations here, to avoid making you read
// ReasonReact when you might precisely be trying to learn it for the first
// time through the examples later.
let style = document##createElement("style");
document##head##appendChild(style);
style##innerHTML #= ExampleStyles.style;

let makeContainer = text => {
  let container = document##createElement("div");
  container##className #= "container";

  let title = document##createElement("div");
  title##className #= "containerTitle";
  title##innerText #= text;

  let content = document##createElement("div");
  content##className #= "containerContent";

  let () = container##appendChild(title);
  let () = container##appendChild(content);
  let () = document##body##appendChild(container);

  content;
};

// All 4 examples.
ReactDOMRe.render(
  <BlinkingGreeting> {React.string("Hello!")} </BlinkingGreeting>,
  makeContainer("Blinking Greeting"),
);

ReactDOMRe.render(<ReducerFromReactJSDocs />, makeContainer("Reducer From ReactJS Docs"));

ReactDOMRe.render(<FetchedDogPictures />, makeContainer("Fetched Dog Pictures"));

ReactDOMRe.render(<ReasonUsingJSUsingReason />, makeContainer("Reason Using JS Using Reason"));

Js.log2(
  "interpret ,. on 5",
  Brainfuck.interpret({numCells: 1, instrs: [|In, Out|], input: [5]}),
);
Js.log2(
  "interpretTrace ,. on 5",
  Brainfuck.interpretTrace({numCells: 1, instrs: [|In, Out|], input: [5]}) |> Array.of_list,
);

Js.log2(
  "interpret ,>,[-<+>]<. on 5 10",
  Brainfuck.interpret({
    numCells: 2,
    instrs: [|In, IncP, In, JmpFwdZ, Dec, DecP, Inc, IncP, JmpBwdNZ, DecP, Out|],
    input: [5, 10],
  }),
);

Js.log2(
  "interpretTrace ,>,[-<+>]<. on 5 10",
  Brainfuck.interpretTrace({
    numCells: 2,
    instrs: [|In, IncP, In, JmpFwdZ, Dec, DecP, Inc, IncP, JmpBwdNZ, DecP, Out|],
    input: [5, 10],
  }),
);