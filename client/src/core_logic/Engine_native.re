type result('a, 'b) = Belt.Result.t('a, 'b) = | Ok('a) | Error('b);

// let result_encode = content => {
//   switch (Array.unsafe_get(content, 0)) {
//   | "Ok" => content->Array.unsafe_get(1)->Obj.magic->Ok
//   | "Error" => content->Array.unsafe_get(1)->Obj.magic->Error
//   | unknown_constructor =>
//     failwith({j|Unknown constructor $unknown_constructor|j})
//   };
// };

let result_encode = _ => {
  failwith("Unimplemented");
};
let result_decode = _ => {
  failwith("Unimplemented");
};

module Types = {
  module Error = {
    [@decco]
    type t = {
      loc: option(Loc.t),
      message: string,
    };
  };

  module Warning = {
    [@decco]
    type t =
      Error.t = {
        loc: option(Loc.t),
        message: string,
      };
  };

  module Parse = {
    [@decco]
    type id = int;
    [@decco]
    type parse_result = {
      id,
      loc: option(Loc.t),
    };

    [@decco]
    type message = result(list(parse_result), Error.t);
  };

  module Execute = {
    [@decco]
    type complete =
      | Exec_complete_success
      | Exec_complete_error;

    [@decco]
    type phr_kind =
      | Directive
      | Code;

    /*
     * Toploop.execute_phrase has 2 ways of expressing errors
     * 1. Return false as the result of `Toploop.execute_phrase`
     *    - Ophr_exception (no ideas)
     *    - Incorrect directive names / directive argument types
     * 2. Throwing exception
     */
    [@decco]
    type exec_content =
      | Exec_phr_true(string)
      | Exec_phr_false(string)
      | Exec_phr_exn(Error.t);

    [@decco]
    type message = {
      exec_id: Parse.id,
      exec_loc: option(Loc.t),
      exec_phr_kind: phr_kind,
      exec_content,
      exec_warning: list(Warning.t),
      exec_stdout: string,
    };
  };

  module Syntax = {
    [@decco]
    type t =
      | RE
      | ML;
  };
};

type js_callback = Js.Json.t => unit;

[@bs.val]
external execute: (. js_callback, js_callback, int) => unit = "execute";

let execute = (~send, ~complete, id) => {
  let send = json => {
    json->Types.Execute.message_decode->send;
  };
  let complete = json => {
    json->Types.Execute.complete_decode->complete;
  };

  execute(. send, complete, id);
};
