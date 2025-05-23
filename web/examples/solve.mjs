import Nissy from '../nissy_web_module.mjs'

const nissy = await Nissy();

var log = process.stdout.write.bind(process.stdout);
//var log = console.log
nissy.setLogger(nissy._addCallbackFunction(log))

var cube = new nissy.Cube();
cube.move('R\' U\' F');

nissy.solve('h48h0k4', cube, nissy.NissFlag.normal, 0, 8, 2, 99, 4, -1);
