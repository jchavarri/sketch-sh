type versionRanges = Js.Dict.t(string);

module Fetcher = {
  type meta = {
    .
    "requiredById": Js.Nullable.t(string),
    "originalRequest": Js.Nullable.t(string),
  };

  type url = string;

  type dependencies =
    | Umd
    | Array(array(string));

  module FetchResult: {
    type t;

    let make:
      (
        ~id: string,
        ~url: string,
        ~dependencies: dependencies=?,
        ~dependencyVersionRanges: versionRanges=?,
        ~css: string=?,
        unit
      ) =>
      t;
  } = {
    type js_dependencies;

    [@bs.deriving abstract]
    type t = {
      id: string,
      url: string,
      [@bs.optional]
      dependencies: js_dependencies,
      [@bs.optional]
      dependencyVersionRanges: versionRanges,
      [@bs.optional]
      css: string,
    };
    external unsafe_make_js_deps: 'a => js_dependencies = "%identity";
    let make =
        (~id, ~url, ~dependencies=?, ~dependencyVersionRanges=?, ~css=?, ()) => {
      t(
        ~id,
        ~url,
        ~dependencies=?
          dependencies->Belt.Option.map(
            fun
            | Umd => unsafe_make_js_deps("umd")
            | Array(array) => unsafe_make_js_deps(array),
          ),
        ~dependencyVersionRanges?,
        ~css?,
        (),
      );
    };
  };
};

type polestar;

type resolver;
type polestar_opts('globals, 'moduleThis) = {
  .
  "globals": Js.t('globals),
  "moduleThis": 'moduleThis,
  "fetcher":
    [@bs.meth] (
      (string, Fetcher.meta) => Js.Promise.t(Fetcher.FetchResult.t)
    ),
  "resolver": resolver,
  "onEntry": [@bs.meth] (unit => unit),
  "onError": [@bs.meth] (exn => unit),
};

[@bs.module "polestar"] [@bs.new]
external make: polestar_opts('a, 'b) => polestar = "Polestar";

[@bs.module "polestar"] external defaultResolver: resolver = "DefaultResolver";

module Sketch_polestar = {
  let globals = {
    "process": {
      "env": Js.Obj.empty,
    },
  };

  type protocol =
    | Npm
    | Vfs
    | Annonymous;
  // let parse
  let fetcher = (url, meta) => {};
};
