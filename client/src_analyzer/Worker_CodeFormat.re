open Belt.Result;

module Evaluator = Worker_Evaluator.Make(Worker_BrowserEvaluator);

let reToMl = code =>
  switch (Evaluator.parseRE(code)) {
  | Ok(ast) => Ok(Evaluator.printML(ast))
  | Error(error) => Error(error)
  };

let mlToRe = code =>
  switch (Evaluator.parseML(code)) {
  | Ok(ast) => Ok(Evaluator.printRE(ast))
  | Error(error) => Error(error)
  };
let reToRe = code =>
  switch (Evaluator.parseRE(code)) {
  | Ok(ast) => Ok(Evaluator.printRE(ast))
  | Error(error) => Error(error)
  };

let operationToFun =
  fun
  | "reToMl" => reToMl
  | "mlToRe" => mlToRe
  | "reToRe" => reToRe
  | _ => failwith("Unknown code format operation");

let codeFormat:
  (. string, list((string, string))) =>
  list((string, Belt.Result.t(string, Evaluator.Refmt.error))) =
  (. operation, codeBlocks) => {
    let formatter = operation->operationToFun;
    let rec loop = (blocks, acc) =>
      switch (blocks) {
      | [] => acc
      | [(id, code), ...rest] =>
        let result = code->formatter;
        loop(rest, [(id, result), ...acc]);
      };
    loop(codeBlocks, []);
  };
