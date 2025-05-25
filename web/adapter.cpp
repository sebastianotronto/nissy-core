#include "../cpp/nissy.h"
#include "storage.h"

#include <emscripten.h>
#include <emscripten/bind.h>
#include <map>
#include <set>
#include <string>
#include <vector>

EM_ASYNC_JS(void, fake_async, (), {});

extern "C" {
	extern int addCallbackFunction(/* args intentionally unspecified */);
	extern void callFunction(int, const char *);
	extern int callFunctionInt(int);
}

static int logger_id = -1;

void log(std::string s)
{
	if (logger_id == -1)
		return;

	callFunction(logger_id, s.c_str());
}

void log_wrapper(const char *cstr, void *data)
{
	log(cstr);
}

void set_logger(int id)
{
	logger_id = id;
	nissy::set_logger(log_wrapper, NULL);
}

// Some of the solvers are not available to the JS interface because of
// memory limitations.
const std::set<std::string> available_solvers
{
	"h48h0k4",
	"h48h1k2",
	"h48h2k2",
	"h48h3k2",
	"h48h7k2",
};

std::map<std::string, nissy::solver> loaded_solvers;

// TODO: this should ask the user if they want to download or generate.
bool init_solver(const std::string& name)
{
	auto se = nissy::solver::get(name);
	nissy::solver solver = std::get<nissy::solver>(se);

	solver.data.resize(solver.size);
	if (storage::read(solver.id, solver.size,
	    reinterpret_cast<char *>(solver.data.data()))) {
		log("Data for solver " + solver.name + " read from storage\n");
	} else {
		log("Could not read data for solver " + solver.name +
		    " from storage, generating it\n");
		auto err = solver.generate_data();

		if (!err.ok()) {
			log("Error generating the data!\n");
			return false;
		}
	}

	log("Checking data integrity "
	    "(this is done only once per session per solver)...\n");
	if (!solver.check_data().ok()) {
		log("Error! Data is corrupted!\n");
		return false;
	}
	loaded_solvers.insert({name, solver});

	if (storage::write(solver.id, solver.size,
	    reinterpret_cast<const char *>(solver.data.data()))) {
		log("Data for solver " + solver.name + " stored\n");
	} else {
		log("Error storing the data (the solver is usable, "
		    "but the data will have to be re-generated next "
		    "time you want to use it)");
	}

	return true;
}

bool solver_valid(const std::string& name)
{
	if (loaded_solvers.contains(name) ||
	    (available_solvers.contains(name) && init_solver(name)))
		return true;

	auto se = nissy::solver::get(name);
	if (std::holds_alternative<nissy::solver>(se))
		log("The solver " + name + " is not available in "
		   "the web version of Nissy. Use a native version.\n");
	else
		log("Invalid solver " + name + "\n");
	return false;
}

int poll_status(void *arg)
{
	if (arg == NULL || *(int *)arg == -1)
		return nissy::status::RUN.value;

	return callFunctionInt(*(int *)arg);
}

nissy::solver::solve_result solve(std::string name,
    nissy::cube cube, nissy::nissflag nissflag, unsigned minmoves,
    unsigned maxmoves, unsigned maxsols, unsigned optimal, unsigned threads,
    int poll_status_id)
{
	// Here we use a dirty trick to make this function always return the
	// same kind of JavaScript object. If we did not do this, the returned
	// object would be a Promise on the first run of the solver for each
	// session (because when loading the table some async JS code is
	// called), and a regular object otherwise.
	// TODO figure out if there is a better way to do this.
	fake_async();

	if (!solver_valid(name))
		return nissy::solver::solve_result
		    {.err = nissy::error::INVALID_SOLVER};

	return loaded_solvers.at(name).solve(cube, nissflag, minmoves,
	    maxmoves, maxsols, optimal, threads, NULL, &poll_status_id);
}

EMSCRIPTEN_BINDINGS(Nissy)
{
	emscripten::class_<nissy::nissflag>("NissFlag")
		.class_property("normal", &nissy::nissflag::NORMAL)
		.class_property("inverse", &nissy::nissflag::INVERSE)
		.class_property("mixed", &nissy::nissflag::MIXED)
		.class_property("linear", &nissy::nissflag::LINEAR)
		.class_property("all", &nissy::nissflag::ALL)
		;

	emscripten::class_<nissy::error>("Error")
		.function("ok", &nissy::error::ok)
		.class_property("unsolvableWarning", &nissy::error::UNSOLVABLE_WARNING)
		.class_property("unsolvableError", &nissy::error::UNSOLVABLE_ERROR)
		.class_property("invalidCube", &nissy::error::INVALID_CUBE)
		.class_property("invalidMoves", &nissy::error::INVALID_MOVES)
		.class_property("invalidTrans", &nissy::error::INVALID_TRANS)
		.class_property("invalidSolver", &nissy::error::INVALID_SOLVER)
		.class_property("nullPointer", &nissy::error::NULL_POINTER)
		.class_property("bufferSize", &nissy::error::BUFFER_SIZE)
		.class_property("data", &nissy::error::DATA)
		.class_property("options", &nissy::error::OPTIONS)
		.class_property("unknown", &nissy::error::UNKNOWN)
		;

	emscripten::constant("statusRUN", nissy::status::RUN.value);
	emscripten::constant("statusSTOP", nissy::status::STOP.value);
	emscripten::constant("statusPAUSE", nissy::status::PAUSE.value);
	emscripten::class_<nissy::status>("Status")
		.class_property("run", &nissy::status::RUN)
		.class_property("stop", &nissy::status::STOP)
		.class_property("pause", &nissy::status::PAUSE)
		;

	emscripten::class_<nissy::cube>("Cube")
		.constructor<>()
		.function("move", &nissy::cube::move)
		.function("transform", &nissy::cube::transform)
		.function("invert", &nissy::cube::invert)
		.function("toString", &nissy::cube::to_string)
		;

	emscripten::class_<nissy::solver::solve_result>("SolveResult")
		.property("err", &nissy::solver::solve_result::err)
		.property("solutions", &nissy::solver::solve_result::solutions)
		;

	emscripten::function("countMoves", &nissy::count_moves);
	emscripten::function("solve", &solve,
	    emscripten::return_value_policy::take_ownership());
	emscripten::function("setLogger", &set_logger,
	    emscripten::allow_raw_pointers());
	emscripten::function("addCallbackFunction", &addCallbackFunction);
}
