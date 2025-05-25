import Nissy from "./nissy_web_module.mjs"

const nissy = await Nissy();
nissy.setLogger(nissy._addCallbackFunction(console.log));

onmessage = (e) => {
  var cube = new nissy.Cube();
  cube.move(e.data.scramble);
  nissy.solve(e.data.solver, cube, nissy.NissFlag.normal, 0, 17, 1, 99, 4, -1)
    .then((sol) => postMessage(sol))
};
