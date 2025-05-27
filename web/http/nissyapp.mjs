// Run ./build web from the main folder before running this example.
// The necessary modules (including worker.mjs) will be built and / or moved
// to this folder when running ./build web.

var solveButton = document.getElementById("solveButton");
var scrField = document.getElementById("scrambleText");
var resultsLabel = document.getElementById("resultsLabel");
var resultsText = document.getElementById("results");
var solverSelector = document.getElementById("solverSelector");

var lastCallbackId = 0;
var callbacks = new Map(); // Values: { f: function, arg: object }
var worker = new Worker("./worker.mjs", { type: "module" });
worker.onmessage = (event) => {
  if (event.data.command == "log") {
    console.log(event.data.object); // TODO
  } else if (!callbacks.has(event.data.id)) {
    console.log("[nissy app] Unknown callback " + event.data.id +
      " for command " + event.data.command);
  } else {
    var callback = callbacks.get(event.data.id);
    callback.f(callback.arg, event.data.arg);
  }
};

function updateResults(label, results, buttonState) {
  resultsLabel.innerText = label;
  resultsText.innerText = results;
  solveButton.disable = !buttonState;
}

solveButton.addEventListener("click", () => {
  updateResults("Loading solutions...", "", false);
  const callbackId = ++lastCallbackId;
  callbacks.set(callbackId, {
    f: (callbackArg, solveResult) => {
      if (solveResult.success) {
        const n = solveResult.solutions.length;
        const label = n == 0 ? "No solution found" :
          ("Found " + n + " solution" + (n == 1 ? "" : "s")) + ":";
        const text = solveResult.solutions.join("\n");
        updateResults(label, text, true);
      } else {
        updateResults("Unexpected error while solving!", "", true);
      }
    },
    arg: "" // Currently unused
  });
  worker.postMessage({
    command: "solve",
    id: callbackId,
    arg: {
      solver: solverSelector.options[solverSelector.selectedIndex].value,
      scramble: scrField.value,
      minmoves: 0,
      maxmoves: 20,
      maxsolutions: 1,
      optimal: 20,
      threads: window.navigator.hardwareConcurrency,
    }
  });
});
