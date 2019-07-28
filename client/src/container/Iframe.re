let bodyFromIframe: Dom.element => Dom.element = [%raw
  iframe => {|return iframe.contentWindow.document.body|}
];

[@react.component]
let make = (~children) => {
  let (contentRef, setContentRef) = React.useState(() => Js.Nullable.null);
  let mountNode =
    switch (contentRef->Js.Nullable.toOption) {
    | None => None
    | Some(ref) => Some(bodyFromIframe(ref))
    };
  <iframe ref={ReactDOMRe.Ref.callbackDomRef(r => setContentRef(_ => r))}>
    {switch (mountNode) {
     | Some(el) => ReactDOMRe.createPortal(React.Children.only(children), el)
     | None => React.null
     }}
  </iframe>;
};

let make = React.memo(make);
