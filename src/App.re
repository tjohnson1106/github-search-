type state = {
  input: string,
  isLoading: bool,
};

type action =
  | UpdateInput(string)
  | Search;

let component = ReasonReact.reducerComponent("App");

let make = _children => {
  ...component,
  initialState: () => {input: "", isLoading: false},
  reducer: (action, state) =>
    switch (action) {
    | UpdateInput(newInput) =>
      ReasonReact.Update({...state, isLoading: true})
    },
  render: _self =>
    <div>
      <Card name="reason" href="" description="reason github app" />
    </div>,
};