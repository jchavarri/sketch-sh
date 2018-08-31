let codeFormat:
  (. string, list((string, string))) =>
  list((string, Belt.Result.t(string, Worker_Evaluator.Types.Refmt.error)));
