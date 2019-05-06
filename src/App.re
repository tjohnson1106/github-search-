type state = {
  input: string,
  isLoading: bool,
  results: list(repository),
};

type action =
  | UpdateInput(string)
  | UpdateResults(list(repository))
  | Search;

type repository = {
  name: string,
  description: string,
  href: string,
};

let component = ReasonReact.reducerComponent("App");

module Api = {
  open Json.Decode;

  let decodeResults =
    field(
      "items",
      list(
        optional(json =>
          {
            name: field("name", string, json),
            description: field("description", string, json),
            href: field("html_url", string, json),
          }
        ),
      ),
    );

  let getResults = query =>
       Js.Promise.(
      Fetch.fetch("https://api.github.com/search/repositories?q=" ++ query)
      |> then_(Fetch.Response.json)
      |> then_(json => decodeResults(json) |> resolve)
      |> then_(results =>
           results
           |> List.filter(optionalItem =>
                switch (optionalItem) {
                | Some(_) => true
                | None => false
                }
              )
            /* Turn our items out of option types into a regular record */
           |> List.map(item =>
                switch (item) {
                | Some(item) => item
                }
              )
           |> resolve
         )
    );


let make = _children => {
  ...component,
  initialState: () => {input: "", isLoading: false, results: []},
  reducer: (action, state) =>
    switch (action) {
    | UpdateInput(input) => ReasonReact.Update({...state, input})
    | UpdateResults(results) => ReasonReact.Update({
      ...state,
      isLoading: false,
      results
    })
    | Search =>
      ReasonReact.UpdateWithSideEffects(
        {...state, isLoading: true},
        (
          self => {
            let value = self.state.input;
            let _ = Api.getResults(value)
            |> Js.Promise.then_(results => {
              self.send(UpdateResults(results))
              Js.Promise.resolve();
            })
            /* This function needs to return a "unit" type, wo we'll insert it here */
            ();
          }
        ),
      )
    },
  render: self =>
    <div>
      <form
        onSubmit={
          ev => {
            ReactEvent.Form.preventDefault(ev);
            self.send(Search);
          }
        }>
        <label htmlFor="search"> {ReasonReact.string("Search")} </label>
        <input
          id="search"
          name="search"
          value={self.state.input}
          onChange={
            ev => {
              let value = ReactEvent.Form.target(ev)##value;
              self.send(UpdateInput(value));
            }
          }
        />
        <button type_="submit">
          {ReasonReact.string("Submit Search")}
        </button>
      </form>
      <div>
        {
          self.state.isLoading ?
            ReasonReact.string("Loading...") :
            self.state.results
            |> Array.of_list
            |> Array.map(({name, href, description}) => 
            <Card key={href} name href description />
            )
            |> ReasonReact.array
        }
      </div>
    </div>,
};