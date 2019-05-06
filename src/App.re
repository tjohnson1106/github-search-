let component = ReasonReact.statelessComponent("App");

let make = _children => {
  ...component,
  render: _self =>
    <div>
      <Card name="reason" href="" description="reason github app" />
    </div>,
};