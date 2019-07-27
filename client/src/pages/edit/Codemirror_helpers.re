module Error = Engine_native.Types.Error;

open! CodeMirror;

let make_loc = err => {
  let loc = err.Error.loc->Loc.erase_option;

  let from_pos =
    Position.make(
      ~line=Loc.start_line(loc),
      ~ch=max(0, Loc.start_col(loc) - 1),
      (),
    );
  let to_pos =
    Position.make(~line=Loc.end_line(loc), ~ch=Loc.end_col(loc), ());

  (from_pos, to_pos);
};

let clean_error_message = msg => {
  let split_point = msg |> Js.String.indexOf("Error:");

  msg |> Js.String.sliceToEnd(~from=split_point);
};

let make_error_marker = err => {
  let (from, to_) = make_loc(err);
  let message = err.Error.message->clean_error_message;

  LintOptions.annotation(~from, ~to_, ~message, ~severity="error");
};

let make_warning_marker = err => {
  let (from, to_) = make_loc(err);
  LintOptions.annotation(
    ~from,
    ~to_,
    ~message=err.Error.message,
    ~severity="warning",
  );
};
