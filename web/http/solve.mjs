var solveButton = document.getElementById("solveButton");
var scrField = document.getElementById("scrambleText");
var solutionText = document.getElementById("solution");
var solverSelector = document.getElementById("solverSelector");

var worker = new Worker("./worker.mjs", { type: "module" });
worker.onmessage = (e) => {
  updateResults(e.data);
  enableSolveButton();
};

function updateResults(s) {
  solutionText.innerText = s;
}

function disableSolveButton() {
  solveButton.disabled = true;
  solveButton.innerText = "Solving...";
}

function enableSolveButton() {
  solveButton.disabled = false;
  solveButton.innerText = "Solve!";
}

solveButton.addEventListener("click", () => {
  disableSolveButton();
  updateResults("Loading solutions...");
  worker.postMessage({
    solver: solverSelector.options[solverSelector.selectedIndex].value,
    scramble: scrField.value
  });
});
