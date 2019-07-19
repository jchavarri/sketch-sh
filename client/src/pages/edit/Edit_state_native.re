open Belt.Result;
open Location;

type parse_success = {
  .
  "id": int,
  "loc": loc,
};

type parse_error = {
  .
  "message": string,
  "loc": loc,
};

type parse_response = Belt.Result.t(array(parse_success), parse_error);

[@bs.val] external parse: string => array('a) = "parse";

let parse: string => parse_response =
  code => {
    let raw_response = parse(code);
    Js.log(raw_response);
    switch (Array.unsafe_get(raw_response, 0)) {
    | "Ok" => raw_response->Array.unsafe_get(1)->Obj.magic->Ok
    | "Error" => raw_response->Array.unsafe_get(1)->Obj.magic->Error
    | unknown_constructor =>
      Error({
        "loc": blank_loc,
        "message": {j|Unknown constructor $unknown_constructor|j},
      })
    };
  };

/*
 * |--- executed ---|--- parsed_and_executable ---|--- unparsable/new code ---|
 *                  ^ last_executed_line
 */
type line_range = (int, int);
type gutter_status = {
  executed: option(line_range),
  parsed_and_executable: option(line_range),
};

let derive_execution_status =
    (arr_parse: array(parse_success), last_executed_line) => {
  let executed = last_executed_line->Belt.Option.map(line => (0, line));

  switch (arr_parse) {
  | [||] => {executed, parsed_and_executable: None}
  | arr_parse =>
    let last_phr = arr_parse[0];
    let last_parsed_line = last_phr##loc##loc_end##line;
    {
      executed,
      parsed_and_executable:
        Some((
          last_executed_line
          ->Belt.Option.map(line => line + 1)
          ->Belt.Option.getWithDefault(0),
          last_parsed_line,
        )),
    };
  };
};

module Execute = Engine_native.Types.Execute;

type state = {
  last_executed_line: option(int),
  parse_error: option(parse_error),
  parse_success: array(parse_success),
  exec_msg: Belt.Map.Int.t(Execute.message),
};

type action =
  | Out_parse(string)
  | In_parse(parse_response)
  | Editor_changed(array(loc))
  | Add_exec_messages(Execute.message)
  | State_reset
  | Execute(int);

let reducer = (action, state) => {
  ReactUpdate.(
    switch (action) {
    | Out_parse(code) =>
      SideEffects(
        ({send}) => {
          send(In_parse(parse(code)));
          None;
        },
      )
    | In_parse(parse_info) =>
      switch (parse_info) {
      | Error(parse_error) =>
        let err_start_line = parse_error##loc##loc_start##line;

        Update({
          ...state,
          parse_error: Some(parse_error),
          parse_success:
            state.parse_success
            ->Belt.Array.keep(phr => err_start_line > phr##loc##loc_end##line),
        });
      | Ok(parse_success) =>
        Update({...state, parse_error: None, parse_success})
      }
    | Editor_changed(loc) =>
      // TODO: handle multiple loc
      let loc = loc[0];
      let changed_start_line = loc##loc_start##line;
      let should_reset =
        switch (state.last_executed_line) {
        | None => false
        | Some(last_executed_line) => changed_start_line <= last_executed_line
        };
      if (should_reset) {
        SideEffects(
          ({send}) => {
            send(State_reset);
            None;
          },
        );
      } else {
        NoUpdate;
      };
    | State_reset => failwith("Unimplemented")
    | Add_exec_messages(message) =>
      Update({
        ...state,
        exec_msg:
          state.exec_msg->Belt.Map.Int.set(message.Execute.exec_id, message),
      })
    | Execute(to_id) =>
      SideEffects(
        ({send}) => {
          let complete = _ => ();
          let js_send = (
            fun
            | Ok(message) => send(Add_exec_messages(message))
            | Error(decco_error) =>
              Js.log2(
                "Ooops! Error while decoding value from worker, looks like types is out of sync",
                decco_error,
              )
          );

          Engine_native.execute(
            ~send={
              js_send;
            },
            ~complete,
            to_id,
          );
          None;
        },
      )
    }
  );
};
