import Nissy from "./nissy_web_module.mjs"

const nissy = await Nissy();
function log(message) {
  postMessage({ command: "log", id: -1, object: message });
  console.log(message);
}
nissy.setLogger(nissy._addCallbackFunction(log));

const commands = [
    { name: "solve", exec: solve },
    { name: "download solver data", exec: downloadSolverData },
];

// Message structure:
// {
//   command: string // The function to run.
//   id: number      // An identification number for the command, used for
//                   // sending responses back to the caller and having them
//                   // caught by the correct handler.
//   arg: object     // The actual argument of the message, command-specific.
//                   // See below for details.
// }
onmessage = (event) => {
  for (var i = 0; i < commands.length; i++) {
    if (commands[i].name == event.data.command) {
      commands[i].exec(event.data.id, event.data.arg);
      return;
    }
  }
 
  log("[nissy worker] unknown command " + event.data.command);
};

// Download solver data.
// Argument: string
async function downloadSolverData(id, solver) {
  const downloadSolverDataReturn = (success, message = "") => postMessage({
    command: "download solver data",
    id: id,
    arg: {
      success: success,
      message: message
    }
  });

  if (await nissy.initSolverDownload(arg.solver, "/tables")) {
    downloadSolverDataReturn(true);
  } else {
    downloadSolverDataReturn(false, "Error retrieving the solver data");
  }
}

// Solve the cube with the given options.
// Argument: {
//   solver: string
//   scramble: string
//   minmoves: number
//   maxmoves: number
//   maxsolutions: number
//   optimal: number
//   threads: number
// }
async function solve(id, arg) {
  const solveReturn = (success, solutions = [], message = "") => postMessage({
    command: "solve",
    id: id,
    arg: {
      success: success,
      solutions: solutions,
      message: message
    }
  });

  if (!(await nissy.isSolverAvailable(arg.solver))) {
    solveReturn(false, [], "Error: solver " + arg.solver +
      " is not available in this version of nissy.");
    return;
  }

  if (!(await nissy.isSolverLoaded(arg.solver)) &&
      !(await nissy.initSolverFromStorage(arg.solver))) {
    solveReturn(false, [], "Error: solver " + arg.solver + " has not been " +
      "loaded. Its data must be donwloaded or generated before using it.");
    return;
  }

  var cube = new nissy.Cube();
  cube.move(arg.scramble);

  // TODO: error handling here?
  const nissFlag = ((str) => {
    switch (str) {
      case "inverse": return nissy.NissFlag.inverse;
      case "mixed": return nissy.NissFlag.mixed;
      case "linear": return nissy.NissFlag.linear;
      case "all": return nissy.NissFlag.all;
      default: return nissy.NissFlag.normal;
    }
  })(arg.nissFlag);

  const result = await nissy.solve(arg.solver, cube, nissFlag, arg.minmoves,
    arg.maxmoves, arg.maxsolutions, arg.optimal, arg.threads, id);

  if (result.err.ok()) {
    //TODO: add solution lenght?
    var solutions = result.solutions == "" ? [] : result.solutions.split("\n");
    solutions.pop(); // Solver always returns string ending in newline
    solveReturn(true, solutions);
  } else {
    solveReturn(false, [],
      "Error while solving (error " + result.err.value + ")");
  }
};
