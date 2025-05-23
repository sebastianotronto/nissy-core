#include "storage.h"

#include "emscripten.h"
#include <filesystem>
#include <fstream>

EM_JS(int, inbrowser, (), { return typeof window !== 'undefined'; });
EM_JS(int, inworker, (), { return typeof WorkerGlobalScope !== 'undefined' &&
    self instanceof WorkerGlobalScope; });

std::string getprefix() {
	return inbrowser() || inworker() ? "/tables/" : "./tables/";
}

EM_ASYNC_JS(int, loadfs, (), {
  const dir = '/tables';
  const inBrowser = typeof window !== 'undefined';
  const inWorker = typeof WorkerGlobalScope !== 'undefined' &&
    self instanceof WorkerGlobalScope;

  if (!(inBrowser || inWorker)) return;

  if (!FS.analyzePath(dir).exists)
    FS.mkdir(dir);

  if (FS.analyzePath(dir).object.mount.mountpoint != dir) {
    FS.mount(IDBFS, { autoPersist: true }, dir);

    await new Promise((resolve, reject) => {
      FS.syncfs(true, function (err) {
        if (err) {
          reject(err);
        } else {
          resolve(true);
        }
      });
    });
  }
});

bool storage::read(std::string key, size_t data_size, char *data)
{
	loadfs();

	std::filesystem::path path(getprefix() + key);
	if (!std::filesystem::exists(path))
		return false;

	std::ifstream ifs(path, std::ios::binary);
	ifs.read(data, data_size);
	ifs.close();

	return !ifs.fail();
}

bool storage::write(std::string key, size_t data_size, const char *data)
{
	loadfs();

	std::filesystem::path path(getprefix() + key);

	std::ofstream ofs(path, std::ios::binary);
	ofs.write(data, data_size);
	ofs.close();

	return !ofs.fail();
}
