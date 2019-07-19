[@decco]
type t = {
  loc_start: Position.t,
  loc_end: Position.t,
};

let dummy_pos = {Position.line: 0, col: 0};
let dummy_loc = {loc_start: dummy_pos, loc_end: dummy_pos};

let start_line = loc => loc.loc_start.Position.line;
let start_col = loc => loc.loc_start.Position.col;
let end_line = loc => loc.loc_end.Position.line;
let end_col = loc => loc.loc_end.Position.col;
