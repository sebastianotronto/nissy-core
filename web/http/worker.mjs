import Nissy from "./nissy_web_module.mjs"

const nissy = await Nissy();
nissy.setLogger(nissy._addCallbackFunction(console.log));

onmessage = (e) => {
//TODO: try to load from storage first, then download
  nissy.initSolverDownload(e.data.solver, "/tables").then(
  (download_result) => {
    if (!download_result) {
      postMessage("Error retrieving the solver data");
    } else {
      var cube = new nissy.Cube();
      cube.move(e.data.scramble);

      nissy.solve(e.data.solver, cube, nissy.NissFlag.normal,
        0, 17, 1, 99, 4, -1).then(
      (solve_result) => {
        if (!solve_result.err.ok())
          postMessage("Error while solving (solve returned " +
            solve_result.err.value + ")");
        else
          postMessage(solve_result.solutions)
      });
    }
  });
};
